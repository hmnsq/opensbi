/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#شامل <sbi/sbi_bitmap.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>

ساکن SBI_LIST_سر(state_list);
ساکن DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

خالی *sbi_domain_state_ptr(ساخت sbi_domain *dom, ساخت sbi_domain_state *state)
{
	اگر (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		واپس dom->state_priv.idx_مقصد_state_ptr[state->state_idx];

	واپس تہی;
}

ساکن صحیح domain_تیاری_state_one(ساخت sbi_domain *dom,
				 ساخت sbi_domain_state *state)
{
	ساخت sbi_domain_state_priv *priv = &dom->state_priv;
	خالی *state_ptr;
	صحیح rc;

	اگر (priv->idx_مقصد_state_ptr[state->state_idx])
		واپس SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_سائز);
	اگر (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		واپس SBI_ENOMEM;
	}

	اگر (state->state_تیاری) {
		rc = state->state_تیاری(dom, state, state_ptr);
		اگر (rc) {
			sbi_free(state_ptr);
			واپس rc;
		}
	}

	priv->idx_مقصد_state_ptr[state->state_idx] = state_ptr;
	واپس 0;
}

ساکن خالی domain_cleanup_state_one(ساخت sbi_domain *dom,
				    ساخت sbi_domain_state *state)
{
	ساخت sbi_domain_state_priv *priv = &dom->state_priv;
	خالی *state_ptr;

	state_ptr = priv->idx_مقصد_state_ptr[state->state_idx];
	اگر (!state_ptr)
		واپس;

	اگر (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_مقصد_state_ptr[state->state_idx] = تہی;
}

صحیح sbi_domain_تیاری_state(ساخت sbi_domain *dom)
{
	ساخت sbi_domain_state *state;
	صحیح rc;

	اگر (!dom)
		واپس SBI_EINVAL;

	sbi_list_for_each_اندراج(state, &state_list, سر) {
		rc = domain_تیاری_state_one(dom, state);
		اگر (rc) {
			sbi_domain_cleanup_state(dom);
			واپس rc;
		}
	}

	واپس 0;
}

خالی sbi_domain_cleanup_state(ساخت sbi_domain *dom)
{
	ساخت sbi_domain_state *state;

	اگر (!dom)
		واپس;

	sbi_list_for_each_اندراج(state, &state_list, سر)
		domain_cleanup_state_one(dom, state);
}

صحیح sbi_domain_رجسٹر_state(ساخت sbi_domain_state *state)
{
	ساخت sbi_domain *dom;
	u32 state_idx;
	صحیح rc;

	اگر (!state || !state->state_سائز)
		واپس SBI_EINVAL;

	برائے (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		اگر (!bitmap_test(state_idx_bmap, state_idx))
			توڑو;
	}
	اگر (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		واپس SBI_ENOSPC;
	bitmap_سیٹ(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_جوڑو_دم(&state->سر, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_تیاری_state_one(dom, state);
		اگر (rc) {
			sbi_domain_unregister_state(state);
			واپس rc;
		}
	}

	واپس 0;
}

خالی sbi_domain_unregister_state(ساخت sbi_domain_state *state)
{
	ساخت sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->سر);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
