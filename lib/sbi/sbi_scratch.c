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

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_count;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
بنية sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

ساكن spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
ساكن unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ساكن unsigned long sbi_get_scratch_alloc_align(فراغ)
{
	ثابت بنية sbi_platform *plat = sbi_platform_thishart_ptr();

	ܐܢ (!plat || !plat->cbom_block_size)
		ܦܢܐ DEFAULT_SCRATCH_ALLOC_ALIGN;
	ܦܢܐ plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		ܐܢ (hartindex_to_hartid_table[i] == hartid)
			ܦܢܐ i;

	ܦܢܐ -1U;
}

تعريفنوع بنية sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

صحيح sbi_scratch_init(بنية sbi_scratch *scratch)
{
	u32 h, hart_count;
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_count = plat->hart_count;
	ܐܢ (hart_count > SBI_HARTMASK_MAX_BITS)
		hart_count = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_count = hart_count;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	ܦܢܐ 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	فراغ *ptr;
	unsigned long ret = 0;
	بنية sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	ܐܢ (!size)
		ܦܢܐ 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	ܐܢ (SBI_SCRATCH_SIZE < (extra_offset + size))
		اذهب done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	ܐܢ (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			ܐܢ (!rscratch)
				استمر;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	ܦܢܐ ret;
}

فراغ sbi_scratch_free_offset(unsigned long offset)
{
	ܐܢ ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		ܦܢܐ;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(فراغ)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	ܦܢܐ ret;
}
