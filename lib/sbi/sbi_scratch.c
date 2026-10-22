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

u32 sbi_scratch_hart_गन्ती;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
संरचना sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

स्थिर spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
स्थिर अचिन्हित दीर्घ
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
स्थिर अचिन्हित दीर्घ sbi_get_scratch_alloc_align(शून्य)
{
	अचर संरचना sbi_platform *plat = sbi_platform_thishart_ptr();

	यदि (!plat || !plat->cbom_block_size)
		फर्काऊ DEFAULT_SCRATCH_ALLOC_ALIGN;
	फर्काऊ plat->cbom_block_size;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		यदि (hartindex_to_hartid_table[i] == hartid)
			फर्काऊ i;

	फर्काऊ -1U;
}

प्रकारनिर्देश संरचना sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

पूर्णाङ्क sbi_scratch_init(संरचना sbi_scratch *scratch)
{
	u32 h, hart_गन्ती;
	अचर संरचना sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_गन्ती = plat->hart_गन्ती;
	यदि (hart_गन्ती > SBI_HARTMASK_MAX_BITS)
		hart_गन्ती = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_गन्ती = hart_गन्ती;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	फर्काऊ 0;
}

अचिन्हित दीर्घ sbi_scratch_alloc_offset(अचिन्हित दीर्घ size)
{
	शून्य *ptr;
	अचिन्हित दीर्घ ret = 0;
	संरचना sbi_scratch *rscratch;
	अचिन्हित दीर्घ scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	यदि (!size)
		फर्काऊ 0;

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
				जारी;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, size);
		}
	}

	फर्काऊ ret;
}

शून्य sbi_scratch_free_offset(अचिन्हित दीर्घ offset)
{
	यदि ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_SIZE <= offset))
		फर्काऊ;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

अचिन्हित दीर्घ sbi_scratch_used_space(शून्य)
{
	अचिन्हित दीर्घ ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	फर्काऊ ret;
}
