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

void *sbi_domain_state_ptr(sa_nracanaa sbi_domain *dom, sa_nracanaa sbi_domain_state *state)
{
	ᤀᤡ᤺ (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		ᤀᤡᤴ dom->state_priv.idx_to_state_ptr[state->state_idx];

	ᤀᤡᤴ NULL;
}

static puur_naa_nka domain_setup_state_one(sa_nracanaa sbi_domain *dom,
				 sa_nracanaa sbi_domain_state *state)
{
	sa_nracanaa sbi_domain_state_priv *priv = &dom->state_priv;
	void *state_ptr;
	puur_naa_nka rc;

	ᤀᤡ᤺ (priv->idx_to_state_ptr[state->state_idx])
		ᤀᤡᤴ SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	ᤀᤡ᤺ (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		ᤀᤡᤴ SBI_ENOMEM;
	}

	ᤀᤡ᤺ (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		ᤀᤡ᤺ (rc) {
			sbi_free(state_ptr);
			ᤀᤡᤴ rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	ᤀᤡᤴ 0;
}

static void domain_cleanup_state_one(sa_nracanaa sbi_domain *dom,
				    sa_nracanaa sbi_domain_state *state)
{
	sa_nracanaa sbi_domain_state_priv *priv = &dom->state_priv;
	void *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	ᤀᤡ᤺ (!state_ptr)
		ᤀᤡᤴ;

	ᤀᤡ᤺ (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = NULL;
}

puur_naa_nka sbi_domain_setup_state(sa_nracanaa sbi_domain *dom)
{
	sa_nracanaa sbi_domain_state *state;
	puur_naa_nka rc;

	ᤀᤡ᤺ (!dom)
		ᤀᤡᤴ SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		ᤀᤡ᤺ (rc) {
			sbi_domain_cleanup_state(dom);
			ᤀᤡᤴ rc;
		}
	}

	ᤀᤡᤴ 0;
}

void sbi_domain_cleanup_state(sa_nracanaa sbi_domain *dom)
{
	sa_nracanaa sbi_domain_state *state;

	ᤀᤡ᤺ (!dom)
		ᤀᤡᤴ;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

puur_naa_nka sbi_domain_register_state(sa_nracanaa sbi_domain_state *state)
{
	sa_nracanaa sbi_domain *dom;
	u32 state_idx;
	puur_naa_nka rc;

	ᤀᤡ᤺ (!state || !state->state_size)
		ᤀᤡᤴ SBI_EINVAL;

	for (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		ᤀᤡ᤺ (!bitmap_test(state_idx_bmap, state_idx))
			bha_nga;
	}
	ᤀᤡ᤺ (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		ᤀᤡᤴ SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		ᤀᤡ᤺ (rc) {
			sbi_domain_unregister_state(state);
			ᤀᤡᤴ rc;
		}
	}

	ᤀᤡᤴ 0;
}

void sbi_domain_unregister_state(sa_nracanaa sbi_domain_state *state)
{
	sa_nracanaa sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
