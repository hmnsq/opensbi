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

struct rpmi_hsm_suspend {
	u32 num_states;
	struct sbi_cpu_idle_state *states;
};

struct rpmi_hsm {
	struct mbox_chan *chan;
	struct rpmi_hsm_suspend *susp;
};

static unsigned long rpmi_hsm_offset;

static struct rpmi_hsm *rpmi_hsm_get_pointer(u32 hartid)
{
	struct sbi_scratch *scratch;

	scratch = sbi_hartid_to_scratch(hartid);
	якщо (!scratch || !rpmi_hsm_offset)
		повернути NULL;

	повернути sbi_scratch_offset_ptr(scratch, rpmi_hsm_offset);
}

static ціле rpmi_hsm_start(u32 hartid, ulong resume_addr)
{
	struct rpmi_hsm_hart_start_req req;
	struct rpmi_hsm_hart_start_resp resp;
	struct rpmi_hsm *rpmi = rpmi_hsm_get_pointer(hartid);

	якщо (!rpmi)
		повернути SBI_ENOSYS;

	req.hartid = hartid;
	req.start_addr_lo = resume_addr;
	req.start_addr_hi = (u64)resume_addr  >> 32;

	повернути rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_START,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
}

static ціле rpmi_hsm_stop(void)
{
	ціле rc;
	struct rpmi_hsm_hart_stop_req req;
	struct rpmi_hsm_hart_stop_resp resp;
	void (*jump_warmboot)(void) =
		(void (*)(void))sbi_scratch_thishart_ptr()->warmboot_addr;
	struct rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	якщо (!rpmi)
		повернути SBI_ENOSYS;

	req.hartid = current_hartid();

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_STOP,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	якщо (rc)
		повернути rc;

	/* Wait for interrupt */
	wfi();

	jump_warmboot();

	повернути 0;
}

static bool is_rpmi_hsm_susp_supported(struct rpmi_hsm_suspend *susp, u32 type)
{
	ціле i;

	для (i = 0; i < susp->num_states; i++)
		якщо (type == susp->states[i].suspend_param)
			повернути true;

	повернути false;
}

static ціле rpmi_hsm_suspend(u32 type, ulong resume_addr)
{
	ціле rc;
	struct rpmi_hsm_hart_susp_req req;
	struct rpmi_hsm_hart_susp_resp resp;
	struct rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	якщо (!rpmi)
		повернути SBI_ENOSYS;

	/* check if harts support this suspend type */
	якщо (!is_rpmi_hsm_susp_supported(rpmi->susp, type))
		повернути SBI_EINVAL;

	req.hartid = current_hartid();
	req.suspend_type = type;
	req.resume_addr_lo = resume_addr;
	req.resume_addr_hi = (u64)resume_addr  >> 32;

	rc = rpmi_normal_request_with_status(
			rpmi->chan, RPMI_HSM_SRV_HART_SUSPEND,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	якщо (rc)
		повернути rc;

	/* Wait for interrupt */
	wfi();

	повернути 0;
}

static struct sbi_hsm_device sbi_hsm_rpmi = {
	.name		= "rpmi-hsm",
	.hart_start	= rpmi_hsm_start,
	.hart_stop	= rpmi_hsm_stop,
	.hart_suspend	= rpmi_hsm_suspend,
};

static void rpmi_hsm_do_fixup(struct fdt_general_fixup *f, void *fdt)
{
	struct rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	якщо (!rpmi || !rpmi->susp || !rpmi->susp->num_states)
		повернути;

	fdt_add_cpu_idle_states(fdt, rpmi->susp->states);
}

static struct fdt_general_fixup rpmi_hsm_fixup = {
	.name = "rpmi-hsm-fixup",
	.do_fixup = rpmi_hsm_do_fixup,
};

static ціле rpmi_hsm_get_num_suspend_states(struct mbox_chan *chan,
					   struct rpmi_hsm_suspend *susp)
{
	ціле rc;
	struct rpmi_hsm_get_susp_types_req req;
	struct rpmi_hsm_get_susp_types_resp resp;

	req.start_index = 0;
	rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
	якщо (rc)
		повернути rc;

	susp->num_states = resp.returned + resp.remaining;
	повернути 0;
}

static ціле rpmi_hsm_get_suspend_states(struct mbox_chan *chan,
					struct rpmi_hsm_suspend *susp)
{
	ціле rc, i, cnt = 0;
	struct rpmi_hsm_get_susp_types_req req;
	struct rpmi_hsm_get_susp_types_resp resp;
	struct rpmi_hsm_get_susp_info_req dreq;
	struct rpmi_hsm_get_susp_info_resp dresp;
	struct sbi_cpu_idle_state *state;

	якщо (!susp->num_states)
		повернути 0;

	req.start_index = 0;
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_TYPES,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		якщо (rc)
			повернути rc;

		для (i = 0; i < resp.returned && cnt < susp->num_states; i++)
			susp->states[cnt++].suspend_param = resp.types[i];
		req.start_index = i;
	} поки (resp.remaining);

	для (i = 0; i < susp->num_states; i++) {
		state = &susp->states[i];

		dreq.suspend_type = state->suspend_param;
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_SUSPEND_INFO,
			&dreq, rpmi_u32_count(dreq), rpmi_u32_count(dreq),
			&dresp, rpmi_u32_count(dresp), rpmi_u32_count(dresp));
		якщо (rc)
			повернути rc;

		state->local_timer_stop =
			(dresp.flags & RPMI_HSM_SUSPEND_INFO_FLAGS_TIMER_STOP) ? true : false;
		state->entry_latency_us = dresp.entry_latency_us;
		state->exit_latency_us = dresp.exit_latency_us;
		state->wakeup_latency_us = dresp.wakeup_latency_us;
		state->min_residency_us = dresp.min_residency_us;
	}

	повернути 0;
}

