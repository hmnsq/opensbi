/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_list.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#تعریف_کرو ڈھیر_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#تعریف_کرو ڈھیر_گرہ_BATCH_سائز		8

ساخت ڈھیر_گرہ {
	ساخت sbi_dlist سر;
	غیرعلامتی طویل addr;
	غیرعلامتی طویل سائز;
};

ساخت sbi_ڈھیر_control {
	spinlock_t تالا_2;
	غیرعلامتی طویل بنیاد_2 GUARDED_BY(&تالا_2);
	غیرعلامتی طویل سائز GUARDED_BY(&تالا_2);
	غیرعلامتی طویل resv GUARDED_BY(&تالا_2);
	ساخت sbi_dlist free_گرہ_list GUARDED_BY(&تالا_2);
	ساخت sbi_dlist free_space_list GUARDED_BY(&تالا_2);
	ساخت sbi_dlist مستعمل_space_list GUARDED_BY(&تالا_2);
	ساخت ڈھیر_گرہ init_free_space_گرہ GUARDED_BY(&تالا_2);
};

ساخت sbi_ڈھیر_control global_hpctrl;

ساکن bool alloc_nodes(ساخت sbi_ڈھیر_control *hpctrl) MUST_HOLD(&hpctrl->تالا_2)
{
	size_t سائز = ڈھیر_گرہ_BATCH_سائز * حجم(ساخت ڈھیر_گرہ);
	ساخت ڈھیر_گرہ *n, *new = تہی;

	/* alloc_with_align() requires at most two free nodes */
	اگر (hpctrl->free_گرہ_list.اگلا != hpctrl->free_گرہ_list.پچھلا)
		واپس true;

	sbi_list_for_each_اندراج_الٹ(n, &hpctrl->free_space_list, سر) {
		اگر (n->سائز >= سائز) {
			n->سائز -= سائز;
			اگر (!n->سائز) {
				sbi_list_del(&n->سر);
				sbi_list_جوڑو_دم(&n->سر, &hpctrl->free_گرہ_list);
			}
			new = (خالی *)(n->addr + n->سائز);
			توڑو;
		}
	}
	اگر (!new)
		واپس false;

	برائے (size_t i = 0; i < ڈھیر_گرہ_BATCH_سائز; i++)
		sbi_list_جوڑو_دم(&new[i].سر, &hpctrl->free_گرہ_list);
	hpctrl->resv += سائز;

	واپس true;
}

