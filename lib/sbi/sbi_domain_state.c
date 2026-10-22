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
	nếu (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		trả_về dom->state_priv.idx_to_state_ptr[state->state_idx];

	trả_về NULL;
}

static số_nguyên domain_setup_state_one(struct sbi_domain *dom,
				 struct sbi_domain_state *state)
{
	struct sbi_domain_state_priv *priv = &dom->state_priv;
	void *state_ptr;
	số_nguyên rc;

	nếu (priv->idx_to_state_ptr[state->state_idx])
		trả_về SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	nếu (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		trả_về SBI_ENOMEM;
	}

	nếu (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		nếu (rc) {
			sbi_free(state_ptr);
			trả_về rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	trả_về 0;
}

static void domain_cleanup_state_one(struct sbi_domain *dom,
				    struct sbi_domain_state *state)
{
	struct sbi_domain_state_priv *priv = &dom->state_priv;
	void *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	nếu (!state_ptr)
		trả_về;

	nếu (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = NULL;
}

số_nguyên sbi_domain_setup_state(struct sbi_domain *dom)
{
	struct sbi_domain_state *state;
	số_nguyên rc;

	nếu (!dom)
		trả_về SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		nếu (rc) {
			sbi_domain_cleanup_state(dom);
			trả_về rc;
		}
	}

	trả_về 0;
}

void sbi_domain_cleanup_state(struct sbi_domain *dom)
{
	struct sbi_domain_state *state;

	nếu (!dom)
		trả_về;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

số_nguyên sbi_domain_register_state(struct sbi_domain_state *state)
{
	struct sbi_domain *dom;
	u32 state_idx;
	số_nguyên rc;

	nếu (!state || !state->state_size)
		trả_về SBI_EINVAL;

	cho (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		nếu (!bitmap_test(state_idx_bmap, state_idx))
			break;
	}
	nếu (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		trả_về SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		nếu (rc) {
			sbi_domain_unregister_state(state);
			trả_về rc;
		}
	}

	trả_về 0;
}

void sbi_domain_unregister_state(struct sbi_domain_state *state)
{
	struct sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
