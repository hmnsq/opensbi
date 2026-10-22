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
#وصف_ڪريو ڍير_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#وصف_ڪريو ڍير_ڳنڍ_BATCH_سائيز		8

بنية ڍير_ڳنڍ {
	بنية sbi_dlist سر;
	بنانشان ڊگهو addr;
	بنانشان ڊگهو سائيز;
};

بنية sbi_ڍير_control {
	spinlock_t تالو;
	بنانشان ڊگهو بنياد_2 GUARDED_BY(&تالو);
	بنانشان ڊگهو سائيز GUARDED_BY(&تالو);
	بنانشان ڊگهو resv GUARDED_BY(&تالو);
	بنية sbi_dlist free_ڳنڍ_list GUARDED_BY(&تالو);
	بنية sbi_dlist free_space_list GUARDED_BY(&تالو);
	بنية sbi_dlist استعمال_ٿيل_space_list GUARDED_BY(&تالو);
	بنية ڍير_ڳنڍ init_free_space_ڳنڍ GUARDED_BY(&تالو);
};

بنية sbi_ڍير_control global_hpctrl;

ساكن bool alloc_nodes(بنية sbi_ڍير_control *hpctrl) MUST_HOLD(&hpctrl->تالو)
{
	size_t سائيز = ڍير_ڳنڍ_BATCH_سائيز * حجم(بنية ڍير_ڳنڍ);
	بنية ڍير_ڳنڍ *n, *new = عدم;

	/* alloc_with_align() requires at most two free nodes */
	إذا (hpctrl->free_ڳنڍ_list.ايندڙ != hpctrl->free_ڳنڍ_list.prev)
		أرجع true;

	sbi_list_for_each_داخلا_ابتو(n, &hpctrl->free_space_list, سر) {
		إذا (n->سائيز >= سائيز) {
			n->سائيز -= سائيز;
			إذا (!n->سائيز) {
				sbi_list_del(&n->سر);
				sbi_list_جوڙيو_پڇ(&n->سر, &hpctrl->free_ڳنڍ_list);
			}
			new = (فراغ *)(n->addr + n->سائيز);
			اكسر;
		}
	}
	إذا (!new)
		أرجع false;

	لكل (size_t i = 0; i < ڍير_ڳنڍ_BATCH_سائيز; i++)
		sbi_list_جوڙيو_پڇ(&new[i].سر, &hpctrl->free_ڳنڍ_list);
	hpctrl->resv += سائيز;

	أرجع true;
}

