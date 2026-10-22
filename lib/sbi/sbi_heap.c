/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_list.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_ਗੰਢ_BATCH_ਮਾਪ		8

ਬਣਤਰ ਢੇਰ_ਗੰਢ {
	ਬਣਤਰ sbi_dlist ਸਿਰ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ;
};

ਬਣਤਰ sbi_ਢੇਰ_control {
	spinlock_t ਤਾਲਾ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2 GUARDED_BY(&ਤਾਲਾ);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ GUARDED_BY(&ਤਾਲਾ);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ resv GUARDED_BY(&ਤਾਲਾ);
	ਬਣਤਰ sbi_dlist free_ਗੰਢ_list GUARDED_BY(&ਤਾਲਾ);
	ਬਣਤਰ sbi_dlist free_space_list GUARDED_BY(&ਤਾਲਾ);
	ਬਣਤਰ sbi_dlist ਵਰਤਿਆ_space_list GUARDED_BY(&ਤਾਲਾ);
	ਬਣਤਰ ਢੇਰ_ਗੰਢ init_free_space_ਗੰਢ GUARDED_BY(&ਤਾਲਾ);
};

ਬਣਤਰ sbi_ਢੇਰ_control global_hpctrl;

ਸਥਿਰ bool alloc_nodes(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl) MUST_HOLD(&hpctrl->ਤਾਲਾ)
{
	size_t ਮਾਪ = ਢੇਰ_ਗੰਢ_BATCH_ਮਾਪ * ਆਕਾਰ(ਬਣਤਰ ਢੇਰ_ਗੰਢ);
	ਬਣਤਰ ਢੇਰ_ਗੰਢ *n, *new = ਨਿਰਾ;

	/* alloc_with_align() requires at most two free nodes */
	ਜੇ (hpctrl->free_ਗੰਢ_list.ਅਗਲਾ != hpctrl->free_ਗੰਢ_list.prev)
		ਵਾਪਸ true;

	sbi_list_for_each_ਇੰਦਰਾਜ_ਉਲਟ(n, &hpctrl->free_space_list, ਸਿਰ) {
		ਜੇ (n->ਮਾਪ >= ਮਾਪ) {
			n->ਮਾਪ -= ਮਾਪ;
			ਜੇ (!n->ਮਾਪ) {
				sbi_list_del(&n->ਸਿਰ);
				sbi_list_ਜੋੜੋ_ਪੂਛ(&n->ਸਿਰ, &hpctrl->free_ਗੰਢ_list);
			}
			new = (ਖਾਲੀ *)(n->addr + n->ਮਾਪ);
			ਰੋਕੋ;
		}
	}
	ਜੇ (!new)
		ਵਾਪਸ false;

	ਲਈ (size_t i = 0; i < ਢੇਰ_ਗੰਢ_BATCH_ਮਾਪ; i++)
		sbi_list_ਜੋੜੋ_ਪੂਛ(&new[i].ਸਿਰ, &hpctrl->free_ਗੰਢ_list);
	hpctrl->resv += ਮਾਪ;

	ਵਾਪਸ true;
}

