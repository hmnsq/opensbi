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

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

بنية rpmi_hsm_suspend {
	u32 لمب_states;
	بنية sbi_cpu_idle_state *states;
};

بنية rpmi_hsm {
	بنية mbox_chan *chan;
	بنية rpmi_hsm_suspend *susp;
};

ساكن unsigned long rpmi_hsm_offset;

ساكن بنية rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	بنية sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	إذا (!scratch || !rpmi_hsm_offset)
		أرجع عدم;

	أرجع sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

ساكن صحيح rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	بنية rpmi_hsm_hart_start_req req;
	بنية rpmi_hsm_hart_start_resp resp;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	إذا (!rpmi)
		أرجع SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	أرجع rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_كدي(req), rpmi_u32_كدي(req),
			&resp, rpmi_u32_كدي(resp), rpmi_u32_كدي(resp));
}

ساكن صحيح rpmi_hsm_stop(فراغ)
{
	صحيح rc;
	بنية rpmi_hsm_hart_stop_req req;
	بنية rpmi_hsm_hart_stop_resp resp;
	فراغ (*jump_warmboot)(فراغ) =
		(فراغ (*)(فراغ))sbi_scratch_thishart_ptr()->warmboot_addr;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	إذا (!rpmi)
		أرجع SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_كدي(req), rpmi_u32_كدي(req),
			&resp, rpmi_u32_كدي(resp), rpmi_u32_كدي(resp));
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

	لكل (i = 0; i < susp->لمب_states; i++)
		إذا (type == susp->states[i].suspend_param)
			أرجع true;

	أرجع false;
}

ساكن صحيح rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	صحيح rc;
	بنية rpmi_hsm_hart_susp_req req;
	بنية rpmi_hsm_hart_susp_resp resp;
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	إذا (!rpmi)
		أرجع SBI_ENOSYS;

	/* check if harts support this suspend type */
	إذا (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		أرجع SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_كدي(req), rpmi_u32_كدي(req),
			&resp, rpmi_u32_كدي(resp), rpmi_u32_كدي(resp));
	إذا (rc)
		أرجع rc;

	/* Wait for interrupt */
	wfi();

	أرجع 0;
}

ساكن بنية sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

ساكن فراغ rpmi_hsm_do_fixup(بنية fdt_general_fixup *f, فراغ *fdt)
{
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	إذا (!rpmi || !rpmi->susp || !rpmi->susp->لمب_states)
		أرجع;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

ساكن بنية fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

ساكن صحيح rpmi_hsm_get_لمب_suspend_states(بنية mbox_chan *chan,
					   بنية rpmi_hsm_suspend *susp)
{
	صحيح rc;
	بنية rpmi_hsm_get_susp_types_req req;
	بنية rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_كدي(req), rpmi_u32_كدي(req),
			&resp, rpmi_u32_كدي(resp), rpmi_u32_كدي(resp));
	إذا (rc)
		أرجع rc;

	susp->لمب_states = resp.returned + resp.remaining;
	أرجع 0;
}

ساكن صحيح rpmi_hsm_get_suspend_states(بنية mbox_chan *chan,
					بنية rpmi_hsm_suspend *susp)
{
	صحيح rc, i, cnt = 0;
	بنية rpmi_hsm_get_susp_types_req req;
	بنية rpmi_hsm_get_susp_types_resp resp;
	بنية rpmi_hsm_get_susp_info_req dreq;
	بنية rpmi_hsm_get_susp_info_resp dresp;
	بنية sbi_cpu_idle_state *state;

	إذا (!susp->لمب_states)
		أرجع 0;

	req.start_index = 0;
	افعل {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_كدي(req), rpmi_u32_كدي(req),
			&resp, rpmi_u32_كدي(resp), rpmi_u32_كدي(resp));
		إذا (rc)
			أرجع rc;

		لكل (i = 0; i < resp.returned && cnt < susp->لمب_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} طالما (resp.remaining);

	لكل (i = 0; i < susp->لمب_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_كدي(dreq), rpmi_u32_كدي(dreq),
			&dresp, rpmi_u32_كدي(dresp), rpmi_u32_كدي(dresp));
		إذا (rc)
			أرجع rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
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
	بنية rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	افعل {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_كدي(req), rpmi_u32_كدي(req),
			&resp, rpmi_u32_كدي(resp), rpmi_u32_كدي(resp));
		إذا (rc)
			أرجع rc;

		لكل (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			إذا (!rpmi)
				أرجع SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} طالما (resp.remaining);

	أرجع 0;
}

ساكن صحيح rpmi_hsm_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_match *match)
{
	صحيح rc, i;
	بنية mbox_chan *chan;
	بنية rpmi_hsm_suspend *susp;

	إذا (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(بنية rpmi_hsm);
		إذا (!rpmi_hsm_offset)
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
	rc = rpmi_hsm_get_لمب_suspend_states(chan, susp);
	إذا (rc)
		اذهب fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	إذا (!susp->لمب_states)
		اذهب skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->لمب_states + 1, حجم(*susp->states));
	إذا (!susp->states) {
		rc = SBI_ENOMEM;
		اذهب fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	لكل (i = 0; i < susp->لمب_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		إذا (!susp->states[i].name) {
			افعل {
				i--;
				sbi_free((فراغ *)susp->states[i].name);
			} طالما (i > 0);

			rc = SBI_ENOMEM;
			اذهب fail_free_susp_states;
		}
		sbi_snprintf((حرف *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	إذا (rc)
		اذهب fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	إذا (rc)
		اذهب fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	إذا (rc && rc != SBI_EALREADY)
		اذهب fail_free_susp_state_names;

	/* Register HSM device */
	إذا (!susp->لمب_states)
		sbi_hsm_rpmi.hart_suspend = عدم;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	أرجع 0;

fail_free_susp_state_names:
	لكل (i = 0; i < susp->لمب_states; i++)
		sbi_free((فراغ *)susp->states[i].name);
fail_free_susp_states:
	إذا (susp->لمب_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	أرجع rc;
}

ساكن ثابت بنية fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

ثابت بنية fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
