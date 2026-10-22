/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#అన్తర్భావ <sbi/sbi_bitmap.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>

స్థిర SBI_LIST_తల(state_list);
స్థిర DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

శూన్య *sbi_domain_state_ptr(నిర్మాణం sbi_domain *dom, నిర్మాణం sbi_domain_state *state)
{
	ఒకవేళ (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		తిరిగి dom->state_priv.idx_గమ్యం_state_ptr[state->state_idx];

	తిరిగి ఖాళీ;
}

స్థిర పూర్ణాంకం domain_అమరిక_state_one(నిర్మాణం sbi_domain *dom,
				 నిర్మాణం sbi_domain_state *state)
{
	నిర్మాణం sbi_domain_state_priv *priv = &dom->state_priv;
	శూన్య *state_ptr;
	పూర్ణాంకం rc;

	ఒకవేళ (priv->idx_గమ్యం_state_ptr[state->state_idx])
		తిరిగి SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_కొలత);
	ఒకవేళ (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		తిరిగి SBI_ENOMEM;
	}

	ఒకవేళ (state->state_అమరిక) {
		rc = state->state_అమరిక(dom, state, state_ptr);
		ఒకవేళ (rc) {
			sbi_free(state_ptr);
			తిరిగి rc;
		}
	}

	priv->idx_గమ్యం_state_ptr[state->state_idx] = state_ptr;
	తిరిగి 0;
}

స్థిర శూన్య domain_cleanup_state_one(నిర్మాణం sbi_domain *dom,
				    నిర్మాణం sbi_domain_state *state)
{
	నిర్మాణం sbi_domain_state_priv *priv = &dom->state_priv;
	శూన్య *state_ptr;

	state_ptr = priv->idx_గమ్యం_state_ptr[state->state_idx];
	ఒకవేళ (!state_ptr)
		తిరిగి;

	ఒకవేళ (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_గమ్యం_state_ptr[state->state_idx] = ఖాళీ;
}

పూర్ణాంకం sbi_domain_అమరిక_state(నిర్మాణం sbi_domain *dom)
{
	నిర్మాణం sbi_domain_state *state;
	పూర్ణాంకం rc;

	ఒకవేళ (!dom)
		తిరిగి SBI_EINVAL;

	sbi_list_for_each_నమోదు(state, &state_list, తల) {
		rc = domain_అమరిక_state_one(dom, state);
		ఒకవేళ (rc) {
			sbi_domain_cleanup_state(dom);
			తిరిగి rc;
		}
	}

	తిరిగి 0;
}

శూన్య sbi_domain_cleanup_state(నిర్మాణం sbi_domain *dom)
{
	నిర్మాణం sbi_domain_state *state;

	ఒకవేళ (!dom)
		తిరిగి;

	sbi_list_for_each_నమోదు(state, &state_list, తల)
		domain_cleanup_state_one(dom, state);
}

పూర్ణాంకం sbi_domain_రిజిస్టరు_state(నిర్మాణం sbi_domain_state *state)
{
	నిర్మాణం sbi_domain *dom;
	u32 state_idx;
	పూర్ణాంకం rc;

	ఒకవేళ (!state || !state->state_కొలత)
		తిరిగి SBI_EINVAL;

	కొరకు (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		ఒకవేళ (!bitmap_test(state_idx_bmap, state_idx))
			ఆపు;
	}
	ఒకవేళ (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		తిరిగి SBI_ENOSPC;
	bitmap_సమితి(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_కూడు_తోక(&state->తల, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_అమరిక_state_one(dom, state);
		ఒకవేళ (rc) {
			sbi_domain_unregister_state(state);
			తిరిగి rc;
		}
	}

	తిరిగి 0;
}

శూన్య sbi_domain_unregister_state(నిర్మాణం sbi_domain_state *state)
{
	నిర్మాణం sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->తల);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
