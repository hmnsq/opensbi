/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#அந்தர்பாவ <sbi/sbi_bitmap.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>

நிலையான SBI_LIST_HEAD(state_list);
நிலையான DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

வெறுமை *sbi_domain_state_ptr(கட்டமைப்பு sbi_domain *dom, கட்டமைப்பு sbi_domain_state *state)
{
	எனில் (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		திரும்பு dom->state_priv.idx_to_state_ptr[state->state_idx];

	திரும்பு வெற்று;
}

நிலையான முழு domain_setup_state_one(கட்டமைப்பு sbi_domain *dom,
				 கட்டமைப்பு sbi_domain_state *state)
{
	கட்டமைப்பு sbi_domain_state_priv *priv = &dom->state_priv;
	வெறுமை *state_ptr;
	முழு rc;

	எனில் (priv->idx_to_state_ptr[state->state_idx])
		திரும்பு SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	எனில் (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		திரும்பு SBI_ENOMEM;
	}

	எனில் (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		எனில் (rc) {
			sbi_free(state_ptr);
			திரும்பு rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	திரும்பு 0;
}

நிலையான வெறுமை domain_cleanup_state_one(கட்டமைப்பு sbi_domain *dom,
				    கட்டமைப்பு sbi_domain_state *state)
{
	கட்டமைப்பு sbi_domain_state_priv *priv = &dom->state_priv;
	வெறுமை *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	எனில் (!state_ptr)
		திரும்பு;

	எனில் (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = வெற்று;
}

முழு sbi_domain_setup_state(கட்டமைப்பு sbi_domain *dom)
{
	கட்டமைப்பு sbi_domain_state *state;
	முழு rc;

	எனில் (!dom)
		திரும்பு SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		எனில் (rc) {
			sbi_domain_cleanup_state(dom);
			திரும்பு rc;
		}
	}

	திரும்பு 0;
}

வெறுமை sbi_domain_cleanup_state(கட்டமைப்பு sbi_domain *dom)
{
	கட்டமைப்பு sbi_domain_state *state;

	எனில் (!dom)
		திரும்பு;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

முழு sbi_domain_register_state(கட்டமைப்பு sbi_domain_state *state)
{
	கட்டமைப்பு sbi_domain *dom;
	u32 state_idx;
	முழு rc;

	எனில் (!state || !state->state_size)
		திரும்பு SBI_EINVAL;

	க்கு (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		எனில் (!bitmap_test(state_idx_bmap, state_idx))
			நிறுத்து;
	}
	எனில் (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		திரும்பு SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		எனில் (rc) {
			sbi_domain_unregister_state(state);
			திரும்பு rc;
		}
	}

	திரும்பு 0;
}

வெறுமை sbi_domain_unregister_state(கட்டமைப்பு sbi_domain_state *state)
{
	கட்டமைப்பு sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