ਸਥਿਰ ਖਾਲੀ *alloc_with_align(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl, size_t align,
			      size_t ਮਾਪ)
	MUST_NOT_HOLD(&hpctrl->ਤਾਲਾ)
{
	ਖਾਲੀ *ret = ਨਿਰਾ;
	ਬਣਤਰ ਢੇਰ_ਗੰਢ *n, *np;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸਭ_ਤੋਂ_ਨੀਵਾਂ_aligned;
	size_t pad;

	ਜੇ (!ਮਾਪ)
		ਵਾਪਸ ਨਿਰਾ;

	ਮਾਪ += align - 1;
	ਮਾਪ &= ~((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)align - 1);

	spin_ਤਾਲਾ(&hpctrl->ਤਾਲਾ);

	/* Ensure at least two free nodes are available for use below */
	ਜੇ (!alloc_nodes(hpctrl))
		ਜਾਓ ਬਾਹਰਲਾ;

	np = ਨਿਰਾ;
	sbi_list_for_each_ਇੰਦਰਾਜ(n, &hpctrl->free_space_list, ਸਿਰ) {
		ਸਭ_ਤੋਂ_ਨੀਵਾਂ_aligned = ROUNDUP(n->addr, align);
		pad = ਸਭ_ਤੋਂ_ਨੀਵਾਂ_aligned - n->addr;

		ਜੇ (ਮਾਪ + pad <= n->ਮਾਪ) {
			np = n;
			ਰੋਕੋ;
		}
	}
	ਜੇ (!np)
		ਜਾਓ ਬਾਹਰਲਾ;

	ਜੇ (pad) {
		n = sbi_list_ਪਹਿਲਾ_ਇੰਦਰਾਜ(&hpctrl->free_ਗੰਢ_list,
					 ਬਣਤਰ ਢੇਰ_ਗੰਢ, ਸਿਰ);
		sbi_list_del(&n->ਸਿਰ);

		n->addr = np->addr;
		n->ਮਾਪ = pad;
		sbi_list_ਜੋੜੋ_ਪੂਛ(&n->ਸਿਰ, &np->ਸਿਰ);

		np->addr += pad;
		np->ਮਾਪ -= pad;
	}

	ਜੇ (ਮਾਪ < np->ਮਾਪ) {
		n = sbi_list_ਪਹਿਲਾ_ਇੰਦਰਾਜ(&hpctrl->free_ਗੰਢ_list,
					 ਬਣਤਰ ਢੇਰ_ਗੰਢ, ਸਿਰ);
		sbi_list_del(&n->ਸਿਰ);

		n->addr = np->addr + ਮਾਪ;
		n->ਮਾਪ = np->ਮਾਪ - ਮਾਪ;
		sbi_list_ਜੋੜੋ(&n->ਸਿਰ, &np->ਸਿਰ);

		np->ਮਾਪ = ਮਾਪ;
	}

	sbi_list_del(&np->ਸਿਰ);
	sbi_list_ਜੋੜੋ_ਪੂਛ(&np->ਸਿਰ, &hpctrl->ਵਰਤਿਆ_space_list);
	ret = (ਖਾਲੀ *)np->addr;

ਬਾਹਰਲਾ:
	spin_unlock(&hpctrl->ਤਾਲਾ);

	ਵਾਪਸ ret;
}

ਖਾਲੀ *sbi_malloc_from(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl, size_t ਮਾਪ)
{
	ਵਾਪਸ alloc_with_align(hpctrl, ਢੇਰ_ALLOC_ALIGN, ਮਾਪ);
}

ਖਾਲੀ *sbi_aligned_alloc_from(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl,
			     size_t alignment, size_t ਮਾਪ)
{
	ਜੇ (alignment < ਢੇਰ_ALLOC_ALIGN)
		alignment = ਢੇਰ_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	ਜੇ ((alignment & (alignment - 1)) != 0)
		ਵਾਪਸ ਨਿਰਾ;

	/* Make sure size is multiple of alignment */
	ਜੇ (ਮਾਪ % alignment != 0)
		ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ alloc_with_align(hpctrl, alignment, ਮਾਪ);
}

ਖਾਲੀ *sbi_zalloc_from(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl, size_t ਮਾਪ)
{
	ਖਾਲੀ *ret = sbi_malloc_from(hpctrl, ਮਾਪ);

	ਜੇ (ret)
		sbi_memset(ret, 0, ਮਾਪ);
	ਵਾਪਸ ret;
}

