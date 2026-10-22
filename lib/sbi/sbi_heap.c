/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_list.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_عقدة_BATCH_SIZE		8

بنية heap_عقدة {
	بنية sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

بنية sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	بنية sbi_dlist free_عقدة_list GUARDED_BY(&lock);
	بنية sbi_dlist free_space_list GUARDED_BY(&lock);
	بنية sbi_dlist used_space_list GUARDED_BY(&lock);
	بنية heap_عقدة init_free_space_عقدة GUARDED_BY(&lock);
};

بنية sbi_heap_control global_hpctrl;

ساكن bool alloc_nodes(بنية sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_عقدة_BATCH_SIZE * حجم(بنية heap_عقدة);
	بنية heap_عقدة *n, *new = عدم;

	/* alloc_with_align() requires at most two free nodes */
	إذا (hpctrl->free_عقدة_list.next != hpctrl->free_عقدة_list.prev)
		إرجاع true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		إذا (n->size >= size) {
			n->size -= size;
			إذا (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_عقدة_list);
			}
			new = (فراغ *)(n->addr + n->size);
			اكسر;
		}
	}
	إذا (!new)
		إرجاع false;

	لكل (size_t i = 0; i < HEAP_عقدة_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_عقدة_list);
	hpctrl->resv += size;

	إرجاع true;
}

ساكن فراغ *alloc_with_align(بنية sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	فراغ *ret = عدم;
	بنية heap_عقدة *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	إذا (!size)
		إرجاع عدم;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	إذا (!alloc_nodes(hpctrl))
		اذهب out;

	np = عدم;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		إذا (size + pad <= n->size) {
			np = n;
			اكسر;
		}
	}
	إذا (!np)
		اذهب out;

	إذا (pad) {
		n = sbi_list_first_entry(&hpctrl->free_عقدة_list,
					 بنية heap_عقدة, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	إذا (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_عقدة_list,
					 بنية heap_عقدة, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (فراغ *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	إرجاع ret;
}

فراغ *sbi_malloc_from(بنية sbi_heap_control *hpctrl, size_t size)
{
	إرجاع alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

فراغ *sbi_aligned_alloc_from(بنية sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	إذا (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	إذا ((alignment & (alignment - 1)) != 0)
		إرجاع عدم;

	/* Make sure size is multiple of alignment */
	إذا (size % alignment != 0)
		إرجاع عدم;

	إرجاع alloc_with_align(hpctrl, alignment, size);
}

فراغ *sbi_zalloc_from(بنية sbi_heap_control *hpctrl, size_t size)
{
	فراغ *ret = sbi_malloc_from(hpctrl, size);

	إذا (ret)
		sbi_memset(ret, 0, size);
	إرجاع ret;
}

فراغ sbi_free_from(بنية sbi_heap_control *hpctrl, فراغ *ptr)
{
	بنية heap_عقدة *n, *np;

	إذا (!ptr)
		إرجاع;

	spin_lock(&hpctrl->lock);

	np = عدم;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		إذا ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			اكسر;
		}
	}
	إذا (!np) {
		spin_unlock(&hpctrl->lock);
		إرجاع;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		إذا ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_عقدة_list);
			np = عدم;
			اكسر;
		} وإلا إذا (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_عقدة_list);
			np = عدم;
			اكسر;
		} وإلا إذا ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = عدم;
			اكسر;
		}
	}
	إذا (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(بنية sbi_heap_control *hpctrl)
{
	بنية heap_عقدة *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	إرجاع ret;
}

unsigned long sbi_heap_used_space_from(بنية sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	إرجاع hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(بنية sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	إرجاع hpctrl->resv;
}

صحيح sbi_heap_init_new(بنية sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	بنية heap_عقدة *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_عقدة_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_عقدة;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	إرجاع 0;
}

صحيح sbi_heap_init(بنية sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	إذا (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		إرجاع SBI_EINVAL;

	إرجاع sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

صحيح sbi_heap_alloc_new(بنية sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, حجم(بنية sbi_heap_control));
	إرجاع 0;
}
