 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hartmask.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>

#నిర్వచించు DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_లెక్క;
u32 hartindex_గమ్యం_hartid_పట్టిక[SBI_HARTMASK_MAX_బిట్లు] = { [0 ... SBI_HARTMASK_MAX_బిట్లు-1] = -1U };
నిర్మాణం sbi_scratch *hartindex_గమ్యం_scratch_పట్టిక[SBI_HARTMASK_MAX_బిట్లు];

స్థిర spinlock_t extra_తాళం = SPIN_తాళం_INITIALIZER;
స్థిర గుర్తులేని దీర్ఘ
	extra_స్థానభ్రంశం GUARDED_BY(&extra_తాళం) = SBI_SCRATCH_EXTRA_SPACE_స్థానభ్రంశం;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
స్థిర గుర్తులేని దీర్ఘ sbi_get_scratch_alloc_align(శూన్య)
{
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_thishart_ptr();

	ఒకవేళ (!plat || !plat->cbom_ఖండం_కొలత)
		తిరిగి DEFAULT_SCRATCH_ALLOC_ALIGN;
	తిరిగి plat->cbom_ఖండం_కొలత;
}

u32 sbi_hartid_గమ్యం_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		ఒకవేళ (hartindex_గమ్యం_hartid_పట్టిక[i] == hartid)
			తిరిగి i;

	తిరిగి -1U;
}

రకనిర్వచనం నిర్మాణం sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

పూర్ణాంకం sbi_scratch_init(నిర్మాణం sbi_scratch *scratch)
{
	u32 h, hart_లెక్క;
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_లెక్క = plat->hart_లెక్క;
	ఒకవేళ (hart_లెక్క > SBI_HARTMASK_MAX_బిట్లు)
		hart_లెక్క = SBI_HARTMASK_MAX_బిట్లు;
	sbi_scratch_hart_లెక్క = hart_లెక్క;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_గమ్యం_hartid_పట్టిక[i] = h;
		hartindex_గమ్యం_scratch_పట్టిక[i] =
			((hartid2scratch)scratch->hartid_గమ్యం_scratch)(h, i);
	}

	తిరిగి 0;
}

గుర్తులేని దీర్ఘ sbi_scratch_alloc_స్థానభ్రంశం(గుర్తులేని దీర్ఘ కొలత)
{
	శూన్య *ptr;
	గుర్తులేని దీర్ఘ ret = 0;
	నిర్మాణం sbi_scratch *rscratch;
	గుర్తులేని దీర్ఘ scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	ఒకవేళ (!కొలత)
		తిరిగి 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	కొలత += scratch_alloc_align - 1;
	కొలత &= ~(scratch_alloc_align - 1);

	spin_తాళం(&extra_తాళం);

	ఒకవేళ (SBI_SCRATCH_కొలత < (extra_స్థానభ్రంశం + కొలత))
		వెళ్ళు పూర్తి;

	ret = extra_స్థానభ్రంశం;
	extra_స్థానభ్రంశం += కొలత;

పూర్తి:
	spin_unlock(&extra_తాళం);

	ఒకవేళ (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_గమ్యం_scratch(i);
			ఒకవేళ (!rscratch)
				కొనసాగు;
			ptr = sbi_scratch_స్థానభ్రంశం_ptr(rscratch, ret);
			sbi_memset(ptr, 0, కొలత);
		}
	}

	తిరిగి ret;
}

శూన్య sbi_scratch_free_స్థానభ్రంశం(గుర్తులేని దీర్ఘ స్థానభ్రంశం)
{
	ఒకవేళ ((స్థానభ్రంశం < SBI_SCRATCH_EXTRA_SPACE_స్థానభ్రంశం) ||
	    (SBI_SCRATCH_కొలత <= స్థానభ్రంశం))
		తిరిగి;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

గుర్తులేని దీర్ఘ sbi_scratch_వాడిన_space(శూన్య)
{
	గుర్తులేని దీర్ఘ ret = 0;

	spin_తాళం(&extra_తాళం);
	ret = extra_స్థానభ్రంశం;
	spin_unlock(&extra_తాళం);

	తిరిగి ret;
}
