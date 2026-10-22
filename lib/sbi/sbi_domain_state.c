/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#અન્તર્ભાવ <sbi/sbi_bitmap.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>

સ્થિર SBI_LIST_HEAD(state_list);
સ્થિર DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

શૂન્ય *sbi_domain_state_ptr(સંરચના sbi_domain *dom, સંરચના sbi_domain_state *state)
{
	જો (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		પાછા dom->state_priv.idx_to_state_ptr[state->state_idx];

	પાછા ખાલી;
}

સ્થિર પૂર્ણાંક domain_setup_state_one(સંરચના sbi_domain *dom,
				 સંરચના sbi_domain_state *state)
{
	સંરચના sbi_domain_state_priv *priv = &dom->state_priv;
	શૂન્ય *state_ptr;
	પૂર્ણાંક rc;

	જો (priv->idx_to_state_ptr[state->state_idx])
		પાછા SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	જો (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		પાછા SBI_ENOMEM;
	}

	જો (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		જો (rc) {
			sbi_free(state_ptr);
			પાછા rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	પાછા 0;
}

સ્થિર શૂન્ય domain_cleanup_state_one(સંરચના sbi_domain *dom,
				    સંરચના sbi_domain_state *state)
{
	સંરચના sbi_domain_state_priv *priv = &dom->state_priv;
	શૂન્ય *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	જો (!state_ptr)
		પાછા;

	જો (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = ખાલી;
}

પૂર્ણાંક sbi_domain_setup_state(સંરચના sbi_domain *dom)
{
	સંરચના sbi_domain_state *state;
	પૂર્ણાંક rc;

	જો (!dom)
		પાછા SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		જો (rc) {
			sbi_domain_cleanup_state(dom);
			પાછા rc;
		}
	}

	પાછા 0;
}

શૂન્ય sbi_domain_cleanup_state(સંરચના sbi_domain *dom)
{
	સંરચના sbi_domain_state *state;

	જો (!dom)
		પાછા;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

પૂર્ણાંક sbi_domain_register_state(સંરચના sbi_domain_state *state)
{
	સંરચના sbi_domain *dom;
	u32 state_idx;
	પૂર્ણાંક rc;

	જો (!state || !state->state_size)
		પાછા SBI_EINVAL;

	માટે (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		જો (!bitmap_test(state_idx_bmap, state_idx))
			તોડો;
	}
	જો (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		પાછા SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		જો (rc) {
			sbi_domain_unregister_state(state);
			પાછા rc;
		}
	}

	પાછા 0;
}

શૂન્ય sbi_domain_unregister_state(સંરચના sbi_domain_state *state)
{
	સંરચના sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
