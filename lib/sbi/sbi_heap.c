/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_list.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_കണ്ണി_BATCH_SIZE		8

ഘടന heap_കണ്ണി {
	ഘടന sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

ഘടന sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	ഘടന sbi_dlist free_കണ്ണി_list GUARDED_BY(&lock);
	ഘടന sbi_dlist free_space_list GUARDED_BY(&lock);
	ഘടന sbi_dlist used_space_list GUARDED_BY(&lock);
	ഘടന heap_കണ്ണി init_free_space_കണ്ണി GUARDED_BY(&lock);
};

ഘടന sbi_heap_control global_hpctrl;

സ്ഥിതം bool alloc_nodes(ഘടന sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_കണ്ണി_BATCH_SIZE * വലുപ്പം(ഘടന heap_കണ്ണി);
	ഘടന heap_കണ്ണി *n, *new = ശൂന്യ;

	/* alloc_with_align() requires at most two free nodes */
	എങ്കിൽ (hpctrl->free_കണ്ണി_list.next != hpctrl->free_കണ്ണി_list.prev)
		തിരികെ true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		എങ്കിൽ (n->size >= size) {
			n->size -= size;
			എങ്കിൽ (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_കണ്ണി_list);
			}
			new = (ശൂന്യം *)(n->addr + n->size);
			നിർത്തുക;
		}
	}
	എങ്കിൽ (!new)
		തിരികെ false;

	വേണ്ടി (size_t i = 0; i < HEAP_കണ്ണി_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_കണ്ണി_list);
	hpctrl->resv += size;

	തിരികെ true;
}

സ്ഥിതം ശൂന്യം *alloc_with_align(ഘടന sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	ശൂന്യം *ret = ശൂന്യ;
	ഘടന heap_കണ്ണി *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	എങ്കിൽ (!size)
		തിരികെ ശൂന്യ;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	എങ്കിൽ (!alloc_nodes(hpctrl))
		പോകുക out;

	np = ശൂന്യ;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		എങ്കിൽ (size + pad <= n->size) {
			np = n;
			നിർത്തുക;
		}
	}
	എങ്കിൽ (!np)
		പോകുക out;

	എങ്കിൽ (pad) {
		n = sbi_list_first_entry(&hpctrl->free_കണ്ണി_list,
					 ഘടന heap_കണ്ണി, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	എങ്കിൽ (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_കണ്ണി_list,
					 ഘടന heap_കണ്ണി, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (ശൂന്യം *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	തിരികെ ret;
}

ശൂന്യം *sbi_malloc_from(ഘടന sbi_heap_control *hpctrl, size_t size)
{
	തിരികെ alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

ശൂന്യം *sbi_aligned_alloc_from(ഘടന sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	എങ്കിൽ (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	എങ്കിൽ ((alignment & (alignment - 1)) != 0)
		തിരികെ ശൂന്യ;

	/* Make sure size is multiple of alignment */
	എങ്കിൽ (size % alignment != 0)
		തിരികെ ശൂന്യ;

	തിരികെ alloc_with_align(hpctrl, alignment, size);
}

ശൂന്യം *sbi_zalloc_from(ഘടന sbi_heap_control *hpctrl, size_t size)
{
	ശൂന്യം *ret = sbi_malloc_from(hpctrl, size);

	എങ്കിൽ (ret)
		sbi_memset(ret, 0, size);
	തിരികെ ret;
}

ശൂന്യം sbi_free_from(ഘടന sbi_heap_control *hpctrl, ശൂന്യം *ptr)
{
	ഘടന heap_കണ്ണി *n, *np;

	എങ്കിൽ (!ptr)
		തിരികെ;

	spin_lock(&hpctrl->lock);

	np = ശൂന്യ;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		എങ്കിൽ ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			നിർത്തുക;
		}
	}
	എങ്കിൽ (!np) {
		spin_unlock(&hpctrl->lock);
		തിരികെ;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		എങ്കിൽ ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_കണ്ണി_list);
			np = ശൂന്യ;
			നിർത്തുക;
		} അല്ലെങ്കിൽ എങ്കിൽ (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_കണ്ണി_list);
			np = ശൂന്യ;
			നിർത്തുക;
		} അല്ലെങ്കിൽ എങ്കിൽ ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = ശൂന്യ;
			നിർത്തുക;
		}
	}
	എങ്കിൽ (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(ഘടന sbi_heap_control *hpctrl)
{
	ഘടന heap_കണ്ണി *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	തിരികെ ret;
}

unsigned long sbi_heap_used_space_from(ഘടന sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	തിരികെ hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(ഘടന sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	തിരികെ hpctrl->resv;
}

പൂർണ്ണസംഖ്യ sbi_heap_init_new(ഘടന sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	ഘടന heap_കണ്ണി *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_കണ്ണി_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_കണ്ണി;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_heap_init(ഘടന sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	എങ്കിൽ (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		തിരികെ SBI_EINVAL;

	തിരികെ sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

പൂർണ്ണസംഖ്യ sbi_heap_alloc_new(ഘടന sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, വലുപ്പം(ഘടന sbi_heap_control));
	തിരികെ 0;
}
