/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_fixup.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/mailbox/fdt_mailbox.h>
#समावेश <sbi_utils/mailbox/mailbox.h>
#समावेश <sbi_utils/mailbox/rpmi_mailbox.h>

#घोषणा MAX_HSM_SUPSEND_STATE_NAMELEN		16

काष्ठा rpmi_hsm_suspend {
	u32 अंक_states;
	काष्ठा sbi_cpu_idle_state *states;
};

काष्ठा rpmi_hsm {
	काष्ठा mbox_chan *chan;
	काष्ठा rpmi_hsm_suspend *susp;
};

अटल अचिन्हित दीर्घ rpmi_hsm_offset;

अटल काष्ठा rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	काष्ठा sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	अगर (!scratch || !rpmi_hsm_offset)
		वापस शून्य;

	वापस sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

अटल पूर्णांक rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	काष्ठा rpmi_hsm_hart_start_req req;
	काष्ठा rpmi_hsm_hart_start_resp resp;
	काष्ठा rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	अगर (!rpmi)
		वापस SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	वापस rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
}

अटल पूर्णांक rpmi_hsm_stop(व्योम)
{
	पूर्णांक rc;
	काष्ठा rpmi_hsm_hart_stop_req req;
	काष्ठा rpmi_hsm_hart_stop_resp resp;
	व्योम (*jump_warmboot)(व्योम) =
		(व्योम (*)(व्योम))sbi_scratch_thishart_ptr()->warmboot_addr;
	काष्ठा rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	अगर (!rpmi)
		वापस SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	अगर (rc)
		वापस rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	वापस 0;
}

अटल bool is_rpmi_hsm_susp_supported(काष्ठा rpmi_hsm_suspend *susp, u32 type)
{
	पूर्णांक i;

	क्रम (i = 0; i < susp->अंक_states; i++)
		अगर (type == susp->states[i].suspend_param)
			वापस true;

	वापस false;
}

अटल पूर्णांक rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	पूर्णांक rc;
	काष्ठा rpmi_hsm_hart_susp_req req;
	काष्ठा rpmi_hsm_hart_susp_resp resp;
	काष्ठा rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	अगर (!rpmi)
		वापस SBI_ENOSYS;

	/* check if harts support this suspend type */
	अगर (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		वापस SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	अगर (rc)
		वापस rc;

	/* Wait for interrupt */
	wfi();

	वापस 0;
}

अटल काष्ठा sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

अटल व्योम rpmi_hsm_do_fixup(काष्ठा fdt_general_fixup *f, व्योम *fdt)
{
	काष्ठा rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	अगर (!rpmi || !rpmi->susp || !rpmi->susp->अंक_states)
		वापस;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

अटल काष्ठा fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

अटल पूर्णांक rpmi_hsm_get_अंक_suspend_states(काष्ठा mbox_chan *chan,
					   काष्ठा rpmi_hsm_suspend *susp)
{
	पूर्णांक rc;
	काष्ठा rpmi_hsm_get_susp_types_req req;
	काष्ठा rpmi_hsm_get_susp_types_resp resp;

	req.start_सूचकांक = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	अगर (rc)
		वापस rc;

	susp->अंक_states = resp.returned + resp.remaining;
	वापस 0;
}

अटल पूर्णांक rpmi_hsm_get_suspend_states(काष्ठा mbox_chan *chan,
					काष्ठा rpmi_hsm_suspend *susp)
{
	पूर्णांक rc, i, cnt = 0;
	काष्ठा rpmi_hsm_get_susp_types_req req;
	काष्ठा rpmi_hsm_get_susp_types_resp resp;
	काष्ठा rpmi_hsm_get_susp_info_req dreq;
	काष्ठा rpmi_hsm_get_susp_info_resp dresp;
	काष्ठा sbi_cpu_idle_state *state;

	अगर (!susp->अंक_states)
		वापस 0;

	req.start_सूचकांक = 0;
	करो {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
		अगर (rc)
			वापस rc;

		क्रम (i = 0; i < resp.returned && cnt < susp->अंक_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_सूचकांक = i;
	} जबतक (resp.remaining);

	क्रम (i = 0; i < susp->अंक_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_गिनती(dreq), rpmi_u32_गिनती(dreq),
			&dresp, rpmi_u32_गिनती(dresp), rpmi_u32_गिनती(dresp));
		अगर (rc)
			वापस rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	वापस 0;
}

अटल पूर्णांक rpmi_hsm_update_hart_scratch(काष्ठा mbox_chan *chan,
					काष्ठा rpmi_hsm_suspend *susp)
{
	पूर्णांक rc, i;
	काष्ठा rpmi_hsm_get_hart_list_req req;
	काष्ठा rpmi_hsm_get_hart_list_resp resp;
	काष्ठा rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_सूचकांक = 0;
	करो {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_गिनती(req), rpmi_u32_गिनती(req),
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
		अगर (rc)
			वापस rc;

		क्रम (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			अगर (!rpmi)
				वापस SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_सूचकांक += resp.returned;
	} जबतक (resp.remaining);

	वापस 0;
}

अटल पूर्णांक rpmi_hsm_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			      स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc, i;
	काष्ठा mbox_chan *chan;
	काष्ठा rpmi_hsm_suspend *susp;

	अगर (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(काष्ठा rpmi_hsm);
		अगर (!rpmi_hsm_offset)
			वापस SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	अगर (rc)
		वापस SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(माप(*susp));
	अगर (!susp)
		वापस SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_अंक_suspend_states(chan, susp);
	अगर (rc)
		जाओ fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	अगर (!susp->अंक_states)
		जाओ skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->अंक_states + 1, माप(*susp->states));
	अगर (!susp->states) {
		rc = SBI_ENOMEM;
		जाओ fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	क्रम (i = 0; i < susp->अंक_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		अगर (!susp->states[i].name) {
			करो {
				i--;
				sbi_free((व्योम *)susp->states[i].name);
			} जबतक (i > 0);

			rc = SBI_ENOMEM;
			जाओ fail_free_susp_states;
		}
		sbi_snprintf((अक्षर *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	अगर (rc)
		जाओ fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	अगर (rc)
		जाओ fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	अगर (rc && rc != SBI_EALREADY)
		जाओ fail_free_susp_state_names;

	/* Register HSM device */
	अगर (!susp->अंक_states)
		sbi_hsm_rpmi.hart_suspend = शून्य;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	वापस 0;

fail_free_susp_state_names:
	क्रम (i = 0; i < susp->अंक_states; i++)
		sbi_free((व्योम *)susp->states[i].name);
fail_free_susp_states:
	अगर (susp->अंक_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	वापस rc;
}

अटल स्थिर काष्ठा fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
