/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/riscv_locks.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_list.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_গ্রন্থি_BATCH_SIZE		8

সংরচনা heap_গ্রন্থি {
	সংরচনা sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

সংরচনা sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	সংরচনা sbi_dlist free_গ্রন্থি_list GUARDED_BY(&lock);
	সংরচনা sbi_dlist free_space_list GUARDED_BY(&lock);
	সংরচনা sbi_dlist used_space_list GUARDED_BY(&lock);
	সংরচনা heap_গ্রন্থি init_free_space_গ্রন্থি GUARDED_BY(&lock);
};

সংরচনা sbi_heap_control global_hpctrl;

স্থির bool alloc_nodes(সংরচনা sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_গ্রন্থি_BATCH_SIZE * পরিমাণ(সংরচনা heap_গ্রন্থি);
	সংরচনা heap_গ্রন্থি *n, *new = রিক্ত;

	/* alloc_with_align() requires at most two free nodes */
	যদি (hpctrl->free_গ্রন্থি_list.next != hpctrl->free_গ্রন্থি_list.prev)
		প্রত্যাগম true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		যদি (n->size >= size) {
			n->size -= size;
			যদি (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_গ্রন্থি_list);
			}
			new = (শূন্য *)(n->addr + n->size);
			ভঙ্গ;
		}
	}
	যদি (!new)
		প্রত্যাগম false;

	প্রতি (size_t i = 0; i < HEAP_গ্রন্থি_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_গ্রন্থি_list);
	hpctrl->resv += size;

	প্রত্যাগম true;
}

স্থির শূন্য *alloc_with_align(সংরচনা sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	শূন্য *ret = রিক্ত;
	সংরচনা heap_গ্রন্থি *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	যদি (!size)
		প্রত্যাগম রিক্ত;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	যদি (!alloc_nodes(hpctrl))
		গচ্ছ out;

	np = রিক্ত;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		যদি (size + pad <= n->size) {
			np = n;
			ভঙ্গ;
		}
	}
	যদি (!np)
		গচ্ছ out;

	যদি (pad) {
		n = sbi_list_first_entry(&hpctrl->free_গ্রন্থি_list,
					 সংরচনা heap_গ্রন্থি, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	যদি (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_গ্রন্থি_list,
					 সংরচনা heap_গ্রন্থি, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (শূন্য *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	প্রত্যাগম ret;
}

শূন্য *sbi_malloc_from(সংরচনা sbi_heap_control *hpctrl, size_t size)
{
	প্রত্যাগম alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

শূন্য *sbi_aligned_alloc_from(সংরচনা sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	যদি (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	যদি ((alignment & (alignment - 1)) != 0)
		প্রত্যাগম রিক্ত;

	/* Make sure size is multiple of alignment */
	যদি (size % alignment != 0)
		প্রত্যাগম রিক্ত;

	প্রত্যাগম alloc_with_align(hpctrl, alignment, size);
}

শূন্য *sbi_zalloc_from(সংরচনা sbi_heap_control *hpctrl, size_t size)
{
	শূন্য *ret = sbi_malloc_from(hpctrl, size);

	যদি (ret)
		sbi_memset(ret, 0, size);
	প্রত্যাগম ret;
}

শূন্য sbi_free_from(সংরচনা sbi_heap_control *hpctrl, শূন্য *ptr)
{
	সংরচনা heap_গ্রন্থি *n, *np;

	যদি (!ptr)
		প্রত্যাগম;

	spin_lock(&hpctrl->lock);

	np = রিক্ত;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		যদি ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			ভঙ্গ;
		}
	}
	যদি (!np) {
		spin_unlock(&hpctrl->lock);
		প্রত্যাগম;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		যদি ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_গ্রন্থি_list);
			np = রিক্ত;
			ভঙ্গ;
		} অন্যথা যদি (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_গ্রন্থি_list);
			np = রিক্ত;
			ভঙ্গ;
		} অন্যথা যদি ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = রিক্ত;
			ভঙ্গ;
		}
	}
	যদি (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(সংরচনা sbi_heap_control *hpctrl)
{
	সংরচনা heap_গ্রন্থি *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	প্রত্যাগম ret;
}

unsigned long sbi_heap_used_space_from(সংরচনা sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	প্রত্যাগম hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(সংরচনা sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	প্রত্যাগম hpctrl->resv;
}

পূর্ণাঙ্ক sbi_heap_init_new(সংরচনা sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	সংরচনা heap_গ্রন্থি *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_গ্রন্থি_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_গ্রন্থি;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_heap_init(সংরচনা sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	যদি (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		প্রত্যাগম SBI_EINVAL;

	প্রত্যাগম sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

পূর্ণাঙ্ক sbi_heap_alloc_new(সংরচনা sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, পরিমাণ(সংরচনা sbi_heap_control));
	প্রত্যাগম 0;
}
