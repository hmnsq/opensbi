/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/riscv_locks.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_list.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#define HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#define HEAP_গ্রন্থি_BATCH_আকার		8

কাঠামো heap_গ্রন্থি {
	কাঠামো sbi_dlist শীর্ষ;
	unsigned long addr;
	unsigned long আকার_2;
};

কাঠামো sbi_heap_control {
	spinlock_t lock;
	unsigned long base GUARDED_BY(&lock);
	unsigned long আকার_2 GUARDED_BY(&lock);
	unsigned long resv GUARDED_BY(&lock);
	কাঠামো sbi_dlist free_গ্রন্থি_list GUARDED_BY(&lock);
	কাঠামো sbi_dlist free_space_list GUARDED_BY(&lock);
	কাঠামো sbi_dlist used_space_list GUARDED_BY(&lock);
	কাঠামো heap_গ্রন্থি init_free_space_গ্রন্থি GUARDED_BY(&lock);
};

কাঠামো sbi_heap_control global_hpctrl;

স্থির bool alloc_nodes(কাঠামো sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	size_t আকার_2 = HEAP_গ্রন্থি_BATCH_আকার * আকার(কাঠামো heap_গ্রন্থি);
	কাঠামো heap_গ্রন্থি *n, *new = ফাঁকা;

	/* alloc_with_align() requires at most two free nodes */
	যদি (hpctrl->free_গ্রন্থি_list.পরবর্তী != hpctrl->free_গ্রন্থি_list.পূর্ব)
		ফেরত true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, শীর্ষ) {
		যদি (n->আকার_2 >= আকার_2) {
			n->আকার_2 -= আকার_2;
			যদি (!n->আকার_2) {
				sbi_list_del(&n->শীর্ষ);
				sbi_list_add_লেজ(&n->শীর্ষ, &hpctrl->free_গ্রন্থি_list);
			}
			new = (শূন্য *)(n->addr + n->আকার_2);
			বিরতি;
		}
	}
	যদি (!new)
		ফেরত false;

	জন্য (size_t i = 0; i < HEAP_গ্রন্থি_BATCH_আকার; i++)
		sbi_list_add_লেজ(&new[i].শীর্ষ, &hpctrl->free_গ্রন্থি_list);
	hpctrl->resv += আকার_2;

	ফেরত true;
}

স্থির শূন্য *alloc_with_align(কাঠামো sbi_heap_control *hpctrl, size_t align,
			      size_t আকার_2)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	শূন্য *ret = ফাঁকা;
	কাঠামো heap_গ্রন্থি *n, *np;
	unsigned long lowest_aligned;
	size_t pad;

	যদি (!আকার_2)
		ফেরত ফাঁকা;

	আকার_2 += align - 1;
	আকার_2 &= ~((unsigned long)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	যদি (!alloc_nodes(hpctrl))
		যাও out;

	np = ফাঁকা;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, শীর্ষ) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		যদি (আকার_2 + pad <= n->আকার_2) {
			np = n;
			বিরতি;
		}
	}
	যদি (!np)
		যাও out;

	যদি (pad) {
		n = sbi_list_first_entry(&hpctrl->free_গ্রন্থি_list,
					 কাঠামো heap_গ্রন্থি, শীর্ষ);
		sbi_list_del(&n->শীর্ষ);

		n->addr = np->addr;
		n->আকার_2 = pad;
		sbi_list_add_লেজ(&n->শীর্ষ, &np->শীর্ষ);

		np->addr += pad;
		np->আকার_2 -= pad;
	}

	যদি (আকার_2 < np->আকার_2) {
		n = sbi_list_first_entry(&hpctrl->free_গ্রন্থি_list,
					 কাঠামো heap_গ্রন্থি, শীর্ষ);
		sbi_list_del(&n->শীর্ষ);

		n->addr = np->addr + আকার_2;
		n->আকার_2 = np->আকার_2 - আকার_2;
		sbi_list_add(&n->শীর্ষ, &np->শীর্ষ);

		np->আকার_2 = আকার_2;
	}

	sbi_list_del(&np->শীর্ষ);
	sbi_list_add_লেজ(&np->শীর্ষ, &hpctrl->used_space_list);
	ret = (শূন্য *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	ফেরত ret;
}

শূন্য *sbi_malloc_from(কাঠামো sbi_heap_control *hpctrl, size_t আকার_2)
{
	ফেরত alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, আকার_2);
}

