/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_list.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_گره_BATCH_SIZE		8

ساختار heap_گره {
	ساختار sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

ساختار sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	ساختار sbi_dlist free_گره_list GUARDED_BY(&lock);
	ساختار sbi_dlist free_space_list GUARDED_BY(&lock);
	ساختار sbi_dlist used_space_list GUARDED_BY(&lock);
	ساختار heap_گره init_free_space_گره GUARDED_BY(&lock);
};

ساختار sbi_heap_control global_hpctrl;

ایستا bool alloc_nodes(ساختار sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_گره_BATCH_SIZE * اندازه(ساختار heap_گره);
	ساختار heap_گره *n, *new = پوچ;

	/* alloc_with_align() requires at most two free nodes */
	اگر (hpctrl->free_گره_list.next != hpctrl->free_گره_list.prev)
		بازگشت true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		اگر (n->size >= size) {
			n->size -= size;
			اگر (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_گره_list);
			}
			new = (تهی *)(n->addr + n->size);
			بشکن;
		}
	}
	اگر (!new)
		بازگشت false;

	برای (size_t i = 0; i < HEAP_گره_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_گره_list);
	hpctrl->resv += size;

	بازگشت true;
}

ایستا تهی *alloc_with_align(ساختار sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	تهی *ret = پوچ;
	ساختار heap_گره *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	اگر (!size)
		بازگشت پوچ;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	اگر (!alloc_nodes(hpctrl))
		برو out;

	np = پوچ;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		اگر (size + pad <= n->size) {
			np = n;
			بشکن;
		}
	}
	اگر (!np)
		برو out;

	اگر (pad) {
		n = sbi_list_first_entry(&hpctrl->free_گره_list,
					 ساختار heap_گره, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	اگر (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_گره_list,
					 ساختار heap_گره, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (تهی *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	بازگشت ret;
}

تهی *sbi_malloc_from(ساختار sbi_heap_control *hpctrl, size_t size)
{
	بازگشت alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

تهی *sbi_aligned_alloc_from(ساختار sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	اگر (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	اگر ((alignment & (alignment - 1)) != 0)
		بازگشت پوچ;

	/* Make sure size is multiple of alignment */
	اگر (size % alignment != 0)
		بازگشت پوچ;

	بازگشت alloc_with_align(hpctrl, alignment, size);
}

تهی *sbi_zalloc_from(ساختار sbi_heap_control *hpctrl, size_t size)
{
	تهی *ret = sbi_malloc_from(hpctrl, size);

	اگر (ret)
		sbi_memset(ret, 0, size);
	بازگشت ret;
}

تهی sbi_free_from(ساختار sbi_heap_control *hpctrl, تهی *ptr)
{
	ساختار heap_گره *n, *np;

	اگر (!ptr)
		بازگشت;

	spin_lock(&hpctrl->lock);

	np = پوچ;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		اگر ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			بشکن;
		}
	}
	اگر (!np) {
		spin_unlock(&hpctrl->lock);
		بازگشت;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		اگر ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_گره_list);
			np = پوچ;
			بشکن;
		} وگرنه اگر (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_گره_list);
			np = پوچ;
			بشکن;
		} وگرنه اگر ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = پوچ;
			بشکن;
		}
	}
	اگر (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(ساختار sbi_heap_control *hpctrl)
{
	ساختار heap_گره *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	بازگشت ret;
}

unsigned long sbi_heap_used_space_from(ساختار sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	بازگشت hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(ساختار sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	بازگشت hpctrl->resv;
}

صحیح sbi_heap_init_new(ساختار sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	ساختار heap_گره *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_گره_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_گره;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	بازگشت 0;
}

صحیح sbi_heap_init(ساختار sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	اگر (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		بازگشت SBI_EINVAL;

	بازگشت sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

صحیح sbi_heap_alloc_new(ساختار sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, اندازه(ساختار sbi_heap_control));
	بازگشت 0;
}
