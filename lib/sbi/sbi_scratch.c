 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hartmask.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>

#define DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_गिनती;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
संरचना sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

स्थिर spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
स्थिर unsigned long
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
स्थिर unsigned long sbi_get_scratch_alloc_align(शून्य)
{
	ध्रुव संरचना sbi_platform *plat = sbi_platform_thishart_ptr();

	यदि (!plat || !plat->cbom_block_size)
		प्रत्यागम DEFAULT_SCRATCH_ALLOC_ALIGN;
	प्रत्यागम plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		यदि (hartindex_to_hartid_table[i] == hartid)
			प्रत्यागम i;

	प्रत्यागम -1U;
}

प्रकारनिर्देश संरचना sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

पूर्णाङ्क sbi_scratch_init(संरचना sbi_scratch *scratch)
{
	u32 h, hart_गिनती;
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_गिनती = plat->hart_गिनती;
	यदि (hart_गिनती > SBI_HARTMASK_MAX_BITS)
		hart_गिनती = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_गिनती = hart_गिनती;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	प्रत्यागम 0;
}

unsigned long sbi_scratch_alloc_offset(unsigned long size)
{
	शून्य *ptr;
	unsigned long ret = 0;
	संरचना sbi_scratch *rscratch;
	unsigned long scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	यदि (!size)
		प्रत्यागम 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	size += scratch_alloc_align - 1;
	size &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	यदि (SBI_SCRATCH_SIZE < (extra_offset + size))
		गच्छ done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	यदि (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			यदि (!rscratch)
				अनुवर्त;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	प्रत्यागम ret;
}

शून्य sbi_scratch_free_offset(unsigned long offset)
{
	यदि ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		प्रत्यागम;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

unsigned long sbi_scratch_used_space(शून्य)
{
	unsigned long ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	प्रत्यागम ret;
}
