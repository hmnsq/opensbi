 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hartmask.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_ગણતરી;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
સંરચના sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

સ્થિર spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
સ્થિર unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
સ્થિર unsigned long sbi_get_scratch_alloc_align(શૂન્ય)
{
	અચલ સંરચના sbi_platform *plat = sbi_platform_thishart_ptr();

	જો (!plat || !plat->cbom_block_size)
		પાછા DEFAULT_SCRATCH_ALLOC_ALIGN;
	પાછા plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		જો (hartindex_to_hartid_table[i] == hartid)
			પાછા i;

	પાછા -1U;
}

પ્રકારવ્યાખ્યા સંરચના sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

પૂર્ણાંક sbi_scratch_init(સંરચના sbi_scratch *scratch)
{
	u32 h, hart_ગણતરી;
	અચલ સંરચના sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_ગણતરી = plat->hart_ગણતરી;
	જો (hart_ગણતરી > SBI_HARTMASK_MAX_BITS)
		hart_ગણતરી = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_ગણતરી = hart_ગણતરી;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	પાછા 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	શૂન્ય *ptr;
	unsigned long ret = 0;
	સંરચના sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	જો (!size)
		પાછા 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	જો (SBI_SCRATCH_SIZE < (extra_offset + size))
		જાઓ done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	જો (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			જો (!rscratch)
				ચાલુ;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	પાછા ret;
}

શૂન્ય sbi_scratch_free_offset(unsigned long offset)
{
	જો ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		પાછા;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(શૂન્ય)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	પાછા ret;
}
