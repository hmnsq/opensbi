/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitmap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>

ସ୍ଥିର SBI_LIST_HEAD(state_list);
ସ୍ଥିର DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

ଶୂନ୍ୟ *sbi_domain_state_ptr(ସଂରଚନା sbi_domain *dom, ସଂରଚନା sbi_domain_state *state)
{
	ଯଦି (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		ଫେରନ୍ତୁ dom->state_priv.idx_to_state_ptr[state->state_idx];

	ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain_setup_state_one(ସଂରଚନା sbi_domain *dom,
				 ସଂରଚନା sbi_domain_state *state)
{
	ସଂରଚନା sbi_domain_state_priv *priv = &dom->state_priv;
	ଶୂନ୍ୟ *state_ptr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (priv->idx_to_state_ptr[state->state_idx])
		ଫେରନ୍ତୁ SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	ଯଦି (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	ଯଦି (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		ଯଦି (rc) {
			sbi_free(state_ptr);
			ଫେରନ୍ତୁ rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ domain_cleanup_state_one(ସଂରଚନା sbi_domain *dom,
				    ସଂରଚନା sbi_domain_state *state)
{
	ସଂରଚନା sbi_domain_state_priv *priv = &dom->state_priv;
	ଶୂନ୍ୟ *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	ଯଦି (!state_ptr)
		ଫେରନ୍ତୁ;

	ଯଦି (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_setup_state(ସଂରଚନା sbi_domain *dom)
{
	ସଂରଚନା sbi_domain_state *state;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (!dom)
		ଫେରନ୍ତୁ SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		ଯଦି (rc) {
			sbi_domain_cleanup_state(dom);
			ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_domain_cleanup_state(ସଂରଚନା sbi_domain *dom)
{
	ସଂରଚନା sbi_domain_state *state;

	ଯଦି (!dom)
		ଫେରନ୍ତୁ;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_domain_register_state(ସଂରଚନା sbi_domain_state *state)
{
	ସଂରଚନା sbi_domain *dom;
	u32 state_idx;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (!state || !state->state_size)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ପାଇଁ (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		ଯଦି (!bitmap_test(state_idx_bmap, state_idx))
			ଭାଙ୍ଗ;
	}
	ଯଦି (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		ଫେରନ୍ତୁ SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		ଯଦି (rc) {
			sbi_domain_unregister_state(state);
			ଫେରନ୍ତୁ rc;
		}
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sbi_domain_unregister_state(ସଂରଚନା sbi_domain_state *state)
{
	ସଂରଚନା sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
