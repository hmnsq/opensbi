/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_fixup.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/fdt_mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/rpmi_mailbox.h>

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

ഘടന rpmi_hsm_suspend {
	u32 അക്കം_states;
	ഘടന sbi_cpu_idle_state *states;
};

ഘടന rpmi_hsm {
	ഘടന mbox_chan *chan;
	ഘടന rpmi_hsm_suspend *susp;
};

സ്ഥിതം unsigned long rpmi_hsm_offset;

സ്ഥിതം ഘടന rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	ഘടന sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	എങ്കിൽ (!scratch || !rpmi_hsm_offset)
		തിരികെ ശൂന്യ;

	തിരികെ sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	ഘടന rpmi_hsm_hart_start_req req;
	ഘടന rpmi_hsm_hart_start_resp resp;
	ഘടന rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	എങ്കിൽ (!rpmi)
		തിരികെ SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	തിരികെ rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_എണ്ണം(req), rpmi_u32_എണ്ണം(req),
			&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_hsm_stop(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന rpmi_hsm_hart_stop_req req;
	ഘടന rpmi_hsm_hart_stop_resp resp;
	ശൂന്യം (*jump_warmboot)(ശൂന്യം) =
		(ശൂന്യം (*)(ശൂന്യം))sbi_scratch_thishart_ptr()->warmboot_addr;
	ഘടന rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	എങ്കിൽ (!rpmi)
		തിരികെ SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_എണ്ണം(req), rpmi_u32_എണ്ണം(req),
			&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	തിരികെ 0;
}

സ്ഥിതം bool is_rpmi_hsm_susp_supported(ഘടന rpmi_hsm_suspend *susp, u32 type)
{
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി (i = 0; i < susp->അക്കം_states; i++)
		എങ്കിൽ (type == susp->states[i].suspend_param)
			തിരികെ true;

	തിരികെ false;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന rpmi_hsm_hart_susp_req req;
	ഘടന rpmi_hsm_hart_susp_resp resp;
	ഘടന rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	എങ്കിൽ (!rpmi)
		തിരികെ SBI_ENOSYS;

	/* check if harts support this suspend type */
	എങ്കിൽ (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		തിരികെ SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_എണ്ണം(req), rpmi_u32_എണ്ണം(req),
			&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* Wait for interrupt */
	wfi();

	തിരികെ 0;
}

സ്ഥിതം ഘടന sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

സ്ഥിതം ശൂന്യം rpmi_hsm_do_fixup(ഘടന fdt_general_fixup *f, ശൂന്യം *fdt)
{
	ഘടന rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	എങ്കിൽ (!rpmi || !rpmi->susp || !rpmi->susp->അക്കം_states)
		തിരികെ;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

സ്ഥിതം ഘടന fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_hsm_get_അക്കം_suspend_states(ഘടന mbox_chan *chan,
					   ഘടന rpmi_hsm_suspend *susp)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന rpmi_hsm_get_susp_types_req req;
	ഘടന rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_എണ്ണം(req), rpmi_u32_എണ്ണം(req),
			&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
	എങ്കിൽ (rc)
		തിരികെ rc;

	susp->അക്കം_states = resp.returned + resp.remaining;
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_hsm_get_suspend_states(ഘടന mbox_chan *chan,
					ഘടന rpmi_hsm_suspend *susp)
{
	പൂർണ്ണസംഖ്യ rc, i, cnt = 0;
	ഘടന rpmi_hsm_get_susp_types_req req;
	ഘടന rpmi_hsm_get_susp_types_resp resp;
	ഘടന rpmi_hsm_get_susp_info_req dreq;
	ഘടന rpmi_hsm_get_susp_info_resp dresp;
	ഘടന sbi_cpu_idle_state *state;

	എങ്കിൽ (!susp->അക്കം_states)
		തിരികെ 0;

	req.start_index = 0;
	ചെയ്യുക {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_എണ്ണം(req), rpmi_u32_എണ്ണം(req),
			&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
		എങ്കിൽ (rc)
			തിരികെ rc;

		വേണ്ടി (i = 0; i < resp.returned && cnt < susp->അക്കം_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} ആകുമ്പോൾ (resp.remaining);

	വേണ്ടി (i = 0; i < susp->അക്കം_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_എണ്ണം(dreq), rpmi_u32_എണ്ണം(dreq),
			&dresp, rpmi_u32_എണ്ണം(dresp), rpmi_u32_എണ്ണം(dresp));
		എങ്കിൽ (rc)
			തിരികെ rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_hsm_update_hart_scratch(ഘടന mbox_chan *chan,
					ഘടന rpmi_hsm_suspend *susp)
{
	പൂർണ്ണസംഖ്യ rc, i;
	ഘടന rpmi_hsm_get_hart_list_req req;
	ഘടന rpmi_hsm_get_hart_list_resp resp;
	ഘടന rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	ചെയ്യുക {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_എണ്ണം(req), rpmi_u32_എണ്ണം(req),
			&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
		എങ്കിൽ (rc)
			തിരികെ rc;

		വേണ്ടി (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			എങ്കിൽ (!rpmi)
				തിരികെ SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} ആകുമ്പോൾ (resp.remaining);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_hsm_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			      സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc, i;
	ഘടന mbox_chan *chan;
	ഘടന rpmi_hsm_suspend *susp;

	എങ്കിൽ (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(ഘടന rpmi_hsm);
		എങ്കിൽ (!rpmi_hsm_offset)
			തിരികെ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	എങ്കിൽ (rc)
		തിരികെ SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(വലുപ്പം(*susp));
	എങ്കിൽ (!susp)
		തിരികെ SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_അക്കം_suspend_states(chan, susp);
	എങ്കിൽ (rc)
		പോകുക fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	എങ്കിൽ (!susp->അക്കം_states)
		പോകുക skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->അക്കം_states + 1, വലുപ്പം(*susp->states));
	എങ്കിൽ (!susp->states) {
		rc = SBI_ENOMEM;
		പോകുക fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	വേണ്ടി (i = 0; i < susp->അക്കം_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		എങ്കിൽ (!susp->states[i].name) {
			ചെയ്യുക {
				i--;
				sbi_free((ശൂന്യം *)susp->states[i].name);
			} ആകുമ്പോൾ (i > 0);

			rc = SBI_ENOMEM;
			പോകുക fail_free_susp_states;
		}
		sbi_snprintf((അക്ഷരം *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	എങ്കിൽ (rc)
		പോകുക fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	എങ്കിൽ (rc)
		പോകുക fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	എങ്കിൽ (rc && rc != SBI_EALREADY)
		പോകുക fail_free_susp_state_names;

	/* Register HSM device */
	എങ്കിൽ (!susp->അക്കം_states)
		sbi_hsm_rpmi.hart_suspend = ശൂന്യ;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	തിരികെ 0;

fail_free_susp_state_names:
	വേണ്ടി (i = 0; i < susp->അക്കം_states; i++)
		sbi_free((ശൂന്യം *)susp->states[i].name);
fail_free_susp_states:
	എങ്കിൽ (susp->അക്കം_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	തിരികെ rc;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
