/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_fixup.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/mailbox/fdt_mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

संरचना rpmi_hsm_suspend {
	u32 अङ्क_states;
	संरचना sbi_cpu_idle_state *states;
};

संरचना rpmi_hsm {
	संरचना mbox_chan *chan;
	संरचना rpmi_hsm_suspend *susp;
};

स्थिर अचिन्हित दीर्घ rpmi_hsm_offset;

स्थिर संरचना rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	यदि (!scratch || !rpmi_hsm_offset)
		फर्काऊ रिक्त;

	फर्काऊ sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

स्थिर पूर्णाङ्क rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	संरचना rpmi_hsm_hart_start_req req;
	संरचना rpmi_hsm_hart_start_resp resp;
	संरचना rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	यदि (!rpmi)
		फर्काऊ SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	फर्काऊ rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_गन्ती(req), rpmi_u32_गन्ती(req),
			&resp, rpmi_u32_गन्ती(resp), rpmi_u32_गन्ती(resp));
}

स्थिर पूर्णाङ्क rpmi_hsm_stop(शून्य)
{
	पूर्णाङ्क rc;
	संरचना rpmi_hsm_hart_stop_req req;
	संरचना rpmi_hsm_hart_stop_resp resp;
	शून्य (*jump_warmboot)(शून्य) =
		(शून्य (*)(शून्य))sbi_scratch_thishart_ptr()->warmboot_addr;
	संरचना rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	यदि (!rpmi)
		फर्काऊ SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_गन्ती(req), rpmi_u32_गन्ती(req),
			&resp, rpmi_u32_गन्ती(resp), rpmi_u32_गन्ती(resp));
	यदि (rc)
		फर्काऊ rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	फर्काऊ 0;
}

स्थिर bool is_rpmi_hsm_susp_supported(संरचना rpmi_hsm_suspend *susp, u32 type)
{
	पूर्णाङ्क i;

	लागि (i = 0; i < susp->अङ्क_states; i++)
		यदि (type == susp->states[i].suspend_param)
			फर्काऊ true;

	फर्काऊ false;
}

स्थिर पूर्णाङ्क rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	पूर्णाङ्क rc;
	संरचना rpmi_hsm_hart_susp_req req;
	संरचना rpmi_hsm_hart_susp_resp resp;
	संरचना rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	यदि (!rpmi)
		फर्काऊ SBI_ENOSYS;

	/* check if harts support this suspend type */
	यदि (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		फर्काऊ SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_गन्ती(req), rpmi_u32_गन्ती(req),
			&resp, rpmi_u32_गन्ती(resp), rpmi_u32_गन्ती(resp));
	यदि (rc)
		फर्काऊ rc;

	/* Wait for interrupt */
	wfi();

	फर्काऊ 0;
}

स्थिर संरचना sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

स्थिर शून्य rpmi_hsm_do_fixup(संरचना fdt_general_fixup *f, शून्य *fdt)
{
	संरचना rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	यदि (!rpmi || !rpmi->susp || !rpmi->susp->अङ्क_states)
		फर्काऊ;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

स्थिर संरचना fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

स्थिर पूर्णाङ्क rpmi_hsm_get_अङ्क_suspend_states(संरचना mbox_chan *chan,
					   संरचना rpmi_hsm_suspend *susp)
{
	पूर्णाङ्क rc;
	संरचना rpmi_hsm_get_susp_types_req req;
	संरचना rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_गन्ती(req), rpmi_u32_गन्ती(req),
			&resp, rpmi_u32_गन्ती(resp), rpmi_u32_गन्ती(resp));
	यदि (rc)
		फर्काऊ rc;

	susp->अङ्क_states = resp.returned + resp.remaining;
	फर्काऊ 0;
}

स्थिर पूर्णाङ्क rpmi_hsm_get_suspend_states(संरचना mbox_chan *chan,
					संरचना rpmi_hsm_suspend *susp)
{
	पूर्णाङ्क rc, i, cnt = 0;
	संरचना rpmi_hsm_get_susp_types_req req;
	संरचना rpmi_hsm_get_susp_types_resp resp;
	संरचना rpmi_hsm_get_susp_info_req dreq;
	संरचना rpmi_hsm_get_susp_info_resp dresp;
	संरचना sbi_cpu_idle_state *state;

	यदि (!susp->अङ्क_states)
		फर्काऊ 0;

	req.start_index = 0;
	गर {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_गन्ती(req), rpmi_u32_गन्ती(req),
			&resp, rpmi_u32_गन्ती(resp), rpmi_u32_गन्ती(resp));
		यदि (rc)
			फर्काऊ rc;

		लागि (i = 0; i < resp.returned && cnt < susp->अङ्क_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} जबसम्म (resp.remaining);

	लागि (i = 0; i < susp->अङ्क_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_गन्ती(dreq), rpmi_u32_गन्ती(dreq),
			&dresp, rpmi_u32_गन्ती(dresp), rpmi_u32_गन्ती(dresp));
		यदि (rc)
			फर्काऊ rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क rpmi_hsm_update_hart_scratch(संरचना mbox_chan *chan,
					संरचना rpmi_hsm_suspend *susp)
{
	पूर्णाङ्क rc, i;
	संरचना rpmi_hsm_get_hart_list_req req;
	संरचना rpmi_hsm_get_hart_list_resp resp;
	संरचना rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	गर {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_गन्ती(req), rpmi_u32_गन्ती(req),
			&resp, rpmi_u32_गन्ती(resp), rpmi_u32_गन्ती(resp));
		यदि (rc)
			फर्काऊ rc;

		लागि (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			यदि (!rpmi)
				फर्काऊ SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} जबसम्म (resp.remaining);

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क rpmi_hsm_cold_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			      अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc, i;
	संरचना mbox_chan *chan;
	संरचना rpmi_hsm_suspend *susp;

	यदि (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(संरचना rpmi_hsm);
		यदि (!rpmi_hsm_offset)
			फर्काऊ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	यदि (rc)
		फर्काऊ SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(परिमाण(*susp));
	यदि (!susp)
		फर्काऊ SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_अङ्क_suspend_states(chan, susp);
	यदि (rc)
		गच्छ fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	यदि (!susp->अङ्क_states)
		गच्छ skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->अङ्क_states + 1, परिमाण(*susp->states));
	यदि (!susp->states) {
		rc = SBI_ENOMEM;
		गच्छ fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	लागि (i = 0; i < susp->अङ्क_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		यदि (!susp->states[i].name) {
			गर {
				i--;
				sbi_free((शून्य *)susp->states[i].name);
			} जबसम्म (i > 0);

			rc = SBI_ENOMEM;
			गच्छ fail_free_susp_states;
		}
		sbi_snprintf((वर्ण *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	यदि (rc)
		गच्छ fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	यदि (rc)
		गच्छ fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	यदि (rc && rc != SBI_EALREADY)
		गच्छ fail_free_susp_state_names;

	/* Register HSM device */
	यदि (!susp->अङ्क_states)
		sbi_hsm_rpmi.hart_suspend = रिक्त;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	फर्काऊ 0;

fail_free_susp_state_names:
	लागि (i = 0; i < susp->अङ्क_states; i++)
		sbi_free((शून्य *)susp->states[i].name);
fail_free_susp_states:
	यदि (susp->अङ्क_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	फर्काऊ rc;
}

स्थिर अचर संरचना fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

अचर संरचना fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
