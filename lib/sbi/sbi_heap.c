/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_list.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#నిర్వచించు కుప్ప_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#నిర్వచించు కుప్ప_కణుపు_BATCH_కొలత		8

నిర్మాణం కుప్ప_కణుపు {
	నిర్మాణం sbi_dlist తల;
	గుర్తులేని దీర్ఘ addr;
	గుర్తులేని దీర్ఘ కొలత;
};

నిర్మాణం sbi_కుప్ప_control {
	spinlock_t తాళం_2;
	గుర్తులేని దీర్ఘ పునాది_2 GUARDED_BY(&తాళం_2);
	గుర్తులేని దీర్ఘ కొలత GUARDED_BY(&తాళం_2);
	గుర్తులేని దీర్ఘ resv GUARDED_BY(&తాళం_2);
	నిర్మాణం sbi_dlist free_కణుపు_list GUARDED_BY(&తాళం_2);
	నిర్మాణం sbi_dlist free_space_list GUARDED_BY(&తాళం_2);
	నిర్మాణం sbi_dlist వాడిన_space_list GUARDED_BY(&తాళం_2);
	నిర్మాణం కుప్ప_కణుపు init_free_space_కణుపు GUARDED_BY(&తాళం_2);
};

నిర్మాణం sbi_కుప్ప_control global_hpctrl;

స్థిర bool alloc_nodes(నిర్మాణం sbi_కుప్ప_control *hpctrl) MUST_HOLD(&hpctrl->తాళం_2)
{
	size_t కొలత = కుప్ప_కణుపు_BATCH_కొలత * పరిమాణం(నిర్మాణం కుప్ప_కణుపు);
	నిర్మాణం కుప్ప_కణుపు *n, *new = ఖాళీ;

	/* alloc_with_align() requires at most two free nodes */
	ఒకవేళ (hpctrl->free_కణుపు_list.తదుపరి != hpctrl->free_కణుపు_list.prev)
		తిరిగి true;

	sbi_list_for_each_నమోదు_తిరగేయి(n, &hpctrl->free_space_list, తల) {
		ఒకవేళ (n->కొలత >= కొలత) {
			n->కొలత -= కొలత;
			ఒకవేళ (!n->కొలత) {
				sbi_list_del(&n->తల);
				sbi_list_కూడు_తోక(&n->తల, &hpctrl->free_కణుపు_list);
			}
			new = (శూన్య *)(n->addr + n->కొలత);
			ఆపు;
		}
	}
	ఒకవేళ (!new)
		తిరిగి false;

	కొరకు (size_t i = 0; i < కుప్ప_కణుపు_BATCH_కొలత; i++)
		sbi_list_కూడు_తోక(&new[i].తల, &hpctrl->free_కణుపు_list);
	hpctrl->resv += కొలత;

	తిరిగి true;
}