static ціле rpmi_hsm_update_hart_scratch(struct mbox_chan *chan,
					struct rpmi_hsm_suspend *susp)
{
	ціле rc, i;
	struct rpmi_hsm_get_hart_list_req req;
	struct rpmi_hsm_get_hart_list_resp resp;
	struct rpmi_hsm *rpmi = rpmi_hsm_get_pointer(current_hartid());

	req.start_index = 0;
	do {
		rc = rpmi_normal_request_with_status(
			chan, RPMI_HSM_SRV_GET_HART_LIST,
			&req, rpmi_u32_count(req), rpmi_u32_count(req),
			&resp, rpmi_u32_count(resp), rpmi_u32_count(resp));
		якщо (rc)
			повернути rc;

		для (i = 0; i < resp.returned; i++) {
			rpmi = rpmi_hsm_get_pointer(resp.hartid[i]);
			якщо (!rpmi)
				повернути SBI_ENOSYS;

			rpmi->chan = chan;
			rpmi->susp = susp;
		}

		req.start_index += resp.returned;
	} поки (resp.remaining);

	повернути 0;
}

static ціле rpmi_hsm_cold_init(const void *fdt, ціле nodeoff,
			      const struct fdt_match *match)
{
	ціле rc, i;
	struct mbox_chan *chan;
	struct rpmi_hsm_suspend *susp;

	якщо (!rpmi_hsm_offset) {
		rpmi_hsm_offset =
			sbi_scratch_alloc_type_offset(struct rpmi_hsm);
		якщо (!rpmi_hsm_offset)
			повернути SBI_ENOMEM;
	}

	/*
	 * If channel request failed then other end does not support
	 * HSM service group so do nothing.
	 */
	rc = fdt_mailbox_request_chan(fdt, nodeoff, 0, &chan);
	якщо (rc)
		повернути SBI_ENODEV;

	/* Allocate context for HART suspend states */
	susp = sbi_zalloc(sizeof(*susp));
	якщо (!susp)
		повернути SBI_ENOMEM;

	/* Get number of HART suspend states */
	rc = rpmi_hsm_get_num_suspend_states(chan, susp);
	якщо (rc)
		goto fail_free_susp;

	/* Skip HART suspend state discovery for zero HART suspend states */
	якщо (!susp->num_states)
		goto skip_suspend_states;

	/* Allocate array of HART suspend states */
	susp->states = sbi_calloc(susp->num_states + 1, sizeof(*susp->states));
	якщо (!susp->states) {
		rc = SBI_ENOMEM;
		goto fail_free_susp;
	}

	/* Allocate name of each HART suspend state */
	для (i = 0; i < susp->num_states; i++) {
		susp->states[i].name =
			sbi_zalloc(MAX_HSM_SUPSEND_STATE_NAMELEN);
		якщо (!susp->states[i].name) {
			do {
				i--;
				sbi_free((void *)susp->states[i].name);
			} поки (i > 0);

			rc = SBI_ENOMEM;
			goto fail_free_susp_states;
		}
		sbi_snprintf((char *)susp->states[i].name,
			     MAX_HSM_SUPSEND_STATE_NAMELEN, "cpu-susp%d", i);
	}

	/* Get details about each HART suspend state */
	rc = rpmi_hsm_get_suspend_states(chan, susp);
	якщо (rc)
		goto fail_free_susp_state_names;

skip_suspend_states:
	/* Update per-HART scratch space */
	rc = rpmi_hsm_update_hart_scratch(chan, susp);
	якщо (rc)
		goto fail_free_susp_state_names;

	/* Register HSM fixup callback */
	rc = fdt_register_general_fixup(&rpmi_hsm_fixup);
	якщо (rc && rc != SBI_EALREADY)
		goto fail_free_susp_state_names;

	/* Register HSM device */
	якщо (!susp->num_states)
		sbi_hsm_rpmi.hart_suspend = NULL;
	sbi_hsm_set_device(&sbi_hsm_rpmi);

	повернути 0;

fail_free_susp_state_names:
	для (i = 0; i < susp->num_states; i++)
		sbi_free((void *)susp->states[i].name);
fail_free_susp_states:
	якщо (susp->num_states)
		sbi_free(susp->states);
fail_free_susp:
	sbi_free(susp);
	повернути rc;
}

static const struct fdt_match rpmi_hsm_match[] = {
	{ .compatible = "riscv,rpmi-hsm" },
	{},
};

const struct fdt_driver fdt_hsm_rpmi = {
	.match_table = rpmi_hsm_match,
	.init = rpmi_hsm_cold_init,
};
