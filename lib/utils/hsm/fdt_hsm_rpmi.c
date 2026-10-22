/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_fixup.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/mailbox/fdt_mailbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/mailbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/rpmi_mailbox.h>

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

સંરચના rpmi_hsm_suspend {
	u32 અંક_states;
	સંરચના sbi_cpu_idle_state *states;
};

સંરચના rpmi_hsm {
	સંરચના mbox_chan *chan;
	સંરચના rpmi_hsm_suspend *susp;
};

સ્થિર unsigned long rpmi_hsm_offset;

સ્થિર સંરચના rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	સંરચના sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	જો (!scratch || !rpmi_hsm_offset)
		પાછા ખાલી;

	પાછા sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

સ્થિર પૂર્ણાંક rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	સંરચના rpmi_hsm_hart_start_req req;
	સંરચના rpmi_hsm_hart_start_resp resp;
	સંરચના rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	જો (!rpmi)
		પાછા SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	પાછા rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_ગણતરી(req), rpmi_u32_ગણતરી(req),
			&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
}

સ્થિર પૂર્ણાંક rpmi_hsm_stop(શૂન્ય)
{
	પૂર્ણાંક rc;
	સંરચના rpmi_hsm_hart_stop_req req;
	સંરચના rpmi_hsm_hart_stop_resp resp;
	શૂન્ય (*jump_warmboot)(શૂન્ય) =
		(શૂન્ય (*)(શૂન્ય))sbi_scratch_thishart_ptr()->warmboot_addr;
	સંરચના rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	જો (!rpmi)
		પાછા SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_ગણતરી(req), rpmi_u32_ગણતરી(req),
			&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
	જો (rc)
		પાછા rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	પાછા 0;
}

સ્થિર bool is_rpmi_hsm_susp_supported(સંરચના rpmi_hsm_suspend *susp, u32 type)
{
	પૂર્ણાંક i;

	માટે (i = 0; i < susp->અંક_states; i++)
		જો (type == susp->states[i].suspend_param)
			પાછા true;

	પાછા false;
}

સ્થિર પૂર્ણાંક rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	પૂર્ણાંક rc;
	સંરચના rpmi_hsm_hart_susp_req req;
	સંરચના rpmi_hsm_hart_susp_resp resp;
	સંરચના rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	જો (!rpmi)
		પાછા SBI_ENOSYS;

	/* check if harts support this suspend type */
	જો (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		પાછા SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_ગણતરી(req), rpmi_u32_ગણતરી(req),
			&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
	જો (rc)
		પાછા rc;

	/* Wait for interrupt */
	wfi();

	પાછા 0;
}

સ્થિર સંરચના sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

સ્થિર શૂન્ય rpmi_hsm_do_fixup(સંરચના fdt_general_fixup *f, શૂન્ય *fdt)
{
	સંરચના rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	જો (!rpmi || !rpmi->susp || !rpmi->susp->અંક_states)
		પાછા;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

સ્થિર સંરચના fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

સ્થિર પૂર્ણાંક rpmi_hsm_get_અંક_suspend_states(સંરચના mbox_chan *chan,
					   સંરચના rpmi_hsm_suspend *susp)
{
	પૂર્ણાંક rc;
	સંરચના rpmi_hsm_get_susp_types_req req;
	સંરચના rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ગણતરી(req), rpmi_u32_ગણતરી(req),
			&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
	જો (rc)
		પાછા rc;

	susp->અંક_states = resp.returned + resp.remaining;
	પાછા 0;
}

સ્થિર પૂર્ણાંક rpmi_hsm_get_suspend_states(સંરચના mbox_chan *chan,
					સંરચના rpmi_hsm_suspend *susp)
{
	પૂર્ણાંક rc, i, cnt = 0;
	સંરચના rpmi_hsm_get_susp_types_req req;
	સંરચના rpmi_hsm_get_susp_types_resp resp;
	સંરચના rpmi_hsm_get_susp_info_req dreq;
	સંરચના rpmi_hsm_get_susp_info_resp dresp;
	સંરચના sbi_cpu_idle_state *state;

	જો (!susp->અંક_states)
		પાછા 0;

	req.start_index = 0;
	કરો {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ગણતરી(req), rpmi_u32_ગણતરી(req),
			&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
		જો (rc)
			પાછા rc;

		માટે (i = 0; i < resp.returned && cnt < susp->અંક_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} જ્યારે (resp.remaining);

	માટે (i = 0; i < susp->અંક_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_ગણતરી(dreq), rpmi_u32_ગણતરી(dreq),
			&dresp, rpmi_u32_ગણતરી(dresp), rpmi_u32_ગણતરી(dresp));
		જો (rc)
			પાછા rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક rpmi_hsm_update_hart_scratch(સંરચના mbox_chan *chan,
					સંરચના rpmi_hsm_suspend *susp)
{
	પૂર્ણાંક rc, i;
	સંરચના rpmi_hsm_get_hart_list_req req;
	સંરચના rpmi_hsm_get_hart_list_resp resp;
	સંરચના rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	કરો {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_ગણતરી(req), rpmi_u32_ગણતરી(req),
			&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
		જો (rc)
			પાછા rc;

		માટે (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			જો (!rpmi)
				પાછા SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} જ્યારે (resp.remaining);

	પાછા 0;
}

સ્થિર પૂર્ણાંક rpmi_hsm_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			      અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc, i;
	સંરચના mbox_chan *chan;
	સંરચના rpmi_hsm_suspend *susp;

	જો (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(સંરચના rpmi_hsm);
		જો (!rpmi_hsm_offset)
			પાછા SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	જો (rc)
		પાછા SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(કદ(*susp));
	જો (!susp)
		પાછા SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_અંક_suspend_states(chan, susp);
	જો (rc)
		જાઓ fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	જો (!susp->અંક_states)
		જાઓ skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->અંક_states + 1, કદ(*susp->states));
	જો (!susp->states) {
		rc = SBI_ENOMEM;
		જાઓ fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	માટે (i = 0; i < susp->અંક_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		જો (!susp->states[i].name) {
			કરો {
				i--;
				sbi_free((શૂન્ય *)susp->states[i].name);
			} જ્યારે (i > 0);

			rc = SBI_ENOMEM;
			જાઓ fail_free_susp_states;
		}
		sbi_snprintf((અક્ષર *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	જો (rc)
		જાઓ fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	જો (rc)
		જાઓ fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	જો (rc && rc != SBI_EALREADY)
		જાઓ fail_free_susp_state_names;

	/* Register HSM device */
	જો (!susp->અંક_states)
		sbi_hsm_rpmi.hart_suspend = ખાલી;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	પાછા 0;

fail_free_susp_state_names:
	માટે (i = 0; i < susp->અંક_states; i++)
		sbi_free((શૂન્ય *)susp->states[i].name);
fail_free_susp_states:
	જો (susp->અંક_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	પાછા rc;
}

સ્થિર અચલ સંરચના fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

અચલ સંરચના fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
