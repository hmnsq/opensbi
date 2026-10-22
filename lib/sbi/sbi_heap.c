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
#define HEAP_NODE_BATCH_SIZE		8

بنية heap_node {
	بنية sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

بنية sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	بنية sbi_dlist free_node_list GUARDED_BY(&lock);
	بنية sbi_dlist free_space_list GUARDED_BY(&lock);
	بنية sbi_dlist used_space_list GUARDED_BY(&lock);
	بنية heap_node init_free_space_node GUARDED_BY(&lock);
};

بنية sbi_heap_control global_hpctrl;

ساكن bool alloc_nodes(بنية sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_NODE_BATCH_SIZE * حجم(بنية heap_node);
	بنية heap_node *n, *new = عدم;

	/* alloc_with_align() requires at most two free nodes */
	אם (hpctrl->free_node_list.next != hpctrl->free_node_list.prev)
		החזר true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		אם (n->size >= size) {
			n->size -= size;
			אם (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_node_list);
			}
			new = (فراغ *)(n->addr + n->size);
			اكسر;
		}
	}
	אם (!new)
		החזר false;

	עבור (size_t i = 0; i < HEAP_NODE_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_node_list);
	hpctrl->resv += size;

	החזר true;
}

ساكن فراغ *alloc_with_align(بنية sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	فراغ *ret = عدم;
	بنية heap_node *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	אם (!size)
		החזר عدم;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	אם (!alloc_nodes(hpctrl))
		اذهب out;

	np = عدم;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		אם (size + pad <= n->size) {
			np = n;
			اكسر;
		}
	}
	אם (!np)
		اذهب out;

	אם (pad) {
		n = sbi_list_first_entry(&hpctrl->free_node_list,
					 بنية heap_node, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	אם (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_node_list,
					 بنية heap_node, head);
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

	החזר ret;
}

فراغ *sbi_malloc_from(بنية sbi_heap_control *hpctrl, size_t size)
{
	החזר alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

فراغ *sbi_aligned_alloc_from(بنية sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	אם (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	אם ((alignment & (alignment - 1)) != 0)
		החזר عدم;

	/* Make sure size is multiple of alignment */
	אם (size % alignment != 0)
		החזר عدم;

	החזר alloc_with_align(hpctrl, alignment, size);
}

فراغ *sbi_zalloc_from(بنية sbi_heap_control *hpctrl, size_t size)
{
	فراغ *ret = sbi_malloc_from(hpctrl, size);

	אם (ret)
		sbi_memset(ret, 0, size);
	החזר ret;
}

فراغ sbi_free_from(بنية sbi_heap_control *hpctrl, فراغ *ptr)
{
	بنية heap_node *n, *np;

	אם (!ptr)
		החזר;

	spin_lock(&hpctrl->lock);

	np = عدم;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		אם ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			اكسر;
		}
	}
	אם (!np) {
		spin_unlock(&hpctrl->lock);
		החזר;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		אם ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_node_list);
			np = عدم;
			اكسر;
		} אחרת אם (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_node_list);
			np = عدم;
			اكسر;
		} אחרת אם ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = عدم;
			اكسر;
		}
	}
	אם (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(بنية sbi_heap_control *hpctrl)
{
	بنية heap_node *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	החזר ret;
}

unsigned long sbi_heap_used_space_from(بنية sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	החזר hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(بنية sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	החזר hpctrl->resv;
}

صحيح sbi_heap_init_new(بنية sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	بنية heap_node *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_node_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_node;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	החזר 0;
}

صحيح sbi_heap_init(بنية sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	אם (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		החזר SBI_EINVAL;

	החזר sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

صحيح sbi_heap_alloc_new(بنية sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, حجم(بنية sbi_heap_control));
	החזר 0;
}
