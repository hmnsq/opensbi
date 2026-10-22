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
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_ಗಂಟು_BATCH_SIZE		8

ಸಂರಚನಾ heap_ಗಂಟು {
	ಸಂರಚನಾ sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

ಸಂರಚನಾ sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	ಸಂರಚನಾ sbi_dlist free_ಗಂಟು_list GUARDED_BY(&lock);
	ಸಂರಚನಾ sbi_dlist free_space_list GUARDED_BY(&lock);
	ಸಂರಚನಾ sbi_dlist used_space_list GUARDED_BY(&lock);
	ಸಂರಚನಾ heap_ಗಂಟು init_free_space_ಗಂಟು GUARDED_BY(&lock);
};

ಸಂರಚನಾ sbi_heap_control global_hpctrl;

ಸ್ಥಿರ bool alloc_nodes(ಸಂರಚನಾ sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_ಗಂಟು_BATCH_SIZE * ಪರಿಮಾಣ(ಸಂರಚನಾ heap_ಗಂಟು);
	ಸಂರಚನಾ heap_ಗಂಟು *n, *new = ರಿಕ್ತ;

	/* alloc_with_align() requires at most two free nodes */
	ಯದಿ (hpctrl->free_ಗಂಟು_list.next != hpctrl->free_ಗಂಟು_list.prev)
		ಪ್ರತ್ಯಾಗಮ true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		ಯದಿ (n->size >= size) {
			n->size -= size;
			ಯದಿ (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_ಗಂಟು_list);
			}
			new = (ಶೂನ್ಯ *)(n->addr + n->size);
			ಭಙ್ಗ;
		}
	}
	ಯದಿ (!new)
		ಪ್ರತ್ಯಾಗಮ false;

	ಪ್ರತಿ (size_t i = 0; i < HEAP_ಗಂಟು_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_ಗಂಟು_list);
	hpctrl->resv += size;

	ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಶೂನ್ಯ *alloc_with_align(ಸಂರಚನಾ sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	ಶೂನ್ಯ *ret = ರಿಕ್ತ;
	ಸಂರಚನಾ heap_ಗಂಟು *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	ಯದಿ (!size)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	ಯದಿ (!alloc_nodes(hpctrl))
		ಗಚ್ಛ out;

	np = ರಿಕ್ತ;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		ಯದಿ (size + pad <= n->size) {
			np = n;
			ಭಙ್ಗ;
		}
	}
	ಯದಿ (!np)
		ಗಚ್ಛ out;

	ಯದಿ (pad) {
		n = sbi_list_first_entry(&hpctrl->free_ಗಂಟು_list,
					 ಸಂರಚನಾ heap_ಗಂಟು, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	ಯದಿ (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_ಗಂಟು_list,
					 ಸಂರಚನಾ heap_ಗಂಟು, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (ಶೂನ್ಯ *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ *sbi_malloc_from(ಸಂರಚನಾ sbi_heap_control *hpctrl, size_t size)
{
	ಪ್ರತ್ಯಾಗಮ alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

ಶೂನ್ಯ *sbi_aligned_alloc_from(ಸಂರಚನಾ sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	ಯದಿ (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	ಯದಿ ((alignment & (alignment - 1)) != 0)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	/* Make sure size is multiple of alignment */
	ಯದಿ (size % alignment != 0)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ alloc_with_align(hpctrl, alignment, size);
}

ಶೂನ್ಯ *sbi_zalloc_from(ಸಂರಚನಾ sbi_heap_control *hpctrl, size_t size)
{
	ಶೂನ್ಯ *ret = sbi_malloc_from(hpctrl, size);

	ಯದಿ (ret)
		sbi_memset(ret, 0, size);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ sbi_free_from(ಸಂರಚನಾ sbi_heap_control *hpctrl, ಶೂನ್ಯ *ptr)
{
	ಸಂರಚನಾ heap_ಗಂಟು *n, *np;

	ಯದಿ (!ptr)
		ಪ್ರತ್ಯಾಗಮ;

	spin_lock(&hpctrl->lock);

	np = ರಿಕ್ತ;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		ಯದಿ ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			ಭಙ್ಗ;
		}
	}
	ಯದಿ (!np) {
		spin_unlock(&hpctrl->lock);
		ಪ್ರತ್ಯಾಗಮ;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		ಯದಿ ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_ಗಂಟು_list);
			np = ರಿಕ್ತ;
			ಭಙ್ಗ;
		} ಅನ್ಯಥಾ ಯದಿ (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_ಗಂಟು_list);
			np = ರಿಕ್ತ;
			ಭಙ್ಗ;
		} ಅನ್ಯಥಾ ಯದಿ ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = ರಿಕ್ತ;
			ಭಙ್ಗ;
		}
	}
	ಯದಿ (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(ಸಂರಚನಾ sbi_heap_control *hpctrl)
{
	ಸಂರಚನಾ heap_ಗಂಟು *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	ಪ್ರತ್ಯಾಗಮ ret;
}

unsigned long sbi_heap_used_space_from(ಸಂರಚನಾ sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ಪ್ರತ್ಯಾಗಮ hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(ಸಂರಚನಾ sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ಪ್ರತ್ಯಾಗಮ hpctrl->resv;
}

ಪೂರ್ಣಾಙ್ಕ sbi_heap_init_new(ಸಂರಚನಾ sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	ಸಂರಚನಾ heap_ಗಂಟು *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_ಗಂಟು_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_ಗಂಟು;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_heap_init(ಸಂರಚನಾ sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	ಯದಿ (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತ್ಯಾಗಮ sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

ಪೂರ್ಣಾಙ್ಕ sbi_heap_alloc_new(ಸಂರಚನಾ sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ sbi_heap_control));
	ಪ್ರತ್ಯಾಗಮ 0;
}
