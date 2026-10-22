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

#تعریف_کرو MAX_HSM_SUPSEND_STATE_NAMELEN		16

بنية rpmi_hsm_suspend {
	u32 عدد_حالتاں;
	بنية sbi_cpu_idle_state *حالتاں;
};

بنية rpmi_hsm {
	بنية mbox_chan *chan;
	بنية rpmi_hsm_suspend *susp;
};

ساكن بنانشانی لما rpmi_hsm_وتھ;

ساكن بنية rpmi_hsm *rpmi_hsm_get_اشارہ_گر(u32 hartid)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartid_منزل_scratch(hartid);
	إذا (!scratch || !rpmi_hsm_وتھ)
		أرجع عدم;

	أرجع sbi_scratch_وتھ_ptr(scratch, rpmi_hsm_وتھ);
}

ساكن صحيح rpmi_hsm_شروع(u32 hartid, ulong resume_addr)
{
	بنية rpmi_hsm_hart_شروع_req req;
	بنية rpmi_hsm_hart_شروع_resp resp;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(hartid);

	إذا (!rpmi)
		أرجع SBI_ENOSYS;

	req.hartid = hartid;
	req.شروع_addr_lo = resume_addr;
	req.شروع_addr_hi = (u64)resume_addr  >> 32;

	أرجع rpmi_عام_request_with_حالت(
			rpmi->chan, RPMI_HSM_SRV_HART_شروع,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
}

ساكن صحيح rpmi_hsm_stop(فراغ)
{
	صحيح rc;
	بنية rpmi_hsm_hart_stop_req req;
	بنية rpmi_hsm_hart_stop_resp resp;
	فراغ (*jump_warmboot)(فراغ) =
		(فراغ (*)(فراغ))sbi_scratch_thishart_ptr()->warmboot_addr;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(current_hartid());

	إذا (!rpmi)
		أرجع SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_عام_request_with_حالت(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
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

	لكل (i = 0; i < susp->عدد_حالتاں; i++)
		إذا (type == susp->حالتاں[i].suspend_param)
			أرجع true;

	أرجع false;
}

ساكن صحيح rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	صحيح rc;
	بنية rpmi_hsm_hart_susp_req req;
	بنية rpmi_hsm_hart_susp_resp resp;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(current_hartid());

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
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	إذا (rc)
		أرجع rc;

	/* Wait for interrupt */
	wfi();

	أرجع 0;
}

ساكن بنية sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_شروع	= rpmi_hsm_شروع,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ساكن فراغ rpmi_hsm_do_fixup(بنية fdt_general_fixup *f, فراغ *fdt)
{
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(current_hartid());

	إذا (!rpmi || !rpmi->susp || !rpmi->susp->عدد_حالتاں)
		أرجع;

	fdt_جوڑو_cpu_idle_حالتاں(fdt, rpmi->susp->حالتاں);
}

ساكن بنية fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ساكن صحيح rpmi_hsm_get_عدد_suspend_حالتاں(بنية mbox_chan *chan,
					   بنية rpmi_hsm_suspend *susp)
{
	صحيح rc;
	بنية rpmi_hsm_get_susp_types_req req;
	بنية rpmi_hsm_get_susp_types_resp resp;

	req.شروع_اشاریہ = 0;
	rc = rpmi_عام_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	إذا (rc)
		أرجع rc;

	susp->عدد_حالتاں = resp.returned + resp.بچیا;
	أرجع 0;
}

ساكن صحيح rpmi_hsm_get_suspend_حالتاں(بنية mbox_chan *chan,
					بنية rpmi_hsm_suspend *susp)
{
	صحيح rc, i, cnt = 0;
	بنية rpmi_hsm_get_susp_types_req req;
	بنية rpmi_hsm_get_susp_types_resp resp;
	بنية rpmi_hsm_get_susp_info_req dreq;
	بنية rpmi_hsm_get_susp_info_resp dresp;
	بنية sbi_cpu_idle_state *state;

	إذا (!susp->عدد_حالتاں)
		أرجع 0;

	req.شروع_اشاریہ = 0;
	افعل {
		rc = rpmi_عام_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
		إذا (rc)
			أرجع rc;

		لكل (i = 0; i < resp.returned && cnt < susp->عدد_حالتاں; i++)
			susp->حالتاں[cnt++].suspend_param = resp.types[i];
		req.شروع_اشاریہ = i;
	} طالما (resp.بچیا);

	لكل (i = 0; i < susp->عدد_حالتاں; i++) {
		state = &susp->حالتاں[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_عام_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_گنتی(dreq), rpmi_u32_گنتی(dreq),
			&dresp, rpmi_u32_گنتی(dresp), rpmi_u32_گنتی(dresp));
		إذا (rc)
			أرجع rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->اندراج_latency_us = dresp.اندراج_latency_us;
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
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(current_hartid());

	req.شروع_اشاریہ = 0;
	افعل {
		rc = rpmi_عام_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
		إذا (rc)
			أرجع rc;

		لكل (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_اشارہ_گر(resp.hartid[i]);
			إذا (!rpmi)
				أرجع SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.شروع_اشاریہ += resp.returned;
	} طالما (resp.بچیا);

	أرجع 0;
}

ساكن صحيح rpmi_hsm_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_میل *میل)
{
	صحيح rc, i;
	بنية mbox_chan *chan;
	بنية rpmi_hsm_suspend *susp;

	إذا (!rpmi_hsm_وتھ) {
		rpmi_hsm_وتھ =
			sbi_scratch_alloc_type_وتھ(بنية rpmi_hsm);
		إذا (!rpmi_hsm_وتھ)
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
	rc = rpmi_hsm_get_عدد_suspend_حالتاں(chan, susp);
	إذا (rc)
		اذهب fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	إذا (!susp->عدد_حالتاں)
		اذهب skip_suspend_حالتاں;

	/* Allocate array of HART suspend states */
	susp->حالتاں = sbi_calloc(susp->عدد_حالتاں + 1, حجم(*susp->حالتاں));
	إذا (!susp->حالتاں) {
		rc = SBI_ENOMEM;
		اذهب fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	لكل (i = 0; i < susp->عدد_حالتاں; i++) {
		susp->حالتاں[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		إذا (!susp->حالتاں[i].name) {
			افعل {
				i--;
				sbi_free((فراغ *)susp->حالتاں[i].name);
			} طالما (i > 0);

			rc = SBI_ENOMEM;
			اذهب fail_free_susp_حالتاں;
		}
		sbi_snprintf((حرف *)susp->حالتاں[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_حالتاں(chan, susp);
	إذا (rc)
		اذهب fail_free_susp_state_ناں;

skip_suspend_حالتاں:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	إذا (rc)
		اذهب fail_free_susp_state_ناں;

	/* Register HSM fixup callback */
	rc = fdt_رجسٹر_general_fixup(&rpmi_hsm_fixup);
	إذا (rc && rc != SBI_EALREADY)
		اذهب fail_free_susp_state_ناں;

	/* Register HSM device */
	إذا (!susp->عدد_حالتاں)
		sbi_hsm_rpmi.hart_suspend = عدم;
	sbi_hsm_سیٹ_device(&sbi_hsm_rpmi);

	أرجع 0;

fail_free_susp_state_ناں:
	لكل (i = 0; i < susp->عدد_حالتاں; i++)
		sbi_free((فراغ *)susp->حالتاں[i].name);
fail_free_susp_حالتاں:
	إذا (susp->عدد_حالتاں)
		sbi_free(susp->حالتاں);
fail_free_susp:
	sbi_free(susp);
	أرجع rc;
}

ساكن ثابت بنية fdt_میل rpmi_hsm_میل[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ثابت بنية fdt_driver fdt_hsm_rpmi = {
	.میل_جدول = rpmi_hsm_میل,
	.init = rpmi_hsm_cold_init,
};
