 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_locks.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_count;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
sa_nracanaa sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

static spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
static unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
static unsigned long sbi_get_scratch_alloc_align(void)
{
	const sa_nracanaa sbi_platform *plat = sbi_platform_thishart_ptr();

	ᨀᨘ (!plat || !plat->cbom_block_size)
		ᨀᨘᨒ DEFAULT_SCRATCH_ALLOC_ALIGN;
	ᨀᨘᨒ plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		ᨀᨘ (hartindex_to_hartid_table[i] == hartid)
			ᨀᨘᨒ i;

	ᨀᨘᨒ -1U;
}

typedef sa_nracanaa sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

puur_naa_nka sbi_scratch_init(sa_nracanaa sbi_scratch *scratch)
{
	u32 h, hart_count;
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_count = plat->hart_count;
	ᨀᨘ (hart_count > SBI_HARTMASK_MAX_BITS)
		hart_count = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_count = hart_count;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	ᨀᨘᨒ 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	void *ptr;
	unsigned long ret = 0;
	sa_nracanaa sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	ᨀᨘ (!size)
		ᨀᨘᨒ 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	ᨀᨘ (SBI_SCRATCH_SIZE < (extra_offset + size))
		goto done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	ᨀᨘ (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			ᨀᨘ (!rscratch)
				continue;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	ᨀᨘᨒ ret;
}

void sbi_scratch_free_offset(unsigned long offset)
{
	ᨀᨘ ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		ᨀᨘᨒ;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(void)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	ᨀᨘᨒ ret;
}