ساكن فراغ *alloc_with_align(بنية sbi_ڍير_control *hpctrl, size_t align,
			      size_t سائيز)
	MUST_NOT_HOLD(&hpctrl->تالو)
{
	فراغ *ret = عدم;
	بنية ڍير_ڳنڍ *n, *np;
	بنانشان ڊگهو سڀ_کان_هيٺ_aligned;
	size_t pad;

	إذا (!سائيز)
		أرجع عدم;

	سائيز += align - 1;
	سائيز &= ~((بنانشان ڊگهو)align - 1);

	spin_تالو(&hpctrl->تالو);

	/* Ensure at least two free nodes are available for use below */
	إذا (!alloc_nodes(hpctrl))
		اذهب ٻاهريون;

	np = عدم;
	sbi_list_for_each_داخلا(n, &hpctrl->free_space_list, سر) {
		سڀ_کان_هيٺ_aligned = ROUNDUP(n->addr, align);
		pad = سڀ_کان_هيٺ_aligned - n->addr;

		إذا (سائيز + pad <= n->سائيز) {
			np = n;
			اكسر;
		}
	}
	إذا (!np)
		اذهب ٻاهريون;

	إذا (pad) {
		n = sbi_list_پهريون_داخلا(&hpctrl->free_ڳنڍ_list,
					 بنية ڍير_ڳنڍ, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr;
		n->سائيز = pad;
		sbi_list_جوڙيو_پڇ(&n->سر, &np->سر);

		np->addr += pad;
		np->سائيز -= pad;
	}

	إذا (سائيز < np->سائيز) {
		n = sbi_list_پهريون_داخلا(&hpctrl->free_ڳنڍ_list,
					 بنية ڍير_ڳنڍ, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr + سائيز;
		n->سائيز = np->سائيز - سائيز;
		sbi_list_جوڙيو(&n->سر, &np->سر);

		np->سائيز = سائيز;
	}

	sbi_list_del(&np->سر);
	sbi_list_جوڙيو_پڇ(&np->سر, &hpctrl->استعمال_ٿيل_space_list);
	ret = (فراغ *)np->addr;

ٻاهريون:
	spin_unlock(&hpctrl->تالو);

	أرجع ret;
}

فراغ *sbi_malloc_from(بنية sbi_ڍير_control *hpctrl, size_t سائيز)
{
	أرجع alloc_with_align(hpctrl, ڍير_ALLOC_ALIGN, سائيز);
}

فراغ *sbi_aligned_alloc_from(بنية sbi_ڍير_control *hpctrl,
			     size_t alignment, size_t سائيز)
{
	إذا (alignment < ڍير_ALLOC_ALIGN)
		alignment = ڍير_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	إذا ((alignment & (alignment - 1)) != 0)
		أرجع عدم;

	/* Make sure size is multiple of alignment */
	إذا (سائيز % alignment != 0)
		أرجع عدم;

	أرجع alloc_with_align(hpctrl, alignment, سائيز);
}

فراغ *sbi_zalloc_from(بنية sbi_ڍير_control *hpctrl, size_t سائيز)
{
	فراغ *ret = sbi_malloc_from(hpctrl, سائيز);

	إذا (ret)
		sbi_memset(ret, 0, سائيز);
	أرجع ret;
}

فراغ sbi_free_from(بنية sbi_ڍير_control *hpctrl, فراغ *ptr)
{
	بنية ڍير_ڳنڍ *n, *np;

	إذا (!ptr)
		أرجع;

	spin_تالو(&hpctrl->تالو);

	np = عدم;
	sbi_list_for_each_داخلا(n, &hpctrl->استعمال_ٿيل_space_list, سر) {
		إذا ((n->addr <= (بنانشان ڊگهو)ptr) &&
		    ((بنانشان ڊگهو)ptr < (n->addr + n->سائيز))) {
			np = n;
			اكسر;
		}
	}
	إذا (!np) {
		spin_unlock(&hpctrl->تالو);
		أرجع;
	}

	sbi_list_del(&np->سر);

	sbi_list_for_each_داخلا(n, &hpctrl->free_space_list, سر) {
		إذا ((np->addr + np->سائيز) == n->addr) {
			n->addr = np->addr;
			n->سائيز += np->سائيز;
			sbi_list_جوڙيو_پڇ(&np->سر, &hpctrl->free_ڳنڍ_list);
			np = عدم;
			اكسر;
		} وإلا إذا (np->addr == (n->addr + n->سائيز)) {
			n->سائيز += np->سائيز;
			sbi_list_جوڙيو_پڇ(&np->سر, &hpctrl->free_ڳنڍ_list);
			np = عدم;
			اكسر;
		} وإلا إذا ((n->addr + n->سائيز) < np->addr) {
			sbi_list_جوڙيو(&np->سر, &n->سر);
			np = عدم;
			اكسر;
		}
	}
	إذا (np)
		sbi_list_جوڙيو_پڇ(&np->سر, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->تالو);
}

بنانشان ڊگهو sbi_ڍير_free_space_from(بنية sbi_ڍير_control *hpctrl)
{
	بنية ڍير_ڳنڍ *n;
	بنانشان ڊگهو ret = 0;

	spin_تالو(&hpctrl->تالو);
	sbi_list_for_each_داخلا(n, &hpctrl->free_space_list, سر)
		ret += n->سائيز;
	spin_unlock(&hpctrl->تالو);

	أرجع ret;
}

بنانشان ڊگهو sbi_ڍير_استعمال_ٿيل_space_from(بنية sbi_ڍير_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	أرجع hpctrl->سائيز - hpctrl->resv - sbi_ڍير_free_space();
}

بنانشان ڊگهو sbi_ڍير_reserved_space_from(بنية sbi_ڍير_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	أرجع hpctrl->resv;
}

صحيح sbi_ڍير_init_new(بنية sbi_ڍير_control *hpctrl, بنانشان ڊگهو بنياد_2,
		      بنانشان ڊگهو سائيز) NO_THREAD_SAFETY_ANALYSIS
{
	بنية ڍير_ڳنڍ *n;

	/* Initialize heap control */
	SPIN_تالو_INIT(hpctrl->تالو);
	hpctrl->بنياد_2 = بنياد_2;
	hpctrl->سائيز = سائيز;
	hpctrl->resv = 0;
	SBI_INIT_LIST_سر(&hpctrl->free_ڳنڍ_list);
	SBI_INIT_LIST_سر(&hpctrl->free_space_list);
	SBI_INIT_LIST_سر(&hpctrl->استعمال_ٿيل_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_ڳنڍ;
	n->addr = بنياد_2;
	n->سائيز = سائيز;
	sbi_list_جوڙيو_پڇ(&n->سر, &hpctrl->free_space_list);

	أرجع 0;
}

صحيح sbi_ڍير_init(بنية sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	إذا (!scratch->fw_ڍير_سائيز ||
	    (scratch->fw_ڍير_سائيز & (ڍير_بنياد_ALIGN - 1)) ||
	    (scratch->fw_ڍير_وٿي < scratch->fw_rw_وٿي) ||
	    (scratch->fw_سائيز < (scratch->fw_ڍير_وٿي + scratch->fw_ڍير_سائيز)) ||
	    (scratch->fw_ڍير_وٿي & (ڍير_بنياد_ALIGN - 1)))
		أرجع SBI_EINVAL;

	أرجع sbi_ڍير_init_new(&global_hpctrl,
				  scratch->fw_شروعات + scratch->fw_ڍير_وٿي,
				  scratch->fw_ڍير_سائيز);
}

صحيح sbi_ڍير_alloc_new(بنية sbi_ڍير_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, حجم(بنية sbi_ڍير_control));
	أرجع 0;
}
