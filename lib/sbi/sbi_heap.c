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
#تعریف_کرو ڈھیر_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#تعریف_کرو ڈھیر_گرہ_BATCH_ناپ		8

بنية ڈھیر_گرہ {
	بنية sbi_dlist سر;
	بنانشانی لما addr;
	بنانشانی لما ناپ;
};

بنية sbi_ڈھیر_control {
	spinlock_t جندرا;
	بنانشانی لما ادھار_2 GUARDED_BY(&جندرا);
	بنانشانی لما ناپ GUARDED_BY(&جندرا);
	بنانشانی لما resv GUARDED_BY(&جندرا);
	بنية sbi_dlist free_گرہ_list GUARDED_BY(&جندرا);
	بنية sbi_dlist free_space_list GUARDED_BY(&جندرا);
	بنية sbi_dlist ورتیا_space_list GUARDED_BY(&جندرا);
	بنية ڈھیر_گرہ init_free_space_گرہ GUARDED_BY(&جندرا);
};

بنية sbi_ڈھیر_control global_hpctrl;

ساكن bool alloc_nodes(بنية sbi_ڈھیر_control *hpctrl) MUST_HOLD(&hpctrl->جندرا)
{
	size_t ناپ = ڈھیر_گرہ_BATCH_ناپ * حجم(بنية ڈھیر_گرہ);
	بنية ڈھیر_گرہ *n, *new = عدم;

	/* alloc_with_align() requires at most two free nodes */
	إذا (hpctrl->free_گرہ_list.اگلا != hpctrl->free_گرہ_list.prev)
		أرجع true;

	sbi_list_for_each_اندراج_الٹ(n, &hpctrl->free_space_list, سر) {
		إذا (n->ناپ >= ناپ) {
			n->ناپ -= ناپ;
			إذا (!n->ناپ) {
				sbi_list_del(&n->سر);
				sbi_list_جوڑو_پوچھ(&n->سر, &hpctrl->free_گرہ_list);
			}
			new = (فراغ *)(n->addr + n->ناپ);
			اكسر;
		}
	}
	إذا (!new)
		أرجع false;

	لكل (size_t i = 0; i < ڈھیر_گرہ_BATCH_ناپ; i++)
		sbi_list_جوڑو_پوچھ(&new[i].سر, &hpctrl->free_گرہ_list);
	hpctrl->resv += ناپ;

	أرجع true;
}

