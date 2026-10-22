/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_fixup.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/fdt_mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_mailbox.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HSM_SUPSEND_STATE_NAMELEN		16

ਬਣਤਰ rpmi_hsm_suspend {
	u32 ਅੰਕ_ਹਾਲਤਾਂ;
	ਬਣਤਰ sbi_cpu_idle_state *ਹਾਲਤਾਂ;
};

ਬਣਤਰ rpmi_hsm {
	ਬਣਤਰ mbox_chan *chan;
	ਬਣਤਰ rpmi_hsm_suspend *susp;
};

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ rpmi_hsm_ਵਿੱਥ;

ਸਥਿਰ ਬਣਤਰ rpmi_hsm *rpmi_hsm_get_ਸੂਚਕ(u32 hartid)
{
	ਬਣਤਰ sbi_scratch *scratch;

	scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
	ਜੇ (!scratch || !rpmi_hsm_ਵਿੱਥ)
		ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ sbi_scratch_ਵਿੱਥ_ptr(scratch, rpmi_hsm_ਵਿੱਥ);
}

ਸਥਿਰ ਪੂਰਨ rpmi_hsm_ਸ਼ੁਰੂ(u32 hartid, ulong resume_addr)
{
	ਬਣਤਰ rpmi_hsm_hart_ਸ਼ੁਰੂ_req req;
	ਬਣਤਰ rpmi_hsm_hart_ਸ਼ੁਰੂ_resp resp;
	ਬਣਤਰ rpmi_hsm *rpmi = rpmi_hsm_get_ਸੂਚਕ(hartid);

	ਜੇ (!rpmi)
		ਵਾਪਸ SBI_ENOSYS;

	req.hartid = hartid;
	req.ਸ਼ੁਰੂ_addr_lo = resume_addr;
	req.ਸ਼ੁਰੂ_addr_hi = (u64)resume_addr  >> 32;

	ਵਾਪਸ rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			rpmi->chan, RPMI_HSM_SRV_HART_ਸ਼ੁਰੂ,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
}

ਸਥਿਰ ਪੂਰਨ rpmi_hsm_stop(ਖਾਲੀ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ rpmi_hsm_hart_stop_req req;
	ਬਣਤਰ rpmi_hsm_hart_stop_resp resp;
	ਖਾਲੀ (*jump_warmboot)(ਖਾਲੀ) =
		(ਖਾਲੀ (*)(ਖਾਲੀ))sbi_scratch_thishart_ptr()->warmboot_addr;
	ਬਣਤਰ rpmi_hsm *rpmi = rpmi_hsm_get_ਸੂਚਕ(current_hartid());

	ਜੇ (!rpmi)
		ਵਾਪਸ SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ਵਾਪਸ 0;
}

ਸਥਿਰ bool is_rpmi_hsm_susp_supported(ਬਣਤਰ rpmi_hsm_suspend *susp, u32 type)
{
	ਪੂਰਨ i;

	ਲਈ (i = 0; i < susp->ਅੰਕ_ਹਾਲਤਾਂ; i++)
		ਜੇ (type == susp->ਹਾਲਤਾਂ[i].suspend_param)
			ਵਾਪਸ true;

	ਵਾਪਸ false;
}

