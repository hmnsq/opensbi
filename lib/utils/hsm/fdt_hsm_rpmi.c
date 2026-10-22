/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_fixup.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/mailbox/fdt_mailbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/mailbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/rpmi_mailbox.h>

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

கட்டமைப்பு rpmi_hsm_suspend {
	u32 இலக்கம்_states;
	கட்டமைப்பு sbi_cpu_idle_state *states;
};

கட்டமைப்பு rpmi_hsm {
	கட்டமைப்பு mbox_chan *chan;
	கட்டமைப்பு rpmi_hsm_suspend *susp;
};

நிலையான unsigned long rpmi_hsm_offset;

நிலையான கட்டமைப்பு rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	கட்டமைப்பு sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	எனில் (!scratch || !rpmi_hsm_offset)
		திரும்பு வெற்று;

	திரும்பு sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

நிலையான முழு rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	கட்டமைப்பு rpmi_hsm_hart_start_req req;
	கட்டமைப்பு rpmi_hsm_hart_start_resp resp;
	கட்டமைப்பு rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	எனில் (!rpmi)
		திரும்பு SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	திரும்பு rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_மொத்தம்(req), rpmi_u32_மொத்தம்(req),
			&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
}

நிலையான முழு rpmi_hsm_stop(வெறுமை)
{
	முழு rc;
	கட்டமைப்பு rpmi_hsm_hart_stop_req req;
	கட்டமைப்பு rpmi_hsm_hart_stop_resp resp;
	வெறுமை (*jump_warmboot)(வெறுமை) =
		(வெறுமை (*)(வெறுமை))sbi_scratch_thishart_ptr()->warmboot_addr;
	கட்டமைப்பு rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	எனில் (!rpmi)
		திரும்பு SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_மொத்தம்(req), rpmi_u32_மொத்தம்(req),
			&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
	எனில் (rc)
		திரும்பு rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	திரும்பு 0;
}

நிலையான bool is_rpmi_hsm_susp_supported(கட்டமைப்பு rpmi_hsm_suspend *susp, u32 type)
{
	முழு i;

	க்கு (i = 0; i < susp->இலக்கம்_states; i++)
		எனில் (type == susp->states[i].suspend_param)
			திரும்பு true;

	திரும்பு false;
}

நிலையான முழு rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	முழு rc;
	கட்டமைப்பு rpmi_hsm_hart_susp_req req;
	கட்டமைப்பு rpmi_hsm_hart_susp_resp resp;
	கட்டமைப்பு rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	எனில் (!rpmi)
		திரும்பு SBI_ENOSYS;

	/* check if harts support this suspend type */
	எனில் (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		திரும்பு SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_மொத்தம்(req), rpmi_u32_மொத்தம்(req),
			&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
	எனில் (rc)
		திரும்பு rc;

	/* Wait for interrupt */
	wfi();

	திரும்பு 0;
}

நிலையான கட்டமைப்பு sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

நிலையான வெறுமை rpmi_hsm_do_fixup(கட்டமைப்பு fdt_general_fixup *f, வெறுமை *fdt)
{
	கட்டமைப்பு rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	எனில் (!rpmi || !rpmi->susp || !rpmi->susp->இலக்கம்_states)
		திரும்பு;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

நிலையான கட்டமைப்பு fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

நிலையான முழு rpmi_hsm_get_இலக்கம்_suspend_states(கட்டமைப்பு mbox_chan *chan,
					   கட்டமைப்பு rpmi_hsm_suspend *susp)
{
	முழு rc;
	கட்டமைப்பு rpmi_hsm_get_susp_types_req req;
	கட்டமைப்பு rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_மொத்தம்(req), rpmi_u32_மொத்தம்(req),
			&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
	எனில் (rc)
		திரும்பு rc;

	susp->இலக்கம்_states = resp.returned + resp.remaining;
	திரும்பு 0;
}

நிலையான முழு rpmi_hsm_get_suspend_states(கட்டமைப்பு mbox_chan *chan,
					கட்டமைப்பு rpmi_hsm_suspend *susp)
{
	முழு rc, i, cnt = 0;
	கட்டமைப்பு rpmi_hsm_get_susp_types_req req;
	கட்டமைப்பு rpmi_hsm_get_susp_types_resp resp;
	கட்டமைப்பு rpmi_hsm_get_susp_info_req dreq;
	கட்டமைப்பு rpmi_hsm_get_susp_info_resp dresp;
	கட்டமைப்பு sbi_cpu_idle_state *state;

	எனில் (!susp->இலக்கம்_states)
		திரும்பு 0;

	req.start_index = 0;
	செய் {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_மொத்தம்(req), rpmi_u32_மொத்தம்(req),
			&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
		எனில் (rc)
			திரும்பு rc;

		க்கு (i = 0; i < resp.returned && cnt < susp->இலக்கம்_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} போது (resp.remaining);

	க்கு (i = 0; i < susp->இலக்கம்_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_மொத்தம்(dreq), rpmi_u32_மொத்தம்(dreq),
			&dresp, rpmi_u32_மொத்தம்(dresp), rpmi_u32_மொத்தம்(dresp));
		எனில் (rc)
			திரும்பு rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	திரும்பு 0;
}

நிலையான முழு rpmi_hsm_update_hart_scratch(கட்டமைப்பு mbox_chan *chan,
					கட்டமைப்பு rpmi_hsm_suspend *susp)
{
	முழு rc, i;
	கட்டமைப்பு rpmi_hsm_get_hart_list_req req;
	கட்டமைப்பு rpmi_hsm_get_hart_list_resp resp;
	கட்டமைப்பு rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	செய் {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_மொத்தம்(req), rpmi_u32_மொத்தம்(req),
			&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
		எனில் (rc)
			திரும்பு rc;

		க்கு (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			எனில் (!rpmi)
				திரும்பு SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} போது (resp.remaining);

	திரும்பு 0;
}

நிலையான முழு rpmi_hsm_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			      மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc, i;
	கட்டமைப்பு mbox_chan *chan;
	கட்டமைப்பு rpmi_hsm_suspend *susp;

	எனில் (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(கட்டமைப்பு rpmi_hsm);
		எனில் (!rpmi_hsm_offset)
			திரும்பு SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	எனில் (rc)
		திரும்பு SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(அளவு(*susp));
	எனில் (!susp)
		திரும்பு SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_இலக்கம்_suspend_states(chan, susp);
	எனில் (rc)
		செல் fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	எனில் (!susp->இலக்கம்_states)
		செல் skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->இலக்கம்_states + 1, அளவு(*susp->states));
	எனில் (!susp->states) {
		rc = SBI_ENOMEM;
		செல் fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	க்கு (i = 0; i < susp->இலக்கம்_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		எனில் (!susp->states[i].name) {
			செய் {
				i--;
				sbi_free((வெறுமை *)susp->states[i].name);
			} போது (i > 0);

			rc = SBI_ENOMEM;
			செல் fail_free_susp_states;
		}
		sbi_snprintf((எழுத்து *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	எனில் (rc)
		செல் fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	எனில் (rc)
		செல் fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	எனில் (rc && rc != SBI_EALREADY)
		செல் fail_free_susp_state_names;

	/* Register HSM device */
	எனில் (!susp->இலக்கம்_states)
		sbi_hsm_rpmi.hart_suspend = வெற்று;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	திரும்பு 0;

fail_free_susp_state_names:
	க்கு (i = 0; i < susp->இலக்கம்_states; i++)
		sbi_free((வெறுமை *)susp->states[i].name);
fail_free_susp_states:
	எனில் (susp->இலக்கம்_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	திரும்பு rc;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
