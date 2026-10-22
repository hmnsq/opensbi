/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>

ساكن SBI_LIST_HEAD(state_list);
ساكن DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

فراغ *sbi_domain_state_ptr(بنية sbi_domain *dom, بنية sbi_domain_state *state)
{
	ܐܢ (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		ܦܢܐ dom->state_priv.idx_to_state_ptr[state->state_idx];

	ܦܢܐ عدم;
}

ساكن صحيح domain_setup_state_one(بنية sbi_domain *dom,
				 بنية sbi_domain_state *state)
{
	بنية sbi_domain_state_priv *priv = &dom->state_priv;
	فراغ *state_ptr;
	صحيح rc;

	ܐܢ (priv->idx_to_state_ptr[state->state_idx])
		ܦܢܐ SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	ܐܢ (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		ܦܢܐ SBI_ENOMEM;
	}

	ܐܢ (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		ܐܢ (rc) {
			sbi_free(state_ptr);
			ܦܢܐ rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	ܦܢܐ 0;
}

ساكن فراغ domain_cleanup_state_one(بنية sbi_domain *dom,
				    بنية sbi_domain_state *state)
{
	بنية sbi_domain_state_priv *priv = &dom->state_priv;
	فراغ *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	ܐܢ (!state_ptr)
		ܦܢܐ;

	ܐܢ (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = عدم;
}

صحيح sbi_domain_setup_state(بنية sbi_domain *dom)
{
	بنية sbi_domain_state *state;
	صحيح rc;

	ܐܢ (!dom)
		ܦܢܐ SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		ܐܢ (rc) {
			sbi_domain_cleanup_state(dom);
			ܦܢܐ rc;
		}
	}

	ܦܢܐ 0;
}

فراغ sbi_domain_cleanup_state(بنية sbi_domain *dom)
{
	بنية sbi_domain_state *state;

	ܐܢ (!dom)
		ܦܢܐ;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

صحيح sbi_domain_register_state(بنية sbi_domain_state *state)
{
	بنية sbi_domain *dom;
	u32 state_idx;
	صحيح rc;

	ܐܢ (!state || !state->state_size)
		ܦܢܐ SBI_EINVAL;

	لكل (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		ܐܢ (!bitmap_test(state_idx_bmap, state_idx))
			اكسر;
	}
	ܐܢ (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		ܦܢܐ SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		ܐܢ (rc) {
			sbi_domain_unregister_state(state);
			ܦܢܐ rc;
		}
	}

	ܦܢܐ 0;
}

فراغ sbi_domain_unregister_state(بنية sbi_domain_state *state)
{
	بنية sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
