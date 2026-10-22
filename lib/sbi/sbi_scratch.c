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

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_شمار;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
ساختار sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

ایستا spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
ایستا unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ایستا unsigned long sbi_get_scratch_alloc_align(تهی)
{
	ثابت ساختار sbi_platform *plat = sbi_platform_thishart_ptr();

	اگر (!plat || !plat->cbom_block_size)
		بازگشت DEFAULT_SCRATCH_ALLOC_ALIGN;
	بازگشت plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		اگر (hartindex_to_hartid_table[i] == hartid)
			بازگشت i;

	بازگشت -1U;
}

تعریفنوع ساختار sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

صحیح sbi_scratch_init(ساختار sbi_scratch *scratch)
{
	u32 h, hart_شمار;
	ثابت ساختار sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_شمار = plat->hart_شمار;
	اگر (hart_شمار > SBI_HARTMASK_MAX_BITS)
		hart_شمار = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_شمار = hart_شمار;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	بازگشت 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	تهی *ptr;
	unsigned long ret = 0;
	ساختار sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	اگر (!size)
		بازگشت 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	اگر (SBI_SCRATCH_SIZE < (extra_offset + size))
		برو done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	اگر (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			اگر (!rscratch)
				ادامه;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	بازگشت ret;
}

تهی sbi_scratch_free_offset(unsigned long offset)
{
	اگر ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		بازگشت;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(تهی)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	بازگشت ret;
}
