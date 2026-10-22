/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 */

#अन्तर्भाव <sbi/sbi_bitmap.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>

स्थिर SBI_LIST_HEAD(state_list);
स्थिर DECLARE_BITMAP(state_idx_bmap, SBI_DOMAIN_MAX_STATE_PTRS);

शून्य *sbi_domain_state_ptr(संरचना sbi_domain *dom, संरचना sbi_domain_state *state)
{
	जइ (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		प्रत्यागम dom->state_priv.idx_to_state_ptr[state->state_idx];

	प्रत्यागम रिक्त;
}

स्थिर पूर्णाङ्क domain_setup_state_one(संरचना sbi_domain *dom,
				 संरचना sbi_domain_state *state)
{
	संरचना sbi_domain_state_priv *priv = &dom->state_priv;
	शून्य *state_ptr;
	पूर्णाङ्क rc;

	जइ (priv->idx_to_state_ptr[state->state_idx])
		प्रत्यागम SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	जइ (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		प्रत्यागम SBI_ENOMEM;
	}

	जइ (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		जइ (rc) {
			sbi_free(state_ptr);
			प्रत्यागम rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	प्रत्यागम 0;
}

स्थिर शून्य domain_cleanup_state_one(संरचना sbi_domain *dom,
				    संरचना sbi_domain_state *state)
{
	संरचना sbi_domain_state_priv *priv = &dom->state_priv;
	शून्य *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	जइ (!state_ptr)
		प्रत्यागम;

	जइ (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = रिक्त;
}

पूर्णाङ्क sbi_domain_setup_state(संरचना sbi_domain *dom)
{
	संरचना sbi_domain_state *state;
	पूर्णाङ्क rc;

	जइ (!dom)
		प्रत्यागम SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		जइ (rc) {
			sbi_domain_cleanup_state(dom);
			प्रत्यागम rc;
		}
	}

	प्रत्यागम 0;
}

शून्य sbi_domain_cleanup_state(संरचना sbi_domain *dom)
{
	संरचना sbi_domain_state *state;

	जइ (!dom)
		प्रत्यागम;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

पूर्णाङ्क sbi_domain_register_state(संरचना sbi_domain_state *state)
{
	संरचना sbi_domain *dom;
	u32 state_idx;
	पूर्णाङ्क rc;

	जइ (!state || !state->state_size)
		प्रत्यागम SBI_EINVAL;

	प्रति (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		जइ (!bitmap_test(state_idx_bmap, state_idx))
			भङ्ग;
	}
	जइ (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		प्रत्यागम SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		जइ (rc) {
			sbi_domain_unregister_state(state);
			प्रत्यागम rc;
		}
	}

	प्रत्यागम 0;
}

शून्य sbi_domain_unregister_state(संरचना sbi_domain_state *state)
{
	संरचना sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