స్థిర శూన్య *alloc_with_align(నిర్మాణం sbi_కుప్ప_control *hpctrl, size_t align,
			      size_t కొలత)
	MUST_NOT_HOLD(&hpctrl->తాళం_2)
{
	శూన్య *ret = ఖాళీ;
	నిర్మాణం కుప్ప_కణుపు *n, *np;
	గుర్తులేని దీర్ఘ అత్యల్ప_aligned;
	size_t pad;

	ఒకవేళ (!కొలత)
		తిరిగి ఖాళీ;

	కొలత += align - 1;
	కొలత &= ~((గుర్తులేని దీర్ఘ)align - 1);

	spin_తాళం(&hpctrl->తాళం_2);

	/* Ensure at least two free nodes are available for use below */
	ఒకవేళ (!alloc_nodes(hpctrl))
		వెళ్ళు బహిర్గతం;

	np = ఖాళీ;
	sbi_list_for_each_నమోదు(n, &hpctrl->free_space_list, తల) {
		అత్యల్ప_aligned = ROUNDUP(n->addr, align);
		pad = అత్యల్ప_aligned - n->addr;

		ఒకవేళ (కొలత + pad <= n->కొలత) {
			np = n;
			ఆపు;
		}
	}
	ఒకవేళ (!np)
		వెళ్ళు బహిర్గతం;

	ఒకవేళ (pad) {
		n = sbi_list_మొదటి_నమోదు(&hpctrl->free_కణుపు_list,
					 నిర్మాణం కుప్ప_కణుపు, తల);
		sbi_list_del(&n->తల);

		n->addr = np->addr;
		n->కొలత = pad;
		sbi_list_కూడు_తోక(&n->తల, &np->తల);

		np->addr += pad;
		np->కొలత -= pad;
	}

	ఒకవేళ (కొలత < np->కొలత) {
		n = sbi_list_మొదటి_నమోదు(&hpctrl->free_కణుపు_list,
					 నిర్మాణం కుప్ప_కణుపు, తల);
		sbi_list_del(&n->తల);

		n->addr = np->addr + కొలత;
		n->కొలత = np->కొలత - కొలత;
		sbi_list_కూడు(&n->తల, &np->తల);

		np->కొలత = కొలత;
	}

	sbi_list_del(&np->తల);
	sbi_list_కూడు_తోక(&np->తల, &hpctrl->వాడిన_space_list);
	ret = (శూన్య *)np->addr;

బహిర్గతం:
	spin_unlock(&hpctrl->తాళం_2);

	తిరిగి ret;
}

శూన్య *sbi_malloc_from(నిర్మాణం sbi_కుప్ప_control *hpctrl, size_t కొలత)
{
	తిరిగి alloc_with_align(hpctrl, కుప్ప_ALLOC_ALIGN, కొలత);
}

శూన్య *sbi_aligned_alloc_from(నిర్మాణం sbi_కుప్ప_control *hpctrl,
			     size_t alignment, size_t కొలత)
{
	ఒకవేళ (alignment < కుప్ప_ALLOC_ALIGN)
		alignment = కుప్ప_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	ఒకవేళ ((alignment & (alignment - 1)) != 0)
		తిరిగి ఖాళీ;

	/* Make sure size is multiple of alignment */
	ఒకవేళ (కొలత % alignment != 0)
		తిరిగి ఖాళీ;

	తిరిగి alloc_with_align(hpctrl, alignment, కొలత);
}

శూన్య *sbi_zalloc_from(నిర్మాణం sbi_కుప్ప_control *hpctrl, size_t కొలత)
{
	శూన్య *ret = sbi_malloc_from(hpctrl, కొలత);

	ఒకవేళ (ret)
		sbi_memset(ret, 0, కొలత);
	తిరిగి ret;
}

శూన్య sbi_free_from(నిర్మాణం sbi_కుప్ప_control *hpctrl, శూన్య *ptr)
{
	నిర్మాణం కుప్ప_కణుపు *n, *np;

	ఒకవేళ (!ptr)
		తిరిగి;

	spin_తాళం(&hpctrl->తాళం_2);

	np = ఖాళీ;
	sbi_list_for_each_నమోదు(n, &hpctrl->వాడిన_space_list, తల) {
		ఒకవేళ ((n->addr <= (గుర్తులేని దీర్ఘ)ptr) &&
		    ((గుర్తులేని దీర్ఘ)ptr < (n->addr + n->కొలత))) {
			np = n;
			ఆపు;
		}
	}
	ఒకవేళ (!np) {
		spin_unlock(&hpctrl->తాళం_2);
		తిరిగి;
	}

	sbi_list_del(&np->తల);

	sbi_list_for_each_నమోదు(n, &hpctrl->free_space_list, తల) {
		ఒకవేళ ((np->addr + np->కొలత) == n->addr) {
			n->addr = np->addr;
			n->కొలత += np->కొలత;
			sbi_list_కూడు_తోక(&np->తల, &hpctrl->free_కణుపు_list);
			np = ఖాళీ;
			ఆపు;
		} లేకపోతే ఒకవేళ (np->addr == (n->addr + n->కొలత)) {
			n->కొలత += np->కొలత;
			sbi_list_కూడు_తోక(&np->తల, &hpctrl->free_కణుపు_list);
			np = ఖాళీ;
			ఆపు;
		} లేకపోతే ఒకవేళ ((n->addr + n->కొలత) < np->addr) {
			sbi_list_కూడు(&np->తల, &n->తల);
			np = ఖాళీ;
			ఆపు;
		}
	}
	ఒకవేళ (np)
		sbi_list_కూడు_తోక(&np->తల, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->తాళం_2);
}