ਸਥਿਰ ਪੂਰਨ rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ rpmi_hsm_hart_susp_req req;
	ਬਣਤਰ rpmi_hsm_hart_susp_resp resp;
	ਬਣਤਰ rpmi_hsm *rpmi = rpmi_hsm_get_ਸੂਚਕ(current_hartid());

	ਜੇ (!rpmi)
		ਵਾਪਸ SBI_ENOSYS;

	/* check if harts support this suspend type */
	ਜੇ (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ਵਾਪਸ SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* Wait for interrupt */
	wfi();

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_ਸ਼ੁਰੂ	= rpmi_hsm_ਸ਼ੁਰੂ,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ਸਥਿਰ ਖਾਲੀ rpmi_hsm_do_fixup(ਬਣਤਰ fdt_general_fixup *f, ਖਾਲੀ *fdt)
{
	ਬਣਤਰ rpmi_hsm *rpmi = rpmi_hsm_get_ਸੂਚਕ(current_hartid());

	ਜੇ (!rpmi || !rpmi->susp || !rpmi->susp->ਅੰਕ_ਹਾਲਤਾਂ)
		ਵਾਪਸ;

	fdt_ਜੋੜੋ_cpu_idle_ਹਾਲਤਾਂ(fdt, rpmi->susp->ਹਾਲਤਾਂ);
}

ਸਥਿਰ ਬਣਤਰ fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ਸਥਿਰ ਪੂਰਨ rpmi_hsm_get_ਅੰਕ_suspend_ਹਾਲਤਾਂ(ਬਣਤਰ mbox_chan *chan,
					   ਬਣਤਰ rpmi_hsm_suspend *susp)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ rpmi_hsm_get_susp_types_req req;
	ਬਣਤਰ rpmi_hsm_get_susp_types_resp resp;

	req.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	ਜੇ (rc)
		ਵਾਪਸ rc;

	susp->ਅੰਕ_ਹਾਲਤਾਂ = resp.returned + resp.ਬਚਿਆ;
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rpmi_hsm_get_suspend_ਹਾਲਤਾਂ(ਬਣਤਰ mbox_chan *chan,
					ਬਣਤਰ rpmi_hsm_suspend *susp)
{
	ਪੂਰਨ rc, i, cnt = 0;
	ਬਣਤਰ rpmi_hsm_get_susp_types_req req;
	ਬਣਤਰ rpmi_hsm_get_susp_types_resp resp;
	ਬਣਤਰ rpmi_hsm_get_susp_info_req dreq;
	ਬਣਤਰ rpmi_hsm_get_susp_info_resp dresp;
	ਬਣਤਰ sbi_cpu_idle_state *state;

	ਜੇ (!susp->ਅੰਕ_ਹਾਲਤਾਂ)
		ਵਾਪਸ 0;

	req.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
	ਕਰੋ {
		rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
		ਜੇ (rc)
			ਵਾਪਸ rc;

		ਲਈ (i = 0; i < resp.returned && cnt < susp->ਅੰਕ_ਹਾਲਤਾਂ; i++)
			susp->ਹਾਲਤਾਂ[cnt++].suspend_param = resp.types[i];
		req.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = i;
	} ਜਦੋਂ (resp.ਬਚਿਆ);

	ਲਈ (i = 0; i < susp->ਅੰਕ_ਹਾਲਤਾਂ; i++) {
		state = &susp->ਹਾਲਤਾਂ[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_ਗਿਣਤੀ(dreq), rpmi_u32_ਗਿਣਤੀ(dreq),
			&dresp, rpmi_u32_ਗਿਣਤੀ(dresp), rpmi_u32_ਗਿਣਤੀ(dresp));
		ਜੇ (rc)
			ਵਾਪਸ rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->ਇੰਦਰਾਜ_latency_us = dresp.ਇੰਦਰਾਜ_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rpmi_hsm_update_hart_scratch(ਬਣਤਰ mbox_chan *chan,
					ਬਣਤਰ rpmi_hsm_suspend *susp)
{
	ਪੂਰਨ rc, i;
	ਬਣਤਰ rpmi_hsm_get_hart_list_req req;
	ਬਣਤਰ rpmi_hsm_get_hart_list_resp resp;
	ਬਣਤਰ rpmi_hsm *rpmi = rpmi_hsm_get_ਸੂਚਕ(current_hartid());

	req.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
	ਕਰੋ {
		rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_ਗਿਣਤੀ(req), rpmi_u32_ਗਿਣਤੀ(req),
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
		ਜੇ (rc)
			ਵਾਪਸ rc;

		ਲਈ (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_ਸੂਚਕ(resp.hartid[i]);
			ਜੇ (!rpmi)
				ਵਾਪਸ SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ += resp.returned;
	} ਜਦੋਂ (resp.ਬਚਿਆ);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rpmi_hsm_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			      ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc, i;
	ਬਣਤਰ mbox_chan *chan;
	ਬਣਤਰ rpmi_hsm_suspend *susp;

	ਜੇ (!rpmi_hsm_ਵਿੱਥ) {
		rpmi_hsm_ਵਿੱਥ =
			sbi_scratch_alloc_type_ਵਿੱਥ(ਬਣਤਰ rpmi_hsm);
		ਜੇ (!rpmi_hsm_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ਜੇ (rc)
		ਵਾਪਸ SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(ਆਕਾਰ(*susp));
	ਜੇ (!susp)
		ਵਾਪਸ SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_ਅੰਕ_suspend_ਹਾਲਤਾਂ(chan, susp);
	ਜੇ (rc)
		ਜਾਓ fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	ਜੇ (!susp->ਅੰਕ_ਹਾਲਤਾਂ)
		ਜਾਓ skip_suspend_ਹਾਲਤਾਂ;

	/* Allocate array of HART suspend states */
	susp->ਹਾਲਤਾਂ = sbi_calloc(susp->ਅੰਕ_ਹਾਲਤਾਂ + 1, ਆਕਾਰ(*susp->ਹਾਲਤਾਂ));
	ਜੇ (!susp->ਹਾਲਤਾਂ) {
		rc = SBI_ENOMEM;
		ਜਾਓ fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	ਲਈ (i = 0; i < susp->ਅੰਕ_ਹਾਲਤਾਂ; i++) {
		susp->ਹਾਲਤਾਂ[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		ਜੇ (!susp->ਹਾਲਤਾਂ[i].name) {
			ਕਰੋ {
				i--;
				sbi_free((ਖਾਲੀ *)susp->ਹਾਲਤਾਂ[i].name);
			} ਜਦੋਂ (i > 0);

			rc = SBI_ENOMEM;
			ਜਾਓ fail_free_susp_ਹਾਲਤਾਂ;
		}
		sbi_snprintf((ਅੱਖਰ *)susp->ਹਾਲਤਾਂ[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_ਹਾਲਤਾਂ(chan, susp);
	ਜੇ (rc)
		ਜਾਓ fail_free_susp_state_ਨਾਂ;

skip_suspend_ਹਾਲਤਾਂ:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	ਜੇ (rc)
		ਜਾਓ fail_free_susp_state_ਨਾਂ;

	/* Register HSM fixup callback */
	rc = fdt_ਰਜਿਸਟਰ_general_fixup(&rpmi_hsm_fixup);
	ਜੇ (rc && rc != SBI_EALREADY)
		ਜਾਓ fail_free_susp_state_ਨਾਂ;

	/* Register HSM device */
	ਜੇ (!susp->ਅੰਕ_ਹਾਲਤਾਂ)
		sbi_hsm_rpmi.hart_suspend = ਨਿਰਾ;
	sbi_hsm_ਸੈੱਟ_device(&sbi_hsm_rpmi);

	ਵਾਪਸ 0;

fail_free_susp_state_ਨਾਂ:
	ਲਈ (i = 0; i < susp->ਅੰਕ_ਹਾਲਤਾਂ; i++)
		sbi_free((ਖਾਲੀ *)susp->ਹਾਲਤਾਂ[i].name);
fail_free_susp_ਹਾਲਤਾਂ:
	ਜੇ (susp->ਅੰਕ_ਹਾਲਤਾਂ)
		sbi_free(susp->ਹਾਲਤਾਂ);
fail_free_susp:
	sbi_free(susp);
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ rpmi_hsm_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_hsm_rpmi = {
	.ਮੇਲ_ਸਾਰਣੀ = rpmi_hsm_ਮੇਲ,
	.init = rpmi_hsm_cold_init,
};
