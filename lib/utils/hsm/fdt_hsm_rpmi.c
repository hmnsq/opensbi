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

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

ساختار rpmi_hsm_suspend {
	u32 رقم_states;
	ساختار sbi_cpu_idle_state *states;
};

ساختار rpmi_hsm {
	ساختار mbox_chan *chan;
	ساختار rpmi_hsm_suspend *susp;
};

ایستا unsigned long rpmi_hsm_offset;

ایستا ساختار rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	ساختار sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	اگر (!scratch || !rpmi_hsm_offset)
		بازگشت پوچ;

	بازگشت sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

ایستا صحیح rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	ساختار rpmi_hsm_hart_start_req req;
	ساختار rpmi_hsm_hart_start_resp resp;
	ساختار rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	اگر (!rpmi)
		بازگشت SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	بازگشت rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
}

ایستا صحیح rpmi_hsm_stop(تهی)
{
	صحیح rc;
	ساختار rpmi_hsm_hart_stop_req req;
	ساختار rpmi_hsm_hart_stop_resp resp;
	تهی (*jump_warmboot)(تهی) =
		(تهی (*)(تهی))sbi_scratch_thishart_ptr()->warmboot_addr;
	ساختار rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	اگر (!rpmi)
		بازگشت SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
	اگر (rc)
		بازگشت rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	بازگشت 0;
}

ایستا bool is_rpmi_hsm_susp_supported(ساختار rpmi_hsm_suspend *susp, u32 type)
{
	صحیح i;

	برای (i = 0; i < susp->رقم_states; i++)
		اگر (type == susp->states[i].suspend_param)
			بازگشت true;

	بازگشت false;
}

ایستا صحیح rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	صحیح rc;
	ساختار rpmi_hsm_hart_susp_req req;
	ساختار rpmi_hsm_hart_susp_resp resp;
	ساختار rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	اگر (!rpmi)
		بازگشت SBI_ENOSYS;

	/* check if harts support this suspend type */
	اگر (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		بازگشت SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
	اگر (rc)
		بازگشت rc;

	/* Wait for interrupt */
	wfi();

	بازگشت 0;
}

ایستا ساختار sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ایستا تهی rpmi_hsm_do_fixup(ساختار fdt_general_fixup *f, تهی *fdt)
{
	ساختار rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	اگر (!rpmi || !rpmi->susp || !rpmi->susp->رقم_states)
		بازگشت;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

ایستا ساختار fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ایستا صحیح rpmi_hsm_get_رقم_suspend_states(ساختار mbox_chan *chan,
					   ساختار rpmi_hsm_suspend *susp)
{
	صحیح rc;
	ساختار rpmi_hsm_get_susp_types_req req;
	ساختار rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
	اگر (rc)
		بازگشت rc;

	susp->رقم_states = resp.returned + resp.remaining;
	بازگشت 0;
}

ایستا صحیح rpmi_hsm_get_suspend_states(ساختار mbox_chan *chan,
					ساختار rpmi_hsm_suspend *susp)
{
	صحیح rc, i, cnt = 0;
	ساختار rpmi_hsm_get_susp_types_req req;
	ساختار rpmi_hsm_get_susp_types_resp resp;
	ساختار rpmi_hsm_get_susp_info_req dreq;
	ساختار rpmi_hsm_get_susp_info_resp dresp;
	ساختار sbi_cpu_idle_state *state;

	اگر (!susp->رقم_states)
		بازگشت 0;

	req.start_index = 0;
	انجامبده {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
		اگر (rc)
			بازگشت rc;

		برای (i = 0; i < resp.returned && cnt < susp->رقم_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} وقتی (resp.remaining);

	برای (i = 0; i < susp->رقم_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_شمار(dreq), rpmi_u32_شمار(dreq),
			&dresp, rpmi_u32_شمار(dresp), rpmi_u32_شمار(dresp));
		اگر (rc)
			بازگشت rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	بازگشت 0;
}

ایستا صحیح rpmi_hsm_update_hart_scratch(ساختار mbox_chan *chan,
					ساختار rpmi_hsm_suspend *susp)
{
	صحیح rc, i;
	ساختار rpmi_hsm_get_hart_list_req req;
	ساختار rpmi_hsm_get_hart_list_resp resp;
	ساختار rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	انجامبده {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_شمار(req), rpmi_u32_شمار(req),
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
		اگر (rc)
			بازگشت rc;

		برای (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			اگر (!rpmi)
				بازگشت SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} وقتی (resp.remaining);

	بازگشت 0;
}

ایستا صحیح rpmi_hsm_cold_init(ثابت تهی *fdt, صحیح nodeoff,
			      ثابت ساختار fdt_match *match)
{
	صحیح rc, i;
	ساختار mbox_chan *chan;
	ساختار rpmi_hsm_suspend *susp;

	اگر (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(ساختار rpmi_hsm);
		اگر (!rpmi_hsm_offset)
			بازگشت SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	اگر (rc)
		بازگشت SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(اندازه(*susp));
	اگر (!susp)
		بازگشت SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_رقم_suspend_states(chan, susp);
	اگر (rc)
		برو fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	اگر (!susp->رقم_states)
		برو skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->رقم_states + 1, اندازه(*susp->states));
	اگر (!susp->states) {
		rc = SBI_ENOMEM;
		برو fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	برای (i = 0; i < susp->رقم_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		اگر (!susp->states[i].name) {
			انجامبده {
				i--;
				sbi_free((تهی *)susp->states[i].name);
			} وقتی (i > 0);

			rc = SBI_ENOMEM;
			برو fail_free_susp_states;
		}
		sbi_snprintf((نویسه *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	اگر (rc)
		برو fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	اگر (rc)
		برو fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	اگر (rc && rc != SBI_EALREADY)
		برو fail_free_susp_state_names;

	/* Register HSM device */
	اگر (!susp->رقم_states)
		sbi_hsm_rpmi.hart_suspend = پوچ;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	بازگشت 0;

fail_free_susp_state_names:
	برای (i = 0; i < susp->رقم_states; i++)
		sbi_free((تهی *)susp->states[i].name);
fail_free_susp_states:
	اگر (susp->رقم_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	بازگشت rc;
}

ایستا ثابت ساختار fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ثابت ساختار fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
