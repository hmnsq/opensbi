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
	إذا (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		إرجاع dom->state_priv.idx_to_state_ptr[state->state_idx];

	إرجاع عدم;
}

ساكن صحيح domain_setup_state_one(بنية sbi_domain *dom,
				 بنية sbi_domain_state *state)
{
	بنية sbi_domain_state_priv *priv = &dom->state_priv;
	فراغ *state_ptr;
	صحيح rc;

	إذا (priv->idx_to_state_ptr[state->state_idx])
		إرجاع SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	إذا (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		إرجاع SBI_ENOMEM;
	}

	إذا (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		إذا (rc) {
			sbi_free(state_ptr);
			إرجاع rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	إرجاع 0;
}

ساكن فراغ domain_cleanup_state_one(بنية sbi_domain *dom,
				    بنية sbi_domain_state *state)
{
	بنية sbi_domain_state_priv *priv = &dom->state_priv;
	فراغ *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	إذا (!state_ptr)
		إرجاع;

	إذا (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = عدم;
}

صحيح sbi_domain_setup_state(بنية sbi_domain *dom)
{
	بنية sbi_domain_state *state;
	صحيح rc;

	إذا (!dom)
		إرجاع SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		إذا (rc) {
			sbi_domain_cleanup_state(dom);
			إرجاع rc;
		}
	}

	إرجاع 0;
}

فراغ sbi_domain_cleanup_state(بنية sbi_domain *dom)
{
	بنية sbi_domain_state *state;

	إذا (!dom)
		إرجاع;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

صحيح sbi_domain_register_state(بنية sbi_domain_state *state)
{
	بنية sbi_domain *dom;
	u32 state_idx;
	صحيح rc;

	إذا (!state || !state->state_size)
		إرجاع SBI_EINVAL;

	لكل (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		إذا (!bitmap_test(state_idx_bmap, state_idx))
			اكسر;
	}
	إذا (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		إرجاع SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		إذا (rc) {
			sbi_domain_unregister_state(state);
			إرجاع rc;
		}
	}

	إرجاع 0;
}

فراغ sbi_domain_unregister_state(بنية sbi_domain_state *state)
{
	بنية sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
