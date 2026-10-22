/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#include <sbi/sbi_bitmap.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>

static SBI_LIST_HEAD(state_list);
static DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

void *sbi_domain_state_ptr(struct sbi_domain *dom, struct sbi_domain_state *state)
{
	如果 (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		返回 dom->state_priv.idx_to_state_ptr[state->state_idx];

	返回 NULL;
}

static 整数 domain_setup_state_one(struct sbi_domain *dom,
				 struct sbi_domain_state *state)
{
	struct sbi_domain_state_priv *priv = &dom->state_priv;
	void *state_ptr;
	整数 rc;

	如果 (priv->idx_to_state_ptr[state->state_idx])
		返回 SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	如果 (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		返回 SBI_ENOMEM;
	}

	如果 (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		如果 (rc) {
			sbi_free(state_ptr);
			返回 rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	返回 0;
}

static void domain_cleanup_state_one(struct sbi_domain *dom,
				    struct sbi_domain_state *state)
{
	struct sbi_domain_state_priv *priv = &dom->state_priv;
	void *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	如果 (!state_ptr)
		返回;

	如果 (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = NULL;
}

整数 sbi_domain_setup_state(struct sbi_domain *dom)
{
	struct sbi_domain_state *state;
	整数 rc;

	如果 (!dom)
		返回 SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		如果 (rc) {
			sbi_domain_cleanup_state(dom);
			返回 rc;
		}
	}

	返回 0;
}

void sbi_domain_cleanup_state(struct sbi_domain *dom)
{
	struct sbi_domain_state *state;

	如果 (!dom)
		返回;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

整数 sbi_domain_register_state(struct sbi_domain_state *state)
{
	struct sbi_domain *dom;
	u32 state_idx;
	整数 rc;

	如果 (!state || !state->state_size)
		返回 SBI_EINVAL;

	为 (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		如果 (!bitmap_test(state_idx_bmap, state_idx))
			break;
	}
	如果 (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		返回 SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		如果 (rc) {
			sbi_domain_unregister_state(state);
			返回 rc;
		}
	}

	返回 0;
}

void sbi_domain_unregister_state(struct sbi_domain_state *state)
{
	struct sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
