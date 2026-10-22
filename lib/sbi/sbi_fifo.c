/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_fifo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>

ਖਾਲੀ sbi_fifo_init(ਬਣਤਰ sbi_fifo *fifo, ਖਾਲੀ *ਕਤਾਰ_mem, u16 entries,
		   u16 ਇੰਦਰਾਜ_ਮਾਪ) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->ਕਤਾਰ	  = ਕਤਾਰ_mem;
	fifo->ਅੰਕ_entries = entries;
	fifo->ਇੰਦਰਾਜ_ਮਾਪ  = ਇੰਦਰਾਜ_ਮਾਪ;
	SPIN_ਤਾਲਾ_INIT(fifo->qlock);
	fifo->avail = fifo->ਪੂਛ = 0;
	sbi_memset(fifo->ਕਤਾਰ, 0, (size_t)entries * ਇੰਦਰਾਜ_ਮਾਪ);
}

/* Note: must be called with fifo->qlock held */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool __sbi_fifo_is_full(ਬਣਤਰ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ਵਾਪਸ (fifo->avail == fifo->ਅੰਕ_entries) ? true : false;
}

u16 sbi_fifo_avail(ਬਣਤਰ sbi_fifo *fifo)
{
	u16 ret;

	ਜੇ (!fifo)
		ਵਾਪਸ 0;

	spin_ਤਾਲਾ(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_fifo_is_ਭਰਿਆ(ਬਣਤਰ sbi_fifo *fifo)
{
	bool ret;

	ਜੇ (!fifo)
		ਵਾਪਸ SBI_EINVAL;

	spin_ਤਾਲਾ(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	ਵਾਪਸ ret;
}

/* Note: must be called with fifo->qlock held */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ __sbi_fifo_enqueue(ਬਣਤਰ sbi_fifo *fifo, ਖਾਲੀ *ਅੰਕੜੇ)
	MUST_HOLD(&fifo->qlock)
{
	u32 ਸਿਰ;

	ਸਿਰ = (u32)fifo->ਪੂਛ + fifo->avail;
	ਜੇ (ਸਿਰ >= fifo->ਅੰਕ_entries)
		ਸਿਰ = ਸਿਰ - fifo->ਅੰਕ_entries;

	ਚੋਣ (fifo->ਇੰਦਰਾਜ_ਮਾਪ) {
	ਹਾਲਤ 1:
		*(ਅੱਖਰ *)(fifo->ਕਤਾਰ + ਸਿਰ * fifo->ਇੰਦਰਾਜ_ਮਾਪ) = *(ਅੱਖਰ *)ਅੰਕੜੇ;
		ਰੋਕੋ;
	ਹਾਲਤ 2:
		*(u16 *)(fifo->ਕਤਾਰ + ਸਿਰ * fifo->ਇੰਦਰਾਜ_ਮਾਪ) = *(u16 *)ਅੰਕੜੇ;
		ਰੋਕੋ;
	ਹਾਲਤ 4:
		*(u32 *)(fifo->ਕਤਾਰ + ਸਿਰ * fifo->ਇੰਦਰਾਜ_ਮਾਪ) = *(u32 *)ਅੰਕੜੇ;
		ਰੋਕੋ;
#ਜੇ __riscv_xlen > 32
	ਹਾਲਤ 8:
		*(u64 *)(fifo->ਕਤਾਰ + ਸਿਰ * fifo->ਇੰਦਰਾਜ_ਮਾਪ) = *(u64 *)ਅੰਕੜੇ;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਮੂਲ:
		sbi_memcpy(fifo->ਕਤਾਰ + ਸਿਰ * fifo->ਇੰਦਰਾਜ_ਮਾਪ,
			   ਅੰਕੜੇ, fifo->ਇੰਦਰਾਜ_ਮਾਪ);
		ਰੋਕੋ;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ __sbi_fifo_dequeue(ਬਣਤਰ sbi_fifo *fifo, ਖਾਲੀ *ਅੰਕੜੇ)
	MUST_HOLD(&fifo->qlock)
{
	ਜੇ (!ਅੰਕੜੇ)
		ਜਾਓ skip_ਅੰਕੜੇ_copy;

	ਚੋਣ (fifo->ਇੰਦਰਾਜ_ਮਾਪ) {
	ਹਾਲਤ 1:
		*(ਅੱਖਰ *)ਅੰਕੜੇ = *(ਅੱਖਰ *)(fifo->ਕਤਾਰ + (u32)fifo->ਪੂਛ * fifo->ਇੰਦਰਾਜ_ਮਾਪ);
		ਰੋਕੋ;
	ਹਾਲਤ 2:
		*(u16 *)ਅੰਕੜੇ = *(u16 *)(fifo->ਕਤਾਰ + (u32)fifo->ਪੂਛ * fifo->ਇੰਦਰਾਜ_ਮਾਪ);
		ਰੋਕੋ;
	ਹਾਲਤ 4:
		*(u32 *)ਅੰਕੜੇ = *(u32 *)(fifo->ਕਤਾਰ + (u32)fifo->ਪੂਛ * fifo->ਇੰਦਰਾਜ_ਮਾਪ);
		ਰੋਕੋ;
#ਜੇ __riscv_xlen > 32
	ਹਾਲਤ 8:
		*(u64 *)ਅੰਕੜੇ = *(u64 *)(fifo->ਕਤਾਰ + (u32)fifo->ਪੂਛ * fifo->ਇੰਦਰਾਜ_ਮਾਪ);
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਮੂਲ:
		sbi_memcpy(ਅੰਕੜੇ, fifo->ਕਤਾਰ + (u32)fifo->ਪੂਛ * fifo->ਇੰਦਰਾਜ_ਮਾਪ,
			   fifo->ਇੰਦਰਾਜ_ਮਾਪ);
		ਰੋਕੋ;
	}

skip_ਅੰਕੜੇ_copy:
	fifo->avail--;
	fifo->ਪੂਛ++;
	ਜੇ (fifo->ਪੂਛ >= fifo->ਅੰਕ_entries)
		fifo->ਪੂਛ = 0;
}

/* Note: must be called with fifo->qlock held */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool __sbi_fifo_is_empty(ਬਣਤਰ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ਵਾਪਸ (fifo->avail == 0) ? true : false;
}

ਪੂਰਨ sbi_fifo_is_ਖਾਲੀ_ਥਾਵਾਂ(ਬਣਤਰ sbi_fifo *fifo)
{
	bool ret;

	ਜੇ (!fifo)
		ਵਾਪਸ SBI_EINVAL;

	spin_ਤਾਲਾ(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	ਵਾਪਸ ret;
}

/* Note: must be called with fifo->qlock held */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ __sbi_fifo_reset(ਬਣਤਰ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t ਮਾਪ = (size_t)fifo->ਅੰਕ_entries * fifo->ਇੰਦਰਾਜ_ਮਾਪ;

	fifo->avail = 0;
	fifo->ਪੂਛ  = 0;
	sbi_memset(fifo->ਕਤਾਰ, 0, ਮਾਪ);
}

bool sbi_fifo_reset(ਬਣਤਰ sbi_fifo *fifo)
{
	ਜੇ (!fifo)
		ਵਾਪਸ false;

	spin_ਤਾਲਾ(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	ਵਾਪਸ true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
ਪੂਰਨ sbi_fifo_inplace_update(ਬਣਤਰ sbi_fifo *fifo, ਖਾਲੀ *ਵਿੱਚ_2,
			    ਪੂਰਨ (*fptr)(ਖਾਲੀ *ਵਿੱਚ_2, ਖਾਲੀ *ਅੰਕੜੇ))
{
	ਪੂਰਨ i, ਸੂਚਕਾਂਕ_2;
	ਪੂਰਨ ret = SBI_FIFO_UNCHANGED;
	ਖਾਲੀ *ਇੰਦਰਾਜ_2;

	ਜੇ (!fifo || !ਵਿੱਚ_2)
		ਵਾਪਸ ret;

	spin_ਤਾਲਾ(&fifo->qlock);

	ਜੇ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ਵਾਪਸ ret;
	}

	ਲਈ (i = 0; i < fifo->avail; i++) {
		ਸੂਚਕਾਂਕ_2 = fifo->ਪੂਛ + i;
		ਜੇ (ਸੂਚਕਾਂਕ_2 >= fifo->ਅੰਕ_entries)
			ਸੂਚਕਾਂਕ_2 -= fifo->ਅੰਕ_entries;
		ਇੰਦਰਾਜ_2 = (ਅੱਖਰ *)fifo->ਕਤਾਰ + (u32)ਸੂਚਕਾਂਕ_2 * fifo->ਇੰਦਰਾਜ_ਮਾਪ;
		ret = fptr(ਵਿੱਚ_2, ਇੰਦਰਾਜ_2);

		ਜੇ (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			ਰੋਕੋ;
		}
	}
	spin_unlock(&fifo->qlock);

	ਵਾਪਸ ret;
}

ਪੂਰਨ sbi_fifo_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਬਣਤਰ sbi_fifo *fifo, ਖਾਲੀ *ਅੰਕੜੇ, bool force)
{
	ਜੇ (!fifo || !ਅੰਕੜੇ)
		ਵਾਪਸ SBI_EINVAL;

	spin_ਤਾਲਾ(&fifo->qlock);

	ਜੇ (__sbi_fifo_is_full(fifo)) {
		ਜੇ (!force) {
			spin_unlock(&fifo->qlock);
			ਵਾਪਸ SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, ਨਿਰਾ);
	}

	__sbi_fifo_enqueue(fifo, ਅੰਕੜੇ);

	spin_unlock(&fifo->qlock);

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_fifo_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(ਬਣਤਰ sbi_fifo *fifo, ਖਾਲੀ *ਅੰਕੜੇ)
{
	ਜੇ (!fifo || !ਅੰਕੜੇ)
		ਵਾਪਸ SBI_EINVAL;

	spin_ਤਾਲਾ(&fifo->qlock);

	ਜੇ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ਵਾਪਸ SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, ਅੰਕੜੇ);

	spin_unlock(&fifo->qlock);

	ਵਾਪਸ 0;
}
