/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_fixup.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/mailbox/fdt_mailbox.h>
#ضمن <sbi_utils/mailbox/mailbox.h>
#ضمن <sbi_utils/mailbox/rpmi_mailbox.h>

#وصف_ڪريو MAX_HSM_SUPSEND_STATE_NAMELEN		16

بنية rpmi_hsm_suspend {
	u32 انگ_حالتون;
	بنية sbi_cpu_idle_state *حالتون;
};

بنية rpmi_hsm {
	بنية mbox_chan *chan;
	بنية rpmi_hsm_suspend *susp;
};

ساكن بنانشان ڊگهو rpmi_hsm_وٿي;

ساكن بنية rpmi_hsm *rpmi_hsm_get_اشارو(u32 hartid)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartid_منزل_scratch(hartid);
	إذا (!scratch || !rpmi_hsm_وٿي)
		أرجع عدم;

	أرجع sbi_scratch_وٿي_ptr(scratch, rpmi_hsm_وٿي);
}

ساكن صحيح rpmi_hsm_شروعات(u32 hartid, ulong resume_addr)
{
	بنية rpmi_hsm_hart_شروعات_req req;
	بنية rpmi_hsm_hart_شروعات_resp resp;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارو(hartid);

	إذا (!rpmi)
		أرجع SBI_ENOSYS;

	req.hartid = hartid;
	req.شروعات_addr_lo = resume_addr;
	req.شروعات_addr_hi = (u64)resume_addr  >> 32;

	أرجع rpmi_عام_request_with_حالت(
			rpmi->chan, RPMI_HSM_SRV_HART_شروعات,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
}

ساكن صحيح rpmi_hsm_stop(فراغ)
{
	صحيح rc;
	بنية rpmi_hsm_hart_stop_req req;
	بنية rpmi_hsm_hart_stop_resp resp;
	فراغ (*jump_warmboot)(فراغ) =
		(فراغ (*)(فراغ))sbi_scratch_thishart_ptr()->warmboot_addr;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارو(current_hartid());

	إذا (!rpmi)
		أرجع SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_عام_request_with_حالت(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
	إذا (rc)
		أرجع rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	أرجع 0;
}

ساكن bool is_rpmi_hsm_susp_supported(بنية rpmi_hsm_suspend *susp, u32 type)
{
	صحيح i;

	لكل (i = 0; i < susp->انگ_حالتون; i++)
		إذا (type == susp->حالتون[i].suspend_param)
			أرجع true;

	أرجع false;
}

ساكن صحيح rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	صحيح rc;
	بنية rpmi_hsm_hart_susp_req req;
	بنية rpmi_hsm_hart_susp_resp resp;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارو(current_hartid());

	إذا (!rpmi)
		أرجع SBI_ENOSYS;

	/* check if harts support this suspend type */
	إذا (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		أرجع SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_عام_request_with_حالت(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
	إذا (rc)
		أرجع rc;

	/* Wait for interrupt */
	wfi();

	أرجع 0;
}

ساكن بنية sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_شروعات	= rpmi_hsm_شروعات,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ساكن فراغ rpmi_hsm_do_fixup(بنية fdt_general_fixup *f, فراغ *fdt)
{
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارو(current_hartid());

	إذا (!rpmi || !rpmi->susp || !rpmi->susp->انگ_حالتون)
		أرجع;

	fdt_جوڙيو_cpu_idle_حالتون(fdt, rpmi->susp->حالتون);
}

ساكن بنية fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ساكن صحيح rpmi_hsm_get_انگ_suspend_حالتون(بنية mbox_chan *chan,
					   بنية rpmi_hsm_suspend *susp)
{
	صحيح rc;
	بنية rpmi_hsm_get_susp_types_req req;
	بنية rpmi_hsm_get_susp_types_resp resp;

	req.شروعات_اشاريو = 0;
	rc = rpmi_عام_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
	إذا (rc)
		أرجع rc;

	susp->انگ_حالتون = resp.returned + resp.بچيل;
	أرجع 0;
}

ساكن صحيح rpmi_hsm_get_suspend_حالتون(بنية mbox_chan *chan,
					بنية rpmi_hsm_suspend *susp)
{
	صحيح rc, i, cnt = 0;
	بنية rpmi_hsm_get_susp_types_req req;
	بنية rpmi_hsm_get_susp_types_resp resp;
	بنية rpmi_hsm_get_susp_info_req dreq;
	بنية rpmi_hsm_get_susp_info_resp dresp;
	بنية sbi_cpu_idle_state *state;

	إذا (!susp->انگ_حالتون)
		أرجع 0;

	req.شروعات_اشاريو = 0;
	افعل {
		rc = rpmi_عام_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
		إذا (rc)
			أرجع rc;

		لكل (i = 0; i < resp.returned && cnt < susp->انگ_حالتون; i++)
			susp->حالتون[cnt++].suspend_param = resp.types[i];
		req.شروعات_اشاريو = i;
	} طالما (resp.بچيل);

	لكل (i = 0; i < susp->انگ_حالتون; i++) {
		state = &susp->حالتون[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_عام_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_ڳڻپ(dreq), rpmi_u32_ڳڻپ(dreq),
			&dresp, rpmi_u32_ڳڻپ(dresp), rpmi_u32_ڳڻپ(dresp));
		إذا (rc)
			أرجع rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->داخلا_latency_us = dresp.داخلا_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	أرجع 0;
}

ساكن صحيح rpmi_hsm_update_hart_scratch(بنية mbox_chan *chan,
					بنية rpmi_hsm_suspend *susp)
{
	صحيح rc, i;
	بنية rpmi_hsm_get_hart_list_req req;
	بنية rpmi_hsm_get_hart_list_resp resp;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارو(current_hartid());

	req.شروعات_اشاريو = 0;
	افعل {
		rc = rpmi_عام_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_ڳڻپ(req), rpmi_u32_ڳڻپ(req),
			&resp, rpmi_u32_ڳڻپ(resp), rpmi_u32_ڳڻپ(resp));
		إذا (rc)
			أرجع rc;

		لكل (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_اشارو(resp.hartid[i]);
			إذا (!rpmi)
				أرجع SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.شروعات_اشاريو += resp.returned;
	} طالما (resp.بچيل);

	أرجع 0;
}

ساكن صحيح rpmi_hsm_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc, i;
	بنية mbox_chan *chan;
	بنية rpmi_hsm_suspend *susp;

	إذا (!rpmi_hsm_وٿي) {
		rpmi_hsm_وٿي =
			sbi_scratch_alloc_type_وٿي(بنية rpmi_hsm);
		إذا (!rpmi_hsm_وٿي)
			أرجع SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	إذا (rc)
		أرجع SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(حجم(*susp));
	إذا (!susp)
		أرجع SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_انگ_suspend_حالتون(chan, susp);
	إذا (rc)
		اذهب fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	إذا (!susp->انگ_حالتون)
		اذهب skip_suspend_حالتون;

	/* Allocate array of HART suspend states */
	susp->حالتون = sbi_calloc(susp->انگ_حالتون + 1, حجم(*susp->حالتون));
	إذا (!susp->حالتون) {
		rc = SBI_ENOMEM;
		اذهب fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	لكل (i = 0; i < susp->انگ_حالتون; i++) {
		susp->حالتون[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		إذا (!susp->حالتون[i].name) {
			افعل {
				i--;
				sbi_free((فراغ *)susp->حالتون[i].name);
			} طالما (i > 0);

			rc = SBI_ENOMEM;
			اذهب fail_free_susp_حالتون;
		}
		sbi_snprintf((حرف *)susp->حالتون[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_حالتون(chan, susp);
	إذا (rc)
		اذهب fail_free_susp_state_نالا;

skip_suspend_حالتون:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	إذا (rc)
		اذهب fail_free_susp_state_نالا;

	/* Register HSM fixup callback */
	rc = fdt_رجسٽر_general_fixup(&rpmi_hsm_fixup);
	إذا (rc && rc != SBI_EALREADY)
		اذهب fail_free_susp_state_نالا;

	/* Register HSM device */
	إذا (!susp->انگ_حالتون)
		sbi_hsm_rpmi.hart_suspend = عدم;
	sbi_hsm_سيٽ_device(&sbi_hsm_rpmi);

	أرجع 0;

fail_free_susp_state_نالا:
	لكل (i = 0; i < susp->انگ_حالتون; i++)
		sbi_free((فراغ *)susp->حالتون[i].name);
fail_free_susp_حالتون:
	إذا (susp->انگ_حالتون)
		sbi_free(susp->حالتون);
fail_free_susp:
	sbi_free(susp);
	أرجع rc;
}

ساكن ثابت بنية fdt_ميلاپ rpmi_hsm_ميلاپ[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ثابت بنية fdt_driver fdt_hsm_rpmi = {
	.ميلاپ_جدول = rpmi_hsm_ميلاپ,
	.init = rpmi_hsm_cold_init,
};
