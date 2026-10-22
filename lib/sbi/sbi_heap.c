/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/riscv_locks.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_list.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_முனை_BATCH_SIZE		8

கட்டமைப்பு heap_முனை {
	கட்டமைப்பு sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

கட்டமைப்பு sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	கட்டமைப்பு sbi_dlist free_முனை_list GUARDED_BY(&lock);
	கட்டமைப்பு sbi_dlist free_space_list GUARDED_BY(&lock);
	கட்டமைப்பு sbi_dlist used_space_list GUARDED_BY(&lock);
	கட்டமைப்பு heap_முனை init_free_space_முனை GUARDED_BY(&lock);
};

கட்டமைப்பு sbi_heap_control global_hpctrl;

நிலையான bool alloc_nodes(கட்டமைப்பு sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_முனை_BATCH_SIZE * அளவு(கட்டமைப்பு heap_முனை);
	கட்டமைப்பு heap_முனை *n, *new = வெற்று;

	/* alloc_with_align() requires at most two free nodes */
	எனில் (hpctrl->free_முனை_list.next != hpctrl->free_முனை_list.prev)
		திரும்பு true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		எனில் (n->size >= size) {
			n->size -= size;
			எனில் (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_முனை_list);
			}
			new = (வெறுமை *)(n->addr + n->size);
			நிறுத்து;
		}
	}
	எனில் (!new)
		திரும்பு false;

	க்கு (size_t i = 0; i < HEAP_முனை_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_முனை_list);
	hpctrl->resv += size;

	திரும்பு true;
}

நிலையான வெறுமை *alloc_with_align(கட்டமைப்பு sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	வெறுமை *ret = வெற்று;
	கட்டமைப்பு heap_முனை *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	எனில் (!size)
		திரும்பு வெற்று;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	எனில் (!alloc_nodes(hpctrl))
		செல் out;

	np = வெற்று;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		எனில் (size + pad <= n->size) {
			np = n;
			நிறுத்து;
		}
	}
	எனில் (!np)
		செல் out;

	எனில் (pad) {
		n = sbi_list_first_entry(&hpctrl->free_முனை_list,
					 கட்டமைப்பு heap_முனை, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	எனில் (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_முனை_list,
					 கட்டமைப்பு heap_முனை, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (வெறுமை *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	திரும்பு ret;
}

வெறுமை *sbi_malloc_from(கட்டமைப்பு sbi_heap_control *hpctrl, size_t size)
{
	திரும்பு alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

வெறுமை *sbi_aligned_alloc_from(கட்டமைப்பு sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	எனில் (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	எனில் ((alignment & (alignment - 1)) != 0)
		திரும்பு வெற்று;

	/* Make sure size is multiple of alignment */
	எனில் (size % alignment != 0)
		திரும்பு வெற்று;

	திரும்பு alloc_with_align(hpctrl, alignment, size);
}

வெறுமை *sbi_zalloc_from(கட்டமைப்பு sbi_heap_control *hpctrl, size_t size)
{
	வெறுமை *ret = sbi_malloc_from(hpctrl, size);

	எனில் (ret)
		sbi_memset(ret, 0, size);
	திரும்பு ret;
}

வெறுமை sbi_free_from(கட்டமைப்பு sbi_heap_control *hpctrl, வெறுமை *ptr)
{
	கட்டமைப்பு heap_முனை *n, *np;

	எனில் (!ptr)
		திரும்பு;

	spin_lock(&hpctrl->lock);

	np = வெற்று;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		எனில் ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			நிறுத்து;
		}
	}
	எனில் (!np) {
		spin_unlock(&hpctrl->lock);
		திரும்பு;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		எனில் ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_முனை_list);
			np = வெற்று;
			நிறுத்து;
		} இல்லையெனில் எனில் (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_முனை_list);
			np = வெற்று;
			நிறுத்து;
		} இல்லையெனில் எனில் ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = வெற்று;
			நிறுத்து;
		}
	}
	எனில் (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(கட்டமைப்பு sbi_heap_control *hpctrl)
{
	கட்டமைப்பு heap_முனை *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	திரும்பு ret;
}

unsigned long sbi_heap_used_space_from(கட்டமைப்பு sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	திரும்பு hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(கட்டமைப்பு sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	திரும்பு hpctrl->resv;
}

முழு sbi_heap_init_new(கட்டமைப்பு sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	கட்டமைப்பு heap_முனை *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_முனை_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_முனை;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	திரும்பு 0;
}

முழு sbi_heap_init(கட்டமைப்பு sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	எனில் (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		திரும்பு SBI_EINVAL;

	திரும்பு sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

முழு sbi_heap_alloc_new(கட்டமைப்பு sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, அளவு(கட்டமைப்பு sbi_heap_control));
	திரும்பு 0;
}
