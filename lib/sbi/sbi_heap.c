/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_list.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_ଗଣ୍ଠି_BATCH_SIZE		8

ସଂରଚନା heap_ଗଣ୍ଠି {
	ସଂରଚନା sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

ସଂରଚନା sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	ସଂରଚନା sbi_dlist free_ଗଣ୍ଠି_list GUARDED_BY(&lock);
	ସଂରଚନା sbi_dlist free_space_list GUARDED_BY(&lock);
	ସଂରଚନା sbi_dlist used_space_list GUARDED_BY(&lock);
	ସଂରଚନା heap_ଗଣ୍ଠି init_free_space_ଗଣ୍ଠି GUARDED_BY(&lock);
};

ସଂରଚନା sbi_heap_control global_hpctrl;

ସ୍ଥିର bool alloc_nodes(ସଂରଚନା sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_ଗଣ୍ଠି_BATCH_SIZE * ଆକାର(ସଂରଚନା heap_ଗଣ୍ଠି);
	ସଂରଚନା heap_ଗଣ୍ଠି *n, *new = ଖାଲି;

	/* alloc_with_align() requires at most two free nodes */
	ଯଦି (hpctrl->free_ଗଣ୍ଠି_list.next != hpctrl->free_ଗଣ୍ଠି_list.prev)
		ଫେରନ୍ତୁ true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		ଯଦି (n->size >= size) {
			n->size -= size;
			ଯଦି (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_ଗଣ୍ଠି_list);
			}
			new = (ଶୂନ୍ୟ *)(n->addr + n->size);
			ଭାଙ୍ଗ;
		}
	}
	ଯଦି (!new)
		ଫେରନ୍ତୁ false;

	ପାଇଁ (size_t i = 0; i < HEAP_ଗଣ୍ଠି_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_ଗଣ୍ଠି_list);
	hpctrl->resv += size;

	ଫେରନ୍ତୁ true;
}

ସ୍ଥିର ଶୂନ୍ୟ *alloc_with_align(ସଂରଚନା sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	ଶୂନ୍ୟ *ret = ଖାଲି;
	ସଂରଚନା heap_ଗଣ୍ଠି *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	ଯଦି (!size)
		ଫେରନ୍ତୁ ଖାଲି;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	ଯଦି (!alloc_nodes(hpctrl))
		ଯାଅ out;

	np = ଖାଲି;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		ଯଦି (size + pad <= n->size) {
			np = n;
			ଭାଙ୍ଗ;
		}
	}
	ଯଦି (!np)
		ଯାଅ out;

	ଯଦି (pad) {
		n = sbi_list_first_entry(&hpctrl->free_ଗଣ୍ଠି_list,
					 ସଂରଚନା heap_ଗଣ୍ଠି, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	ଯଦି (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_ଗଣ୍ଠି_list,
					 ସଂରଚନା heap_ଗଣ୍ଠି, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (ଶୂନ୍ୟ *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ *sbi_malloc_from(ସଂରଚନା sbi_heap_control *hpctrl, size_t size)
{
	ଫେରନ୍ତୁ alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

ଶୂନ୍ୟ *sbi_aligned_alloc_from(ସଂରଚନା sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	ଯଦି (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	ଯଦି ((alignment & (alignment - 1)) != 0)
		ଫେରନ୍ତୁ ଖାଲି;

	/* Make sure size is multiple of alignment */
	ଯଦି (size % alignment != 0)
		ଫେରନ୍ତୁ ଖାଲି;

	ଫେରନ୍ତୁ alloc_with_align(hpctrl, alignment, size);
}

ଶୂନ୍ୟ *sbi_zalloc_from(ସଂରଚନା sbi_heap_control *hpctrl, size_t size)
{
	ଶୂନ୍ୟ *ret = sbi_malloc_from(hpctrl, size);

	ଯଦି (ret)
		sbi_memset(ret, 0, size);
	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ sbi_free_from(ସଂରଚନା sbi_heap_control *hpctrl, ଶୂନ୍ୟ *ptr)
{
	ସଂରଚନା heap_ଗଣ୍ଠି *n, *np;

	ଯଦି (!ptr)
		ଫେରନ୍ତୁ;

	spin_lock(&hpctrl->lock);

	np = ଖାଲି;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		ଯଦି ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			ଭାଙ୍ଗ;
		}
	}
	ଯଦି (!np) {
		spin_unlock(&hpctrl->lock);
		ଫେରନ୍ତୁ;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		ଯଦି ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_ଗଣ୍ଠି_list);
			np = ଖାଲି;
			ଭାଙ୍ଗ;
		} ଅନ୍ୟଥା ଯଦି (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_ଗଣ୍ଠି_list);
			np = ଖାଲି;
			ଭାଙ୍ଗ;
		} ଅନ୍ୟଥା ଯଦି ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = ଖାଲି;
			ଭାଙ୍ଗ;
		}
	}
	ଯଦି (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(ସଂରଚନା sbi_heap_control *hpctrl)
{
	ସଂରଚନା heap_ଗଣ୍ଠି *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	ଫେରନ୍ତୁ ret;
}

unsigned long sbi_heap_used_space_from(ସଂରଚନା sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ଫେରନ୍ତୁ hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(ସଂରଚନା sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ଫେରନ୍ତୁ hpctrl->resv;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_heap_init_new(ସଂରଚନା sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	ସଂରଚନା heap_ଗଣ୍ଠି *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_ଗଣ୍ଠି_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_ଗଣ୍ଠି;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_heap_init(ସଂରଚନା sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	ଯଦି (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_heap_alloc_new(ସଂରଚନା sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, ଆକାର(ସଂରଚନା sbi_heap_control));
	ଫେରନ୍ତୁ 0;
}
