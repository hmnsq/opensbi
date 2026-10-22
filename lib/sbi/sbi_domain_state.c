/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#অন্তর্ভাব <sbi/sbi_bitmap.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>

স্থির SBI_LIST_HEAD(state_list);
স্থির DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

শূন্য *sbi_domain_state_ptr(সংরচনা sbi_domain *dom, সংরচনা sbi_domain_state *state)
{
	যদি (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		প্রত্যাগম dom->state_priv.idx_to_state_ptr[state->state_idx];

	প্রত্যাগম রিক্ত;
}

স্থির পূর্ণাঙ্ক domain_setup_state_one(সংরচনা sbi_domain *dom,
				 সংরচনা sbi_domain_state *state)
{
	সংরচনা sbi_domain_state_priv *priv = &dom->state_priv;
	শূন্য *state_ptr;
	পূর্ণাঙ্ক rc;

	যদি (priv->idx_to_state_ptr[state->state_idx])
		প্রত্যাগম SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	যদি (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		প্রত্যাগম SBI_ENOMEM;
	}

	যদি (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		যদি (rc) {
			sbi_free(state_ptr);
			প্রত্যাগম rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	প্রত্যাগম 0;
}

স্থির শূন্য domain_cleanup_state_one(সংরচনা sbi_domain *dom,
				    সংরচনা sbi_domain_state *state)
{
	সংরচনা sbi_domain_state_priv *priv = &dom->state_priv;
	শূন্য *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	যদি (!state_ptr)
		প্রত্যাগম;

	যদি (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = রিক্ত;
}

পূর্ণাঙ্ক sbi_domain_setup_state(সংরচনা sbi_domain *dom)
{
	সংরচনা sbi_domain_state *state;
	পূর্ণাঙ্ক rc;

	যদি (!dom)
		প্রত্যাগম SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		যদি (rc) {
			sbi_domain_cleanup_state(dom);
			প্রত্যাগম rc;
		}
	}

	প্রত্যাগম 0;
}

শূন্য sbi_domain_cleanup_state(সংরচনা sbi_domain *dom)
{
	সংরচনা sbi_domain_state *state;

	যদি (!dom)
		প্রত্যাগম;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

পূর্ণাঙ্ক sbi_domain_register_state(সংরচনা sbi_domain_state *state)
{
	সংরচনা sbi_domain *dom;
	u32 state_idx;
	পূর্ণাঙ্ক rc;

	যদি (!state || !state->state_size)
		প্রত্যাগম SBI_EINVAL;

	প্রতি (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		যদি (!bitmap_test(state_idx_bmap, state_idx))
			ভঙ্গ;
	}
	যদি (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		প্রত্যাগম SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		যদি (rc) {
			sbi_domain_unregister_state(state);
			প্রত্যাগম rc;
		}
	}

	প্রত্যাগম 0;
}

শূন্য sbi_domain_unregister_state(সংরচনা sbi_domain_state *state)
{
	সংরচনা sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
