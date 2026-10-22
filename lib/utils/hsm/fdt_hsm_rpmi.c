/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_scratch.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/mailbox/fdt_mailbox.h>
#include <sbi_utils/mailbox/mailbox.h>
#include <sbi_utils/mailbox/rpmi_mailbox.h>

#define MAX_HSM_SUPSEND_STATE_NAMELEN		16

sa_nracanaa rpmi_hsm_suspend {
	u32 num_states;
	sa_nracanaa sbi_cpu_idle_state *states;
};

sa_nracanaa rpmi_hsm {
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa rpmi_hsm_suspend *susp;
};

static unsigned long rpmi_hsm_offset;

static sa_nracanaa rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	sa_nracanaa sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	အကယ်၍ (!scratch || !rpmi_hsm_offset)
		ပြန်ပေး NULL;

	ပြန်ပေး sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

static ကိန်းပြည့် rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	sa_nracanaa rpmi_hsm_hart_start_req req;
	sa_nracanaa rpmi_hsm_hart_start_resp resp;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	အကယ်၍ (!rpmi)
		ပြန်ပေး SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	ပြန်ပေး rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
}

static ကိန်းပြည့် rpmi_hsm_stop(void)
{
	ကိန်းပြည့် rc;
	sa_nracanaa rpmi_hsm_hart_stop_req req;
	sa_nracanaa rpmi_hsm_hart_stop_resp resp;
	void (*jump_warmboot)(void) =
		(void (*)(void))sbi_scratch_thishart_ptr()->warmboot_addr;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	အကယ်၍ (!rpmi)
		ပြန်ပေး SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ပြန်ပေး 0;
}

static bool is_rpmi_hsm_susp_supported(sa_nracanaa rpmi_hsm_suspend *susp, u32 type)
{
	ကိန်းပြည့် i;

	အတွက် (i = 0; i < susp->num_states; i++)
		အကယ်၍ (type == susp->states[i].suspend_param)
			ပြန်ပေး true;

	ပြန်ပေး false;
}

static ကိန်းပြည့် rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	ကိန်းပြည့် rc;
	sa_nracanaa rpmi_hsm_hart_susp_req req;
	sa_nracanaa rpmi_hsm_hart_susp_resp resp;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	အကယ်၍ (!rpmi)
		ပြန်ပေး SBI_ENOSYS;

	/* check if harts support this suspend type */
	အကယ်၍ (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ပြန်ပေး SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	/* Wait for interrupt */
	wfi();

	ပြန်ပေး 0;
}

static sa_nracanaa sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

static void rpmi_hsm_do_fixup(sa_nracanaa fdt_general_fixup *f, void *fdt)
{
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	အကယ်၍ (!rpmi || !rpmi->susp || !rpmi->susp->num_states)
		ပြန်ပေး;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

static sa_nracanaa fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

static ကိန်းပြည့် rpmi_hsm_get_num_suspend_states(sa_nracanaa mbox_chan *chan,
					   sa_nracanaa rpmi_hsm_suspend *susp)
{
	ကိန်းပြည့် rc;
	sa_nracanaa rpmi_hsm_get_susp_types_req req;
	sa_nracanaa rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	susp->num_states = resp.returned + resp.remaining;
	ပြန်ပေး 0;
}

static ကိန်းပြည့် rpmi_hsm_get_suspend_states(sa_nracanaa mbox_chan *chan,
					sa_nracanaa rpmi_hsm_suspend *susp)
{
	ကိန်းပြည့် rc, i, cnt = 0;
	sa_nracanaa rpmi_hsm_get_susp_types_req req;
	sa_nracanaa rpmi_hsm_get_susp_types_resp resp;
	sa_nracanaa rpmi_hsm_get_susp_info_req dreq;
	sa_nracanaa rpmi_hsm_get_susp_info_resp dresp;
	sa_nracanaa sbi_cpu_idle_state *state;

	အကယ်၍ (!susp->num_states)
		ပြန်ပေး 0;

	req.start_index = 0;
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		အတွက် (i = 0; i < resp.returned && cnt < susp->num_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} နေစဉ် (resp.remaining);

	အတွက် (i = 0; i < susp->num_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_count(dreq), rpmi_u32_count(dreq),
			&dresp, rpmi_u32_count(dresp), rpmi_u32_count(dresp));
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ပြန်ပေး 0;
}

static ကိန်းပြည့် rpmi_hsm_update_hart_scratch(sa_nracanaa mbox_chan *chan,
					sa_nracanaa rpmi_hsm_suspend *susp)
{
	ကိန်းပြည့် rc, i;
	sa_nracanaa rpmi_hsm_get_hart_list_req req;
	sa_nracanaa rpmi_hsm_get_hart_list_resp resp;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		အတွက် (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			အကယ်၍ (!rpmi)
				ပြန်ပေး SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} နေစဉ် (resp.remaining);

	ပြန်ပေး 0;
}

static ကိန်းပြည့် rpmi_hsm_cold_init(const void *fdt, ကိန်းပြည့် nodeoff,
			      const sa_nracanaa fdt_match *match)
{
	ကိန်းပြည့် rc, i;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa rpmi_hsm_suspend *susp;

	အကယ်၍ (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(sa_nracanaa rpmi_hsm);
		အကယ်၍ (!rpmi_hsm_offset)
			ပြန်ပေး SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	အကယ်၍ (rc)
		ပြန်ပေး SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(parimaa_na(*susp));
	အကယ်၍ (!susp)
		ပြန်ပေး SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_num_suspend_states(chan, susp);
	အကယ်၍ (rc)
		goto fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	အကယ်၍ (!susp->num_states)
		goto skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->num_states + 1, parimaa_na(*susp->states));
	အကယ်၍ (!susp->states) {
		rc = SBI_ENOMEM;
		goto fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	အတွက် (i = 0; i < susp->num_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		အကယ်၍ (!susp->states[i].name) {
			do {
				i--;
				sbi_free((void *)susp->states[i].name);
			} နေစဉ် (i > 0);

			rc = SBI_ENOMEM;
			goto fail_free_susp_states;
		}
		sbi_snprintf((var_na *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	အကယ်၍ (rc)
		goto fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	အကယ်၍ (rc)
		goto fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	အကယ်၍ (rc && rc != SBI_EALREADY)
		goto fail_free_susp_state_names;

	/* Register HSM device */
	အကယ်၍ (!susp->num_states)
		sbi_hsm_rpmi.hart_suspend = NULL;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	ပြန်ပေး 0;

fail_free_susp_state_names:
	အတွက် (i = 0; i < susp->num_states; i++)
		sbi_free((void *)susp->states[i].name);
fail_free_susp_states:
	အကယ်၍ (susp->num_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	ပြန်ပေး rc;
}

static const sa_nracanaa fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

const sa_nracanaa fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
