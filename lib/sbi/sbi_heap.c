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
#تعریف_کن انباشته_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#تعریف_کن انباشته_گره_BATCH_اندازه		8

بنية انباشته_گره {
	بنية sbi_dlist سر;
	بیعلامت دراز addr;
	بیعلامت دراز اندازه;
};

بنية sbi_انباشته_control {
	spinlock_t قفل_2;
	بیعلامت دراز پایه_2 GUARDED_BY(&قفل_2);
	بیعلامت دراز اندازه GUARDED_BY(&قفل_2);
	بیعلامت دراز resv GUARDED_BY(&قفل_2);
	بنية sbi_dlist free_گره_list GUARDED_BY(&قفل_2);
	بنية sbi_dlist free_space_list GUARDED_BY(&قفل_2);
	بنية sbi_dlist استفاده_شده_space_list GUARDED_BY(&قفل_2);
	بنية انباشته_گره init_free_space_گره GUARDED_BY(&قفل_2);
};

بنية sbi_انباشته_control global_hpctrl;

ساكن bool alloc_nodes(بنية sbi_انباشته_control *hpctrl) MUST_HOLD(&hpctrl->قفل_2)
{
	size_t اندازه = انباشته_گره_BATCH_اندازه * حجم(بنية انباشته_گره);
	بنية انباشته_گره *n, *new = عدم;

	/* alloc_with_align() requires at most two free nodes */
	إذا (hpctrl->free_گره_list.بعدی != hpctrl->free_گره_list.prev)
		أرجع true;

	sbi_list_for_each_مدخل_معکوس(n, &hpctrl->free_space_list, سر) {
		إذا (n->اندازه >= اندازه) {
			n->اندازه -= اندازه;
			إذا (!n->اندازه) {
				sbi_list_del(&n->سر);
				sbi_list_جمع_کن_دم(&n->سر, &hpctrl->free_گره_list);
			}
			new = (فراغ *)(n->addr + n->اندازه);
			اكسر;
		}
	}
	إذا (!new)
		أرجع false;

	لكل (size_t i = 0; i < انباشته_گره_BATCH_اندازه; i++)
		sbi_list_جمع_کن_دم(&new[i].سر, &hpctrl->free_گره_list);
	hpctrl->resv += اندازه;

	أرجع true;
}

