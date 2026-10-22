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

#परिभाषय DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_गणनम्;
u32 hartindex_गन्तव्यम्_hartid_सारणी[SBI_HARTMASK_MAX_द्वयङ्काः] = { [0 ... SBI_HARTMASK_MAX_द्वयङ्काः-1] = -1U };
संरचना sbi_scratch *hartindex_गन्तव्यम्_scratch_सारणी[SBI_HARTMASK_MAX_द्वयङ्काः];

स्थिर spinlock_t extra_तालकम् = SPIN_तालकम्_INITIALIZER;
स्थिर अचिह्नित दीर्घ
	extra_विस्थापनम् GUARDED_BY(&extra_तालकम्) = SBI_SCRATCH_EXTRA_SPACE_विस्थापनम्;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
स्थिर अचिह्नित दीर्घ sbi_get_scratch_alloc_align(शून्य)
{
	ध्रुव संरचना sbi_platform *plat = sbi_platform_thishart_ptr();

	यदि (!plat || !plat->cbom_खण्डम्_आकार)
		प्रत्यागम DEFAULT_SCRATCH_ALLOC_ALIGN;
	प्रत्यागम plat->cbom_खण्डम्_आकार;
}

u32 sbi_hartid_गन्तव्यम्_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		यदि (hartindex_गन्तव्यम्_hartid_सारणी[i] == hartid)
			प्रत्यागम i;

	प्रत्यागम -1U;
}

प्रकारनिर्देश संरचना sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

पूर्णाङ्क sbi_scratch_init(संरचना sbi_scratch *scratch)
{
	u32 h, hart_गणनम्;
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_गणनम् = plat->hart_गणनम्;
	यदि (hart_गणनम् > SBI_HARTMASK_MAX_द्वयङ्काः)
		hart_गणनम् = SBI_HARTMASK_MAX_द्वयङ्काः;
	sbi_scratch_hart_गणनम् = hart_गणनम्;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_गन्तव्यम्_hartid_सारणी[i] = h;
		hartindex_गन्तव्यम्_scratch_सारणी[i] =
			((hartid2scratch)scratch->hartid_गन्तव्यम्_scratch)(h, i);
	}

	प्रत्यागम 0;
}

अचिह्नित दीर्घ sbi_scratch_alloc_विस्थापनम्(अचिह्नित दीर्घ आकार)
{
	शून्य *ptr;
	अचिह्नित दीर्घ ret = 0;
	संरचना sbi_scratch *rscratch;
	अचिह्नित दीर्घ scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	यदि (!आकार)
		प्रत्यागम 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	आकार += scratch_alloc_align - 1;
	आकार &= ~(scratch_alloc_align - 1);

	spin_तालकम्(&extra_तालकम्);

	यदि (SBI_SCRATCH_आकार < (extra_विस्थापनम् + आकार))
		गच्छ समाप्तम्;

	ret = extra_विस्थापनम्;
	extra_विस्थापनम् += आकार;

समाप्तम्:
	spin_unlock(&extra_तालकम्);

	यदि (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_गन्तव्यम्_scratch(i);
			यदि (!rscratch)
				अनुवर्त;
			ptr = sbi_scratch_विस्थापनम्_ptr(rscratch, ret);
			sbi_memset(ptr, 0, आकार);
		}
	}

	प्रत्यागम ret;
}

शून्य sbi_scratch_free_विस्थापनम्(अचिह्नित दीर्घ विस्थापनम्)
{
	यदि ((विस्थापनम् < SBI_SCRATCH_EXTRA_SPACE_विस्थापनम्) ||
	    (SBI_SCRATCH_आकार <= विस्थापनम्))
		प्रत्यागम;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

अचिह्नित दीर्घ sbi_scratch_प्रयुक्तम्_space(शून्य)
{
	अचिह्नित दीर्घ ret = 0;

	spin_तालकम्(&extra_तालकम्);
	ret = extra_विस्थापनम्;
	spin_unlock(&extra_तालकम्);

	प्रत्यागम ret;
}
