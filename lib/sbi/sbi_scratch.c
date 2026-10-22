 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hartmask.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_എണ്ണം;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
ഘടന sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

സ്ഥിതം spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
സ്ഥിതം unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
സ്ഥിതം unsigned long sbi_get_scratch_alloc_align(ശൂന്യം)
{
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_thishart_ptr();

	എങ്കിൽ (!plat || !plat->cbom_block_size)
		തിരികെ DEFAULT_SCRATCH_ALLOC_ALIGN;
	തിരികെ plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		എങ്കിൽ (hartindex_to_hartid_table[i] == hartid)
			തിരികെ i;

	തിരികെ -1U;
}

തരനിർവചനം ഘടന sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

പൂർണ്ണസംഖ്യ sbi_scratch_init(ഘടന sbi_scratch *scratch)
{
	u32 h, hart_എണ്ണം;
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_എണ്ണം = plat->hart_എണ്ണം;
	എങ്കിൽ (hart_എണ്ണം > SBI_HARTMASK_MAX_BITS)
		hart_എണ്ണം = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_എണ്ണം = hart_എണ്ണം;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	തിരികെ 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	ശൂന്യം *ptr;
	unsigned long ret = 0;
	ഘടന sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	എങ്കിൽ (!size)
		തിരികെ 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	എങ്കിൽ (SBI_SCRATCH_SIZE < (extra_offset + size))
		പോകുക done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	എങ്കിൽ (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			എങ്കിൽ (!rscratch)
				തുടരുക;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	തിരികെ ret;
}

ശൂന്യം sbi_scratch_free_offset(unsigned long offset)
{
	എങ്കിൽ ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		തിരികെ;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(ശൂന്യം)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	തിരികെ ret;
}
