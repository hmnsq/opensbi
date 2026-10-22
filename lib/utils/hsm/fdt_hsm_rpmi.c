/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_fixup.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/fdt_mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/rpmi_mailbox.h>

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

ସଂରଚନା rpmi_hsm_suspend {
	u32 ଅଙ୍କ_states;
	ସଂରଚନା sbi_cpu_idle_state *states;
};

ସଂରଚନା rpmi_hsm {
	ସଂରଚନା mbox_chan *chan;
	ସଂରଚନା rpmi_hsm_suspend *susp;
};

ସ୍ଥିର unsigned long rpmi_hsm_offset;

ସ୍ଥିର ସଂରଚନା rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	ସଂରଚନା sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	ଯଦି (!scratch || !rpmi_hsm_offset)
		ଫେରନ୍ତୁ ଖାଲି;

	ଫେରନ୍ତୁ sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	ସଂରଚନା rpmi_hsm_hart_start_req req;
	ସଂରଚନା rpmi_hsm_hart_start_resp resp;
	ସଂରଚନା rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	ଯଦି (!rpmi)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	ଫେରନ୍ତୁ rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_ଗଣନା(req), rpmi_u32_ଗଣନା(req),
			&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_hsm_stop(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା rpmi_hsm_hart_stop_req req;
	ସଂରଚନା rpmi_hsm_hart_stop_resp resp;
	ଶୂନ୍ୟ (*jump_warmboot)(ଶୂନ୍ୟ) =
		(ଶୂନ୍ୟ (*)(ଶୂନ୍ୟ))sbi_scratch_thishart_ptr()->warmboot_addr;
	ସଂରଚନା rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ଯଦି (!rpmi)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_ଗଣନା(req), rpmi_u32_ଗଣନା(req),
			&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର bool is_rpmi_hsm_susp_supported(ସଂରଚନା rpmi_hsm_suspend *susp, u32 type)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ (i = 0; i < susp->ଅଙ୍କ_states; i++)
		ଯଦି (type == susp->states[i].suspend_param)
			ଫେରନ୍ତୁ true;

	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା rpmi_hsm_hart_susp_req req;
	ସଂରଚନା rpmi_hsm_hart_susp_resp resp;
	ସଂରଚନା rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ଯଦି (!rpmi)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	/* check if harts support this suspend type */
	ଯଦି (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ଫେରନ୍ତୁ SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_ଗଣନା(req), rpmi_u32_ଗଣନା(req),
			&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* Wait for interrupt */
	wfi();

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ସ୍ଥିର ଶୂନ୍ୟ rpmi_hsm_do_fixup(ସଂରଚନା fdt_general_fixup *f, ଶୂନ୍ୟ *fdt)
{
	ସଂରଚନା rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ଯଦି (!rpmi || !rpmi->susp || !rpmi->susp->ଅଙ୍କ_states)
		ଫେରନ୍ତୁ;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

ସ୍ଥିର ସଂରଚନା fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_hsm_get_ଅଙ୍କ_suspend_states(ସଂରଚନା mbox_chan *chan,
					   ସଂରଚନା rpmi_hsm_suspend *susp)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା rpmi_hsm_get_susp_types_req req;
	ସଂରଚନା rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ଗଣନା(req), rpmi_u32_ଗଣନା(req),
			&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	susp->ଅଙ୍କ_states = resp.returned + resp.remaining;
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_hsm_get_suspend_states(ସଂରଚନା mbox_chan *chan,
					ସଂରଚନା rpmi_hsm_suspend *susp)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, i, cnt = 0;
	ସଂରଚନା rpmi_hsm_get_susp_types_req req;
	ସଂରଚନା rpmi_hsm_get_susp_types_resp resp;
	ସଂରଚନା rpmi_hsm_get_susp_info_req dreq;
	ସଂରଚନା rpmi_hsm_get_susp_info_resp dresp;
	ସଂରଚନା sbi_cpu_idle_state *state;

	ଯଦି (!susp->ଅଙ୍କ_states)
		ଫେରନ୍ତୁ 0;

	req.start_index = 0;
	କର {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ଗଣନା(req), rpmi_u32_ଗଣନା(req),
			&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		ପାଇଁ (i = 0; i < resp.returned && cnt < susp->ଅଙ୍କ_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} ଯେତେବେଳେ (resp.remaining);

	ପାଇଁ (i = 0; i < susp->ଅଙ୍କ_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_ଗଣନା(dreq), rpmi_u32_ଗଣନା(dreq),
			&dresp, rpmi_u32_ଗଣନା(dresp), rpmi_u32_ଗଣନା(dresp));
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_hsm_update_hart_scratch(ସଂରଚନା mbox_chan *chan,
					ସଂରଚନା rpmi_hsm_suspend *susp)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, i;
	ସଂରଚନା rpmi_hsm_get_hart_list_req req;
	ସଂରଚନା rpmi_hsm_get_hart_list_resp resp;
	ସଂରଚନା rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	କର {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_ଗଣନା(req), rpmi_u32_ଗଣନା(req),
			&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		ପାଇଁ (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			ଯଦି (!rpmi)
				ଫେରନ୍ତୁ SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} ଯେତେବେଳେ (resp.remaining);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_hsm_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			      ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, i;
	ସଂରଚନା mbox_chan *chan;
	ସଂରଚନା rpmi_hsm_suspend *susp;

	ଯଦି (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(ସଂରଚନା rpmi_hsm);
		ଯଦି (!rpmi_hsm_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ଯଦି (rc)
		ଫେରନ୍ତୁ SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(ଆକାର(*susp));
	ଯଦି (!susp)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_ଅଙ୍କ_suspend_states(chan, susp);
	ଯଦି (rc)
		ଯାଅ fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	ଯଦି (!susp->ଅଙ୍କ_states)
		ଯାଅ skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->ଅଙ୍କ_states + 1, ଆକାର(*susp->states));
	ଯଦି (!susp->states) {
		rc = SBI_ENOMEM;
		ଯାଅ fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	ପାଇଁ (i = 0; i < susp->ଅଙ୍କ_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		ଯଦି (!susp->states[i].name) {
			କର {
				i--;
				sbi_free((ଶୂନ୍ୟ *)susp->states[i].name);
			} ଯେତେବେଳେ (i > 0);

			rc = SBI_ENOMEM;
			ଯାଅ fail_free_susp_states;
		}
		sbi_snprintf((ଅକ୍ଷର *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	ଯଦି (rc)
		ଯାଅ fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	ଯଦି (rc)
		ଯାଅ fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	ଯଦି (rc && rc != SBI_EALREADY)
		ଯାଅ fail_free_susp_state_names;

	/* Register HSM device */
	ଯଦି (!susp->ଅଙ୍କ_states)
		sbi_hsm_rpmi.hart_suspend = ଖାଲି;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	ଫେରନ୍ତୁ 0;

fail_free_susp_state_names:
	ପାଇଁ (i = 0; i < susp->ଅଙ୍କ_states; i++)
		sbi_free((ଶୂନ୍ୟ *)susp->states[i].name);
fail_free_susp_states:
	ଯଦି (susp->ଅଙ୍କ_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