ساكن فراغ *alloc_with_align(بنية sbi_ڈھیر_control *hpctrl, size_t align,
			      size_t ناپ)
	MUST_NOT_HOLD(&hpctrl->جندرا)
{
	فراغ *ret = عدم;
	بنية ڈھیر_گرہ *n, *np;
	بنانشانی لما سبھ_توں_نیواں_aligned;
	size_t pad;

	إذا (!ناپ)
		أرجع عدم;

	ناپ += align - 1;
	ناپ &= ~((بنانشانی لما)align - 1);

	spin_جندرا(&hpctrl->جندرا);

	/* Ensure at least two free nodes are available for use below */
	إذا (!alloc_nodes(hpctrl))
		اذهب باہرلا;

	np = عدم;
	sbi_list_for_each_اندراج(n, &hpctrl->free_space_list, سر) {
		سبھ_توں_نیواں_aligned = ROUNDUP(n->addr, align);
		pad = سبھ_توں_نیواں_aligned - n->addr;

		إذا (ناپ + pad <= n->ناپ) {
			np = n;
			اكسر;
		}
	}
	إذا (!np)
		اذهب باہرلا;

	إذا (pad) {
		n = sbi_list_پہلا_اندراج(&hpctrl->free_گرہ_list,
					 بنية ڈھیر_گرہ, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr;
		n->ناپ = pad;
		sbi_list_جوڑو_پوچھ(&n->سر, &np->سر);

		np->addr += pad;
		np->ناپ -= pad;
	}

	إذا (ناپ < np->ناپ) {
		n = sbi_list_پہلا_اندراج(&hpctrl->free_گرہ_list,
					 بنية ڈھیر_گرہ, سر);
		sbi_list_del(&n->سر);

		n->addr = np->addr + ناپ;
		n->ناپ = np->ناپ - ناپ;
		sbi_list_جوڑو(&n->سر, &np->سر);

		np->ناپ = ناپ;
	}

	sbi_list_del(&np->سر);
	sbi_list_جوڑو_پوچھ(&np->سر, &hpctrl->ورتیا_space_list);
	ret = (فراغ *)np->addr;

باہرلا:
	spin_unlock(&hpctrl->جندرا);

	أرجع ret;
}

فراغ *sbi_malloc_from(بنية sbi_ڈھیر_control *hpctrl, size_t ناپ)
{
	أرجع alloc_with_align(hpctrl, ڈھیر_ALLOC_ALIGN, ناپ);
}

فراغ *sbi_aligned_alloc_from(بنية sbi_ڈھیر_control *hpctrl,
			     size_t alignment, size_t ناپ)
{
	إذا (alignment < ڈھیر_ALLOC_ALIGN)
		alignment = ڈھیر_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	إذا ((alignment & (alignment - 1)) != 0)
		أرجع عدم;

	/* Make sure size is multiple of alignment */
	إذا (ناپ % alignment != 0)
		أرجع عدم;

	أرجع alloc_with_align(hpctrl, alignment, ناپ);
}

فراغ *sbi_zalloc_from(بنية sbi_ڈھیر_control *hpctrl, size_t ناپ)
{
	فراغ *ret = sbi_malloc_from(hpctrl, ناپ);

	إذا (ret)
		sbi_memset(ret, 0, ناپ);
	أرجع ret;
}

فراغ sbi_free_from(بنية sbi_ڈھیر_control *hpctrl, فراغ *ptr)
{
	بنية ڈھیر_گرہ *n, *np;

	إذا (!ptr)
		أرجع;

	spin_جندرا(&hpctrl->جندرا);

	np = عدم;
	sbi_list_for_each_اندراج(n, &hpctrl->ورتیا_space_list, سر) {
		إذا ((n->addr <= (بنانشانی لما)ptr) &&
		    ((بنانشانی لما)ptr < (n->addr + n->ناپ))) {
			np = n;
			اكسر;
		}
	}
	إذا (!np) {
		spin_unlock(&hpctrl->جندرا);
		أرجع;
	}

	sbi_list_del(&np->سر);

	sbi_list_for_each_اندراج(n, &hpctrl->free_space_list, سر) {
		إذا ((np->addr + np->ناپ) == n->addr) {
			n->addr = np->addr;
			n->ناپ += np->ناپ;
			sbi_list_جوڑو_پوچھ(&np->سر, &hpctrl->free_گرہ_list);
			np = عدم;
			اكسر;
		} وإلا إذا (np->addr == (n->addr + n->ناپ)) {
			n->ناپ += np->ناپ;
			sbi_list_جوڑو_پوچھ(&np->سر, &hpctrl->free_گرہ_list);
			np = عدم;
			اكسر;
		} وإلا إذا ((n->addr + n->ناپ) < np->addr) {
			sbi_list_جوڑو(&np->سر, &n->سر);
			np = عدم;
			اكسر;
		}
	}
	إذا (np)
		sbi_list_جوڑو_پوچھ(&np->سر, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->جندرا);
}

بنانشانی لما sbi_ڈھیر_free_space_from(بنية sbi_ڈھیر_control *hpctrl)
{
	بنية ڈھیر_گرہ *n;
	بنانشانی لما ret = 0;

	spin_جندرا(&hpctrl->جندرا);
	sbi_list_for_each_اندراج(n, &hpctrl->free_space_list, سر)
		ret += n->ناپ;
	spin_unlock(&hpctrl->جندرا);

	أرجع ret;
}

بنانشانی لما sbi_ڈھیر_ورتیا_space_from(بنية sbi_ڈھیر_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	أرجع hpctrl->ناپ - hpctrl->resv - sbi_ڈھیر_free_space();
}

بنانشانی لما sbi_ڈھیر_reserved_space_from(بنية sbi_ڈھیر_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	أرجع hpctrl->resv;
}

صحيح sbi_ڈھیر_init_new(بنية sbi_ڈھیر_control *hpctrl, بنانشانی لما ادھار_2,
		      بنانشانی لما ناپ) NO_THREAD_SAFETY_ANALYSIS
{
	بنية ڈھیر_گرہ *n;

	/* Initialize heap control */
	SPIN_جندرا_INIT(hpctrl->جندرا);
	hpctrl->ادھار_2 = ادھار_2;
	hpctrl->ناپ = ناپ;
	hpctrl->resv = 0;
	SBI_INIT_LIST_سر(&hpctrl->free_گرہ_list);
	SBI_INIT_LIST_سر(&hpctrl->free_space_list);
	SBI_INIT_LIST_سر(&hpctrl->ورتیا_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_گرہ;
	n->addr = ادھار_2;
	n->ناپ = ناپ;
	sbi_list_جوڑو_پوچھ(&n->سر, &hpctrl->free_space_list);

	أرجع 0;
}

صحيح sbi_ڈھیر_init(بنية sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	إذا (!scratch->fw_ڈھیر_ناپ ||
	    (scratch->fw_ڈھیر_ناپ & (ڈھیر_ادھار_ALIGN - 1)) ||
	    (scratch->fw_ڈھیر_وتھ < scratch->fw_rw_وتھ) ||
	    (scratch->fw_ناپ < (scratch->fw_ڈھیر_وتھ + scratch->fw_ڈھیر_ناپ)) ||
	    (scratch->fw_ڈھیر_وتھ & (ڈھیر_ادھار_ALIGN - 1)))
		أرجع SBI_EINVAL;

	أرجع sbi_ڈھیر_init_new(&global_hpctrl,
				  scratch->fw_شروع + scratch->fw_ڈھیر_وتھ,
				  scratch->fw_ڈھیر_ناپ);
}

صحيح sbi_ڈھیر_alloc_new(بنية sbi_ڈھیر_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, حجم(بنية sbi_ڈھیر_control));
	أرجع 0;
}
