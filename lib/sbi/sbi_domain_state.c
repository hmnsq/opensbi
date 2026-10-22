/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#समावेश <sbi/sbi_bitmap.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>

अटल SBI_LIST_शीर्ष(state_list);
अटल DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

व्योम *sbi_domain_state_ptr(काष्ठा sbi_domain *dom, काष्ठा sbi_domain_state *state)
{
	अगर (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		वापस dom->state_priv.idx_to_state_ptr[state->state_idx];

	वापस शून्य;
}

अटल पूर्णांक domain_setup_state_one(काष्ठा sbi_domain *dom,
				 काष्ठा sbi_domain_state *state)
{
	काष्ठा sbi_domain_state_priv *priv = &dom->state_priv;
	व्योम *state_ptr;
	पूर्णांक rc;

	अगर (priv->idx_to_state_ptr[state->state_idx])
		वापस SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_आकार);
	अगर (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		वापस SBI_ENOMEM;
	}

	अगर (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		अगर (rc) {
			sbi_free(state_ptr);
			वापस rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	वापस 0;
}

अटल व्योम domain_cleanup_state_one(काष्ठा sbi_domain *dom,
				    काष्ठा sbi_domain_state *state)
{
	काष्ठा sbi_domain_state_priv *priv = &dom->state_priv;
	व्योम *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	अगर (!state_ptr)
		वापस;

	अगर (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = शून्य;
}

पूर्णांक sbi_domain_setup_state(काष्ठा sbi_domain *dom)
{
	काष्ठा sbi_domain_state *state;
	पूर्णांक rc;

	अगर (!dom)
		वापस SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, शीर्ष) {
		rc = domain_setup_state_one(dom, state);
		अगर (rc) {
			sbi_domain_cleanup_state(dom);
			वापस rc;
		}
	}

	वापस 0;
}

व्योम sbi_domain_cleanup_state(काष्ठा sbi_domain *dom)
{
	काष्ठा sbi_domain_state *state;

	अगर (!dom)
		वापस;

	sbi_list_for_each_entry(state, &state_list, शीर्ष)
		domain_cleanup_state_one(dom, state);
}

पूर्णांक sbi_domain_register_state(काष्ठा sbi_domain_state *state)
{
	काष्ठा sbi_domain *dom;
	u32 state_idx;
	पूर्णांक rc;

	अगर (!state || !state->state_आकार)
		वापस SBI_EINVAL;

	क्रम (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		अगर (!bitmap_test(state_idx_bmap, state_idx))
			अवरोध;
	}
	अगर (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		वापस SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_पुच्छ(&state->शीर्ष, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		अगर (rc) {
			sbi_domain_unregister_state(state);
			वापस rc;
		}
	}

	वापस 0;
}

व्योम sbi_domain_unregister_state(काष्ठा sbi_domain_state *state)
{
	काष्ठा sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->शीर्ष);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
