/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_fifo.h>
#ضمن <sbi/sbi_string.h>

فراغ sbi_fifo_init(بنية sbi_fifo *fifo, فراغ *قطار_mem, u16 entries,
		   u16 اندراج_ناپ) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->قطار	  = قطار_mem;
	fifo->عدد_entries = entries;
	fifo->اندراج_ناپ  = اندراج_ناپ;
	SPIN_جندرا_INIT(fifo->qlock);
	fifo->avail = fifo->پوچھ = 0;
	sbi_memset(fifo->قطار, 0, (size_t)entries * اندراج_ناپ);
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن bool __sbi_fifo_is_full(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	أرجع (fifo->avail == fifo->عدد_entries) ? true : false;
}

u16 sbi_fifo_avail(بنية sbi_fifo *fifo)
{
	u16 ret;

	إذا (!fifo)
		أرجع 0;

	spin_جندرا(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_is_بھریا(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_جندرا(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_enqueue(بنية sbi_fifo *fifo, فراغ *انکڑے)
	MUST_HOLD(&fifo->qlock)
{
	u32 سر;

	سر = (u32)fifo->پوچھ + fifo->avail;
	إذا (سر >= fifo->عدد_entries)
		سر = سر - fifo->عدد_entries;

	بدل (fifo->اندراج_ناپ) {
	حالة 1:
		*(حرف *)(fifo->قطار + سر * fifo->اندراج_ناپ) = *(حرف *)انکڑے;
		اكسر;
	حالة 2:
		*(u16 *)(fifo->قطار + سر * fifo->اندراج_ناپ) = *(u16 *)انکڑے;
		اكسر;
	حالة 4:
		*(u32 *)(fifo->قطار + سر * fifo->اندراج_ناپ) = *(u32 *)انکڑے;
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)(fifo->قطار + سر * fifo->اندراج_ناپ) = *(u64 *)انکڑے;
		اكسر;
#جے_انت
	افتراضي:
		sbi_memcpy(fifo->قطار + سر * fifo->اندراج_ناپ,
			   انکڑے, fifo->اندراج_ناپ);
		اكسر;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_dequeue(بنية sbi_fifo *fifo, فراغ *انکڑے)
	MUST_HOLD(&fifo->qlock)
{
	إذا (!انکڑے)
		اذهب skip_انکڑے_copy;

	بدل (fifo->اندراج_ناپ) {
	حالة 1:
		*(حرف *)انکڑے = *(حرف *)(fifo->قطار + (u32)fifo->پوچھ * fifo->اندراج_ناپ);
		اكسر;
	حالة 2:
		*(u16 *)انکڑے = *(u16 *)(fifo->قطار + (u32)fifo->پوچھ * fifo->اندراج_ناپ);
		اكسر;
	حالة 4:
		*(u32 *)انکڑے = *(u32 *)(fifo->قطار + (u32)fifo->پوچھ * fifo->اندراج_ناپ);
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)انکڑے = *(u64 *)(fifo->قطار + (u32)fifo->پوچھ * fifo->اندراج_ناپ);
		اكسر;
#جے_انت
	افتراضي:
		sbi_memcpy(انکڑے, fifo->قطار + (u32)fifo->پوچھ * fifo->اندراج_ناپ,
			   fifo->اندراج_ناپ);
		اكسر;
	}

skip_انکڑے_copy:
	fifo->avail--;
	fifo->پوچھ++;
	إذا (fifo->پوچھ >= fifo->عدد_entries)
		fifo->پوچھ = 0;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن bool __sbi_fifo_is_empty(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	أرجع (fifo->avail == 0) ? true : false;
}

صحيح sbi_fifo_is_خالی_تھاواں(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_جندرا(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_reset(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t ناپ = (size_t)fifo->عدد_entries * fifo->اندراج_ناپ;

	fifo->avail = 0;
	fifo->پوچھ  = 0;
	sbi_memset(fifo->قطار, 0, ناپ);
}

bool sbi_fifo_reset(بنية sbi_fifo *fifo)
{
	إذا (!fifo)
		أرجع false;

	spin_جندرا(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	أرجع true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
صحيح sbi_fifo_inplace_update(بنية sbi_fifo *fifo, فراغ *وچ,
			    صحيح (*fptr)(فراغ *وچ, فراغ *انکڑے))
{
	صحيح i, اشاریہ_2;
	صحيح ret = SBI_FIFO_UNCHANGED;
	فراغ *اندراج_2;

	إذا (!fifo || !وچ)
		أرجع ret;

	spin_جندرا(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع ret;
	}

	لكل (i = 0; i < fifo->avail; i++) {
		اشاریہ_2 = fifo->پوچھ + i;
		إذا (اشاریہ_2 >= fifo->عدد_entries)
			اشاریہ_2 -= fifo->عدد_entries;
		اندراج_2 = (حرف *)fifo->قطار + (u32)اشاریہ_2 * fifo->اندراج_ناپ;
		ret = fptr(وچ, اندراج_2);

		إذا (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			اكسر;
		}
	}
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_قطار_وچ_پاؤ(بنية sbi_fifo *fifo, فراغ *انکڑے, bool force)
{
	إذا (!fifo || !انکڑے)
		أرجع SBI_EINVAL;

	spin_جندرا(&fifo->qlock);

	إذا (__sbi_fifo_is_full(fifo)) {
		إذا (!force) {
			spin_unlock(&fifo->qlock);
			أرجع SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, عدم);
	}

	__sbi_fifo_enqueue(fifo, انکڑے);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}

صحيح sbi_fifo_قطار_توں_کڈھو(بنية sbi_fifo *fifo, فراغ *انکڑے)
{
	إذا (!fifo || !انکڑے)
		أرجع SBI_EINVAL;

	spin_جندرا(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, انکڑے);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}
