/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#شامل <sbi/sbi_bitmap.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>

ایستا SBI_LIST_HEAD(state_list);
ایستا DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

تهی *sbi_domain_state_ptr(ساختار sbi_domain *dom, ساختار sbi_domain_state *state)
{
	اگر (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		بازگشت dom->state_priv.idx_to_state_ptr[state->state_idx];

	بازگشت پوچ;
}

ایستا صحیح domain_setup_state_one(ساختار sbi_domain *dom,
				 ساختار sbi_domain_state *state)
{
	ساختار sbi_domain_state_priv *priv = &dom->state_priv;
	تهی *state_ptr;
	صحیح rc;

	اگر (priv->idx_to_state_ptr[state->state_idx])
		بازگشت SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	اگر (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		بازگشت SBI_ENOMEM;
	}

	اگر (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		اگر (rc) {
			sbi_free(state_ptr);
			بازگشت rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	بازگشت 0;
}

ایستا تهی domain_cleanup_state_one(ساختار sbi_domain *dom,
				    ساختار sbi_domain_state *state)
{
	ساختار sbi_domain_state_priv *priv = &dom->state_priv;
	تهی *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	اگر (!state_ptr)
		بازگشت;

	اگر (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = پوچ;
}

صحیح sbi_domain_setup_state(ساختار sbi_domain *dom)
{
	ساختار sbi_domain_state *state;
	صحیح rc;

	اگر (!dom)
		بازگشت SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		اگر (rc) {
			sbi_domain_cleanup_state(dom);
			بازگشت rc;
		}
	}

	بازگشت 0;
}

تهی sbi_domain_cleanup_state(ساختار sbi_domain *dom)
{
	ساختار sbi_domain_state *state;

	اگر (!dom)
		بازگشت;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

صحیح sbi_domain_register_state(ساختار sbi_domain_state *state)
{
	ساختار sbi_domain *dom;
	u32 state_idx;
	صحیح rc;

	اگر (!state || !state->state_size)
		بازگشت SBI_EINVAL;

	برای (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		اگر (!bitmap_test(state_idx_bmap, state_idx))
			بشکن;
	}
	اگر (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		بازگشت SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		اگر (rc) {
			sbi_domain_unregister_state(state);
			بازگشت rc;
		}
	}

	بازگشت 0;
}

تهی sbi_domain_unregister_state(ساختار sbi_domain_state *state)
{
	ساختار sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
