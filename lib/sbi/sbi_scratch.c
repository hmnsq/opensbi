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

#تعریف_کړه DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_شمېر;
u32 hartindex_منزل_hartid_جدول[SBI_HARTMASK_MAX_بټونه] = { [0 ... SBI_HARTMASK_MAX_بټونه-1] = -1U };
بنية sbi_scratch *hartindex_منزل_scratch_جدول[SBI_HARTMASK_MAX_بټونه];

ساكن spinlock_t extra_قلف = SPIN_قلف_INITIALIZER;
ساكن بینښه اوږد
	extra_دننه_واټن GUARDED_BY(&extra_قلف) = SBI_SCRATCH_EXTRA_SPACE_دننه_واټن;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ساكن بینښه اوږد sbi_get_scratch_alloc_align(فراغ)
{
	ثابت بنية sbi_platform *plat = sbi_platform_thishart_ptr();

	إذا (!plat || !plat->cbom_بلاک_اندازه)
		أرجع DEFAULT_SCRATCH_ALLOC_ALIGN;
	أرجع plat->cbom_بلاک_اندازه;
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
	u32 h, hart_شمېر;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_شمېر = plat->hart_شمېر;
	إذا (hart_شمېر > SBI_HARTMASK_MAX_بټونه)
		hart_شمېر = SBI_HARTMASK_MAX_بټونه;
	sbi_scratch_hart_شمېر = hart_شمېر;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_منزل_hartid_جدول[i] = h;
		hartindex_منزل_scratch_جدول[i] =
			((hartid2scratch)scratch->hartid_منزل_scratch)(h, i);
	}

	أرجع 0;
}

بینښه اوږد sbi_scratch_alloc_دننه_واټن(بینښه اوږد اندازه)
{
	فراغ *ptr;
	بینښه اوږد ret = 0;
	بنية sbi_scratch *rscratch;
	بینښه اوږد scratch_alloc_align = 0;

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

	spin_قلف(&extra_قلف);

	إذا (SBI_SCRATCH_اندازه < (extra_دننه_واټن + اندازه))
		اذهب بشپړ;

	ret = extra_دننه_واټن;
	extra_دننه_واټن += اندازه;

بشپړ:
	spin_unlock(&extra_قلف);

	إذا (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_منزل_scratch(i);
			إذا (!rscratch)
				استمر;
			ptr = sbi_scratch_دننه_واټن_ptr(rscratch, ret);
			sbi_memset(ptr, 0, اندازه);
		}
	}

	أرجع ret;
}

فراغ sbi_scratch_free_دننه_واټن(بینښه اوږد دننه_واټن)
{
	إذا ((دننه_واټن < SBI_SCRATCH_EXTRA_SPACE_دننه_واټن) ||
	    (SBI_SCRATCH_اندازه <= دننه_واټن))
		أرجع;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

بینښه اوږد sbi_scratch_کارول_شوی_space(فراغ)
{
	بینښه اوږد ret = 0;

	spin_قلف(&extra_قلف);
	ret = extra_دننه_واټن;
	spin_unlock(&extra_قلف);

	أرجع ret;
}
