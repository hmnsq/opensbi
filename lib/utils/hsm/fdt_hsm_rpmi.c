/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_fixup.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/mailbox/fdt_mailbox.h>
#شامل <sbi_utils/mailbox/mailbox.h>
#شامل <sbi_utils/mailbox/rpmi_mailbox.h>

#تعریف_کرو MAX_HSM_SUPSEND_STATE_NAMELEN		16

ساخت rpmi_hsm_suspend {
	u32 عدد_حالتیں;
	ساخت sbi_cpu_idle_state *حالتیں;
};

ساخت rpmi_hsm {
	ساخت mbox_chan *chan;
	ساخت rpmi_hsm_suspend *susp;
};

ساکن غیرعلامتی طویل rpmi_hsm_انحراف;

ساکن ساخت rpmi_hsm *rpmi_hsm_get_اشارہ_گر(u32 hartid)
{
	ساخت sbi_scratch *scratch;

	scratch = sbi_hartid_مقصد_scratch(hartid);
	اگر (!scratch || !rpmi_hsm_انحراف)
		واپس تہی;

	واپس sbi_scratch_انحراف_ptr(scratch, rpmi_hsm_انحراف);
}

ساکن صحیح rpmi_hsm_آغاز(u32 hartid, ulong resume_addr)
{
	ساخت rpmi_hsm_hart_آغاز_req req;
	ساخت rpmi_hsm_hart_آغاز_resp resp;
	ساخت rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(hartid);

	اگر (!rpmi)
		واپس SBI_ENOSYS;

	req.hartid = hartid;
	req.آغاز_addr_lo = resume_addr;
	req.آغاز_addr_hi = (u64)resume_addr  >> 32;

	واپس rpmi_معمول_request_with_حالت(
			rpmi->chan, RPMI_HSM_SRV_HART_آغاز,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
}

ساکن صحیح rpmi_hsm_stop(خالی)
{
	صحیح rc;
	ساخت rpmi_hsm_hart_stop_req req;
	ساخت rpmi_hsm_hart_stop_resp resp;
	خالی (*jump_warmboot)(خالی) =
		(خالی (*)(خالی))sbi_scratch_thishart_ptr()->warmboot_addr;
	ساخت rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(current_hartid());

	اگر (!rpmi)
		واپس SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_معمول_request_with_حالت(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	اگر (rc)
		واپس rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	واپس 0;
}

ساکن bool is_rpmi_hsm_susp_supported(ساخت rpmi_hsm_suspend *susp, u32 type)
{
	صحیح i;

	برائے (i = 0; i < susp->عدد_حالتیں; i++)
		اگر (type == susp->حالتیں[i].suspend_param)
			واپس true;

	واپس false;
}

ساکن صحیح rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	صحیح rc;
	ساخت rpmi_hsm_hart_susp_req req;
	ساخت rpmi_hsm_hart_susp_resp resp;
	ساخت rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(current_hartid());

	اگر (!rpmi)
		واپس SBI_ENOSYS;

	/* check if harts support this suspend type */
	اگر (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		واپس SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_معمول_request_with_حالت(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	اگر (rc)
		واپس rc;

	/* Wait for interrupt */
	wfi();

	واپس 0;
}

ساکن ساخت sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_آغاز	= rpmi_hsm_آغاز,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ساکن خالی rpmi_hsm_do_fixup(ساخت fdt_general_fixup *f, خالی *fdt)
{
	ساخت rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(current_hartid());

	اگر (!rpmi || !rpmi->susp || !rpmi->susp->عدد_حالتیں)
		واپس;

	fdt_جوڑو_cpu_idle_حالتیں(fdt, rpmi->susp->حالتیں);
}

ساکن ساخت fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ساکن صحیح rpmi_hsm_get_عدد_suspend_حالتیں(ساخت mbox_chan *chan,
					   ساخت rpmi_hsm_suspend *susp)
{
	صحیح rc;
	ساخت rpmi_hsm_get_susp_types_req req;
	ساخت rpmi_hsm_get_susp_types_resp resp;

	req.آغاز_اشاریہ = 0;
	rc = rpmi_معمول_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	اگر (rc)
		واپس rc;

	susp->عدد_حالتیں = resp.returned + resp.بقایا;
	واپس 0;
}

ساکن صحیح rpmi_hsm_get_suspend_حالتیں(ساخت mbox_chan *chan,
					ساخت rpmi_hsm_suspend *susp)
{
	صحیح rc, i, cnt = 0;
	ساخت rpmi_hsm_get_susp_types_req req;
	ساخت rpmi_hsm_get_susp_types_resp resp;
	ساخت rpmi_hsm_get_susp_info_req dreq;
	ساخت rpmi_hsm_get_susp_info_resp dresp;
	ساخت sbi_cpu_idle_state *state;

	اگر (!susp->عدد_حالتیں)
		واپس 0;

	req.آغاز_اشاریہ = 0;
	کرو {
		rc = rpmi_معمول_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
		اگر (rc)
			واپس rc;

		برائے (i = 0; i < resp.returned && cnt < susp->عدد_حالتیں; i++)
			susp->حالتیں[cnt++].suspend_param = resp.types[i];
		req.آغاز_اشاریہ = i;
	} جب (resp.بقایا);

	برائے (i = 0; i < susp->عدد_حالتیں; i++) {
		state = &susp->حالتیں[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_معمول_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_گنتی(dreq), rpmi_u32_گنتی(dreq),
			&dresp, rpmi_u32_گنتی(dresp), rpmi_u32_گنتی(dresp));
		اگر (rc)
			واپس rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->اندراج_latency_us = dresp.اندراج_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	واپس 0;
}

ساکن صحیح rpmi_hsm_update_hart_scratch(ساخت mbox_chan *chan,
					ساخت rpmi_hsm_suspend *susp)
{
	صحیح rc, i;
	ساخت rpmi_hsm_get_hart_list_req req;
	ساخت rpmi_hsm_get_hart_list_resp resp;
	ساخت rpmi_hsm *rpmi = rpmi_hsm_get_اشارہ_گر(current_hartid());

	req.آغاز_اشاریہ = 0;
	کرو {
		rc = rpmi_معمول_request_with_حالت(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_گنتی(req), rpmi_u32_گنتی(req),
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
		اگر (rc)
			واپس rc;

		برائے (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_اشارہ_گر(resp.hartid[i]);
			اگر (!rpmi)
				واپس SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.آغاز_اشاریہ += resp.returned;
	} جب (resp.بقایا);

	واپس 0;
}

ساکن صحیح rpmi_hsm_cold_init(ثابت خالی *fdt, صحیح nodeoff,
			      ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc, i;
	ساخت mbox_chan *chan;
	ساخت rpmi_hsm_suspend *susp;

	اگر (!rpmi_hsm_انحراف) {
		rpmi_hsm_انحراف =
			sbi_scratch_alloc_type_انحراف(ساخت rpmi_hsm);
		اگر (!rpmi_hsm_انحراف)
			واپس SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	اگر (rc)
		واپس SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(حجم(*susp));
	اگر (!susp)
		واپس SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_عدد_suspend_حالتیں(chan, susp);
	اگر (rc)
		جاؤ fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	اگر (!susp->عدد_حالتیں)
		جاؤ skip_suspend_حالتیں;

	/* Allocate array of HART suspend states */
	susp->حالتیں = sbi_calloc(susp->عدد_حالتیں + 1, حجم(*susp->حالتیں));
	اگر (!susp->حالتیں) {
		rc = SBI_ENOMEM;
		جاؤ fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	برائے (i = 0; i < susp->عدد_حالتیں; i++) {
		susp->حالتیں[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		اگر (!susp->حالتیں[i].name) {
			کرو {
				i--;
				sbi_free((خالی *)susp->حالتیں[i].name);
			} جب (i > 0);

			rc = SBI_ENOMEM;
			جاؤ fail_free_susp_حالتیں;
		}
		sbi_snprintf((حرف *)susp->حالتیں[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_حالتیں(chan, susp);
	اگر (rc)
		جاؤ fail_free_susp_state_نام;

skip_suspend_حالتیں:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	اگر (rc)
		جاؤ fail_free_susp_state_نام;

	/* Register HSM fixup callback */
	rc = fdt_رجسٹر_general_fixup(&rpmi_hsm_fixup);
	اگر (rc && rc != SBI_EALREADY)
		جاؤ fail_free_susp_state_نام;

	/* Register HSM device */
	اگر (!susp->عدد_حالتیں)
		sbi_hsm_rpmi.hart_suspend = تہی;
	sbi_hsm_سیٹ_device(&sbi_hsm_rpmi);

	واپس 0;

fail_free_susp_state_نام:
	برائے (i = 0; i < susp->عدد_حالتیں; i++)
		sbi_free((خالی *)susp->حالتیں[i].name);
fail_free_susp_حالتیں:
	اگر (susp->عدد_حالتیں)
		sbi_free(susp->حالتیں);
fail_free_susp:
	sbi_free(susp);
	واپس rc;
}

ساکن ثابت ساخت fdt_مطابقت rpmi_hsm_مطابقت[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ثابت ساخت fdt_driver fdt_hsm_rpmi = {
	.مطابقت_جدول = rpmi_hsm_مطابقت,
	.init = rpmi_hsm_cold_init,
};
