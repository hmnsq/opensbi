/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_fixup.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/mailbox/fdt_mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_mailbox.h>

#నిర్వచించు MAX_HSM_SUPSEND_STATE_NAMELEN		16

నిర్మాణం rpmi_hsm_suspend {
	u32 అంకె_స్థితులు;
	నిర్మాణం sbi_cpu_idle_state *స్థితులు;
};

నిర్మాణం rpmi_hsm {
	నిర్మాణం mbox_chan *chan;
	నిర్మాణం rpmi_hsm_suspend *susp;
};

స్థిర గుర్తులేని దీర్ఘ rpmi_hsm_స్థానభ్రంశం;

స్థిర నిర్మాణం rpmi_hsm *rpmi_hsm_get_సూచిక(u32 hartid)
{
	నిర్మాణం sbi_scratch *scratch;

	scratch = sbi_hartid_గమ్యం_scratch(hartid);
	ఒకవేళ (!scratch || !rpmi_hsm_స్థానభ్రంశం)
		తిరిగి ఖాళీ;

	తిరిగి sbi_scratch_స్థానభ్రంశం_ptr(scratch, rpmi_hsm_స్థానభ్రంశం);
}

స్థిర పూర్ణాంకం rpmi_hsm_ప్రారంభం(u32 hartid, ulong resume_addr)
{
	నిర్మాణం rpmi_hsm_hart_ప్రారంభం_req req;
	నిర్మాణం rpmi_hsm_hart_ప్రారంభం_resp resp;
	నిర్మాణం rpmi_hsm *rpmi = rpmi_hsm_get_సూచిక(hartid);

	ఒకవేళ (!rpmi)
		తిరిగి SBI_ENOSYS;

	req.hartid = hartid;
	req.ప్రారంభం_addr_lo = resume_addr;
	req.ప్రారంభం_addr_hi = (u64)resume_addr  >> 32;

	తిరిగి rpmi_సాధారణ_request_with_స్థితి(
			rpmi->chan, RPMI_HSM_SRV_HART_ప్రారంభం,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
}

స్థిర పూర్ణాంకం rpmi_hsm_stop(శూన్య)
{
	పూర్ణాంకం rc;
	నిర్మాణం rpmi_hsm_hart_stop_req req;
	నిర్మాణం rpmi_hsm_hart_stop_resp resp;
	శూన్య (*jump_warmboot)(శూన్య) =
		(శూన్య (*)(శూన్య))sbi_scratch_thishart_ptr()->warmboot_addr;
	నిర్మాణం rpmi_hsm *rpmi = rpmi_hsm_get_సూచిక(current_hartid());

	ఒకవేళ (!rpmi)
		తిరిగి SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_సాధారణ_request_with_స్థితి(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	ఒకవేళ (rc)
		తిరిగి rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	తిరిగి 0;
}

స్థిర bool is_rpmi_hsm_susp_supported(నిర్మాణం rpmi_hsm_suspend *susp, u32 type)
{
	పూర్ణాంకం i;

	కొరకు (i = 0; i < susp->అంకె_స్థితులు; i++)
		ఒకవేళ (type == susp->స్థితులు[i].suspend_param)
			తిరిగి true;

	తిరిగి false;
}

స్థిర పూర్ణాంకం rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	పూర్ణాంకం rc;
	నిర్మాణం rpmi_hsm_hart_susp_req req;
	నిర్మాణం rpmi_hsm_hart_susp_resp resp;
	నిర్మాణం rpmi_hsm *rpmi = rpmi_hsm_get_సూచిక(current_hartid());

	ఒకవేళ (!rpmi)
		తిరిగి SBI_ENOSYS;

	/* check if harts support this suspend type */
	ఒకవేళ (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		తిరిగి SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_సాధారణ_request_with_స్థితి(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	ఒకవేళ (rc)
		తిరిగి rc;

	/* Wait for interrupt */
	wfi();

	తిరిగి 0;
}

స్థిర నిర్మాణం sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_ప్రారంభం	= rpmi_hsm_ప్రారంభం,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

స్థిర శూన్య rpmi_hsm_do_fixup(నిర్మాణం fdt_general_fixup *f, శూన్య *fdt)
{
	నిర్మాణం rpmi_hsm *rpmi = rpmi_hsm_get_సూచిక(current_hartid());

	ఒకవేళ (!rpmi || !rpmi->susp || !rpmi->susp->అంకె_స్థితులు)
		తిరిగి;

	fdt_కూడు_cpu_idle_స్థితులు(fdt, rpmi->susp->స్థితులు);
}

స్థిర నిర్మాణం fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

స్థిర పూర్ణాంకం rpmi_hsm_get_అంకె_suspend_స్థితులు(నిర్మాణం mbox_chan *chan,
					   నిర్మాణం rpmi_hsm_suspend *susp)
{
	పూర్ణాంకం rc;
	నిర్మాణం rpmi_hsm_get_susp_types_req req;
	నిర్మాణం rpmi_hsm_get_susp_types_resp resp;

	req.ప్రారంభం_సూచీ = 0;
	rc = rpmi_సాధారణ_request_with_స్థితి(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	ఒకవేళ (rc)
		తిరిగి rc;

	susp->అంకె_స్థితులు = resp.returned + resp.మిగిలిన;
	తిరిగి 0;
}

స్థిర పూర్ణాంకం rpmi_hsm_get_suspend_స్థితులు(నిర్మాణం mbox_chan *chan,
					నిర్మాణం rpmi_hsm_suspend *susp)
{
	పూర్ణాంకం rc, i, cnt = 0;
	నిర్మాణం rpmi_hsm_get_susp_types_req req;
	నిర్మాణం rpmi_hsm_get_susp_types_resp resp;
	నిర్మాణం rpmi_hsm_get_susp_info_req dreq;
	నిర్మాణం rpmi_hsm_get_susp_info_resp dresp;
	నిర్మాణం sbi_cpu_idle_state *state;

	ఒకవేళ (!susp->అంకె_స్థితులు)
		తిరిగి 0;

	req.ప్రారంభం_సూచీ = 0;
	చేయి {
		rc = rpmi_సాధారణ_request_with_స్థితి(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
		ఒకవేళ (rc)
			తిరిగి rc;

		కొరకు (i = 0; i < resp.returned && cnt < susp->అంకె_స్థితులు; i++)
			susp->స్థితులు[cnt++].suspend_param = resp.types[i];
		req.ప్రారంభం_సూచీ = i;
	} అయితే (resp.మిగిలిన);

	కొరకు (i = 0; i < susp->అంకె_స్థితులు; i++) {
		state = &susp->స్థితులు[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_సాధారణ_request_with_స్థితి(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_లెక్క(dreq), rpmi_u32_లెక్క(dreq),
			&dresp, rpmi_u32_లెక్క(dresp), rpmi_u32_లెక్క(dresp));
		ఒకవేళ (rc)
			తిరిగి rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->నమోదు_latency_us = dresp.నమోదు_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం rpmi_hsm_update_hart_scratch(నిర్మాణం mbox_chan *chan,
					నిర్మాణం rpmi_hsm_suspend *susp)
{
	పూర్ణాంకం rc, i;
	నిర్మాణం rpmi_hsm_get_hart_list_req req;
	నిర్మాణం rpmi_hsm_get_hart_list_resp resp;
	నిర్మాణం rpmi_hsm *rpmi = rpmi_hsm_get_సూచిక(current_hartid());

	req.ప్రారంభం_సూచీ = 0;
	చేయి {
		rc = rpmi_సాధారణ_request_with_స్థితి(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_లెక్క(req), rpmi_u32_లెక్క(req),
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
		ఒకవేళ (rc)
			తిరిగి rc;

		కొరకు (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_సూచిక(resp.hartid[i]);
			ఒకవేళ (!rpmi)
				తిరిగి SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.ప్రారంభం_సూచీ += resp.returned;
	} అయితే (resp.మిగిలిన);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం rpmi_hsm_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			      స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc, i;
	నిర్మాణం mbox_chan *chan;
	నిర్మాణం rpmi_hsm_suspend *susp;

	ఒకవేళ (!rpmi_hsm_స్థానభ్రంశం) {
		rpmi_hsm_స్థానభ్రంశం =
			sbi_scratch_alloc_type_స్థానభ్రంశం(నిర్మాణం rpmi_hsm);
		ఒకవేళ (!rpmi_hsm_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ఒకవేళ (rc)
		తిరిగి SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(పరిమాణం(*susp));
	ఒకవేళ (!susp)
		తిరిగి SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_అంకె_suspend_స్థితులు(chan, susp);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	ఒకవేళ (!susp->అంకె_స్థితులు)
		వెళ్ళు skip_suspend_స్థితులు;

	/* Allocate array of HART suspend states */
	susp->స్థితులు = sbi_calloc(susp->అంకె_స్థితులు + 1, పరిమాణం(*susp->స్థితులు));
	ఒకవేళ (!susp->స్థితులు) {
		rc = SBI_ENOMEM;
		వెళ్ళు fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	కొరకు (i = 0; i < susp->అంకె_స్థితులు; i++) {
		susp->స్థితులు[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		ఒకవేళ (!susp->స్థితులు[i].name) {
			చేయి {
				i--;
				sbi_free((శూన్య *)susp->స్థితులు[i].name);
			} అయితే (i > 0);

			rc = SBI_ENOMEM;
			వెళ్ళు fail_free_susp_స్థితులు;
		}
		sbi_snprintf((అక్షరం *)susp->స్థితులు[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_స్థితులు(chan, susp);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_susp_state_పేర్లు;

skip_suspend_స్థితులు:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_susp_state_పేర్లు;

	/* Register HSM fixup callback */
	rc = fdt_రిజిస్టరు_general_fixup(&rpmi_hsm_fixup);
	ఒకవేళ (rc && rc != SBI_EALREADY)
		వెళ్ళు fail_free_susp_state_పేర్లు;

	/* Register HSM device */
	ఒకవేళ (!susp->అంకె_స్థితులు)
		sbi_hsm_rpmi.hart_suspend = ఖాళీ;
	sbi_hsm_సమితి_device(&sbi_hsm_rpmi);

	తిరిగి 0;

fail_free_susp_state_పేర్లు:
	కొరకు (i = 0; i < susp->అంకె_స్థితులు; i++)
		sbi_free((శూన్య *)susp->స్థితులు[i].name);
fail_free_susp_స్థితులు:
	ఒకవేళ (susp->అంకె_స్థితులు)
		sbi_free(susp->స్థితులు);
fail_free_susp:
	sbi_free(susp);
	తిరిగి rc;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక rpmi_hsm_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_hsm_rpmi = {
	.సరిపోలిక_పట్టిక = rpmi_hsm_సరిపోలిక,
	.init = rpmi_hsm_cold_init,
};
