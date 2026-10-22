 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_locks.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hartmask.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_மொத்தம்;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
கட்டமைப்பு sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

நிலையான spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
நிலையான unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
நிலையான unsigned long sbi_get_scratch_alloc_align(வெறுமை)
{
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_thishart_ptr();

	எனில் (!plat || !plat->cbom_block_size)
		திரும்பு DEFAULT_SCRATCH_ALLOC_ALIGN;
	திரும்பு plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		எனில் (hartindex_to_hartid_table[i] == hartid)
			திரும்பு i;

	திரும்பு -1U;
}

வகைவரை கட்டமைப்பு sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

முழு sbi_scratch_init(கட்டமைப்பு sbi_scratch *scratch)
{
	u32 h, hart_மொத்தம்;
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_மொத்தம் = plat->hart_மொத்தம்;
	எனில் (hart_மொத்தம் > SBI_HARTMASK_MAX_BITS)
		hart_மொத்தம் = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_மொத்தம் = hart_மொத்தம்;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	திரும்பு 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	வெறுமை *ptr;
	unsigned long ret = 0;
	கட்டமைப்பு sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	எனில் (!size)
		திரும்பு 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	எனில் (SBI_SCRATCH_SIZE < (extra_offset + size))
		செல் done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	எனில் (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			எனில் (!rscratch)
				தொடர்;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	திரும்பு ret;
}

வெறுமை sbi_scratch_free_offset(unsigned long offset)
{
	எனில் ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		திரும்பு;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(வெறுமை)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	திரும்பு ret;
}
