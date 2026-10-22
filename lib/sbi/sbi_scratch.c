 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hartmask.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>

#تعریف_کرو DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_گنتی;
u32 hartindex_منزل_hartid_جدول[SBI_HARTMASK_MAX_بٹ] = { [0 ... SBI_HARTMASK_MAX_بٹ-1] = -1U };
بنية sbi_scratch *hartindex_منزل_scratch_جدول[SBI_HARTMASK_MAX_بٹ];

ساكن spinlock_t extra_جندرا = SPIN_جندرا_INITIALIZER;
ساكن بنانشانی لما
	extra_وتھ GUARDED_BY(&extra_جندرا) = SBI_SCRATCH_EXTRA_SPACE_وتھ;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ساكن بنانشانی لما sbi_get_scratch_alloc_align(فراغ)
{
	ثابت بنية sbi_platform *plat = sbi_platform_thishart_ptr();

	إذا (!plat || !plat->cbom_بلاک_ناپ)
		أرجع DEFAULT_SCRATCH_ALLOC_ALIGN;
	أرجع plat->cbom_بلاک_ناپ;
}

u32 sbi_hartid_منزل_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		إذا (hartindex_منزل_hartid_جدول[i] == hartid)
			أرجع i;

	أرجع -1U;
}

تعريفنوع بنية sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

صحيح sbi_scratch_init(بنية sbi_scratch *scratch)
{
	u32 h, hart_گنتی;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_گنتی = plat->hart_گنتی;
	إذا (hart_گنتی > SBI_HARTMASK_MAX_بٹ)
		hart_گنتی = SBI_HARTMASK_MAX_بٹ;
	sbi_scratch_hart_گنتی = hart_گنتی;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_منزل_hartid_جدول[i] = h;
		hartindex_منزل_scratch_جدول[i] =
			((hartid2scratch)scratch->hartid_منزل_scratch)(h, i);
	}

	أرجع 0;
}

بنانشانی لما sbi_scratch_alloc_وتھ(بنانشانی لما ناپ)
{
	فراغ *ptr;
	بنانشانی لما ret = 0;
	بنية sbi_scratch *rscratch;
	بنانشانی لما scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	إذا (!ناپ)
		أرجع 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	ناپ += scratch_alloc_align - 1;
	ناپ &= ~(scratch_alloc_align - 1);

	spin_جندرا(&extra_جندرا);

	إذا (SBI_SCRATCH_ناپ < (extra_وتھ + ناپ))
		اذهب پورا;

	ret = extra_وتھ;
	extra_وتھ += ناپ;

پورا:
	spin_unlock(&extra_جندرا);

	إذا (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_منزل_scratch(i);
			إذا (!rscratch)
				استمر;
			ptr = sbi_scratch_وتھ_ptr(rscratch, ret);
			sbi_memset(ptr, 0, ناپ);
		}
	}

	أرجع ret;
}

فراغ sbi_scratch_free_وتھ(بنانشانی لما وتھ)
{
	إذا ((وتھ < SBI_SCRATCH_EXTRA_SPACE_وتھ) ||
	    (SBI_SCRATCH_ناپ <= وتھ))
		أرجع;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

بنانشانی لما sbi_scratch_ورتیا_space(فراغ)
{
	بنانشانی لما ret = 0;

	spin_جندرا(&extra_جندرا);
	ret = extra_وتھ;
	spin_unlock(&extra_جندرا);

	أرجع ret;
}
