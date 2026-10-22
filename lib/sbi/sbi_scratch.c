 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hartmask.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>

#घोषणा DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_गिनती;
u32 hartindex_to_hartid_table[SBI_HARTMASK_MAX_BITS] = { [0 ... SBI_HARTMASK_MAX_BITS-1] = -1U };
काष्ठा sbi_scratch *hartindex_to_scratch_table[SBI_HARTMASK_MAX_BITS];

अटल spinlock_t extra_lock = SPIN_LOCK_INITIALIZER;
अटल अचिन्हित दीर्घ
	extra_offset GUARDED_BY(&extra_lock) = SBI_SCRATCH_EXTRA_SPACE_OFFSET;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
अटल अचिन्हित दीर्घ sbi_get_scratch_alloc_align(व्योम)
{
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_thishart_ptr();

	अगर (!plat || !plat->cbom_block_आकार)
		वापस DEFAULT_SCRATCH_ALLOC_ALIGN;
	वापस plat->cbom_block_आकार;
}

u32 sbi_hartid_to_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		अगर (hartindex_to_hartid_table[i] == hartid)
			वापस i;

	वापस -1U;
}

प्रकार काष्ठा sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

पूर्णांक sbi_scratch_init(काष्ठा sbi_scratch *scratch)
{
	u32 h, hart_गिनती;
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_गिनती = plat->hart_गिनती;
	अगर (hart_गिनती > SBI_HARTMASK_MAX_BITS)
		hart_गिनती = SBI_HARTMASK_MAX_BITS;
	sbi_scratch_hart_गिनती = hart_गिनती;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_to_hartid_table[i] = h;
		hartindex_to_scratch_table[i] =
			((hartid2scratch)scratch->hartid_to_scratch)(h, i);
	}

	वापस 0;
}

अचिन्हित दीर्घ sbi_scratch_alloc_offset(अचिन्हित दीर्घ आकार)
{
	व्योम *ptr;
	अचिन्हित दीर्घ ret = 0;
	काष्ठा sbi_scratch *rscratch;
	अचिन्हित दीर्घ scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	अगर (!आकार)
		वापस 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	आकार += scratch_alloc_align - 1;
	आकार &= ~(scratch_alloc_align - 1);

	spin_lock(&extra_lock);

	अगर (SBI_SCRATCH_आकार < (extra_offset + आकार))
		जाओ done;

	ret = extra_offset;
	extra_offset += आकार;

done:
	spin_unlock(&extra_lock);

	अगर (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			अगर (!rscratch)
				जारी;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			sbi_memset(ptr, 0, आकार);
		}
	}

	वापस ret;
}

व्योम sbi_scratch_free_offset(अचिन्हित दीर्घ offset)
{
	अगर ((offset < SBI_SCRATCH_EXTRA_SPACE_OFFSET) ||
	    (SBI_SCRATCH_आकार <= offset))
		वापस;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

अचिन्हित दीर्घ sbi_scratch_used_space(व्योम)
{
	अचिन्हित दीर्घ ret = 0;

	spin_lock(&extra_lock);
	ret = extra_offset;
	spin_unlock(&extra_lock);

	वापस ret;
}
