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
	यदि (dom && state && state->state_idx < SBI_DOMAIN_MAX_STATE_PTRS)
		फर्काऊ dom->state_priv.idx_to_state_ptr[state->state_idx];

	फर्काऊ रिक्त;
}

स्थिर पूर्णाङ्क domain_setup_state_one(संरचना sbi_domain *dom,
				 संरचना sbi_domain_state *state)
{
	संरचना sbi_domain_state_priv *priv = &dom->state_priv;
	शून्य *state_ptr;
	पूर्णाङ्क rc;

	यदि (priv->idx_to_state_ptr[state->state_idx])
		फर्काऊ SBI_EALREADY;

	state_ptr = sbi_zalloc(state->state_size);
	यदि (!state_ptr) {
		sbi_domain_cleanup_state(dom);
		फर्काऊ SBI_ENOMEM;
	}

	यदि (state->state_setup) {
		rc = state->state_setup(dom, state, state_ptr);
		यदि (rc) {
			sbi_free(state_ptr);
			फर्काऊ rc;
		}
	}

	priv->idx_to_state_ptr[state->state_idx] = state_ptr;
	फर्काऊ 0;
}

स्थिर शून्य domain_cleanup_state_one(संरचना sbi_domain *dom,
				    संरचना sbi_domain_state *state)
{
	संरचना sbi_domain_state_priv *priv = &dom->state_priv;
	शून्य *state_ptr;

	state_ptr = priv->idx_to_state_ptr[state->state_idx];
	यदि (!state_ptr)
		फर्काऊ;

	यदि (state->state_cleanup)
		state->state_cleanup(dom, state, state_ptr);

	sbi_free(state_ptr);
	priv->idx_to_state_ptr[state->state_idx] = रिक्त;
}

पूर्णाङ्क sbi_domain_setup_state(संरचना sbi_domain *dom)
{
	संरचना sbi_domain_state *state;
	पूर्णाङ्क rc;

	यदि (!dom)
		फर्काऊ SBI_EINVAL;

	sbi_list_for_each_entry(state, &state_list, head) {
		rc = domain_setup_state_one(dom, state);
		यदि (rc) {
			sbi_domain_cleanup_state(dom);
			फर्काऊ rc;
		}
	}

	फर्काऊ 0;
}

शून्य sbi_domain_cleanup_state(संरचना sbi_domain *dom)
{
	संरचना sbi_domain_state *state;

	यदि (!dom)
		फर्काऊ;

	sbi_list_for_each_entry(state, &state_list, head)
		domain_cleanup_state_one(dom, state);
}

पूर्णाङ्क sbi_domain_register_state(संरचना sbi_domain_state *state)
{
	संरचना sbi_domain *dom;
	u32 state_idx;
	पूर्णाङ्क rc;

	यदि (!state || !state->state_size)
		फर्काऊ SBI_EINVAL;

	लागि (state_idx = 0; state_idx < SBI_DOMAIN_MAX_STATE_PTRS; state_idx++) {
		यदि (!bitmap_test(state_idx_bmap, state_idx))
			रोक;
	}
	यदि (SBI_DOMAIN_MAX_STATE_PTRS <= state_idx)
		फर्काऊ SBI_ENOSPC;
	bitmap_set(state_idx_bmap, state_idx, 1);

	state->state_idx = state_idx;
	sbi_list_add_tail(&state->head, &state_list);

	sbi_domain_for_each(dom) {
		rc = domain_setup_state_one(dom, state);
		यदि (rc) {
			sbi_domain_unregister_state(state);
			फर्काऊ rc;
		}
	}

	फर्काऊ 0;
}

शून्य sbi_domain_unregister_state(संरचना sbi_domain_state *state)
{
	संरचना sbi_domain *dom;

	sbi_domain_for_each(dom)
		domain_cleanup_state_one(dom, state);

	sbi_list_del(&state->head);
	bitmap_clear(state_idx_bmap, state->state_idx, 1);
}
