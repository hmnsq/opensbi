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
#تعریف_کړه ډېرۍ_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#تعریف_کړه ډېرۍ_غوټه_BATCH_اندازه		8

بنية ډېرۍ_غوټه {
	بنية sbi_dlist سر;
	بینښه اوږد addr;
	بینښه اوږد اندازه;
};

بنية sbi_ډېرۍ_control {
	spinlock_t قلف;
	بینښه اوږد بنسټ_2 GUARDED_BY(&قلف);
	بینښه اوږد اندازه GUARDED_BY(&قلف);
	بینښه اوږد resv GUARDED_BY(&قلف);
	بنية sbi_dlist free_غوټه_list GUARDED_BY(&قلف);
	بنية sbi_dlist free_space_list GUARDED_BY(&قلف);
	بنية sbi_dlist کارول_شوی_space_list GUARDED_BY(&قلف);
	بنية ډېرۍ_غوټه init_free_space_غوټه GUARDED_BY(&قلف);
};

بنية sbi_ډېرۍ_control global_hpctrl;

ساكن bool alloc_nodes(بنية sbi_ډېرۍ_control *hpctrl) MUST_HOLD(&hpctrl->قلف)
{
	size_t اندازه = ډېرۍ_غوټه_BATCH_اندازه * حجم(بنية ډېرۍ_غوټه);
	بنية ډېرۍ_غوټه *n, *new = عدم;

	/* alloc_with_align() requires at most two free nodes */
	إذا (hpctrl->free_غوټه_list.بل != hpctrl->free_غوټه_list.prev)
		أرجع true;

	sbi_list_for_each_ثبت_سرچپه(n, &hpctrl->free_space_list, سر) {
		إذا (n->اندازه >= اندازه) {
			n->اندازه -= اندازه;
			إذا (!n->اندازه) {
				sbi_list_del(&n->سر);
				sbi_list_جمع_کړه_لکۍ(&n->سر, &hpctrl->free_غوټه_list);
			}
			new = (فراغ *)(n->addr + n->اندازه);
			اكسر;
		}
	}
	إذا (!new)
		أرجع false;

	لكل (size_t i = 0; i < ډېرۍ_غوټه_BATCH_اندازه; i++)
		sbi_list_جمع_کړه_لکۍ(&new[i].سر, &hpctrl->free_غوټه_list);
	hpctrl->resv += اندازه;

	أرجع true;
}

