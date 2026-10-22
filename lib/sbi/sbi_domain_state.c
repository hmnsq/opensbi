/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>

ಸ್ಥಿರ SBI_LIST_ತಲೆ(state_list);
ಸ್ಥಿರ DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

ಶೂನ್ಯ *sbi_domain_state_ptr(ರಚನೆ sbi_domain *dom, ರಚನೆ sbi_domain_state *state)
{
	ಆದರೆ (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		ಹಿಂತಿರುಗಿ dom->state_priv.idx_ಗಮ್ಯ_state_ptr[state->state_idx];

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ domain_ಸಿದ್ಧತೆ_state_one(ರಚನೆ sbi_domain *dom,
				 ರಚನೆ sbi_domain_state *state)
{
	ರಚನೆ sbi_domain_state_priv *priv = &dom->state_priv;
	ಶೂನ್ಯ *state_ptr;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (priv->idx_ಗಮ್ಯ_state_ptr[state->state_idx])
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_ಗಾತ್ರ);
	ಆದರೆ (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	ಆದರೆ (state->state_ಸಿದ್ಧತೆ) {
		rc = state->state_ಸಿದ್ಧತೆ(dom, state, state_ptr);
		ಆದರೆ (rc) {
			sbi_free(state_ptr);
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	priv->idx_ಗಮ್ಯ_state_ptr[state->state_idx] = state_ptr;
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ domain_cleanup_state_one(ರಚನೆ sbi_domain *dom,
				    ರಚನೆ sbi_domain_state *state)
{
	ರಚನೆ sbi_domain_state_priv *priv = &dom->state_priv;
	ಶೂನ್ಯ *state_ptr;

	state_ptr = priv->idx_ಗಮ್ಯ_state_ptr[state->state_idx];
	ಆದರೆ (!state_ptr)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_ಗಮ್ಯ_state_ptr[state->state_idx] = ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ sbi_domain_ಸಿದ್ಧತೆ_state(ರಚನೆ sbi_domain *dom)
{
	ರಚನೆ sbi_domain_state *state;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (!dom)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	sbi_list_for_each_ನಮೂದು(state, &state_list, ತಲೆ) {
		rc = domain_ಸಿದ್ಧತೆ_state_one(dom, state);
		ಆದರೆ (rc) {
			sbi_domain_cleanup_state(dom);
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_domain_cleanup_state(ರಚನೆ sbi_domain *dom)
{
	ರಚನೆ sbi_domain_state *state;

	ಆದರೆ (!dom)
		ಹಿಂತಿರುಗಿ;

	sbi_list_for_each_ನಮೂದು(state, &state_list, ತಲೆ)
		domain_cleanup_state_one(dom, state);
}

ಪೂರ್ಣಾಂಕ sbi_domain_ರಿಜಿಸ್ಟರ್_state(ರಚನೆ sbi_domain_state *state)
{
	ರಚನೆ sbi_domain *dom;
	u32 state_idx;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (!state || !state->state_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಗಾಗಿ (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		ಆದರೆ (!bitmap_test(state_idx_bmap, state_idx))
			ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		ಹಿಂತಿರುಗಿ SBI_ENOSPC;
	bitmap_ಗಣ(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&state->ತಲೆ, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_ಸಿದ್ಧತೆ_state_one(dom, state);
		ಆದರೆ (rc) {
			sbi_domain_unregister_state(state);
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_domain_unregister_state(ರಚನೆ sbi_domain_state *state)
{
	ರಚನೆ sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->ತಲೆ);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