ساكن فراغ *alloc_with_align(بنية sbi_انباشته_control *hpctrl, size_t align,
			      size_t اندازه)
	MUST_NOT_HOLD(&hpctrl->قفل_2)
{
	فراغ *ret = عدم;
	بنية انباشته_گره *n, *np;
	بیعلامت دراز پایینترین_aligned;
	size_t pad;

	إذا (!اندازه)
		أرجع عدم;

	اندازه += align - 1;
	اندازه &= ~((بیعلامت دراز)align - 1);

	spin_قفل(&hpctrl->قفل_2);

	/* Ensure at least two free nodes are available for use below */
	إذا (!alloc_nodes(hpctrl))
		اذهب بیرون;

	np = عدم;
	sbi_list_for_each_مدخل(n, &hpctrl->free_space_list, سر) {
		پایینترین_aligned = ROUNDUP(n->addr, align);
		pad = پایینترین_aligned - n->addr;

		إذا (اندازه + pad <= n->اندازه) {
			np = n;
			اكسر;
		}
	}
	إذا (!np)
		اذهب بیرون;

	إذا (pad) {
		n = sbi_list_اولی_مدخل(&hpctrl->free_گره_list,
					 بنية انباشته_گره, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr;
		n->اندازه = pad;
		sbi_list_جمع_کن_دم(&n->سر, &np->سر);

		np->addr += pad;
		np->اندازه -= pad;
	}

	إذا (اندازه < np->اندازه) {
		n = sbi_list_اولی_مدخل(&hpctrl->free_گره_list,
					 بنية انباشته_گره, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr + اندازه;
		n->اندازه = np->اندازه - اندازه;
		sbi_list_جمع_کن(&n->سر, &np->سر);

		np->اندازه = اندازه;
	}

	sbi_list_del(&np->سر);
	sbi_list_جمع_کن_دم(&np->سر, &hpctrl->استفاده_شده_space_list);
	ret = (فراغ *)np->addr;

بیرون:
	spin_unlock(&hpctrl->قفل_2);

	أرجع ret;
}

فراغ *sbi_malloc_from(بنية sbi_انباشته_control *hpctrl, size_t اندازه)
{
	أرجع alloc_with_align(hpctrl, انباشته_ALLOC_ALIGN, اندازه);
}

فراغ *sbi_aligned_alloc_from(بنية sbi_انباشته_control *hpctrl,
			     size_t alignment, size_t اندازه)
{
	إذا (alignment < انباشته_ALLOC_ALIGN)
		alignment = انباشته_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	إذا ((alignment & (alignment - 1)) != 0)
		أرجع عدم;

	/* Make sure size is multiple of alignment */
	إذا (اندازه % alignment != 0)
		أرجع عدم;

	أرجع alloc_with_align(hpctrl, alignment, اندازه);
}

فراغ *sbi_zalloc_from(بنية sbi_انباشته_control *hpctrl, size_t اندازه)
{
	فراغ *ret = sbi_malloc_from(hpctrl, اندازه);

	إذا (ret)
		sbi_memset(ret, 0, اندازه);
	أرجع ret;
}

فراغ sbi_free_from(بنية sbi_انباشته_control *hpctrl, فراغ *ptr)
{
	بنية انباشته_گره *n, *np;

	إذا (!ptr)
		أرجع;

	spin_قفل(&hpctrl->قفل_2);

	np = عدم;
	sbi_list_for_each_مدخل(n, &hpctrl->استفاده_شده_space_list, سر) {
		إذا ((n->addr <= (بیعلامت دراز)ptr) &&
		    ((بیعلامت دراز)ptr < (n->addr + n->اندازه))) {
			np = n;
			اكسر;
		}
	}
	إذا (!np) {
		spin_unlock(&hpctrl->قفل_2);
		أرجع;
	}

	sbi_list_del(&np->سر);

	sbi_list_for_each_مدخل(n, &hpctrl->free_space_list, سر) {
		إذا ((np->addr + np->اندازه) == n->addr) {
			n->addr = np->addr;
			n->اندازه += np->اندازه;
			sbi_list_جمع_کن_دم(&np->سر, &hpctrl->free_گره_list);
			np = عدم;
			اكسر;
		} وإلا إذا (np->addr == (n->addr + n->اندازه)) {
			n->اندازه += np->اندازه;
			sbi_list_جمع_کن_دم(&np->سر, &hpctrl->free_گره_list);
			np = عدم;
			اكسر;
		} وإلا إذا ((n->addr + n->اندازه) < np->addr) {
			sbi_list_جمع_کن(&np->سر, &n->سر);
			np = عدم;
			اكسر;
		}
	}
	إذا (np)
		sbi_list_جمع_کن_دم(&np->سر, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->قفل_2);
}

بیعلامت دراز sbi_انباشته_free_space_from(بنية sbi_انباشته_control *hpctrl)
{
	بنية انباشته_گره *n;
	بیعلامت دراز ret = 0;

	spin_قفل(&hpctrl->قفل_2);
	sbi_list_for_each_مدخل(n, &hpctrl->free_space_list, سر)
		ret += n->اندازه;
	spin_unlock(&hpctrl->قفل_2);

	أرجع ret;
}

بیعلامت دراز sbi_انباشته_استفاده_شده_space_from(بنية sbi_انباشته_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	أرجع hpctrl->اندازه - hpctrl->resv - sbi_انباشته_free_space();
}

بیعلامت دراز sbi_انباشته_reserved_space_from(بنية sbi_انباشته_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	أرجع hpctrl->resv;
}

صحيح sbi_انباشته_init_new(بنية sbi_انباشته_control *hpctrl, بیعلامت دراز پایه_2,
		      بیعلامت دراز اندازه) NO_THREAD_SAFETY_ANALYSIS
{
	بنية انباشته_گره *n;

	/* Initialize heap control */
	SPIN_قفل_INIT(hpctrl->قفل_2);
	hpctrl->پایه_2 = پایه_2;
	hpctrl->اندازه = اندازه;
	hpctrl->resv = 0;
	SBI_INIT_LIST_سر(&hpctrl->free_گره_list);
	SBI_INIT_LIST_سر(&hpctrl->free_space_list);
	SBI_INIT_LIST_سر(&hpctrl->استفاده_شده_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_گره;
	n->addr = پایه_2;
	n->اندازه = اندازه;
	sbi_list_جمع_کن_دم(&n->سر, &hpctrl->free_space_list);

	أرجع 0;
}

صحيح sbi_انباشته_init(بنية sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	إذا (!scratch->fw_انباشته_اندازه ||
	    (scratch->fw_انباشته_اندازه & (انباشته_پایه_ALIGN - 1)) ||
	    (scratch->fw_انباشته_جابه_جایی < scratch->fw_rw_جابه_جایی) ||
	    (scratch->fw_اندازه < (scratch->fw_انباشته_جابه_جایی + scratch->fw_انباشته_اندازه)) ||
	    (scratch->fw_انباشته_جابه_جایی & (انباشته_پایه_ALIGN - 1)))
		أرجع SBI_EINVAL;

	أرجع sbi_انباشته_init_new(&global_hpctrl,
				  scratch->fw_شروع + scratch->fw_انباشته_جابه_جایی,
				  scratch->fw_انباشته_اندازه);
}

صحيح sbi_انباشته_alloc_new(بنية sbi_انباشته_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, حجم(بنية sbi_انباشته_control));
	أرجع 0;
}
