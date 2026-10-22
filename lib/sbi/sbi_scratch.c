 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hartmask.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_ಎಣಿಕೆ;
u32 hartindex_ಗಮ್ಯ_hartid_ಕೋಷ್ಟಕ[SBI_HARTMASK_MAX_ಬಿಟ್ಟುಗಳು] = { [0 ... SBI_HARTMASK_MAX_ಬಿಟ್ಟುಗಳು-1] = -1U };
ರಚನೆ sbi_scratch *hartindex_ಗಮ್ಯ_scratch_ಕೋಷ್ಟಕ[SBI_HARTMASK_MAX_ಬಿಟ್ಟುಗಳು];

ಸ್ಥಿರ spinlock_t extra_ಬೀಗ = SPIN_ಬೀಗ_INITIALIZER;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ
	extra_ಸ್ಥಾನಾಂತರ GUARDED_BY(&extra_ಬೀಗ) = SBI_SCRATCH_EXTRA_SPACE_ಸ್ಥಾನಾಂತರ;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_get_scratch_alloc_align(ಶೂನ್ಯ)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_thishart_ptr();

	ಆದರೆ (!plat || !plat->cbom_ಖಂಡ_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ DEFAULT_SCRATCH_ALLOC_ALIGN;
	ಹಿಂತಿರುಗಿ plat->cbom_ಖಂಡ_ಗಾತ್ರ;
}

u32 sbi_hartid_ಗಮ್ಯ_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		ಆದರೆ (hartindex_ಗಮ್ಯ_hartid_ಕೋಷ್ಟಕ[i] == hartid)
			ಹಿಂತಿರುಗಿ i;

	ಹಿಂತಿರುಗಿ -1U;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

ಪೂರ್ಣಾಂಕ sbi_scratch_init(ರಚನೆ sbi_scratch *scratch)
{
	u32 h, hart_ಎಣಿಕೆ;
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_ಎಣಿಕೆ = plat->hart_ಎಣಿಕೆ;
	ಆದರೆ (hart_ಎಣಿಕೆ > SBI_HARTMASK_MAX_ಬಿಟ್ಟುಗಳು)
		hart_ಎಣಿಕೆ = SBI_HARTMASK_MAX_ಬಿಟ್ಟುಗಳು;
	sbi_scratch_hart_ಎಣಿಕೆ = hart_ಎಣಿಕೆ;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_ಗಮ್ಯ_hartid_ಕೋಷ್ಟಕ[i] = h;
		hartindex_ಗಮ್ಯ_scratch_ಕೋಷ್ಟಕ[i] =
			((hartid2scratch)scratch->hartid_ಗಮ್ಯ_scratch)(h, i);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
	ಶೂನ್ಯ *ptr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ret = 0;
	ರಚನೆ sbi_scratch *rscratch;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	ಆದರೆ (!ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	ಗಾತ್ರ_2 += scratch_alloc_align - 1;
	ಗಾತ್ರ_2 &= ~(scratch_alloc_align - 1);

	spin_ಬೀಗ(&extra_ಬೀಗ);

	ಆದರೆ (SBI_SCRATCH_ಗಾತ್ರ < (extra_ಸ್ಥಾನಾಂತರ + ಗಾತ್ರ_2))
		ಹೋಗು ಮುಗಿದ;

	ret = extra_ಸ್ಥಾನಾಂತರ;
	extra_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ_2;

ಮುಗಿದ:
	spin_unlock(&extra_ಬೀಗ);

	ಆದರೆ (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_ಗಮ್ಯ_scratch(i);
			ಆದರೆ (!rscratch)
				ಮುಂದುವರಿ;
			ptr = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(rscratch, ret);
			sbi_memset(ptr, 0, ಗಾತ್ರ_2);
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ sbi_scratch_free_ಸ್ಥಾನಾಂತರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸ್ಥಾನಾಂತರ)
{
	ಆದರೆ ((ಸ್ಥಾನಾಂತರ < SBI_SCRATCH_EXTRA_SPACE_ಸ್ಥಾನಾಂತರ) ||
	    (SBI_SCRATCH_ಗಾತ್ರ <= ಸ್ಥಾನಾಂತರ))
		ಹಿಂತಿರುಗಿ;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_scratch_ಬಳಸಿದ_space(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ret = 0;

	spin_ಬೀಗ(&extra_ಬೀಗ);
	ret = extra_ಸ್ಥಾನಾಂತರ;
	spin_unlock(&extra_ಬೀಗ);

	ಹಿಂತಿರುಗಿ ret;
}
