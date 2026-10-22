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

#وصف_ڪريو DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_ڳڻپ;
u32 hartindex_منزل_hartid_جدول[SBI_HARTMASK_MAX_بٽ] = { [0 ... SBI_HARTMASK_MAX_بٽ-1] = -1U };
بنية sbi_scratch *hartindex_منزل_scratch_جدول[SBI_HARTMASK_MAX_بٽ];

ساكن spinlock_t extra_تالو = SPIN_تالو_INITIALIZER;
ساكن بنانشان ڊگهو
	extra_وٿي GUARDED_BY(&extra_تالو) = SBI_SCRATCH_EXTRA_SPACE_وٿي;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ساكن بنانشان ڊگهو sbi_get_scratch_alloc_align(فراغ)
{
	ثابت بنية sbi_platform *plat = sbi_platform_thishart_ptr();

	إذا (!plat || !plat->cbom_بلاڪ_سائيز)
		أرجع DEFAULT_SCRATCH_ALLOC_ALIGN;
	أرجع plat->cbom_بلاڪ_سائيز;
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
	u32 h, hart_ڳڻپ;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_ڳڻپ = plat->hart_ڳڻپ;
	إذا (hart_ڳڻپ > SBI_HARTMASK_MAX_بٽ)
		hart_ڳڻپ = SBI_HARTMASK_MAX_بٽ;
	sbi_scratch_hart_ڳڻپ = hart_ڳڻپ;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_منزل_hartid_جدول[i] = h;
		hartindex_منزل_scratch_جدول[i] =
			((hartid2scratch)scratch->hartid_منزل_scratch)(h, i);
	}

	أرجع 0;
}

بنانشان ڊگهو sbi_scratch_alloc_وٿي(بنانشان ڊگهو سائيز)
{
	فراغ *ptr;
	بنانشان ڊگهو ret = 0;
	بنية sbi_scratch *rscratch;
	بنانشان ڊگهو scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	إذا (!سائيز)
		أرجع 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	سائيز += scratch_alloc_align - 1;
	سائيز &= ~(scratch_alloc_align - 1);

	spin_تالو(&extra_تالو);

	إذا (SBI_SCRATCH_سائيز < (extra_وٿي + سائيز))
		اذهب پورو;

	ret = extra_وٿي;
	extra_وٿي += سائيز;

پورو:
	spin_unlock(&extra_تالو);

	إذا (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_منزل_scratch(i);
			إذا (!rscratch)
				استمر;
			ptr = sbi_scratch_وٿي_ptr(rscratch, ret);
			sbi_memset(ptr, 0, سائيز);
		}
	}

	أرجع ret;
}

فراغ sbi_scratch_free_وٿي(بنانشان ڊگهو وٿي)
{
	إذا ((وٿي < SBI_SCRATCH_EXTRA_SPACE_وٿي) ||
	    (SBI_SCRATCH_سائيز <= وٿي))
		أرجع;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

بنانشان ڊگهو sbi_scratch_استعمال_ٿيل_space(فراغ)
{
	بنانشان ڊگهو ret = 0;

	spin_تالو(&extra_تالو);
	ret = extra_وٿي;
	spin_unlock(&extra_تالو);

	أرجع ret;
}