ساکن خالی *alloc_with_align(ساخت sbi_ڈھیر_control *hpctrl, size_t align,
			      size_t سائز)
	MUST_NOT_HOLD(&hpctrl->تالا_2)
{
	خالی *ret = تہی;
	ساخت ڈھیر_گرہ *n, *np;
	غیرعلامتی طویل سب_سے_نیچا_aligned;
	size_t pad;

	اگر (!سائز)
		واپس تہی;

	سائز += align - 1;
	سائز &= ~((غیرعلامتی طویل)align - 1);

	spin_تالا(&hpctrl->تالا_2);

	/* Ensure at least two free nodes are available for use below */
	اگر (!alloc_nodes(hpctrl))
		جاؤ برآمد;

	np = تہی;
	sbi_list_for_each_اندراج(n, &hpctrl->free_space_list, سر) {
		سب_سے_نیچا_aligned = ROUNDUP(n->addr, align);
		pad = سب_سے_نیچا_aligned - n->addr;

		اگر (سائز + pad <= n->سائز) {
			np = n;
			توڑو;
		}
	}
	اگر (!np)
		جاؤ برآمد;

	اگر (pad) {
		n = sbi_list_پہلا_اندراج(&hpctrl->free_گرہ_list,
					 ساخت ڈھیر_گرہ, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr;
		n->سائز = pad;
		sbi_list_جوڑو_دم(&n->سر, &np->سر);

		np->addr += pad;
		np->سائز -= pad;
	}

	اگر (سائز < np->سائز) {
		n = sbi_list_پہلا_اندراج(&hpctrl->free_گرہ_list,
					 ساخت ڈھیر_گرہ, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr + سائز;
		n->سائز = np->سائز - سائز;
		sbi_list_جوڑو(&n->سر, &np->سر);

		np->سائز = سائز;
	}

	sbi_list_del(&np->سر);
	sbi_list_جوڑو_دم(&np->سر, &hpctrl->مستعمل_space_list);
	ret = (خالی *)np->addr;

برآمد:
	spin_unlock(&hpctrl->تالا_2);

	واپس ret;
}

خالی *sbi_malloc_from(ساخت sbi_ڈھیر_control *hpctrl, size_t سائز)
{
	واپس alloc_with_align(hpctrl, ڈھیر_ALLOC_ALIGN, سائز);
}

خالی *sbi_aligned_alloc_from(ساخت sbi_ڈھیر_control *hpctrl,
			     size_t alignment, size_t سائز)
{
	اگر (alignment < ڈھیر_ALLOC_ALIGN)
		alignment = ڈھیر_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	اگر ((alignment & (alignment - 1)) != 0)
		واپس تہی;

	/* Make sure size is multiple of alignment */
	اگر (سائز % alignment != 0)
		واپس تہی;

	واپس alloc_with_align(hpctrl, alignment, سائز);
}

خالی *sbi_zalloc_from(ساخت sbi_ڈھیر_control *hpctrl, size_t سائز)
{
	خالی *ret = sbi_malloc_from(hpctrl, سائز);

	اگر (ret)
		sbi_memset(ret, 0, سائز);
	واپس ret;
}

خالی sbi_free_from(ساخت sbi_ڈھیر_control *hpctrl, خالی *ptr)
{
	ساخت ڈھیر_گرہ *n, *np;

	اگر (!ptr)
		واپس;

	spin_تالا(&hpctrl->تالا_2);

	np = تہی;
	sbi_list_for_each_اندراج(n, &hpctrl->مستعمل_space_list, سر) {
		اگر ((n->addr <= (غیرعلامتی طویل)ptr) &&
		    ((غیرعلامتی طویل)ptr < (n->addr + n->سائز))) {
			np = n;
			توڑو;
		}
	}
	اگر (!np) {
		spin_unlock(&hpctrl->تالا_2);
		واپس;
	}

	sbi_list_del(&np->سر);

	sbi_list_for_each_اندراج(n, &hpctrl->free_space_list, سر) {
		اگر ((np->addr + np->سائز) == n->addr) {
			n->addr = np->addr;
			n->سائز += np->سائز;
			sbi_list_جوڑو_دم(&np->سر, &hpctrl->free_گرہ_list);
			np = تہی;
			توڑو;
		} ورنہ اگر (np->addr == (n->addr + n->سائز)) {
			n->سائز += np->سائز;
			sbi_list_جوڑو_دم(&np->سر, &hpctrl->free_گرہ_list);
			np = تہی;
			توڑو;
		} ورنہ اگر ((n->addr + n->سائز) < np->addr) {
			sbi_list_جوڑو(&np->سر, &n->سر);
			np = تہی;
			توڑو;
		}
	}
	اگر (np)
		sbi_list_جوڑو_دم(&np->سر, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->تالا_2);
}

غیرعلامتی طویل sbi_ڈھیر_free_space_from(ساخت sbi_ڈھیر_control *hpctrl)
{
	ساخت ڈھیر_گرہ *n;
	غیرعلامتی طویل ret = 0;

	spin_تالا(&hpctrl->تالا_2);
	sbi_list_for_each_اندراج(n, &hpctrl->free_space_list, سر)
		ret += n->سائز;
	spin_unlock(&hpctrl->تالا_2);

	واپس ret;
}

غیرعلامتی طویل sbi_ڈھیر_مستعمل_space_from(ساخت sbi_ڈھیر_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	واپس hpctrl->سائز - hpctrl->resv - sbi_ڈھیر_free_space();
}

غیرعلامتی طویل sbi_ڈھیر_reserved_space_from(ساخت sbi_ڈھیر_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	واپس hpctrl->resv;
}

صحیح sbi_ڈھیر_init_new(ساخت sbi_ڈھیر_control *hpctrl, غیرعلامتی طویل بنیاد_2,
		      غیرعلامتی طویل سائز) NO_THREAD_SAFETY_ANALYSIS
{
	ساخت ڈھیر_گرہ *n;

	/* Initialize heap control */
	SPIN_تالا_INIT(hpctrl->تالا_2);
	hpctrl->بنیاد_2 = بنیاد_2;
	hpctrl->سائز = سائز;
	hpctrl->resv = 0;
	SBI_INIT_LIST_سر(&hpctrl->free_گرہ_list);
	SBI_INIT_LIST_سر(&hpctrl->free_space_list);
	SBI_INIT_LIST_سر(&hpctrl->مستعمل_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_گرہ;
	n->addr = بنیاد_2;
	n->سائز = سائز;
	sbi_list_جوڑو_دم(&n->سر, &hpctrl->free_space_list);

	واپس 0;
}

صحیح sbi_ڈھیر_init(ساخت sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	اگر (!scratch->fw_ڈھیر_سائز ||
	    (scratch->fw_ڈھیر_سائز & (ڈھیر_بنیاد_ALIGN - 1)) ||
	    (scratch->fw_ڈھیر_انحراف < scratch->fw_rw_انحراف) ||
	    (scratch->fw_سائز < (scratch->fw_ڈھیر_انحراف + scratch->fw_ڈھیر_سائز)) ||
	    (scratch->fw_ڈھیر_انحراف & (ڈھیر_بنیاد_ALIGN - 1)))
		واپس SBI_EINVAL;

	واپس sbi_ڈھیر_init_new(&global_hpctrl,
				  scratch->fw_آغاز + scratch->fw_ڈھیر_انحراف,
				  scratch->fw_ڈھیر_سائز);
}

صحیح sbi_ڈھیر_alloc_new(ساخت sbi_ڈھیر_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, حجم(ساخت sbi_ڈھیر_control));
	واپس 0;
}
