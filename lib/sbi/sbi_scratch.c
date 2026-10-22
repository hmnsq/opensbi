 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hartmask.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_ଗଣନା;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
ସଂରଚନା sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

ସ୍ଥିର spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
ସ୍ଥିର unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ସ୍ଥିର unsigned long sbi_get_scratch_alloc_align(ଶୂନ୍ୟ)
{
	ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_thishart_ptr();

	ଯଦି (!plat || !plat->cbom_block_size)
		ଫେରନ୍ତୁ DEFAULT_SCRATCH_ALLOC_ALIGN;
	ଫେରନ୍ତୁ plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		ଯଦି (hartindex_to_hartid_table[i] == hartid)
			ଫେରନ୍ତୁ i;

	ଫେରନ୍ତୁ -1U;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_scratch_init(ସଂରଚନା sbi_scratch *scratch)
{
	u32 h, hart_ଗଣନା;
	ଅଚଳ ସଂରଚନା sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_ଗଣନା = plat->hart_ଗଣନା;
	ଯଦି (hart_ଗଣନା > SBI_HARTMASK_MAX_BITS)
		hart_ଗଣନା = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_ଗଣନା = hart_ଗଣନା;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	ଫେରନ୍ତୁ 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	ଶୂନ୍ୟ *ptr;
	unsigned long ret = 0;
	ସଂରଚନା sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	ଯଦି (!size)
		ଫେରନ୍ତୁ 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	ଯଦି (SBI_SCRATCH_SIZE < (extra_offset + size))
		ଯାଅ done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	ଯଦି (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			ଯଦି (!rscratch)
				ଜାରି;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ sbi_scratch_free_offset(unsigned long offset)
{
	ଯଦି ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		ଫେରନ୍ତୁ;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(ଶୂନ୍ୟ)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	ଫେରନ୍ତୁ ret;
}
