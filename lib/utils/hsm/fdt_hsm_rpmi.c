/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_fixup.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/fdt_mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_mailbox.h>

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HSM_SUPSEND_STATE_NAMELEN		16

ರಚನೆ rpmi_hsm_suspend {
	u32 ಅಂಕೆ_ಸ್ಥಿತಿಗಳು;
	ರಚನೆ sbi_cpu_idle_state *ಸ್ಥಿತಿಗಳು;
};

ರಚನೆ rpmi_hsm {
	ರಚನೆ mbox_chan *chan;
	ರಚನೆ rpmi_hsm_suspend *susp;
};

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ rpmi_hsm_ಸ್ಥಾನಾಂತರ;

ಸ್ಥಿರ ರಚನೆ rpmi_hsm *rpmi_hsm_get_ಸೂಚಕ(u32 hartid)
{
	ರಚನೆ sbi_scratch *scratch;

	scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
	ಆದರೆ (!scratch || !rpmi_hsm_ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, rpmi_hsm_ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_hsm_ಆರಂಭ(u32 hartid, ulong resume_addr)
{
	ರಚನೆ rpmi_hsm_hart_ಆರಂಭ_req req;
	ರಚನೆ rpmi_hsm_hart_ಆರಂಭ_resp resp;
	ರಚನೆ rpmi_hsm *rpmi = rpmi_hsm_get_ಸೂಚಕ(hartid);

	ಆದರೆ (!rpmi)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	req.hartid = hartid;
	req.ಆರಂಭ_addr_lo = resume_addr;
	req.ಆರಂಭ_addr_hi = (u64)resume_addr  >> 32;

	ಹಿಂತಿರುಗಿ rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			rpmi->chan, RPMI_HSM_SRV_HART_ಆರಂಭ,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_hsm_stop(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ rpmi_hsm_hart_stop_req req;
	ರಚನೆ rpmi_hsm_hart_stop_resp resp;
	ಶೂನ್ಯ (*jump_warmboot)(ಶೂನ್ಯ) =
		(ಶೂನ್ಯ (*)(ಶೂನ್ಯ))sbi_scratch_thishart_ptr()->warmboot_addr;
	ರಚನೆ rpmi_hsm *rpmi = rpmi_hsm_get_ಸೂಚಕ(current_hartid());

	ಆದರೆ (!rpmi)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ bool is_rpmi_hsm_susp_supported(ರಚನೆ rpmi_hsm_suspend *susp, u32 type)
{
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ (i = 0; i < susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; i++)
		ಆದರೆ (type == susp->ಸ್ಥಿತಿಗಳು[i].suspend_param)
			ಹಿಂತಿರುಗಿ true;

	ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ rpmi_hsm_hart_susp_req req;
	ರಚನೆ rpmi_hsm_hart_susp_resp resp;
	ರಚನೆ rpmi_hsm *rpmi = rpmi_hsm_get_ಸೂಚಕ(current_hartid());

	ಆದರೆ (!rpmi)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	/* check if harts support this suspend type */
	ಆದರೆ (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* Wait for interrupt */
	wfi();

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_ಆರಂಭ	= rpmi_hsm_ಆರಂಭ,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ಸ್ಥಿರ ಶೂನ್ಯ rpmi_hsm_do_fixup(ರಚನೆ fdt_general_fixup *f, ಶೂನ್ಯ *fdt)
{
	ರಚನೆ rpmi_hsm *rpmi = rpmi_hsm_get_ಸೂಚಕ(current_hartid());

	ಆದರೆ (!rpmi || !rpmi->susp || !rpmi->susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು)
		ಹಿಂತಿರುಗಿ;

	fdt_ಕೂಡಿಸು_cpu_idle_ಸ್ಥಿತಿಗಳು(fdt, rpmi->susp->ಸ್ಥಿತಿಗಳು);
}

ಸ್ಥಿರ ರಚನೆ fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_hsm_get_ಅಂಕೆ_suspend_ಸ್ಥಿತಿಗಳು(ರಚನೆ mbox_chan *chan,
					   ರಚನೆ rpmi_hsm_suspend *susp)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ rpmi_hsm_get_susp_types_req req;
	ರಚನೆ rpmi_hsm_get_susp_types_resp resp;

	req.ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು = resp.returned + resp.ಉಳಿದ;
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_hsm_get_suspend_ಸ್ಥಿತಿಗಳು(ರಚನೆ mbox_chan *chan,
					ರಚನೆ rpmi_hsm_suspend *susp)
{
	ಪೂರ್ಣಾಂಕ rc, i, cnt = 0;
	ರಚನೆ rpmi_hsm_get_susp_types_req req;
	ರಚನೆ rpmi_hsm_get_susp_types_resp resp;
	ರಚನೆ rpmi_hsm_get_susp_info_req dreq;
	ರಚನೆ rpmi_hsm_get_susp_info_resp dresp;
	ರಚನೆ sbi_cpu_idle_state *state;

	ಆದರೆ (!susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು)
		ಹಿಂತಿರುಗಿ 0;

	req.ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
	ಮಾಡು {
		rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		ಗಾಗಿ (i = 0; i < resp.returned && cnt < susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; i++)
			susp->ಸ್ಥಿತಿಗಳು[cnt++].suspend_param = resp.types[i];
		req.ಆರಂಭ_ಸೂಚ್ಯಂಕ = i;
	} ಆಗ (resp.ಉಳಿದ);

	ಗಾಗಿ (i = 0; i < susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; i++) {
		state = &susp->ಸ್ಥಿತಿಗಳು[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_ಎಣಿಕೆ(dreq), rpmi_u32_ಎಣಿಕೆ(dreq),
			&dresp, rpmi_u32_ಎಣಿಕೆ(dresp), rpmi_u32_ಎಣಿಕೆ(dresp));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->ನಮೂದು_latency_us = dresp.ನಮೂದು_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_hsm_update_hart_scratch(ರಚನೆ mbox_chan *chan,
					ರಚನೆ rpmi_hsm_suspend *susp)
{
	ಪೂರ್ಣಾಂಕ rc, i;
	ರಚನೆ rpmi_hsm_get_hart_list_req req;
	ರಚನೆ rpmi_hsm_get_hart_list_resp resp;
	ರಚನೆ rpmi_hsm *rpmi = rpmi_hsm_get_ಸೂಚಕ(current_hartid());

	req.ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
	ಮಾಡು {
		rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		ಗಾಗಿ (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_ಸೂಚಕ(resp.hartid[i]);
			ಆದರೆ (!rpmi)
				ಹಿಂತಿರುಗಿ SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.ಆರಂಭ_ಸೂಚ್ಯಂಕ += resp.returned;
	} ಆಗ (resp.ಉಳಿದ);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_hsm_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			      ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc, i;
	ರಚನೆ mbox_chan *chan;
	ರಚನೆ rpmi_hsm_suspend *susp;

	ಆದರೆ (!rpmi_hsm_ಸ್ಥಾನಾಂತರ) {
		rpmi_hsm_ಸ್ಥಾನಾಂತರ =
			sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ರಚನೆ rpmi_hsm);
		ಆದರೆ (!rpmi_hsm_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(ಗಾತ್ರ(*susp));
	ಆದರೆ (!susp)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_ಅಂಕೆ_suspend_ಸ್ಥಿತಿಗಳು(chan, susp);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	ಆದರೆ (!susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು)
		ಹೋಗು skip_suspend_ಸ್ಥಿತಿಗಳು;

	/* Allocate array of HART suspend states */
	susp->ಸ್ಥಿತಿಗಳು = sbi_calloc(susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು + 1, ಗಾತ್ರ(*susp->ಸ್ಥಿತಿಗಳು));
	ಆದರೆ (!susp->ಸ್ಥಿತಿಗಳು) {
		rc = SBI_ENOMEM;
		ಹೋಗು fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	ಗಾಗಿ (i = 0; i < susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; i++) {
		susp->ಸ್ಥಿತಿಗಳು[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		ಆದರೆ (!susp->ಸ್ಥಿತಿಗಳು[i].name) {
			ಮಾಡು {
				i--;
				sbi_free((ಶೂನ್ಯ *)susp->ಸ್ಥಿತಿಗಳು[i].name);
			} ಆಗ (i > 0);

			rc = SBI_ENOMEM;
			ಹೋಗು fail_free_susp_ಸ್ಥಿತಿಗಳು;
		}
		sbi_snprintf((ಅಕ್ಷರ *)susp->ಸ್ಥಿತಿಗಳು[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_ಸ್ಥಿತಿಗಳು(chan, susp);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_susp_state_ಹೆಸರುಗಳು;

skip_suspend_ಸ್ಥಿತಿಗಳು:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_susp_state_ಹೆಸರುಗಳು;

	/* Register HSM fixup callback */
	rc = fdt_ರಿಜಿಸ್ಟರ್_general_fixup(&rpmi_hsm_fixup);
	ಆದರೆ (rc && rc != SBI_EALREADY)
		ಹೋಗು fail_free_susp_state_ಹೆಸರುಗಳು;

	/* Register HSM device */
	ಆದರೆ (!susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು)
		sbi_hsm_rpmi.hart_suspend = ಖಾಲಿ;
	sbi_hsm_ಗಣ_device(&sbi_hsm_rpmi);

	ಹಿಂತಿರುಗಿ 0;

fail_free_susp_state_ಹೆಸರುಗಳು:
	ಗಾಗಿ (i = 0; i < susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; i++)
		sbi_free((ಶೂನ್ಯ *)susp->ಸ್ಥಿತಿಗಳು[i].name);
fail_free_susp_ಸ್ಥಿತಿಗಳು:
	ಆದರೆ (susp->ಅಂಕೆ_ಸ್ಥಿತಿಗಳು)
		sbi_free(susp->ಸ್ಥಿತಿಗಳು);
fail_free_susp:
	sbi_free(susp);
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ rpmi_hsm_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_hsm_rpmi = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = rpmi_hsm_ಹೊಂದಿಕೆ,
	.init = rpmi_hsm_cold_init,
};
