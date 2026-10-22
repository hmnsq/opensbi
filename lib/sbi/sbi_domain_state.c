/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>

ਸਥਿਰ SBI_LIST_ਸਿਰ(state_list);
ਸਥਿਰ DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

ਖਾਲੀ *sbi_domain_state_ptr(ਬਣਤਰ sbi_domain *dom, ਬਣਤਰ sbi_domain_state *state)
{
	ਜੇ (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		ਵਾਪਸ dom->state_priv.idx_ਮੰਜ਼ਿਲ_state_ptr[state->state_idx];

	ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਪੂਰਨ domain_ਤਿਆਰੀ_state_one(ਬਣਤਰ sbi_domain *dom,
				 ਬਣਤਰ sbi_domain_state *state)
{
	ਬਣਤਰ sbi_domain_state_priv *priv = &dom->state_priv;
	ਖਾਲੀ *state_ptr;
	ਪੂਰਨ rc;

	ਜੇ (priv->idx_ਮੰਜ਼ਿਲ_state_ptr[state->state_idx])
		ਵਾਪਸ SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_ਮਾਪ);
	ਜੇ (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		ਵਾਪਸ SBI_ENOMEM;
	}

	ਜੇ (state->state_ਤਿਆਰੀ) {
		rc = state->state_ਤਿਆਰੀ(dom, state, state_ptr);
		ਜੇ (rc) {
			sbi_free(state_ptr);
			ਵਾਪਸ rc;
		}
	}

	priv->idx_ਮੰਜ਼ਿਲ_state_ptr[state->state_idx] = state_ptr;
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ domain_cleanup_state_one(ਬਣਤਰ sbi_domain *dom,
				    ਬਣਤਰ sbi_domain_state *state)
{
	ਬਣਤਰ sbi_domain_state_priv *priv = &dom->state_priv;
	ਖਾਲੀ *state_ptr;

	state_ptr = priv->idx_ਮੰਜ਼ਿਲ_state_ptr[state->state_idx];
	ਜੇ (!state_ptr)
		ਵਾਪਸ;

	ਜੇ (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_ਮੰਜ਼ਿਲ_state_ptr[state->state_idx] = ਨਿਰਾ;
}

ਪੂਰਨ sbi_domain_ਤਿਆਰੀ_state(ਬਣਤਰ sbi_domain *dom)
{
	ਬਣਤਰ sbi_domain_state *state;
	ਪੂਰਨ rc;

	ਜੇ (!dom)
		ਵਾਪਸ SBI_EINVAL;

	sbi_list_for_each_ਇੰਦਰਾਜ(state, &state_list, ਸਿਰ) {
		rc = domain_ਤਿਆਰੀ_state_one(dom, state);
		ਜੇ (rc) {
			sbi_domain_cleanup_state(dom);
			ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_domain_cleanup_state(ਬਣਤਰ sbi_domain *dom)
{
	ਬਣਤਰ sbi_domain_state *state;

	ਜੇ (!dom)
		ਵਾਪਸ;

	sbi_list_for_each_ਇੰਦਰਾਜ(state, &state_list, ਸਿਰ)
		domain_cleanup_state_one(dom, state);
}

ਪੂਰਨ sbi_domain_ਰਜਿਸਟਰ_state(ਬਣਤਰ sbi_domain_state *state)
{
	ਬਣਤਰ sbi_domain *dom;
	u32 state_idx;
	ਪੂਰਨ rc;

	ਜੇ (!state || !state->state_ਮਾਪ)
		ਵਾਪਸ SBI_EINVAL;

	ਲਈ (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		ਜੇ (!bitmap_test(state_idx_bmap, state_idx))
			ਰੋਕੋ;
	}
	ਜੇ (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		ਵਾਪਸ SBI_ENOSPC;
	bitmap_ਸੈੱਟ(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_ਜੋੜੋ_ਪੂਛ(&state->ਸਿਰ, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_ਤਿਆਰੀ_state_one(dom, state);
		ਜੇ (rc) {
			sbi_domain_unregister_state(state);
			ਵਾਪਸ rc;
		}
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_domain_unregister_state(ਬਣਤਰ sbi_domain_state *state)
{
	ਬਣਤਰ sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->ਸਿਰ);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
