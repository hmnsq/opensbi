/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_list.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_ગાંઠ_BATCH_SIZE		8

સંરચના heap_ગાંઠ {
	સંરચના sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

સંરચના sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	સંરચના sbi_dlist free_ગાંઠ_list GUARDED_BY(&lock);
	સંરચના sbi_dlist free_space_list GUARDED_BY(&lock);
	સંરચના sbi_dlist used_space_list GUARDED_BY(&lock);
	સંરચના heap_ગાંઠ init_free_space_ગાંઠ GUARDED_BY(&lock);
};

સંરચના sbi_heap_control global_hpctrl;

સ્થિર bool alloc_nodes(સંરચના sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_ગાંઠ_BATCH_SIZE * કદ(સંરચના heap_ગાંઠ);
	સંરચના heap_ગાંઠ *n, *new = ખાલી;

	/* alloc_with_align() requires at most two free nodes */
	જો (hpctrl->free_ગાંઠ_list.next != hpctrl->free_ગાંઠ_list.prev)
		પાછા true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		જો (n->size >= size) {
			n->size -= size;
			જો (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_ગાંઠ_list);
			}
			new = (શૂન્ય *)(n->addr + n->size);
			તોડો;
		}
	}
	જો (!new)
		પાછા false;

	માટે (size_t i = 0; i < HEAP_ગાંઠ_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_ગાંઠ_list);
	hpctrl->resv += size;

	પાછા true;
}

સ્થિર શૂન્ય *alloc_with_align(સંરચના sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	શૂન્ય *ret = ખાલી;
	સંરચના heap_ગાંઠ *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	જો (!size)
		પાછા ખાલી;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	જો (!alloc_nodes(hpctrl))
		જાઓ out;

	np = ખાલી;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		જો (size + pad <= n->size) {
			np = n;
			તોડો;
		}
	}
	જો (!np)
		જાઓ out;

	જો (pad) {
		n = sbi_list_first_entry(&hpctrl->free_ગાંઠ_list,
					 સંરચના heap_ગાંઠ, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	જો (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_ગાંઠ_list,
					 સંરચના heap_ગાંઠ, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (શૂન્ય *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	પાછા ret;
}

શૂન્ય *sbi_malloc_from(સંરચના sbi_heap_control *hpctrl, size_t size)
{
	પાછા alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

શૂન્ય *sbi_aligned_alloc_from(સંરચના sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	જો (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	જો ((alignment & (alignment - 1)) != 0)
		પાછા ખાલી;

	/* Make sure size is multiple of alignment */
	જો (size % alignment != 0)
		પાછા ખાલી;

	પાછા alloc_with_align(hpctrl, alignment, size);
}

શૂન્ય *sbi_zalloc_from(સંરચના sbi_heap_control *hpctrl, size_t size)
{
	શૂન્ય *ret = sbi_malloc_from(hpctrl, size);

	જો (ret)
		sbi_memset(ret, 0, size);
	પાછા ret;
}

શૂન્ય sbi_free_from(સંરચના sbi_heap_control *hpctrl, શૂન્ય *ptr)
{
	સંરચના heap_ગાંઠ *n, *np;

	જો (!ptr)
		પાછા;

	spin_lock(&hpctrl->lock);

	np = ખાલી;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		જો ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			તોડો;
		}
	}
	જો (!np) {
		spin_unlock(&hpctrl->lock);
		પાછા;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		જો ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_ગાંઠ_list);
			np = ખાલી;
			તોડો;
		} નહીંતર જો (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_ગાંઠ_list);
			np = ખાલી;
			તોડો;
		} નહીંતર જો ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = ખાલી;
			તોડો;
		}
	}
	જો (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(સંરચના sbi_heap_control *hpctrl)
{
	સંરચના heap_ગાંઠ *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	પાછા ret;
}

unsigned long sbi_heap_used_space_from(સંરચના sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	પાછા hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(સંરચના sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	પાછા hpctrl->resv;
}

પૂર્ણાંક sbi_heap_init_new(સંરચના sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	સંરચના heap_ગાંઠ *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_ગાંઠ_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_ગાંઠ;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	પાછા 0;
}

પૂર્ણાંક sbi_heap_init(સંરચના sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	જો (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		પાછા SBI_EINVAL;

	પાછા sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

પૂર્ણાંક sbi_heap_alloc_new(સંરચના sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, કદ(સંરચના sbi_heap_control));
	પાછા 0;
}
