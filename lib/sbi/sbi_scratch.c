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

#تعریف_کن DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_شمار;
u32 hartindex_مقصد_hartid_جدول[SBI_HARTMASK_MAX_بیتها] = { [0 ... SBI_HARTMASK_MAX_بیتها-1] = -1U };
بنية sbi_scratch *hartindex_مقصد_scratch_جدول[SBI_HARTMASK_MAX_بیتها];

ساكن spinlock_t extra_قفل = SPIN_قفل_INITIALIZER;
ساكن بیعلامت دراز
	extra_جابه_جایی GUARDED_BY(&extra_قفل) = SBI_SCRATCH_EXTRA_SPACE_جابه_جایی;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ساكن بیعلامت دراز sbi_get_scratch_alloc_align(فراغ)
{
	ثابت بنية sbi_platform *plat = sbi_platform_thishart_ptr();

	إذا (!plat || !plat->cbom_بلوک_اندازه)
		أرجع DEFAULT_SCRATCH_ALLOC_ALIGN;
	أرجع plat->cbom_بلوک_اندازه;
}

u32 sbi_hartid_مقصد_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		إذا (hartindex_مقصد_hartid_جدول[i] == hartid)
			أرجع i;

	أرجع -1U;
}

تعريفنوع بنية sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

صحيح sbi_scratch_init(بنية sbi_scratch *scratch)
{
	u32 h, hart_شمار;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_شمار = plat->hart_شمار;
	إذا (hart_شمار > SBI_HARTMASK_MAX_بیتها)
		hart_شمار = SBI_HARTMASK_MAX_بیتها;
	sbi_scratch_hart_شمار = hart_شمار;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_مقصد_hartid_جدول[i] = h;
		hartindex_مقصد_scratch_جدول[i] =
			((hartid2scratch)scratch->hartid_مقصد_scratch)(h, i);
	}

	أرجع 0;
}

بیعلامت دراز sbi_scratch_alloc_جابه_جایی(بیعلامت دراز اندازه)
{
	فراغ *ptr;
	بیعلامت دراز ret = 0;
	بنية sbi_scratch *rscratch;
	بیعلامت دراز scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	إذا (!اندازه)
		أرجع 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	اندازه += scratch_alloc_align - 1;
	اندازه &= ~(scratch_alloc_align - 1);

	spin_قفل(&extra_قفل);

	إذا (SBI_SCRATCH_اندازه < (extra_جابه_جایی + اندازه))
		اذهب انجامشده;

	ret = extra_جابه_جایی;
	extra_جابه_جایی += اندازه;

انجامشده:
	spin_unlock(&extra_قفل);

	إذا (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_مقصد_scratch(i);
			إذا (!rscratch)
				استمر;
			ptr = sbi_scratch_جابه_جایی_ptr(rscratch, ret);
			sbi_memset(ptr, 0, اندازه);
		}
	}

	أرجع ret;
}

فراغ sbi_scratch_free_جابه_جایی(بیعلامت دراز جابه_جایی)
{
	إذا ((جابه_جایی < SBI_SCRATCH_EXTRA_SPACE_جابه_جایی) ||
	    (SBI_SCRATCH_اندازه <= جابه_جایی))
		أرجع;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

بیعلامت دراز sbi_scratch_استفاده_شده_space(فراغ)
{
	بیعلامت دراز ret = 0;

	spin_قفل(&extra_قفل);
	ret = extra_جابه_جایی;
	spin_unlock(&extra_قفل);

	أرجع ret;
}
