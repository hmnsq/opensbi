/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>

ಸ್ಥಿರ SBI_LIST_HEAD(state_list);
ಸ್ಥಿರ DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

ಶೂನ್ಯ *sbi_domain_state_ptr(ಸಂರಚನಾ sbi_domain *dom, ಸಂರಚನಾ sbi_domain_state *state)
{
	ಯದಿ (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		ಪ್ರತ್ಯಾಗಮ dom->state_priv.idx_to_state_ptr[state->state_idx];

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ domain_setup_state_one(ಸಂರಚನಾ sbi_domain *dom,
				 ಸಂರಚನಾ sbi_domain_state *state)
{
	ಸಂರಚನಾ sbi_domain_state_priv *priv = &dom->state_priv;
	ಶೂನ್ಯ *state_ptr;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (priv->idx_to_state_ptr[state->state_idx])
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	ಯದಿ (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	ಯದಿ (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		ಯದಿ (rc) {
			sbi_free(state_ptr);
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ domain_cleanup_state_one(ಸಂರಚನಾ sbi_domain *dom,
				    ಸಂರಚನಾ sbi_domain_state *state)
{
	ಸಂರಚನಾ sbi_domain_state_priv *priv = &dom->state_priv;
	ಶೂನ್ಯ *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	ಯದಿ (!state_ptr)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ sbi_domain_setup_state(ಸಂರಚನಾ sbi_domain *dom)
{
	ಸಂರಚನಾ sbi_domain_state *state;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (!dom)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		ಯದಿ (rc) {
			sbi_domain_cleanup_state(dom);
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_domain_cleanup_state(ಸಂರಚನಾ sbi_domain *dom)
{
	ಸಂರಚನಾ sbi_domain_state *state;

	ಯದಿ (!dom)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

ಪೂರ್ಣಾಙ್ಕ sbi_domain_register_state(ಸಂರಚನಾ sbi_domain_state *state)
{
	ಸಂರಚನಾ sbi_domain *dom;
	u32 state_idx;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (!state || !state->state_size)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತಿ (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		ಯದಿ (!bitmap_test(state_idx_bmap, state_idx))
			ಭಙ್ಗ;
	}
	ಯದಿ (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		ಯದಿ (rc) {
			sbi_domain_unregister_state(state);
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_domain_unregister_state(ಸಂರಚನಾ sbi_domain_state *state)
{
	ಸಂರಚನಾ sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
