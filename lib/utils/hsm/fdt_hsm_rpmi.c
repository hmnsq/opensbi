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
	ပေႃး (!scratch || !rpmi_hsm_offset)
		ပွၵ်ႈ NULL;

	ပွၵ်ႈ sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

static puur_naa_nka rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	sa_nracanaa rpmi_hsm_hart_start_req req;
	sa_nracanaa rpmi_hsm_hart_start_resp resp;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	ပေႃး (!rpmi)
		ပွၵ်ႈ SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	ပွၵ်ႈ rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
}

static puur_naa_nka rpmi_hsm_stop(void)
{
	puur_naa_nka rc;
	sa_nracanaa rpmi_hsm_hart_stop_req req;
	sa_nracanaa rpmi_hsm_hart_stop_resp resp;
	void (*jump_warmboot)(void) =
		(void (*)(void))sbi_scratch_thishart_ptr()->warmboot_addr;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ပေႃး (!rpmi)
		ပွၵ်ႈ SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ပေႃး (rc)
		ပွၵ်ႈ rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ပွၵ်ႈ 0;
}

static bool is_rpmi_hsm_susp_supported(sa_nracanaa rpmi_hsm_suspend *susp, u32 type)
{
	puur_naa_nka i;

	for (i = 0; i < susp->num_states; i++)
		ပေႃး (type == susp->states[i].suspend_param)
			ပွၵ်ႈ true;

	ပွၵ်ႈ false;
}

static puur_naa_nka rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	puur_naa_nka rc;
	sa_nracanaa rpmi_hsm_hart_susp_req req;
	sa_nracanaa rpmi_hsm_hart_susp_resp resp;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ပေႃး (!rpmi)
		ပွၵ်ႈ SBI_ENOSYS;

	/* check if harts support this suspend type */
	ပေႃး (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ပွၵ်ႈ SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ပေႃး (rc)
		ပွၵ်ႈ rc;

	/* Wait for interrupt */
	wfi();

	ပွၵ်ႈ 0;
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

	ပေႃး (!rpmi || !rpmi->susp || !rpmi->susp->num_states)
		ပွၵ်ႈ;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

static sa_nracanaa fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

static puur_naa_nka rpmi_hsm_get_num_suspend_states(sa_nracanaa mbox_chan *chan,
					   sa_nracanaa rpmi_hsm_suspend *susp)
{
	puur_naa_nka rc;
	sa_nracanaa rpmi_hsm_get_susp_types_req req;
	sa_nracanaa rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ပေႃး (rc)
		ပွၵ်ႈ rc;

	susp->num_states = resp.returned + resp.remaining;
	ပွၵ်ႈ 0;
}

static puur_naa_nka rpmi_hsm_get_suspend_states(sa_nracanaa mbox_chan *chan,
					sa_nracanaa rpmi_hsm_suspend *susp)
{
	puur_naa_nka rc, i, cnt = 0;
	sa_nracanaa rpmi_hsm_get_susp_types_req req;
	sa_nracanaa rpmi_hsm_get_susp_types_resp resp;
	sa_nracanaa rpmi_hsm_get_susp_info_req dreq;
	sa_nracanaa rpmi_hsm_get_susp_info_resp dresp;
	sa_nracanaa sbi_cpu_idle_state *state;

	ပေႃး (!susp->num_states)
		ပွၵ်ႈ 0;

	req.start_index = 0;
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		ပေႃး (rc)
			ပွၵ်ႈ rc;

		for (i = 0; i < resp.returned && cnt < susp->num_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} ၶၢဝ်း (resp.remaining);

	for (i = 0; i < susp->num_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_count(dreq), rpmi_u32_count(dreq),
			&dresp, rpmi_u32_count(dresp), rpmi_u32_count(dresp));
		ပေႃး (rc)
			ပွၵ်ႈ rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ပွၵ်ႈ 0;
}

static puur_naa_nka rpmi_hsm_update_hart_scratch(sa_nracanaa mbox_chan *chan,
					sa_nracanaa rpmi_hsm_suspend *susp)
{
	puur_naa_nka rc, i;
	sa_nracanaa rpmi_hsm_get_hart_list_req req;
	sa_nracanaa rpmi_hsm_get_hart_list_resp resp;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		ပေႃး (rc)
			ပွၵ်ႈ rc;

		for (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			ပေႃး (!rpmi)
				ပွၵ်ႈ SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} ၶၢဝ်း (resp.remaining);

	ပွၵ်ႈ 0;
}

static puur_naa_nka rpmi_hsm_cold_init(const void *fdt, puur_naa_nka nodeoff,
			      const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc, i;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa rpmi_hsm_suspend *susp;

	ပေႃး (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(sa_nracanaa rpmi_hsm);
		ပေႃး (!rpmi_hsm_offset)
			ပွၵ်ႈ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ပေႃး (rc)
		ပွၵ်ႈ SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(parimaa_na(*susp));
	ပေႃး (!susp)
		ပွၵ်ႈ SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_num_suspend_states(chan, susp);
	ပေႃး (rc)
		goto fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	ပေႃး (!susp->num_states)
		goto skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->num_states + 1, parimaa_na(*susp->states));
	ပေႃး (!susp->states) {
		rc = SBI_ENOMEM;
		goto fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	for (i = 0; i < susp->num_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		ပေႃး (!susp->states[i].name) {
			do {
				i--;
				sbi_free((void *)susp->states[i].name);
			} ၶၢဝ်း (i > 0);

			rc = SBI_ENOMEM;
			goto fail_free_susp_states;
		}
		sbi_snprintf((var_na *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	ပေႃး (rc)
		goto fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	ပေႃး (rc)
		goto fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	ပေႃး (rc && rc != SBI_EALREADY)
		goto fail_free_susp_state_names;

	/* Register HSM device */
	ပေႃး (!susp->num_states)
		sbi_hsm_rpmi.hart_suspend = NULL;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	ပွၵ်ႈ 0;

fail_free_susp_state_names:
	for (i = 0; i < susp->num_states; i++)
		sbi_free((void *)susp->states[i].name);
fail_free_susp_states:
	ပေႃး (susp->num_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	ပွၵ်ႈ rc;
}

static const sa_nracanaa fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

const sa_nracanaa fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
