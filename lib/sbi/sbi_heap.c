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
#define HEAP_गाठ_BATCH_SIZE		8

संरचना heap_गाठ {
	संरचना sbi_dlist head;
	unsigned long addr;
	unsigned long size;
};

संरचना sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long size GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	संरचना sbi_dlist free_गाठ_list GUARDED_BY(&lock);
	संरचना sbi_dlist free_space_list GUARDED_BY(&lock);
	संरचना sbi_dlist used_space_list GUARDED_BY(&lock);
	संरचना heap_गाठ init_free_space_गाठ GUARDED_BY(&lock);
};

संरचना sbi_heap_control global_hpctrl;

स्थिर bool alloc_nodes(संरचना sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t size = HEAP_गाठ_BATCH_SIZE * परिमाण(संरचना heap_गाठ);
	संरचना heap_गाठ *n, *new = रिक्त;

	/* alloc_with_align() requires at most two free nodes */
	यदि (hpctrl->free_गाठ_list.next != hpctrl->free_गाठ_list.prev)
		प्रत्यागम true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, head) {
		यदि (n->size >= size) {
			n->size -= size;
			यदि (!n->size) {
				sbi_list_del(&n->head);
				sbi_list_add_tail(&n->head, &hpctrl->free_गाठ_list);
			}
			new = (शून्य *)(n->addr + n->size);
			भङ्ग;
		}
	}
	यदि (!new)
		प्रत्यागम false;

	प्रति (size_t i = 0; i < HEAP_गाठ_BATCH_SIZE; i++)
		sbi_list_add_tail(&new[i].head, &hpctrl->free_गाठ_list);
	hpctrl->resv += size;

	प्रत्यागम true;
}

स्थिर शून्य *alloc_with_align(संरचना sbi_heap_control *hpctrl, size_t align,
			      size_t size)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	शून्य *ret = रिक्त;
	संरचना heap_गाठ *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	यदि (!size)
		प्रत्यागम रिक्त;

	size += align - 1;
	size &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	यदि (!alloc_nodes(hpctrl))
		गच्छ out;

	np = रिक्त;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		यदि (size + pad <= n->size) {
			np = n;
			भङ्ग;
		}
	}
	यदि (!np)
		गच्छ out;

	यदि (pad) {
		n = sbi_list_first_entry(&hpctrl->free_गाठ_list,
					 संरचना heap_गाठ, head);
		sbi_list_del(&n->head);

		n->addr = np->addr;
		n->size = pad;
		sbi_list_add_tail(&n->head, &np->head);

		np->addr += pad;
		np->size -= pad;
	}

	यदि (size < np->size) {
		n = sbi_list_first_entry(&hpctrl->free_गाठ_list,
					 संरचना heap_गाठ, head);
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

	प्रत्यागम ret;
}

शून्य *sbi_malloc_from(संरचना sbi_heap_control *hpctrl, size_t size)
{
	प्रत्यागम alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, size);
}

शून्य *sbi_aligned_alloc_from(संरचना sbi_heap_control *hpctrl,
			     size_t alignment, size_t size)
{
	यदि (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	यदि ((alignment & (alignment - 1)) != 0)
		प्रत्यागम रिक्त;

	/* Make sure size is multiple of alignment */
	यदि (size % alignment != 0)
		प्रत्यागम रिक्त;

	प्रत्यागम alloc_with_align(hpctrl, alignment, size);
}

शून्य *sbi_zalloc_from(संरचना sbi_heap_control *hpctrl, size_t size)
{
	शून्य *ret = sbi_malloc_from(hpctrl, size);

	यदि (ret)
		sbi_memset(ret, 0, size);
	प्रत्यागम ret;
}

शून्य sbi_free_from(संरचना sbi_heap_control *hpctrl, शून्य *ptr)
{
	संरचना heap_गाठ *n, *np;

	यदि (!ptr)
		प्रत्यागम;

	spin_lock(&hpctrl->lock);

	np = रिक्त;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, head) {
		यदि ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->size))) {
			np = n;
			भङ्ग;
		}
	}
	यदि (!np) {
		spin_unlock(&hpctrl->lock);
		प्रत्यागम;
	}

	sbi_list_del(&np->head);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head) {
		यदि ((np->addr + np->size) == n->addr) {
			n->addr = np->addr;
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_गाठ_list);
			np = रिक्त;
			भङ्ग;
		} अन्यथा यदि (np->addr == (n->addr + n->size)) {
			n->size += np->size;
			sbi_list_add_tail(&np->head, &hpctrl->free_गाठ_list);
			np = रिक्त;
			भङ्ग;
		} अन्यथा यदि ((n->addr + n->size) < np->addr) {
			sbi_list_add(&np->head, &n->head);
			np = रिक्त;
			भङ्ग;
		}
	}
	यदि (np)
		sbi_list_add_tail(&np->head, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(संरचना sbi_heap_control *hpctrl)
{
	संरचना heap_गाठ *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, head)
		ret += n->size;
	spin_unlock(&hpctrl->lock);

	प्रत्यागम ret;
}

unsigned long sbi_heap_used_space_from(संरचना sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	प्रत्यागम hpctrl->size - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(संरचना sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	प्रत्यागम hpctrl->resv;
}

पूर्णाङ्क sbi_heap_init_new(संरचना sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long size) NO_THREAD_SAFETY_ANALYSIS
{
	संरचना heap_गाठ *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->size = size;
	hpctrl->resv = 0;
	SBI_INIT_LIST_HEAD(&hpctrl->free_गाठ_list);
	SBI_INIT_LIST_HEAD(&hpctrl->free_space_list);
	SBI_INIT_LIST_HEAD(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_गाठ;
	n->addr = base;
	n->size = size;
	sbi_list_add_tail(&n->head, &hpctrl->free_space_list);

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_heap_init(संरचना sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	यदि (!scratch->fw_heap_size ||
	    (scratch->fw_heap_size & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_size < (scratch->fw_heap_offset + scratch->fw_heap_size)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_size);
}

पूर्णाङ्क sbi_heap_alloc_new(संरचना sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, परिमाण(संरचना sbi_heap_control));
	प्रत्यागम 0;
}
