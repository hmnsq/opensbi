/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#അന്തര്ഭാവ <sbi/sbi_bitmap.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>

സ്ഥിതം SBI_LIST_HEAD(state_list);
സ്ഥിതം DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

ശൂന്യം *sbi_domain_state_ptr(ഘടന sbi_domain *dom, ഘടന sbi_domain_state *state)
{
	എങ്കിൽ (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		തിരികെ dom->state_priv.idx_to_state_ptr[state->state_idx];

	തിരികെ ശൂന്യ;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ domain_setup_state_one(ഘടന sbi_domain *dom,
				 ഘടന sbi_domain_state *state)
{
	ഘടന sbi_domain_state_priv *priv = &dom->state_priv;
	ശൂന്യം *state_ptr;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (priv->idx_to_state_ptr[state->state_idx])
		തിരികെ SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	എങ്കിൽ (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		തിരികെ SBI_ENOMEM;
	}

	എങ്കിൽ (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		എങ്കിൽ (rc) {
			sbi_free(state_ptr);
			തിരികെ rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം domain_cleanup_state_one(ഘടന sbi_domain *dom,
				    ഘടന sbi_domain_state *state)
{
	ഘടന sbi_domain_state_priv *priv = &dom->state_priv;
	ശൂന്യം *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	എങ്കിൽ (!state_ptr)
		തിരികെ;

	എങ്കിൽ (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = ശൂന്യ;
}

പൂർണ്ണസംഖ്യ sbi_domain_setup_state(ഘടന sbi_domain *dom)
{
	ഘടന sbi_domain_state *state;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (!dom)
		തിരികെ SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		എങ്കിൽ (rc) {
			sbi_domain_cleanup_state(dom);
			തിരികെ rc;
		}
	}

	തിരികെ 0;
}

ശൂന്യം sbi_domain_cleanup_state(ഘടന sbi_domain *dom)
{
	ഘടന sbi_domain_state *state;

	എങ്കിൽ (!dom)
		തിരികെ;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

പൂർണ്ണസംഖ്യ sbi_domain_register_state(ഘടന sbi_domain_state *state)
{
	ഘടന sbi_domain *dom;
	u32 state_idx;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (!state || !state->state_size)
		തിരികെ SBI_EINVAL;

	വേണ്ടി (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		എങ്കിൽ (!bitmap_test(state_idx_bmap, state_idx))
			നിർത്തുക;
	}
	എങ്കിൽ (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		തിരികെ SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		എങ്കിൽ (rc) {
			sbi_domain_unregister_state(state);
			തിരികെ rc;
		}
	}

	തിരികെ 0;
}

ശൂന്യം sbi_domain_unregister_state(ഘടന sbi_domain_state *state)
{
	ഘടന sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
