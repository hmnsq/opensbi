 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>

#تعریف_کرو DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_گنتی;
u32 hartindex_مقصد_hartid_جدول[SBI_HARTMASK_MAX_بٹ] = { [0 ... SBI_HARTMASK_MAX_بٹ-1] = -1U };
ساخت sbi_scratch *hartindex_مقصد_scratch_جدول[SBI_HARTMASK_MAX_بٹ];

ساکن spinlock_t extra_تالا = SPIN_تالا_INITIALIZER;
ساکن غیرعلامتی طویل
	extra_انحراف GUARDED_BY(&extra_تالا) = SBI_SCRATCH_EXTRA_SPACE_انحراف;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ساکن غیرعلامتی طویل sbi_get_scratch_alloc_align(خالی)
{
	ثابت ساخت sbi_platform *plat = sbi_platform_thishart_ptr();

	اگر (!plat || !plat->cbom_بلاک_سائز)
		واپس DEFAULT_SCRATCH_ALLOC_ALIGN;
	واپس plat->cbom_بلاک_سائز;
}

u32 sbi_hartid_مقصد_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		اگر (hartindex_مقصد_hartid_جدول[i] == hartid)
			واپس i;

	واپس -1U;
}

قسمتعریف ساخت sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

صحیح sbi_scratch_init(ساخت sbi_scratch *scratch)
{
	u32 h, hart_گنتی;
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_گنتی = plat->hart_گنتی;
	اگر (hart_گنتی > SBI_HARTMASK_MAX_بٹ)
		hart_گنتی = SBI_HARTMASK_MAX_بٹ;
	sbi_scratch_hart_گنتی = hart_گنتی;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_مقصد_hartid_جدول[i] = h;
		hartindex_مقصد_scratch_جدول[i] =
			((hartid2scratch)scratch->hartid_مقصد_scratch)(h, i);
	}

	واپس 0;
}

غیرعلامتی طویل sbi_scratch_alloc_انحراف(غیرعلامتی طویل سائز)
{
	خالی *ptr;
	غیرعلامتی طویل ret = 0;
	ساخت sbi_scratch *rscratch;
	غیرعلامتی طویل scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	اگر (!سائز)
		واپس 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	سائز += scratch_alloc_align - 1;
	سائز &= ~(scratch_alloc_align - 1);

	spin_تالا(&extra_تالا);

	اگر (SBI_SCRATCH_سائز < (extra_انحراف + سائز))
		جاؤ مکمل;

	ret = extra_انحراف;
	extra_انحراف += سائز;

مکمل:
	spin_unlock(&extra_تالا);

	اگر (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_مقصد_scratch(i);
			اگر (!rscratch)
				جاری;
			ptr = sbi_scratch_انحراف_ptr(rscratch, ret);
			sbi_memset(ptr, 0, سائز);
		}
	}

	واپس ret;
}

خالی sbi_scratch_free_انحراف(غیرعلامتی طویل انحراف)
{
	اگر ((انحراف < SBI_SCRATCH_EXTRA_SPACE_انحراف) ||
	    (SBI_SCRATCH_سائز <= انحراف))
		واپس;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

غیرعلامتی طویل sbi_scratch_مستعمل_space(خالی)
{
	غیرعلامتی طویل ret = 0;

	spin_تالا(&extra_تالا);
	ret = extra_انحراف;
	spin_unlock(&extra_تالا);

	واپس ret;
}
