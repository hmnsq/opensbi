/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_hsm.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_fixup.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/fdt_mailbox.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/mailbox.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/rpmi_mailbox.h>

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

সংরচনা rpmi_hsm_suspend {
	u32 অঙ্ক_states;
	সংরচনা sbi_cpu_idle_state *states;
};

সংরচনা rpmi_hsm {
	সংরচনা mbox_chan *chan;
	সংরচনা rpmi_hsm_suspend *susp;
};

স্থির unsigned long rpmi_hsm_offset;

স্থির সংরচনা rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	সংরচনা sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	যদি (!scratch || !rpmi_hsm_offset)
		ঘূৰাই রিক্ত;

	ঘূৰাই sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

স্থির পূর্ণাঙ্ক rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	সংরচনা rpmi_hsm_hart_start_req req;
	সংরচনা rpmi_hsm_hart_start_resp resp;
	সংরচনা rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	যদি (!rpmi)
		ঘূৰাই SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	ঘূৰাই rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
}

স্থির পূর্ণাঙ্ক rpmi_hsm_stop(শূন্য)
{
	পূর্ণাঙ্ক rc;
	সংরচনা rpmi_hsm_hart_stop_req req;
	সংরচনা rpmi_hsm_hart_stop_resp resp;
	শূন্য (*jump_warmboot)(শূন্য) =
		(শূন্য (*)(শূন্য))sbi_scratch_thishart_ptr()->warmboot_addr;
	সংরচনা rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	যদি (!rpmi)
		ঘূৰাই SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ঘূৰাই rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ঘূৰাই 0;
}

স্থির bool is_rpmi_hsm_susp_supported(সংরচনা rpmi_hsm_suspend *susp, u32 type)
{
	পূর্ণাঙ্ক i;

	বাবে (i = 0; i < susp->অঙ্ক_states; i++)
		যদি (type == susp->states[i].suspend_param)
			ঘূৰাই true;

	ঘূৰাই false;
}

স্থির পূর্ণাঙ্ক rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	পূর্ণাঙ্ক rc;
	সংরচনা rpmi_hsm_hart_susp_req req;
	সংরচনা rpmi_hsm_hart_susp_resp resp;
	সংরচনা rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	যদি (!rpmi)
		ঘূৰাই SBI_ENOSYS;

	/* check if harts support this suspend type */
	যদি (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ঘূৰাই SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ঘূৰাই rc;

	/* Wait for interrupt */
	wfi();

	ঘূৰাই 0;
}

স্থির সংরচনা sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

স্থির শূন্য rpmi_hsm_do_fixup(সংরচনা fdt_general_fixup *f, শূন্য *fdt)
{
	সংরচনা rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	যদি (!rpmi || !rpmi->susp || !rpmi->susp->অঙ্ক_states)
		ঘূৰাই;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

স্থির সংরচনা fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

স্থির পূর্ণাঙ্ক rpmi_hsm_get_অঙ্ক_suspend_states(সংরচনা mbox_chan *chan,
					   সংরচনা rpmi_hsm_suspend *susp)
{
	পূর্ণাঙ্ক rc;
	সংরচনা rpmi_hsm_get_susp_types_req req;
	সংরচনা rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (rc)
		ঘূৰাই rc;

	susp->অঙ্ক_states = resp.returned + resp.remaining;
	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক rpmi_hsm_get_suspend_states(সংরচনা mbox_chan *chan,
					সংরচনা rpmi_hsm_suspend *susp)
{
	পূর্ণাঙ্ক rc, i, cnt = 0;
	সংরচনা rpmi_hsm_get_susp_types_req req;
	সংরচনা rpmi_hsm_get_susp_types_resp resp;
	সংরচনা rpmi_hsm_get_susp_info_req dreq;
	সংরচনা rpmi_hsm_get_susp_info_resp dresp;
	সংরচনা sbi_cpu_idle_state *state;

	যদি (!susp->অঙ্ক_states)
		ঘূৰাই 0;

	req.start_index = 0;
	কৰা {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
		যদি (rc)
			ঘূৰাই rc;

		বাবে (i = 0; i < resp.returned && cnt < susp->অঙ্ক_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} যেতিয়ালৈ (resp.remaining);

	বাবে (i = 0; i < susp->অঙ্ক_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_গুনতি(dreq), rpmi_u32_গুনতি(dreq),
			&dresp, rpmi_u32_গুনতি(dresp), rpmi_u32_গুনতি(dresp));
		যদি (rc)
			ঘূৰাই rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক rpmi_hsm_update_hart_scratch(সংরচনা mbox_chan *chan,
					সংরচনা rpmi_hsm_suspend *susp)
{
	পূর্ণাঙ্ক rc, i;
	সংরচনা rpmi_hsm_get_hart_list_req req;
	সংরচনা rpmi_hsm_get_hart_list_resp resp;
	সংরচনা rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	কৰা {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_গুনতি(req), rpmi_u32_গুনতি(req),
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
		যদি (rc)
			ঘূৰাই rc;

		বাবে (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			যদি (!rpmi)
				ঘূৰাই SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} যেতিয়ালৈ (resp.remaining);

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক rpmi_hsm_cold_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc, i;
	সংরচনা mbox_chan *chan;
	সংরচনা rpmi_hsm_suspend *susp;

	যদি (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(সংরচনা rpmi_hsm);
		যদি (!rpmi_hsm_offset)
			ঘূৰাই SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	যদি (rc)
		ঘূৰাই SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(পরিমাণ(*susp));
	যদি (!susp)
		ঘূৰাই SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_অঙ্ক_suspend_states(chan, susp);
	যদি (rc)
		গচ্ছ fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	যদি (!susp->অঙ্ক_states)
		গচ্ছ skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->অঙ্ক_states + 1, পরিমাণ(*susp->states));
	যদি (!susp->states) {
		rc = SBI_ENOMEM;
		গচ্ছ fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	বাবে (i = 0; i < susp->অঙ্ক_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		যদি (!susp->states[i].name) {
			কৰা {
				i--;
				sbi_free((শূন্য *)susp->states[i].name);
			} যেতিয়ালৈ (i > 0);

			rc = SBI_ENOMEM;
			গচ্ছ fail_free_susp_states;
		}
		sbi_snprintf((ৱর্ণ *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	যদি (rc)
		গচ্ছ fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	যদি (rc)
		গচ্ছ fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	যদি (rc && rc != SBI_EALREADY)
		গচ্ছ fail_free_susp_state_names;

	/* Register HSM device */
	যদি (!susp->অঙ্ক_states)
		sbi_hsm_rpmi.hart_suspend = রিক্ত;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	ঘূৰাই 0;

fail_free_susp_state_names:
	বাবে (i = 0; i < susp->অঙ্ক_states; i++)
		sbi_free((শূন্য *)susp->states[i].name);
fail_free_susp_states:
	যদি (susp->অঙ্ক_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	ঘূৰাই rc;
}

স্থির ধ্রুৱ সংরচনা fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ধ্রুৱ সংরচনা fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
