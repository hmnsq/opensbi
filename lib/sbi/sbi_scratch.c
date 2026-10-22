 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_locks.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_hartmask.h>
#অন্তর্ভাৱ <sbi/sbi_platform.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_গুনতি;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
সংরচনা sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

স্থির spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
স্থির unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
স্থির unsigned long sbi_get_scratch_alloc_align(শূন্য)
{
	ধ্রুৱ সংরচনা sbi_platform *plat = sbi_platform_thishart_ptr();

	যদি (!plat || !plat->cbom_block_size)
		ঘূৰাই DEFAULT_SCRATCH_ALLOC_ALIGN;
	ঘূৰাই plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		যদি (hartindex_to_hartid_table[i] == hartid)
			ঘূৰাই i;

	ঘূৰাই -1U;
}

প্রকারনির্দেশ সংরচনা sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

পূর্ণাঙ্ক sbi_scratch_init(সংরচনা sbi_scratch *scratch)
{
	u32 h, hart_গুনতি;
	ধ্রুৱ সংরচনা sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_গুনতি = plat->hart_গুনতি;
	যদি (hart_গুনতি > SBI_HARTMASK_MAX_BITS)
		hart_গুনতি = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_গুনতি = hart_গুনতি;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	ঘূৰাই 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	শূন্য *ptr;
	unsigned long ret = 0;
	সংরচনা sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	যদি (!size)
		ঘূৰাই 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	যদি (SBI_SCRATCH_SIZE < (extra_offset + size))
		গচ্ছ done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	যদি (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			যদি (!rscratch)
				অনুৱর্ত;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	ঘূৰাই ret;
}

শূন্য sbi_scratch_free_offset(unsigned long offset)
{
	যদি ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		ঘূৰাই;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(শূন্য)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	ঘূৰাই ret;
}