ساكن فراغ *alloc_with_align(بنية sbi_ډېرۍ_control *hpctrl, size_t align,
			      size_t اندازه)
	MUST_NOT_HOLD(&hpctrl->قلف)
{
	فراغ *ret = عدم;
	بنية ډېرۍ_غوټه *n, *np;
	بینښه اوږد ټیټ_ترین_aligned;
	size_t pad;

	إذا (!اندازه)
		أرجع عدم;

	اندازه += align - 1;
	اندازه &= ~((بینښه اوږد)align - 1);

	spin_قلف(&hpctrl->قلف);

	/* Ensure at least two free nodes are available for use below */
	إذا (!alloc_nodes(hpctrl))
		اذهب بهر;

	np = عدم;
	sbi_list_for_each_ثبت(n, &hpctrl->free_space_list, سر) {
		ټیټ_ترین_aligned = ROUNDUP(n->addr, align);
		pad = ټیټ_ترین_aligned - n->addr;

		إذا (اندازه + pad <= n->اندازه) {
			np = n;
			اكسر;
		}
	}
	إذا (!np)
		اذهب بهر;

	إذا (pad) {
		n = sbi_list_لومړی_ثبت(&hpctrl->free_غوټه_list,
					 بنية ډېرۍ_غوټه, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr;
		n->اندازه = pad;
		sbi_list_جمع_کړه_لکۍ(&n->سر, &np->سر);

		np->addr += pad;
		np->اندازه -= pad;
	}

	إذا (اندازه < np->اندازه) {
		n = sbi_list_لومړی_ثبت(&hpctrl->free_غوټه_list,
					 بنية ډېرۍ_غوټه, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr + اندازه;
		n->اندازه = np->اندازه - اندازه;
		sbi_list_جمع_کړه(&n->سر, &np->سر);

		np->اندازه = اندازه;
	}

	sbi_list_del(&np->سر);
	sbi_list_جمع_کړه_لکۍ(&np->سر, &hpctrl->کارول_شوی_space_list);
	ret = (فراغ *)np->addr;

بهر:
	spin_unlock(&hpctrl->قلف);

	أرجع ret;
}

فراغ *sbi_malloc_from(بنية sbi_ډېرۍ_control *hpctrl, size_t اندازه)
{
	أرجع alloc_with_align(hpctrl, ډېرۍ_ALLOC_ALIGN, اندازه);
}

فراغ *sbi_aligned_alloc_from(بنية sbi_ډېرۍ_control *hpctrl,
			     size_t alignment, size_t اندازه)
{
	إذا (alignment < ډېرۍ_ALLOC_ALIGN)
		alignment = ډېرۍ_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	إذا ((alignment & (alignment - 1)) != 0)
		أرجع عدم;

	/* Make sure size is multiple of alignment */
	إذا (اندازه % alignment != 0)
		أرجع عدم;

	أرجع alloc_with_align(hpctrl, alignment, اندازه);
}

فراغ *sbi_zalloc_from(بنية sbi_ډېرۍ_control *hpctrl, size_t اندازه)
{
	فراغ *ret = sbi_malloc_from(hpctrl, اندازه);

	إذا (ret)
		sbi_memset(ret, 0, اندازه);
	أرجع ret;
}

فراغ sbi_free_from(بنية sbi_ډېرۍ_control *hpctrl, فراغ *ptr)
{
	بنية ډېرۍ_غوټه *n, *np;

	إذا (!ptr)
		أرجع;

	spin_قلف(&hpctrl->قلف);

	np = عدم;
	sbi_list_for_each_ثبت(n, &hpctrl->کارول_شوی_space_list, سر) {
		إذا ((n->addr <= (بینښه اوږد)ptr) &&
		    ((بینښه اوږد)ptr < (n->addr + n->اندازه))) {
			np = n;
			اكسر;
		}
	}
	إذا (!np) {
		spin_unlock(&hpctrl->قلف);
		أرجع;
	}

	sbi_list_del(&np->سر);

	sbi_list_for_each_ثبت(n, &hpctrl->free_space_list, سر) {
		إذا ((np->addr + np->اندازه) == n->addr) {
			n->addr = np->addr;
			n->اندازه += np->اندازه;
			sbi_list_جمع_کړه_لکۍ(&np->سر, &hpctrl->free_غوټه_list);
			np = عدم;
			اكسر;
		} وإلا إذا (np->addr == (n->addr + n->اندازه)) {
			n->اندازه += np->اندازه;
			sbi_list_جمع_کړه_لکۍ(&np->سر, &hpctrl->free_غوټه_list);
			np = عدم;
			اكسر;
		} وإلا إذا ((n->addr + n->اندازه) < np->addr) {
			sbi_list_جمع_کړه(&np->سر, &n->سر);
			np = عدم;
			اكسر;
		}
	}
	إذا (np)
		sbi_list_جمع_کړه_لکۍ(&np->سر, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->قلف);
}

بینښه اوږد sbi_ډېرۍ_free_space_from(بنية sbi_ډېرۍ_control *hpctrl)
{
	بنية ډېرۍ_غوټه *n;
	بینښه اوږد ret = 0;

	spin_قلف(&hpctrl->قلف);
	sbi_list_for_each_ثبت(n, &hpctrl->free_space_list, سر)
		ret += n->اندازه;
	spin_unlock(&hpctrl->قلف);

	أرجع ret;
}

بینښه اوږد sbi_ډېرۍ_کارول_شوی_space_from(بنية sbi_ډېرۍ_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	أرجع hpctrl->اندازه - hpctrl->resv - sbi_ډېرۍ_free_space();
}

بینښه اوږد sbi_ډېرۍ_reserved_space_from(بنية sbi_ډېرۍ_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	أرجع hpctrl->resv;
}

صحيح sbi_ډېرۍ_init_new(بنية sbi_ډېرۍ_control *hpctrl, بینښه اوږد بنسټ_2,
		      بینښه اوږد اندازه) NO_THREAD_SAFETY_ANALYSIS
{
	بنية ډېرۍ_غوټه *n;

	/* Initialize heap control */
	SPIN_قلف_INIT(hpctrl->قلف);
	hpctrl->بنسټ_2 = بنسټ_2;
	hpctrl->اندازه = اندازه;
	hpctrl->resv = 0;
	SBI_INIT_LIST_سر(&hpctrl->free_غوټه_list);
	SBI_INIT_LIST_سر(&hpctrl->free_space_list);
	SBI_INIT_LIST_سر(&hpctrl->کارول_شوی_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_غوټه;
	n->addr = بنسټ_2;
	n->اندازه = اندازه;
	sbi_list_جمع_کړه_لکۍ(&n->سر, &hpctrl->free_space_list);

	أرجع 0;
}

صحيح sbi_ډېرۍ_init(بنية sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	إذا (!scratch->fw_ډېرۍ_اندازه ||
	    (scratch->fw_ډېرۍ_اندازه & (ډېرۍ_بنسټ_ALIGN - 1)) ||
	    (scratch->fw_ډېرۍ_دننه_واټن < scratch->fw_rw_دننه_واټن) ||
	    (scratch->fw_اندازه < (scratch->fw_ډېرۍ_دننه_واټن + scratch->fw_ډېرۍ_اندازه)) ||
	    (scratch->fw_ډېرۍ_دننه_واټن & (ډېرۍ_بنسټ_ALIGN - 1)))
		أرجع SBI_EINVAL;

	أرجع sbi_ډېرۍ_init_new(&global_hpctrl,
				  scratch->fw_پیل + scratch->fw_ډېرۍ_دننه_واټن,
				  scratch->fw_ډېرۍ_اندازه);
}

صحيح sbi_ډېرۍ_alloc_new(بنية sbi_ډېرۍ_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, حجم(بنية sbi_ډېرۍ_control));
	أرجع 0;
}
