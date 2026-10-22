/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_fixup.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/mailbox/fdt_mailbox.h>
#অন্তর্ভাব <sbi_utils/mailbox/mailbox.h>
#অন্তর্ভাব <sbi_utils/mailbox/rpmi_mailbox.h>

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

কাঠামো rpmi_hsm_suspend {
	u32 অঙ্ক_states;
	কাঠামো sbi_cpu_idle_state *states;
};

কাঠামো rpmi_hsm {
	কাঠামো mbox_chan *chan;
	কাঠামো rpmi_hsm_suspend *susp;
};

স্থির unsigned long rpmi_hsm_offset;

স্থির কাঠামো rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	কাঠামো sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	যদি (!scratch || !rpmi_hsm_offset)
		ফেরত ফাঁকা;

	ফেরত sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

স্থির পূর্ণসংখ্যা rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	কাঠামো rpmi_hsm_hart_start_req req;
	কাঠামো rpmi_hsm_hart_start_resp resp;
	কাঠামো rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	যদি (!rpmi)
		ফেরত SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	ফেরত rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
}

স্থির পূর্ণসংখ্যা rpmi_hsm_stop(শূন্য)
{
	পূর্ণসংখ্যা rc;
	কাঠামো rpmi_hsm_hart_stop_req req;
	কাঠামো rpmi_hsm_hart_stop_resp resp;
	শূন্য (*jump_warmboot)(শূন্য) =
		(শূন্য (*)(শূন্য))sbi_scratch_thishart_ptr()->warmboot_addr;
	কাঠামো rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	যদি (!rpmi)
		ফেরত SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ফেরত rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ফেরত 0;
}

স্থির bool is_rpmi_hsm_susp_supported(কাঠামো rpmi_hsm_suspend *susp, u32 type)
{
	পূর্ণসংখ্যা i;

	জন্য (i = 0; i < susp->অঙ্ক_states; i++)
		যদি (type == susp->states[i].suspend_param)
			ফেরত true;

	ফেরত false;
}

স্থির পূর্ণসংখ্যা rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	পূর্ণসংখ্যা rc;
	কাঠামো rpmi_hsm_hart_susp_req req;
	কাঠামো rpmi_hsm_hart_susp_resp resp;
	কাঠামো rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	যদি (!rpmi)
		ফেরত SBI_ENOSYS;

	/* check if harts support this suspend type */
	যদি (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ফেরত SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ফেরত rc;

	/* Wait for interrupt */
	wfi();

	ফেরত 0;
}

স্থির কাঠামো sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

স্থির শূন্য rpmi_hsm_do_fixup(কাঠামো fdt_general_fixup *f, শূন্য *fdt)
{
	কাঠামো rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	যদি (!rpmi || !rpmi->susp || !rpmi->susp->অঙ্ক_states)
		ফেরত;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

স্থির কাঠামো fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

স্থির পূর্ণসংখ্যা rpmi_hsm_get_অঙ্ক_suspend_states(কাঠামো mbox_chan *chan,
					   কাঠামো rpmi_hsm_suspend *susp)
{
	পূর্ণসংখ্যা rc;
	কাঠামো rpmi_hsm_get_susp_types_req req;
	কাঠামো rpmi_hsm_get_susp_types_resp resp;

	req.start_সূচক = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ফেরত rc;

	susp->অঙ্ক_states = resp.returned + resp.remaining;
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা rpmi_hsm_get_suspend_states(কাঠামো mbox_chan *chan,
					কাঠামো rpmi_hsm_suspend *susp)
{
	পূর্ণসংখ্যা rc, i, cnt = 0;
	কাঠামো rpmi_hsm_get_susp_types_req req;
	কাঠামো rpmi_hsm_get_susp_types_resp resp;
	কাঠামো rpmi_hsm_get_susp_info_req dreq;
	কাঠামো rpmi_hsm_get_susp_info_resp dresp;
	কাঠামো sbi_cpu_idle_state *state;

	যদি (!susp->অঙ্ক_states)
		ফেরত 0;

	req.start_সূচক = 0;
	করো {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
		যদি (rc)
			ফেরত rc;

		জন্য (i = 0; i < resp.returned && cnt < susp->অঙ্ক_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_সূচক = i;
	} যখন (resp.remaining);

	জন্য (i = 0; i < susp->অঙ্ক_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_গুনতি(dreq), rpmi_u32_গুনতি(dreq),
			&dresp, rpmi_u32_গুনতি(dresp), rpmi_u32_গুনতি(dresp));
		যদি (rc)
			ফেরত rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা rpmi_hsm_update_hart_scratch(কাঠামো mbox_chan *chan,
					কাঠামো rpmi_hsm_suspend *susp)
{
	পূর্ণসংখ্যা rc, i;
	কাঠামো rpmi_hsm_get_hart_list_req req;
	কাঠামো rpmi_hsm_get_hart_list_resp resp;
	কাঠামো rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_সূচক = 0;
	করো {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
		যদি (rc)
			ফেরত rc;

		জন্য (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			যদি (!rpmi)
				ফেরত SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_সূচক += resp.returned;
	} যখন (resp.remaining);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা rpmi_hsm_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			      ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc, i;
	কাঠামো mbox_chan *chan;
	কাঠামো rpmi_hsm_suspend *susp;

	যদি (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(কাঠামো rpmi_hsm);
		যদি (!rpmi_hsm_offset)
			ফেরত SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	যদি (rc)
		ফেরত SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(আকার(*susp));
	যদি (!susp)
		ফেরত SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_অঙ্ক_suspend_states(chan, susp);
	যদি (rc)
		যাও fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	যদি (!susp->অঙ্ক_states)
		যাও skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->অঙ্ক_states + 1, আকার(*susp->states));
	যদি (!susp->states) {
		rc = SBI_ENOMEM;
		যাও fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	জন্য (i = 0; i < susp->অঙ্ক_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		যদি (!susp->states[i].name) {
			করো {
				i--;
				sbi_free((শূন্য *)susp->states[i].name);
			} যখন (i > 0);

			rc = SBI_ENOMEM;
			যাও fail_free_susp_states;
		}
		sbi_snprintf((অক্ষর *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	যদি (rc)
		যাও fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	যদি (rc)
		যাও fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	যদি (rc && rc != SBI_EALREADY)
		যাও fail_free_susp_state_names;

	/* Register HSM device */
	যদি (!susp->অঙ্ক_states)
		sbi_hsm_rpmi.hart_suspend = ফাঁকা;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	ফেরত 0;

fail_free_susp_state_names:
	জন্য (i = 0; i < susp->অঙ্ক_states; i++)
		sbi_free((শূন্য *)susp->states[i].name);
fail_free_susp_states:
	যদি (susp->অঙ্ক_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	ফেরত rc;
}

স্থির ধ্রুবক কাঠামো fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ধ্রুবক কাঠামো fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
