/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#include <sbi/riscv_locks.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_list.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_NODE_BATCH_SIZE		8

struct heap_node {
	struct sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

struct sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	struct sbi_dlist free_node_list GUARDED_BY(&lock);
	struct sbi_dlist free_space_list GUARDED_BY(&lock);
	struct sbi_dlist used_space_list GUARDED_BY(&lock);
	struct heap_node init_free_space_node GUARDED_BY(&lock);
};

struct sbi_heap_control global_hpctrl;

static bool alloc_nodes(struct sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_NODE_BATCH_SIZE * sizeof(struct heap_node);
	struct heap_node *n, *new = NULL;

	/* alloc_with_align() requires at most two free nodes */
	nếu (hpctrl->free_node_list.next != hpctrl->free_node_list.prev)
		trả_về true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		nếu (n->size >= size) {
			n->size -= size;
			nếu (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_node_list);
			}
			new = (void *)(n->addr + n->size);
			break;
		}
	}
	nếu (!new)
		trả_về false;

	cho (size_t i = 0; i < HEAP_NODE_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_node_list);
	hpctrl->resv += size;

	trả_về true;
}

static void *alloc_with_align(struct sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	void *ret = NULL;
	struct heap_node *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	nếu (!size)
		trả_về NULL;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	nếu (!alloc_nodes(hpctrl))
		goto out;

	np = NULL;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		nếu (size + pad <= n->size) {
			np = n;
			break;
		}
	}
	nếu (!np)
		goto out;

	nếu (pad) {
		n = sbi_list_first_entry(&hpctrl->free_node_list,
					 struct heap_node, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	nếu (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_node_list,
					 struct heap_node, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (void *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	trả_về ret;
}

void *sbi_malloc_from(struct sbi_heap_control *hpctrl, size_t size)
{
	trả_về alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

void *sbi_aligned_alloc_from(struct sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	nếu (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	nếu ((alignment & (alignment - 1)) != 0)
		trả_về NULL;

	/* Make sure size is multiple of alignment */
	nếu (size % alignment != 0)
		trả_về NULL;

	trả_về alloc_with_align(hpctrl, alignment, size);
}

void *sbi_zalloc_from(struct sbi_heap_control *hpctrl, size_t size)
{
	void *ret = sbi_malloc_from(hpctrl, size);

	nếu (ret)
		sbi_memset(ret, 0, size);
	trả_về ret;
}

void sbi_free_from(struct sbi_heap_control *hpctrl, void *ptr)
{
	struct heap_node *n, *np;

	nếu (!ptr)
		trả_về;

	spin_lock(&hpctrl->lock);

	np = NULL;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		nếu ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			break;
		}
	}
	nếu (!np) {
		spin_unlock(&hpctrl->lock);
		trả_về;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		nếu ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_node_list);
			np = NULL;
			break;
		} không_thì nếu (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_node_list);
			np = NULL;
			break;
		} không_thì nếu ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = NULL;
			break;
		}
	}
	nếu (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(struct sbi_heap_control *hpctrl)
{
	struct heap_node *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	trả_về ret;
}

unsigned long sbi_heap_used_space_from(struct sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	trả_về hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(struct sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	trả_về hpctrl->resv;
}

số_nguyên sbi_heap_init_new(struct sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	struct heap_node *n;

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

	trả_về 0;
}

số_nguyên sbi_heap_init(struct sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	nếu (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		trả_về SBI_EINVAL;

	trả_về sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

số_nguyên sbi_heap_alloc_new(struct sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, sizeof(struct sbi_heap_control));
	trả_về 0;
}
