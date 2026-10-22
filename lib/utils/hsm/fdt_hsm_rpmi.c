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

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

ಸಂರಚನಾ rpmi_hsm_suspend {
	u32 ಅಂಕೆ_states;
	ಸಂರಚನಾ sbi_cpu_idle_state *states;
};

ಸಂರಚನಾ rpmi_hsm {
	ಸಂರಚನಾ mbox_chan *chan;
	ಸಂರಚನಾ rpmi_hsm_suspend *susp;
};

ಸ್ಥಿರ unsigned long rpmi_hsm_offset;

ಸ್ಥಿರ ಸಂರಚನಾ rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	ಸಂರಚನಾ sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	ಯದಿ (!scratch || !rpmi_hsm_offset)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	ಸಂರಚನಾ rpmi_hsm_hart_start_req req;
	ಸಂರಚನಾ rpmi_hsm_hart_start_resp resp;
	ಸಂರಚನಾ rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	ಯದಿ (!rpmi)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	ಪ್ರತ್ಯಾಗಮ rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_hsm_stop(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ rpmi_hsm_hart_stop_req req;
	ಸಂರಚನಾ rpmi_hsm_hart_stop_resp resp;
	ಶೂನ್ಯ (*jump_warmboot)(ಶೂನ್ಯ) =
		(ಶೂನ್ಯ (*)(ಶೂನ್ಯ))sbi_scratch_thishart_ptr()->warmboot_addr;
	ಸಂರಚನಾ rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ಯದಿ (!rpmi)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ bool is_rpmi_hsm_susp_supported(ಸಂರಚನಾ rpmi_hsm_suspend *susp, u32 type)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ (i = 0; i < susp->ಅಂಕೆ_states; i++)
		ಯದಿ (type == susp->states[i].suspend_param)
			ಪ್ರತ್ಯಾಗಮ true;

	ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ rpmi_hsm_hart_susp_req req;
	ಸಂರಚನಾ rpmi_hsm_hart_susp_resp resp;
	ಸಂರಚನಾ rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ಯದಿ (!rpmi)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	/* check if harts support this suspend type */
	ಯದಿ (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* Wait for interrupt */
	wfi();

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ಸ್ಥಿರ ಶೂನ್ಯ rpmi_hsm_do_fixup(ಸಂರಚನಾ fdt_general_fixup *f, ಶೂನ್ಯ *fdt)
{
	ಸಂರಚನಾ rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ಯದಿ (!rpmi || !rpmi->susp || !rpmi->susp->ಅಂಕೆ_states)
		ಪ್ರತ್ಯಾಗಮ;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

ಸ್ಥಿರ ಸಂರಚನಾ fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_hsm_get_ಅಂಕೆ_suspend_states(ಸಂರಚನಾ mbox_chan *chan,
					   ಸಂರಚನಾ rpmi_hsm_suspend *susp)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ rpmi_hsm_get_susp_types_req req;
	ಸಂರಚನಾ rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	susp->ಅಂಕೆ_states = resp.returned + resp.remaining;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_hsm_get_suspend_states(ಸಂರಚನಾ mbox_chan *chan,
					ಸಂರಚನಾ rpmi_hsm_suspend *susp)
{
	ಪೂರ್ಣಾಙ್ಕ rc, i, cnt = 0;
	ಸಂರಚನಾ rpmi_hsm_get_susp_types_req req;
	ಸಂರಚನಾ rpmi_hsm_get_susp_types_resp resp;
	ಸಂರಚನಾ rpmi_hsm_get_susp_info_req dreq;
	ಸಂರಚನಾ rpmi_hsm_get_susp_info_resp dresp;
	ಸಂರಚನಾ sbi_cpu_idle_state *state;

	ಯದಿ (!susp->ಅಂಕೆ_states)
		ಪ್ರತ್ಯಾಗಮ 0;

	req.start_index = 0;
	ಕುರು {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		ಪ್ರತಿ (i = 0; i < resp.returned && cnt < susp->ಅಂಕೆ_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} ಯಾವತ್ (resp.remaining);

	ಪ್ರತಿ (i = 0; i < susp->ಅಂಕೆ_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_ಎಣಿಕೆ(dreq), rpmi_u32_ಎಣಿಕೆ(dreq),
			&dresp, rpmi_u32_ಎಣಿಕೆ(dresp), rpmi_u32_ಎಣಿಕೆ(dresp));
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_hsm_update_hart_scratch(ಸಂರಚನಾ mbox_chan *chan,
					ಸಂರಚನಾ rpmi_hsm_suspend *susp)
{
	ಪೂರ್ಣಾಙ್ಕ rc, i;
	ಸಂರಚನಾ rpmi_hsm_get_hart_list_req req;
	ಸಂರಚನಾ rpmi_hsm_get_hart_list_resp resp;
	ಸಂರಚನಾ rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	ಕುರು {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_ಎಣಿಕೆ(req), rpmi_u32_ಎಣಿಕೆ(req),
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		ಪ್ರತಿ (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			ಯದಿ (!rpmi)
				ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} ಯಾವತ್ (resp.remaining);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_hsm_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			      ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc, i;
	ಸಂರಚನಾ mbox_chan *chan;
	ಸಂರಚನಾ rpmi_hsm_suspend *susp;

	ಯದಿ (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(ಸಂರಚನಾ rpmi_hsm);
		ಯದಿ (!rpmi_hsm_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(ಪರಿಮಾಣ(*susp));
	ಯದಿ (!susp)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_ಅಂಕೆ_suspend_states(chan, susp);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	ಯದಿ (!susp->ಅಂಕೆ_states)
		ಗಚ್ಛ skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->ಅಂಕೆ_states + 1, ಪರಿಮಾಣ(*susp->states));
	ಯದಿ (!susp->states) {
		rc = SBI_ENOMEM;
		ಗಚ್ಛ fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	ಪ್ರತಿ (i = 0; i < susp->ಅಂಕೆ_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		ಯದಿ (!susp->states[i].name) {
			ಕುರು {
				i--;
				sbi_free((ಶೂನ್ಯ *)susp->states[i].name);
			} ಯಾವತ್ (i > 0);

			rc = SBI_ENOMEM;
			ಗಚ್ಛ fail_free_susp_states;
		}
		sbi_snprintf((ವರ್ಣ *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	ಯದಿ (rc && rc != SBI_EALREADY)
		ಗಚ್ಛ fail_free_susp_state_names;

	/* Register HSM device */
	ಯದಿ (!susp->ಅಂಕೆ_states)
		sbi_hsm_rpmi.hart_suspend = ರಿಕ್ತ;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	ಪ್ರತ್ಯಾಗಮ 0;

fail_free_susp_state_names:
	ಪ್ರತಿ (i = 0; i < susp->ಅಂಕೆ_states; i++)
		sbi_free((ಶೂನ್ಯ *)susp->states[i].name);
fail_free_susp_states:
	ಯದಿ (susp->ಅಂಕೆ_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