ਖਾਲੀ sbi_free_from(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl, ਖਾਲੀ *ptr)
{
	ਬਣਤਰ ਢੇਰ_ਗੰਢ *n, *np;

	ਜੇ (!ptr)
		ਵਾਪਸ;

	spin_ਤਾਲਾ(&hpctrl->ਤਾਲਾ);

	np = ਨਿਰਾ;
	sbi_list_for_each_ਇੰਦਰਾਜ(n, &hpctrl->ਵਰਤਿਆ_space_list, ਸਿਰ) {
		ਜੇ ((n->addr <= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ptr) &&
		    ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ptr < (n->addr + n->ਮਾਪ))) {
			np = n;
			ਰੋਕੋ;
		}
	}
	ਜੇ (!np) {
		spin_unlock(&hpctrl->ਤਾਲਾ);
		ਵਾਪਸ;
	}

	sbi_list_del(&np->ਸਿਰ);

	sbi_list_for_each_ਇੰਦਰਾਜ(n, &hpctrl->free_space_list, ਸਿਰ) {
		ਜੇ ((np->addr + np->ਮਾਪ) == n->addr) {
			n->addr = np->addr;
			n->ਮਾਪ += np->ਮਾਪ;
			sbi_list_ਜੋੜੋ_ਪੂਛ(&np->ਸਿਰ, &hpctrl->free_ਗੰਢ_list);
			np = ਨਿਰਾ;
			ਰੋਕੋ;
		} ਨਹੀਂਤਾਂ ਜੇ (np->addr == (n->addr + n->ਮਾਪ)) {
			n->ਮਾਪ += np->ਮਾਪ;
			sbi_list_ਜੋੜੋ_ਪੂਛ(&np->ਸਿਰ, &hpctrl->free_ਗੰਢ_list);
			np = ਨਿਰਾ;
			ਰੋਕੋ;
		} ਨਹੀਂਤਾਂ ਜੇ ((n->addr + n->ਮਾਪ) < np->addr) {
			sbi_list_ਜੋੜੋ(&np->ਸਿਰ, &n->ਸਿਰ);
			np = ਨਿਰਾ;
			ਰੋਕੋ;
		}
	}
	ਜੇ (np)
		sbi_list_ਜੋੜੋ_ਪੂਛ(&np->ਸਿਰ, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->ਤਾਲਾ);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_ਢੇਰ_free_space_from(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl)
{
	ਬਣਤਰ ਢੇਰ_ਗੰਢ *n;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ret = 0;

	spin_ਤਾਲਾ(&hpctrl->ਤਾਲਾ);
	sbi_list_for_each_ਇੰਦਰਾਜ(n, &hpctrl->free_space_list, ਸਿਰ)
		ret += n->ਮਾਪ;
	spin_unlock(&hpctrl->ਤਾਲਾ);

	ਵਾਪਸ ret;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_ਢੇਰ_ਵਰਤਿਆ_space_from(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ਵਾਪਸ hpctrl->ਮਾਪ - hpctrl->resv - sbi_ਢੇਰ_free_space();
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_ਢੇਰ_reserved_space_from(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ਵਾਪਸ hpctrl->resv;
}

ਪੂਰਨ sbi_ਢੇਰ_init_new(ਬਣਤਰ sbi_ਢੇਰ_control *hpctrl, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2,
		      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ) NO_THREAD_SAFETY_ANALYSIS
{
	ਬਣਤਰ ਢੇਰ_ਗੰਢ *n;

	/* Initialize heap control */
	SPIN_ਤਾਲਾ_INIT(hpctrl->ਤਾਲਾ);
	hpctrl->ਅਧਾਰ_2 = ਅਧਾਰ_2;
	hpctrl->ਮਾਪ = ਮਾਪ;
	hpctrl->resv = 0;
	SBI_INIT_LIST_ਸਿਰ(&hpctrl->free_ਗੰਢ_list);
	SBI_INIT_LIST_ਸਿਰ(&hpctrl->free_space_list);
	SBI_INIT_LIST_ਸਿਰ(&hpctrl->ਵਰਤਿਆ_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_ਗੰਢ;
	n->addr = ਅਧਾਰ_2;
	n->ਮਾਪ = ਮਾਪ;
	sbi_list_ਜੋੜੋ_ਪੂਛ(&n->ਸਿਰ, &hpctrl->free_space_list);

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_ਢੇਰ_init(ਬਣਤਰ sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	ਜੇ (!scratch->fw_ਢੇਰ_ਮਾਪ ||
	    (scratch->fw_ਢੇਰ_ਮਾਪ & (ਢੇਰ_ਅਧਾਰ_ALIGN - 1)) ||
	    (scratch->fw_ਢੇਰ_ਵਿੱਥ < scratch->fw_rw_ਵਿੱਥ) ||
	    (scratch->fw_ਮਾਪ < (scratch->fw_ਢੇਰ_ਵਿੱਥ + scratch->fw_ਢੇਰ_ਮਾਪ)) ||
	    (scratch->fw_ਢੇਰ_ਵਿੱਥ & (ਢੇਰ_ਅਧਾਰ_ALIGN - 1)))
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ sbi_ਢੇਰ_init_new(&global_hpctrl,
				  scratch->fw_ਸ਼ੁਰੂ + scratch->fw_ਢੇਰ_ਵਿੱਥ,
				  scratch->fw_ਢੇਰ_ਮਾਪ);
}

ਪੂਰਨ sbi_ਢੇਰ_alloc_new(ਬਣਤਰ sbi_ਢੇਰ_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, ਆਕਾਰ(ਬਣਤਰ sbi_ਢੇਰ_control));
	ਵਾਪਸ 0;
}
