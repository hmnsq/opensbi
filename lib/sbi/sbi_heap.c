/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_list.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_ग्रन्थिः_BATCH_SIZE		8

संरचना heap_ग्रन्थिः {
	संरचना sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

संरचना sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	संरचना sbi_dlist free_ग्रन्थिः_list GUARDED_BY(&lock);
	संरचना sbi_dlist free_space_list GUARDED_BY(&lock);
	संरचना sbi_dlist used_space_list GUARDED_BY(&lock);
	संरचना heap_ग्रन्थिः init_free_space_ग्रन्थिः GUARDED_BY(&lock);
};

संरचना sbi_heap_control global_hpctrl;

स्थिर bool alloc_nodes(संरचना sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_ग्रन्थिः_BATCH_SIZE * परिमाण(संरचना heap_ग्रन्थिः);
	संरचना heap_ग्रन्थिः *n, *new = रिक्त;

	/* alloc_with_align() requires at most two free nodes */
	सचे (hpctrl->free_ग्रन्थिः_list.next != hpctrl->free_ग्रन्थिः_list.prev)
		पटिनिवत्त true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		सचे (n->size >= size) {
			n->size -= size;
			सचे (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_ग्रन्थिः_list);
			}
			new = (शून्य *)(n->addr + n->size);
			भङ्ग;
		}
	}
	सचे (!new)
		पटिनिवत्त false;

	अत्थाय (size_t i = 0; i < HEAP_ग्रन्थिः_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_ग्रन्थिः_list);
	hpctrl->resv += size;

	पटिनिवत्त true;
}

स्थिर शून्य *alloc_with_align(संरचना sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	शून्य *ret = रिक्त;
	संरचना heap_ग्रन्थिः *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	सचे (!size)
		पटिनिवत्त रिक्त;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	सचे (!alloc_nodes(hpctrl))
		गच्छ out;

	np = रिक्त;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		सचे (size + pad <= n->size) {
			np = n;
			भङ्ग;
		}
	}
	सचे (!np)
		गच्छ out;

	सचे (pad) {
		n = sbi_list_first_entry(&hpctrl->free_ग्रन्थिः_list,
					 संरचना heap_ग्रन्थिः, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	सचे (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_ग्रन्थिः_list,
					 संरचना heap_ग्रन्थिः, head);
		sbi_list_del(&n->head);

		n->addr = np->addr + size;
		n->size = np->size - size;
		sbi_list_add(&n->head, &np->head);

		np->size = size;
	}

	sbi_list_del(&np->head);
	sbi_list_add_tail(&np->head, &hpctrl->used_space_list);
	ret = (शून्य *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	पटिनिवत्त ret;
}

शून्य *sbi_malloc_from(संरचना sbi_heap_control *hpctrl, size_t size)
{
	पटिनिवत्त alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

शून्य *sbi_aligned_alloc_from(संरचना sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	सचे (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	सचे ((alignment & (alignment - 1)) != 0)
		पटिनिवत्त रिक्त;

	/* Make sure size is multiple of alignment */
	सचे (size % alignment != 0)
		पटिनिवत्त रिक्त;

	पटिनिवत्त alloc_with_align(hpctrl, alignment, size);
}

शून्य *sbi_zalloc_from(संरचना sbi_heap_control *hpctrl, size_t size)
{
	शून्य *ret = sbi_malloc_from(hpctrl, size);

	सचे (ret)
		sbi_memset(ret, 0, size);
	पटिनिवत्त ret;
}

शून्य sbi_free_from(संरचना sbi_heap_control *hpctrl, शून्य *ptr)
{
	संरचना heap_ग्रन्थिः *n, *np;

	सचे (!ptr)
		पटिनिवत्त;

	spin_lock(&hpctrl->lock);

	np = रिक्त;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		सचे ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			भङ्ग;
		}
	}
	सचे (!np) {
		spin_unlock(&hpctrl->lock);
		पटिनिवत्त;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		सचे ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_ग्रन्थिः_list);
			np = रिक्त;
			भङ्ग;
		} अञ्ञथा सचे (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_ग्रन्थिः_list);
			np = रिक्त;
			भङ्ग;
		} अञ्ञथा सचे ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = रिक्त;
			भङ्ग;
		}
	}
	सचे (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(संरचना sbi_heap_control *hpctrl)
{
	संरचना heap_ग्रन्थिः *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	पटिनिवत्त ret;
}

unsigned long sbi_heap_used_space_from(संरचना sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	पटिनिवत्त hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(संरचना sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	पटिनिवत्त hpctrl->resv;
}

पूर्णाङ्क sbi_heap_init_new(संरचना sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	संरचना heap_ग्रन्थिः *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_ग्रन्थिः_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_ग्रन्थिः;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_heap_init(संरचना sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	सचे (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		पटिनिवत्त SBI_EINVAL;

	पटिनिवत्त sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

पूर्णाङ्क sbi_heap_alloc_new(संरचना sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, परिमाण(संरचना sbi_heap_control));
	पटिनिवत्त 0;
}
