/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#অন্তর্ভাব <sbi/sbi_bitmap.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>

স্থির SBI_LIST_শীর্ষ(state_list);
স্থির DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

শূন্য *sbi_domain_state_ptr(কাঠামো sbi_domain *dom, কাঠামো sbi_domain_state *state)
{
	যদি (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		ফেরত dom->state_priv.idx_to_state_ptr[state->state_idx];

	ফেরত ফাঁকা;
}

স্থির পূর্ণসংখ্যা domain_setup_state_one(কাঠামো sbi_domain *dom,
				 কাঠামো sbi_domain_state *state)
{
	কাঠামো sbi_domain_state_priv *priv = &dom->state_priv;
	শূন্য *state_ptr;
	পূর্ণসংখ্যা rc;

	যদি (priv->idx_to_state_ptr[state->state_idx])
		ফেরত SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_আকার);
	যদি (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		ফেরত SBI_ENOMEM;
	}

	যদি (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		যদি (rc) {
			sbi_free(state_ptr);
			ফেরত rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	ফেরত 0;
}

স্থির শূন্য domain_cleanup_state_one(কাঠামো sbi_domain *dom,
				    কাঠামো sbi_domain_state *state)
{
	কাঠামো sbi_domain_state_priv *priv = &dom->state_priv;
	শূন্য *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	যদি (!state_ptr)
		ফেরত;

	যদি (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = ফাঁকা;
}

পূর্ণসংখ্যা sbi_domain_setup_state(কাঠামো sbi_domain *dom)
{
	কাঠামো sbi_domain_state *state;
	পূর্ণসংখ্যা rc;

	যদি (!dom)
		ফেরত SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, শীর্ষ) {
		rc = domain_setup_state_one(dom, state);
		যদি (rc) {
			sbi_domain_cleanup_state(dom);
			ফেরত rc;
		}
	}

	ফেরত 0;
}

শূন্য sbi_domain_cleanup_state(কাঠামো sbi_domain *dom)
{
	কাঠামো sbi_domain_state *state;

	যদি (!dom)
		ফেরত;

	sbi_list_for_each_entry(state, &state_list, শীর্ষ)
		domain_cleanup_state_one(dom, state);
}

পূর্ণসংখ্যা sbi_domain_register_state(কাঠামো sbi_domain_state *state)
{
	কাঠামো sbi_domain *dom;
	u32 state_idx;
	পূর্ণসংখ্যা rc;

	যদি (!state || !state->state_আকার)
		ফেরত SBI_EINVAL;

	জন্য (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		যদি (!bitmap_test(state_idx_bmap, state_idx))
			বিরতি;
	}
	যদি (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		ফেরত SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_লেজ(&state->শীর্ষ, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		যদি (rc) {
			sbi_domain_unregister_state(state);
			ফেরত rc;
		}
	}

	ফেরত 0;
}

শূন্য sbi_domain_unregister_state(কাঠামো sbi_domain_state *state)
{
	কাঠামো sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->শীর্ষ);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
