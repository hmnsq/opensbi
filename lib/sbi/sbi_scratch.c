 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hartmask.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_SCRATCH_ALLOC_ALIGN __SIZEOF_POINTER__

u32 sbi_scratch_hart_ਗਿਣਤੀ;
u32 hartindex_ਮੰਜ਼ਿਲ_hartid_ਸਾਰਣੀ[SBI_HARTMASK_MAX_ਬਿੱਟ] = { [0 ... SBI_HARTMASK_MAX_ਬਿੱਟ-1] = -1U };
ਬਣਤਰ sbi_scratch *hartindex_ਮੰਜ਼ਿਲ_scratch_ਸਾਰਣੀ[SBI_HARTMASK_MAX_ਬਿੱਟ];

ਸਥਿਰ spinlock_t extra_ਤਾਲਾ = SPIN_ਤਾਲਾ_INITIALIZER;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ
	extra_ਵਿੱਥ GUARDED_BY(&extra_ਤਾਲਾ) = SBI_SCRATCH_EXTRA_SPACE_ਵਿੱਥ;

/*
 * Get the alignment size.
 * Return DEFAULT_SCRATCH_ALLOC_ALIGNMENT or riscv,cbom_block_size
 */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_get_scratch_alloc_align(ਖਾਲੀ)
{
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_thishart_ptr();

	ਜੇ (!plat || !plat->cbom_ਖੰਡ_ਮਾਪ)
		ਵਾਪਸ DEFAULT_SCRATCH_ALLOC_ALIGN;
	ਵਾਪਸ plat->cbom_ਖੰਡ_ਮਾਪ;
}

u32 sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(u32 hartid)
{
	sbi_for_each_hartindex(i)
		ਜੇ (hartindex_ਮੰਜ਼ਿਲ_hartid_ਸਾਰਣੀ[i] == hartid)
			ਵਾਪਸ i;

	ਵਾਪਸ -1U;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sbi_scratch *(*hartid2scratch)(ulong hartid, ulong hartindex);

ਪੂਰਨ sbi_scratch_init(ਬਣਤਰ sbi_scratch *scratch)
{
	u32 h, hart_ਗਿਣਤੀ;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);

	hart_ਗਿਣਤੀ = plat->hart_ਗਿਣਤੀ;
	ਜੇ (hart_ਗਿਣਤੀ > SBI_HARTMASK_MAX_ਬਿੱਟ)
		hart_ਗਿਣਤੀ = SBI_HARTMASK_MAX_ਬਿੱਟ;
	sbi_scratch_hart_ਗਿਣਤੀ = hart_ਗਿਣਤੀ;

	sbi_for_each_hartindex(i) {
		h = (plat->hart_index2id) ? plat->hart_index2id[i] : i;
		hartindex_ਮੰਜ਼ਿਲ_hartid_ਸਾਰਣੀ[i] = h;
		hartindex_ਮੰਜ਼ਿਲ_scratch_ਸਾਰਣੀ[i] =
			((hartid2scratch)scratch->hartid_ਮੰਜ਼ਿਲ_scratch)(h, i);
	}

	ਵਾਪਸ 0;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_scratch_alloc_ਵਿੱਥ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
	ਖਾਲੀ *ptr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ret = 0;
	ਬਣਤਰ sbi_scratch *rscratch;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ scratch_alloc_align = 0;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	ਜੇ (!ਮਾਪ)
		ਵਾਪਸ 0;

	scratch_alloc_align = sbi_get_scratch_alloc_align();

	/*
	 * We let the allocation align to cacheline bytes to avoid livelock on
	 * certain platforms due to atomic variables from the same cache line.
	 */
	ਮਾਪ += scratch_alloc_align - 1;
	ਮਾਪ &= ~(scratch_alloc_align - 1);

	spin_ਤਾਲਾ(&extra_ਤਾਲਾ);

	ਜੇ (SBI_SCRATCH_ਮਾਪ < (extra_ਵਿੱਥ + ਮਾਪ))
		ਜਾਓ ਪੂਰਾ;

	ret = extra_ਵਿੱਥ;
	extra_ਵਿੱਥ += ਮਾਪ;

ਪੂਰਾ:
	spin_unlock(&extra_ਤਾਲਾ);

	ਜੇ (ret) {
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(i);
			ਜੇ (!rscratch)
				ਜਾਰੀ;
			ptr = sbi_scratch_ਵਿੱਥ_ptr(rscratch, ret);
			sbi_memset(ptr, 0, ਮਾਪ);
		}
	}

	ਵਾਪਸ ret;
}

ਖਾਲੀ sbi_scratch_free_ਵਿੱਥ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਵਿੱਥ)
{
	ਜੇ ((ਵਿੱਥ < SBI_SCRATCH_EXTRA_SPACE_ਵਿੱਥ) ||
	    (SBI_SCRATCH_ਮਾਪ <= ਵਿੱਥ))
		ਵਾਪਸ;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_scratch_ਵਰਤਿਆ_space(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ret = 0;

	spin_ਤਾਲਾ(&extra_ਤਾਲਾ);
	ret = extra_ਵਿੱਥ;
	spin_unlock(&extra_ਤਾਲਾ);

	ਵਾਪਸ ret;
}
