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
	ᨀᨘ (!scratch || !rpmi_hsm_offset)
		ᨀᨘᨒ NULL;

	ᨀᨘᨒ sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

static puur_naa_nka rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	sa_nracanaa rpmi_hsm_hart_start_req req;
	sa_nracanaa rpmi_hsm_hart_start_resp resp;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	ᨀᨘ (!rpmi)
		ᨀᨘᨒ SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	ᨀᨘᨒ rpmi_normal_request_with_status(
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

	ᨀᨘ (!rpmi)
		ᨀᨘᨒ SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ᨀᨘ (rc)
		ᨀᨘᨒ rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	ᨀᨘᨒ 0;
}

static bool is_rpmi_hsm_susp_supported(sa_nracanaa rpmi_hsm_suspend *susp, u32 type)
{
	puur_naa_nka i;

	for (i = 0; i < susp->num_states; i++)
		ᨀᨘ (type == susp->states[i].suspend_param)
			ᨀᨘᨒ true;

	ᨀᨘᨒ false;
}

static puur_naa_nka rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	puur_naa_nka rc;
	sa_nracanaa rpmi_hsm_hart_susp_req req;
	sa_nracanaa rpmi_hsm_hart_susp_resp resp;
	sa_nracanaa rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	ᨀᨘ (!rpmi)
		ᨀᨘᨒ SBI_ENOSYS;

	/* check if harts support this suspend type */
	ᨀᨘ (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		ᨀᨘᨒ SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	ᨀᨘ (rc)
		ᨀᨘᨒ rc;

	/* Wait for interrupt */
	wfi();

	ᨀᨘᨒ 0;
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

	ᨀᨘ (!rpmi || !rpmi->susp || !rpmi->susp->num_states)
		ᨀᨘᨒ;

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
	ᨀᨘ (rc)
		ᨀᨘᨒ rc;

	susp->num_states = resp.returned + resp.remaining;
	ᨀᨘᨒ 0;
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

	ᨀᨘ (!susp->num_states)
		ᨀᨘᨒ 0;

	req.start_index = 0;
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		ᨀᨘ (rc)
			ᨀᨘᨒ rc;

		for (i = 0; i < resp.returned && cnt < susp->num_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} ᨀᨛ (resp.remaining);

	for (i = 0; i < susp->num_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_count(dreq), rpmi_u32_count(dreq),
			&dresp, rpmi_u32_count(dresp), rpmi_u32_count(dresp));
		ᨀᨘ (rc)
			ᨀᨘᨒ rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	ᨀᨘᨒ 0;
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
		ᨀᨘ (rc)
			ᨀᨘᨒ rc;

		for (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			ᨀᨘ (!rpmi)
				ᨀᨘᨒ SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} ᨀᨛ (resp.remaining);

	ᨀᨘᨒ 0;
}

static puur_naa_nka rpmi_hsm_cold_init(const void *fdt, puur_naa_nka nodeoff,
			      const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc, i;
	sa_nracanaa mbox_chan *chan;
	sa_nracanaa rpmi_hsm_suspend *susp;

	ᨀᨘ (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(sa_nracanaa rpmi_hsm);
		ᨀᨘ (!rpmi_hsm_offset)
			ᨀᨘᨒ SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	ᨀᨘ (rc)
		ᨀᨘᨒ SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(parimaa_na(*susp));
	ᨀᨘ (!susp)
		ᨀᨘᨒ SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_num_suspend_states(chan, susp);
	ᨀᨘ (rc)
		goto fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	ᨀᨘ (!susp->num_states)
		goto skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->num_states + 1, parimaa_na(*susp->states));
	ᨀᨘ (!susp->states) {
		rc = SBI_ENOMEM;
		goto fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	for (i = 0; i < susp->num_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		ᨀᨘ (!susp->states[i].name) {
			do {
				i--;
				sbi_free((void *)susp->states[i].name);
			} ᨀᨛ (i > 0);

			rc = SBI_ENOMEM;
			goto fail_free_susp_states;
		}
		sbi_snprintf((var_na *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	ᨀᨘ (rc)
		goto fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	ᨀᨘ (rc)
		goto fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	ᨀᨘ (rc && rc != SBI_EALREADY)
		goto fail_free_susp_state_names;

	/* Register HSM device */
	ᨀᨘ (!susp->num_states)
		sbi_hsm_rpmi.hart_suspend = NULL;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	ᨀᨘᨒ 0;

fail_free_susp_state_names:
	for (i = 0; i < susp->num_states; i++)
		sbi_free((void *)susp->states[i].name);
fail_free_susp_states:
	ᨀᨘ (susp->num_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	ᨀᨘᨒ rc;
}

static const sa_nracanaa fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

const sa_nracanaa fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
