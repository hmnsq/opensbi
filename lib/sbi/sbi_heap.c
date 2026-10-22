/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_list.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಗಂಟು_BATCH_ಗಾತ್ರ		8

ರಚನೆ ರಾಶಿ_ಗಂಟು {
	ರಚನೆ sbi_dlist ತಲೆ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2;
};

ರಚನೆ sbi_ರಾಶಿ_control {
	spinlock_t ಬೀಗ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2 GUARDED_BY(&ಬೀಗ_2);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2 GUARDED_BY(&ಬೀಗ_2);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ resv GUARDED_BY(&ಬೀಗ_2);
	ರಚನೆ sbi_dlist free_ಗಂಟು_list GUARDED_BY(&ಬೀಗ_2);
	ರಚನೆ sbi_dlist free_space_list GUARDED_BY(&ಬೀಗ_2);
	ರಚನೆ sbi_dlist ಬಳಸಿದ_space_list GUARDED_BY(&ಬೀಗ_2);
	ರಚನೆ ರಾಶಿ_ಗಂಟು init_free_space_ಗಂಟು GUARDED_BY(&ಬೀಗ_2);
};

ರಚನೆ sbi_ರಾಶಿ_control global_hpctrl;

ಸ್ಥಿರ bool alloc_nodes(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl) MUST_HOLD(&hpctrl->ಬೀಗ_2)
{
	size_t ಗಾತ್ರ_2 = ರಾಶಿ_ಗಂಟು_BATCH_ಗಾತ್ರ * ಗಾತ್ರ(ರಚನೆ ರಾಶಿ_ಗಂಟು);
	ರಚನೆ ರಾಶಿ_ಗಂಟು *n, *new = ಖಾಲಿ;

	/* alloc_with_align() requires at most two free nodes */
	ಆದರೆ (hpctrl->free_ಗಂಟು_list.ಮುಂದಿನ != hpctrl->free_ಗಂಟು_list.prev)
		ಹಿಂತಿರುಗಿ true;

	sbi_list_for_each_ನಮೂದು_ಹಿಮ್ಮುಖ(n, &hpctrl->free_space_list, ತಲೆ) {
		ಆದರೆ (n->ಗಾತ್ರ_2 >= ಗಾತ್ರ_2) {
			n->ಗಾತ್ರ_2 -= ಗಾತ್ರ_2;
			ಆದರೆ (!n->ಗಾತ್ರ_2) {
				sbi_list_del(&n->ತಲೆ);
				sbi_list_ಕೂಡಿಸು_ಬಾಲ(&n->ತಲೆ, &hpctrl->free_ಗಂಟು_list);
			}
			new = (ಶೂನ್ಯ *)(n->addr + n->ಗಾತ್ರ_2);
			ನಿಲ್ಲಿಸು;
		}
	}
	ಆದರೆ (!new)
		ಹಿಂತಿರುಗಿ false;

	ಗಾಗಿ (size_t i = 0; i < ರಾಶಿ_ಗಂಟು_BATCH_ಗಾತ್ರ; i++)
		sbi_list_ಕೂಡಿಸು_ಬಾಲ(&new[i].ತಲೆ, &hpctrl->free_ಗಂಟು_list);
	hpctrl->resv += ಗಾತ್ರ_2;

	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಶೂನ್ಯ *alloc_with_align(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl, size_t align,
			      size_t ಗಾತ್ರ_2)
	MUST_NOT_HOLD(&hpctrl->ಬೀಗ_2)
{
	ಶೂನ್ಯ *ret = ಖಾಲಿ;
	ರಚನೆ ರಾಶಿ_ಗಂಟು *n, *np;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಅತಿಕಡಿಮೆ_aligned;
	size_t pad;

	ಆದರೆ (!ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಗಾತ್ರ_2 += align - 1;
	ಗಾತ್ರ_2 &= ~((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)align - 1);

	spin_ಬೀಗ(&hpctrl->ಬೀಗ_2);

	/* Ensure at least two free nodes are available for use below */
	ಆದರೆ (!alloc_nodes(hpctrl))
		ಹೋಗು ಹೊರಹೋಗು;

	np = ಖಾಲಿ;
	sbi_list_for_each_ನಮೂದು(n, &hpctrl->free_space_list, ತಲೆ) {
		ಅತಿಕಡಿಮೆ_aligned = ROUNDUP(n->addr, align);
		pad = ಅತಿಕಡಿಮೆ_aligned - n->addr;

		ಆದರೆ (ಗಾತ್ರ_2 + pad <= n->ಗಾತ್ರ_2) {
			np = n;
			ನಿಲ್ಲಿಸು;
		}
	}
	ಆದರೆ (!np)
		ಹೋಗು ಹೊರಹೋಗು;

	ಆದರೆ (pad) {
		n = sbi_list_ಮೊದಲ_ನಮೂದು(&hpctrl->free_ಗಂಟು_list,
					 ರಚನೆ ರಾಶಿ_ಗಂಟು, ತಲೆ);
		sbi_list_del(&n->ತಲೆ);

		n->addr = np->addr;
		n->ಗಾತ್ರ_2 = pad;
		sbi_list_ಕೂಡಿಸು_ಬಾಲ(&n->ತಲೆ, &np->ತಲೆ);

		np->addr += pad;
		np->ಗಾತ್ರ_2 -= pad;
	}

	ಆದರೆ (ಗಾತ್ರ_2 < np->ಗಾತ್ರ_2) {
		n = sbi_list_ಮೊದಲ_ನಮೂದು(&hpctrl->free_ಗಂಟು_list,
					 ರಚನೆ ರಾಶಿ_ಗಂಟು, ತಲೆ);
		sbi_list_del(&n->ತಲೆ);

		n->addr = np->addr + ಗಾತ್ರ_2;
		n->ಗಾತ್ರ_2 = np->ಗಾತ್ರ_2 - ಗಾತ್ರ_2;
		sbi_list_ಕೂಡಿಸು(&n->ತಲೆ, &np->ತಲೆ);

		np->ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
	}

	sbi_list_del(&np->ತಲೆ);
	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&np->ತಲೆ, &hpctrl->ಬಳಸಿದ_space_list);
	ret = (ಶೂನ್ಯ *)np->addr;

ಹೊರಹೋಗು:
	spin_unlock(&hpctrl->ಬೀಗ_2);

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ *sbi_malloc_from(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl, size_t ಗಾತ್ರ_2)
{
	ಹಿಂತಿರುಗಿ alloc_with_align(hpctrl, ರಾಶಿ_ALLOC_ALIGN, ಗಾತ್ರ_2);
}

ಶೂನ್ಯ *sbi_aligned_alloc_from(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl,
			     size_t alignment, size_t ಗಾತ್ರ_2)
{
	ಆದರೆ (alignment < ರಾಶಿ_ALLOC_ALIGN)
		alignment = ರಾಶಿ_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	ಆದರೆ ((alignment & (alignment - 1)) != 0)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	/* Make sure size is multiple of alignment */
	ಆದರೆ (ಗಾತ್ರ_2 % alignment != 0)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ alloc_with_align(hpctrl, alignment, ಗಾತ್ರ_2);
}

ಶೂನ್ಯ *sbi_zalloc_from(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl, size_t ಗಾತ್ರ_2)
{
	ಶೂನ್ಯ *ret = sbi_malloc_from(hpctrl, ಗಾತ್ರ_2);

	ಆದರೆ (ret)
		sbi_memset(ret, 0, ಗಾತ್ರ_2);
	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ sbi_free_from(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl, ಶೂನ್ಯ *ptr)
{
	ರಚನೆ ರಾಶಿ_ಗಂಟು *n, *np;

	ಆದರೆ (!ptr)
		ಹಿಂತಿರುಗಿ;

	spin_ಬೀಗ(&hpctrl->ಬೀಗ_2);

	np = ಖಾಲಿ;
	sbi_list_for_each_ನಮೂದು(n, &hpctrl->ಬಳಸಿದ_space_list, ತಲೆ) {
		ಆದರೆ ((n->addr <= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ptr) &&
		    ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ptr < (n->addr + n->ಗಾತ್ರ_2))) {
			np = n;
			ನಿಲ್ಲಿಸು;
		}
	}
	ಆದರೆ (!np) {
		spin_unlock(&hpctrl->ಬೀಗ_2);
		ಹಿಂತಿರುಗಿ;
	}

	sbi_list_del(&np->ತಲೆ);

	sbi_list_for_each_ನಮೂದು(n, &hpctrl->free_space_list, ತಲೆ) {
		ಆದರೆ ((np->addr + np->ಗಾತ್ರ_2) == n->addr) {
			n->addr = np->addr;
			n->ಗಾತ್ರ_2 += np->ಗಾತ್ರ_2;
			sbi_list_ಕೂಡಿಸು_ಬಾಲ(&np->ತಲೆ, &hpctrl->free_ಗಂಟು_list);
			np = ಖಾಲಿ;
			ನಿಲ್ಲಿಸು;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (np->addr == (n->addr + n->ಗಾತ್ರ_2)) {
			n->ಗಾತ್ರ_2 += np->ಗಾತ್ರ_2;
			sbi_list_ಕೂಡಿಸು_ಬಾಲ(&np->ತಲೆ, &hpctrl->free_ಗಂಟು_list);
			np = ಖಾಲಿ;
			ನಿಲ್ಲಿಸು;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((n->addr + n->ಗಾತ್ರ_2) < np->addr) {
			sbi_list_ಕೂಡಿಸು(&np->ತಲೆ, &n->ತಲೆ);
			np = ಖಾಲಿ;
			ನಿಲ್ಲಿಸು;
		}
	}
	ಆದರೆ (np)
		sbi_list_ಕೂಡಿಸು_ಬಾಲ(&np->ತಲೆ, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->ಬೀಗ_2);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_ರಾಶಿ_free_space_from(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl)
{
	ರಚನೆ ರಾಶಿ_ಗಂಟು *n;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ret = 0;

	spin_ಬೀಗ(&hpctrl->ಬೀಗ_2);
	sbi_list_for_each_ನಮೂದು(n, &hpctrl->free_space_list, ತಲೆ)
		ret += n->ಗಾತ್ರ_2;
	spin_unlock(&hpctrl->ಬೀಗ_2);

	ಹಿಂತಿರುಗಿ ret;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_ರಾಶಿ_ಬಳಸಿದ_space_from(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ಹಿಂತಿರುಗಿ hpctrl->ಗಾತ್ರ_2 - hpctrl->resv - sbi_ರಾಶಿ_free_space();
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_ರಾಶಿ_reserved_space_from(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ಹಿಂತಿರುಗಿ hpctrl->resv;
}

ಪೂರ್ಣಾಂಕ sbi_ರಾಶಿ_init_new(ರಚನೆ sbi_ರಾಶಿ_control *hpctrl, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2,
		      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2) NO_THREAD_SAFETY_ANALYSIS
{
	ರಚನೆ ರಾಶಿ_ಗಂಟು *n;

	/* Initialize heap control */
	SPIN_ಬೀಗ_INIT(hpctrl->ಬೀಗ_2);
	hpctrl->ತಳ_2 = ತಳ_2;
	hpctrl->ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
	hpctrl->resv = 0;
	SBI_INIT_LIST_ತಲೆ(&hpctrl->free_ಗಂಟು_list);
	SBI_INIT_LIST_ತಲೆ(&hpctrl->free_space_list);
	SBI_INIT_LIST_ತಲೆ(&hpctrl->ಬಳಸಿದ_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_ಗಂಟು;
	n->addr = ತಳ_2;
	n->ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&n->ತಲೆ, &hpctrl->free_space_list);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_ರಾಶಿ_init(ರಚನೆ sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	ಆದರೆ (!scratch->fw_ರಾಶಿ_ಗಾತ್ರ ||
	    (scratch->fw_ರಾಶಿ_ಗಾತ್ರ & (ರಾಶಿ_ತಳ_ALIGN - 1)) ||
	    (scratch->fw_ರಾಶಿ_ಸ್ಥಾನಾಂತರ < scratch->fw_rw_ಸ್ಥಾನಾಂತರ) ||
	    (scratch->fw_ಗಾತ್ರ < (scratch->fw_ರಾಶಿ_ಸ್ಥಾನಾಂತರ + scratch->fw_ರಾಶಿ_ಗಾತ್ರ)) ||
	    (scratch->fw_ರಾಶಿ_ಸ್ಥಾನಾಂತರ & (ರಾಶಿ_ತಳ_ALIGN - 1)))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ sbi_ರಾಶಿ_init_new(&global_hpctrl,
				  scratch->fw_ಆರಂಭ + scratch->fw_ರಾಶಿ_ಸ್ಥಾನಾಂತರ,
				  scratch->fw_ರಾಶಿ_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ sbi_ರಾಶಿ_alloc_new(ರಚನೆ sbi_ರಾಶಿ_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, ಗಾತ್ರ(ರಚನೆ sbi_ರಾಶಿ_control));
	ಹಿಂತಿರುಗಿ 0;
}