শূন্য *sbi_aligned_alloc_from(কাঠামো sbi_heap_control *hpctrl,
			     size_t alignment, size_t আকার_2)
{
	যদি (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	যদি ((alignment & (alignment - 1)) != 0)
		ফেরত ফাঁকা;

	/* Make sure size is multiple of alignment */
	যদি (আকার_2 % alignment != 0)
		ফেরত ফাঁকা;

	ফেরত alloc_with_align(hpctrl, alignment, আকার_2);
}

শূন্য *sbi_zalloc_from(কাঠামো sbi_heap_control *hpctrl, size_t আকার_2)
{
	শূন্য *ret = sbi_malloc_from(hpctrl, আকার_2);

	যদি (ret)
		sbi_memset(ret, 0, আকার_2);
	ফেরত ret;
}

শূন্য sbi_free_from(কাঠামো sbi_heap_control *hpctrl, শূন্য *ptr)
{
	কাঠামো heap_গ্রন্থি *n, *np;

	যদি (!ptr)
		ফেরত;

	spin_lock(&hpctrl->lock);

	np = ফাঁকা;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, শীর্ষ) {
		যদি ((n->addr <= (unsigned long)ptr) &&
		    ((unsigned long)ptr < (n->addr + n->আকার_2))) {
			np = n;
			বিরতি;
		}
	}
	যদি (!np) {
		spin_unlock(&hpctrl->lock);
		ফেরত;
	}

	sbi_list_del(&np->শীর্ষ);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, শীর্ষ) {
		যদি ((np->addr + np->আকার_2) == n->addr) {
			n->addr = np->addr;
			n->আকার_2 += np->আকার_2;
			sbi_list_add_লেজ(&np->শীর্ষ, &hpctrl->free_গ্রন্থি_list);
			np = ফাঁকা;
			বিরতি;
		} অন্যথা যদি (np->addr == (n->addr + n->আকার_2)) {
			n->আকার_2 += np->আকার_2;
			sbi_list_add_লেজ(&np->শীর্ষ, &hpctrl->free_গ্রন্থি_list);
			np = ফাঁকা;
			বিরতি;
		} অন্যথা যদি ((n->addr + n->আকার_2) < np->addr) {
			sbi_list_add(&np->শীর্ষ, &n->শীর্ষ);
			np = ফাঁকা;
			বিরতি;
		}
	}
	যদি (np)
		sbi_list_add_লেজ(&np->শীর্ষ, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

unsigned long sbi_heap_free_space_from(কাঠামো sbi_heap_control *hpctrl)
{
	কাঠামো heap_গ্রন্থি *n;
	unsigned long ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, শীর্ষ)
		ret += n->আকার_2;
	spin_unlock(&hpctrl->lock);

	ফেরত ret;
}

unsigned long sbi_heap_used_space_from(কাঠামো sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ফেরত hpctrl->আকার_2 - hpctrl->resv - sbi_heap_free_space();
}

unsigned long sbi_heap_reserved_space_from(কাঠামো sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	ফেরত hpctrl->resv;
}

পূর্ণসংখ্যা sbi_heap_init_new(কাঠামো sbi_heap_control *hpctrl, unsigned long base,
		      unsigned long আকার_2) NO_THREAD_SAFETY_ANALYSIS
{
	কাঠামো heap_গ্রন্থি *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->আকার_2 = আকার_2;
	hpctrl->resv = 0;
	SBI_INIT_LIST_শীর্ষ(&hpctrl->free_গ্রন্থি_list);
	SBI_INIT_LIST_শীর্ষ(&hpctrl->free_space_list);
	SBI_INIT_LIST_শীর্ষ(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_গ্রন্থি;
	n->addr = base;
	n->আকার_2 = আকার_2;
	sbi_list_add_লেজ(&n->শীর্ষ, &hpctrl->free_space_list);

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_heap_init(কাঠামো sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	যদি (!scratch->fw_heap_আকার ||
	    (scratch->fw_heap_আকার & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_আকার < (scratch->fw_heap_offset + scratch->fw_heap_আকার)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		ফেরত SBI_EINVAL;

	ফেরত sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_আকার);
}

পূর্ণসংখ্যা sbi_heap_alloc_new(কাঠামো sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, আকার(কাঠামো sbi_heap_control));
	ফেরত 0;
}
