/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_fifo.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

ಶೂನ್ಯ sbi_fifo_init(ರಚನೆ sbi_fifo *fifo, ಶೂನ್ಯ *ಸರತಿ_mem, u16 entries,
		   u16 ನಮೂದು_ಗಾತ್ರ) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->ಸರತಿ	  = ಸರತಿ_mem;
	fifo->ಅಂಕೆ_entries = entries;
	fifo->ನಮೂದು_ಗಾತ್ರ  = ನಮೂದು_ಗಾತ್ರ;
	SPIN_ಬೀಗ_INIT(fifo->qlock);
	fifo->avail = fifo->ಬಾಲ = 0;
	sbi_memset(fifo->ಸರತಿ, 0, (size_t)entries * ನಮೂದು_ಗಾತ್ರ);
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool __sbi_fifo_is_full(ರಚನೆ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ಹಿಂತಿರುಗಿ (fifo->avail == fifo->ಅಂಕೆ_entries) ? true : false;
}

u16 sbi_fifo_avail(ರಚನೆ sbi_fifo *fifo)
{
	u16 ret;

	ಆದರೆ (!fifo)
		ಹಿಂತಿರುಗಿ 0;

	spin_ಬೀಗ(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_fifo_is_ತುಂಬಿದ(ರಚನೆ sbi_fifo *fifo)
{
	bool ret;

	ಆದರೆ (!fifo)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	spin_ಬೀಗ(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	ಹಿಂತಿರುಗಿ ret;
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ __sbi_fifo_enqueue(ರಚನೆ sbi_fifo *fifo, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
	MUST_HOLD(&fifo->qlock)
{
	u32 ತಲೆ;

	ತಲೆ = (u32)fifo->ಬಾಲ + fifo->avail;
	ಆದರೆ (ತಲೆ >= fifo->ಅಂಕೆ_entries)
		ತಲೆ = ತಲೆ - fifo->ಅಂಕೆ_entries;

	ಆಯ್ಕೆ (fifo->ನಮೂದು_ಗಾತ್ರ) {
	ಸ್ಥಿತಿ 1:
		*(ಅಕ್ಷರ *)(fifo->ಸರತಿ + ತಲೆ * fifo->ನಮೂದು_ಗಾತ್ರ) = *(ಅಕ್ಷರ *)ದತ್ತಾಂಶ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2:
		*(u16 *)(fifo->ಸರತಿ + ತಲೆ * fifo->ನಮೂದು_ಗಾತ್ರ) = *(u16 *)ದತ್ತಾಂಶ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 4:
		*(u32 *)(fifo->ಸರತಿ + ತಲೆ * fifo->ನಮೂದು_ಗಾತ್ರ) = *(u32 *)ದತ್ತಾಂಶ;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ __riscv_xlen > 32
	ಸ್ಥಿತಿ 8:
		*(u64 *)(fifo->ಸರತಿ + ತಲೆ * fifo->ನಮೂದು_ಗಾತ್ರ) = *(u64 *)ದತ್ತಾಂಶ;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ವನಿಯೋಜಿತ:
		sbi_memcpy(fifo->ಸರತಿ + ತಲೆ * fifo->ನಮೂದು_ಗಾತ್ರ,
			   ದತ್ತಾಂಶ, fifo->ನಮೂದು_ಗಾತ್ರ);
		ನಿಲ್ಲಿಸು;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ __sbi_fifo_dequeue(ರಚನೆ sbi_fifo *fifo, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
	MUST_HOLD(&fifo->qlock)
{
	ಆದರೆ (!ದತ್ತಾಂಶ)
		ಹೋಗು skip_ದತ್ತಾಂಶ_copy;

	ಆಯ್ಕೆ (fifo->ನಮೂದು_ಗಾತ್ರ) {
	ಸ್ಥಿತಿ 1:
		*(ಅಕ್ಷರ *)ದತ್ತಾಂಶ = *(ಅಕ್ಷರ *)(fifo->ಸರತಿ + (u32)fifo->ಬಾಲ * fifo->ನಮೂದು_ಗಾತ್ರ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2:
		*(u16 *)ದತ್ತಾಂಶ = *(u16 *)(fifo->ಸರತಿ + (u32)fifo->ಬಾಲ * fifo->ನಮೂದು_ಗಾತ್ರ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 4:
		*(u32 *)ದತ್ತಾಂಶ = *(u32 *)(fifo->ಸರತಿ + (u32)fifo->ಬಾಲ * fifo->ನಮೂದು_ಗಾತ್ರ);
		ನಿಲ್ಲಿಸು;
#ಆದರೆ __riscv_xlen > 32
	ಸ್ಥಿತಿ 8:
		*(u64 *)ದತ್ತಾಂಶ = *(u64 *)(fifo->ಸರತಿ + (u32)fifo->ಬಾಲ * fifo->ನಮೂದು_ಗಾತ್ರ);
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ವನಿಯೋಜಿತ:
		sbi_memcpy(ದತ್ತಾಂಶ, fifo->ಸರತಿ + (u32)fifo->ಬಾಲ * fifo->ನಮೂದು_ಗಾತ್ರ,
			   fifo->ನಮೂದು_ಗಾತ್ರ);
		ನಿಲ್ಲಿಸು;
	}

skip_ದತ್ತಾಂಶ_copy:
	fifo->avail--;
	fifo->ಬಾಲ++;
	ಆದರೆ (fifo->ಬಾಲ >= fifo->ಅಂಕೆ_entries)
		fifo->ಬಾಲ = 0;
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool __sbi_fifo_is_empty(ರಚನೆ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ಹಿಂತಿರುಗಿ (fifo->avail == 0) ? true : false;
}

ಪೂರ್ಣಾಂಕ sbi_fifo_is_ಖಾಲಿಗಳು(ರಚನೆ sbi_fifo *fifo)
{
	bool ret;

	ಆದರೆ (!fifo)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	spin_ಬೀಗ(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	ಹಿಂತಿರುಗಿ ret;
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ __sbi_fifo_reset(ರಚನೆ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t ಗಾತ್ರ_2 = (size_t)fifo->ಅಂಕೆ_entries * fifo->ನಮೂದು_ಗಾತ್ರ;

	fifo->avail = 0;
	fifo->ಬಾಲ  = 0;
	sbi_memset(fifo->ಸರತಿ, 0, ಗಾತ್ರ_2);
}

bool sbi_fifo_reset(ರಚನೆ sbi_fifo *fifo)
{
	ಆದರೆ (!fifo)
		ಹಿಂತಿರುಗಿ false;

	spin_ಬೀಗ(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	ಹಿಂತಿರುಗಿ true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
ಪೂರ್ಣಾಂಕ sbi_fifo_inplace_update(ರಚನೆ sbi_fifo *fifo, ಶೂನ್ಯ *ಒಳ,
			    ಪೂರ್ಣಾಂಕ (*fptr)(ಶೂನ್ಯ *ಒಳ, ಶೂನ್ಯ *ದತ್ತಾಂಶ))
{
	ಪೂರ್ಣಾಂಕ i, ಸೂಚ್ಯಂಕ_2;
	ಪೂರ್ಣಾಂಕ ret = SBI_FIFO_UNCHANGED;
	ಶೂನ್ಯ *ನಮೂದು_2;

	ಆದರೆ (!fifo || !ಒಳ)
		ಹಿಂತಿರುಗಿ ret;

	spin_ಬೀಗ(&fifo->qlock);

	ಆದರೆ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ಹಿಂತಿರುಗಿ ret;
	}

	ಗಾಗಿ (i = 0; i < fifo->avail; i++) {
		ಸೂಚ್ಯಂಕ_2 = fifo->ಬಾಲ + i;
		ಆದರೆ (ಸೂಚ್ಯಂಕ_2 >= fifo->ಅಂಕೆ_entries)
			ಸೂಚ್ಯಂಕ_2 -= fifo->ಅಂಕೆ_entries;
		ನಮೂದು_2 = (ಅಕ್ಷರ *)fifo->ಸರತಿ + (u32)ಸೂಚ್ಯಂಕ_2 * fifo->ನಮೂದು_ಗಾತ್ರ;
		ret = fptr(ಒಳ, ನಮೂದು_2);

		ಆದರೆ (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			ನಿಲ್ಲಿಸು;
		}
	}
	spin_unlock(&fifo->qlock);

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ sbi_fifo_ಸರತಿಗೆ_ಸೇರಿಸು(ರಚನೆ sbi_fifo *fifo, ಶೂನ್ಯ *ದತ್ತಾಂಶ, bool force)
{
	ಆದರೆ (!fifo || !ದತ್ತಾಂಶ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	spin_ಬೀಗ(&fifo->qlock);

	ಆದರೆ (__sbi_fifo_is_full(fifo)) {
		ಆದರೆ (!force) {
			spin_unlock(&fifo->qlock);
			ಹಿಂತಿರುಗಿ SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, ಖಾಲಿ);
	}

	__sbi_fifo_enqueue(fifo, ದತ್ತಾಂಶ);

	spin_unlock(&fifo->qlock);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_fifo_ಸರತಿಯಿಂದ_ತೆಗೆ(ರಚನೆ sbi_fifo *fifo, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
	ಆದರೆ (!fifo || !ದತ್ತಾಂಶ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	spin_ಬೀಗ(&fifo->qlock);

	ಆದರೆ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ಹಿಂತಿರುಗಿ SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, ದತ್ತಾಂಶ);

	spin_unlock(&fifo->qlock);

	ಹಿಂತಿರುಗಿ 0;
}