గుర్తులేని దీర్ఘ sbi_కుప్ప_free_space_from(నిర్మాణం sbi_కుప్ప_control *hpctrl)
{
	నిర్మాణం కుప్ప_కణుపు *n;
	గుర్తులేని దీర్ఘ ret = 0;

	spin_తాళం(&hpctrl->తాళం_2);
	sbi_list_for_each_నమోదు(n, &hpctrl->free_space_list, తల)
		ret += n->కొలత;
	spin_unlock(&hpctrl->తాళం_2);

	తిరిగి ret;
}

గుర్తులేని దీర్ఘ sbi_కుప్ప_వాడిన_space_from(నిర్మాణం sbi_కుప్ప_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	తిరిగి hpctrl->కొలత - hpctrl->resv - sbi_కుప్ప_free_space();
}

గుర్తులేని దీర్ఘ sbi_కుప్ప_reserved_space_from(నిర్మాణం sbi_కుప్ప_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	తిరిగి hpctrl->resv;
}

పూర్ణాంకం sbi_కుప్ప_init_new(నిర్మాణం sbi_కుప్ప_control *hpctrl, గుర్తులేని దీర్ఘ పునాది_2,
		      గుర్తులేని దీర్ఘ కొలత) NO_THREAD_SAFETY_ANALYSIS
{
	నిర్మాణం కుప్ప_కణుపు *n;

	/* Initialize heap control */
	SPIN_తాళం_INIT(hpctrl->తాళం_2);
	hpctrl->పునాది_2 = పునాది_2;
	hpctrl->కొలత = కొలత;
	hpctrl->resv = 0;
	SBI_INIT_LIST_తల(&hpctrl->free_కణుపు_list);
	SBI_INIT_LIST_తల(&hpctrl->free_space_list);
	SBI_INIT_LIST_తల(&hpctrl->వాడిన_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_కణుపు;
	n->addr = పునాది_2;
	n->కొలత = కొలత;
	sbi_list_కూడు_తోక(&n->తల, &hpctrl->free_space_list);

	తిరిగి 0;
}

పూర్ణాంకం sbi_కుప్ప_init(నిర్మాణం sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	ఒకవేళ (!scratch->fw_కుప్ప_కొలత ||
	    (scratch->fw_కుప్ప_కొలత & (కుప్ప_పునాది_ALIGN - 1)) ||
	    (scratch->fw_కుప్ప_స్థానభ్రంశం < scratch->fw_rw_స్థానభ్రంశం) ||
	    (scratch->fw_కొలత < (scratch->fw_కుప్ప_స్థానభ్రంశం + scratch->fw_కుప్ప_కొలత)) ||
	    (scratch->fw_కుప్ప_స్థానభ్రంశం & (కుప్ప_పునాది_ALIGN - 1)))
		తిరిగి SBI_EINVAL;

	తిరిగి sbi_కుప్ప_init_new(&global_hpctrl,
				  scratch->fw_ప్రారంభం + scratch->fw_కుప్ప_స్థానభ్రంశం,
				  scratch->fw_కుప్ప_కొలత);
}

పూర్ణాంకం sbi_కుప్ప_alloc_new(నిర్మాణం sbi_కుప్ప_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, పరిమాణం(నిర్మాణం sbi_కుప్ప_control));
	తిరిగి 0;
}
