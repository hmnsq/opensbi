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

فراغ sbi_fifo_init(بنية sbi_fifo *fifo, فراغ *کتار_mem, u16 entries,
		   u16 ثبت_اندازه) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->کتار	  = کتار_mem;
	fifo->عدد_entries = entries;
	fifo->ثبت_اندازه  = ثبت_اندازه;
	SPIN_قلف_INIT(fifo->qlock);
	fifo->avail = fifo->لکۍ = 0;
	sbi_memset(fifo->کتار, 0, (size_t)entries * ثبت_اندازه);
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

	spin_قلف(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_is_ډک(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_قلف(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_enqueue(بنية sbi_fifo *fifo, فراغ *معلومات)
	MUST_HOLD(&fifo->qlock)
{
	u32 سر;

	سر = (u32)fifo->لکۍ + fifo->avail;
	إذا (سر >= fifo->عدد_entries)
		سر = سر - fifo->عدد_entries;

	بدل (fifo->ثبت_اندازه) {
	حالة 1:
		*(حرف *)(fifo->کتار + سر * fifo->ثبت_اندازه) = *(حرف *)معلومات;
		اكسر;
	حالة 2:
		*(u16 *)(fifo->کتار + سر * fifo->ثبت_اندازه) = *(u16 *)معلومات;
		اكسر;
	حالة 4:
		*(u32 *)(fifo->کتار + سر * fifo->ثبت_اندازه) = *(u32 *)معلومات;
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)(fifo->کتار + سر * fifo->ثبت_اندازه) = *(u64 *)معلومات;
		اكسر;
#د_که_پای
	افتراضي:
		sbi_memcpy(fifo->کتار + سر * fifo->ثبت_اندازه,
			   معلومات, fifo->ثبت_اندازه);
		اكسر;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_dequeue(بنية sbi_fifo *fifo, فراغ *معلومات)
	MUST_HOLD(&fifo->qlock)
{
	إذا (!معلومات)
		اذهب skip_معلومات_copy;

	بدل (fifo->ثبت_اندازه) {
	حالة 1:
		*(حرف *)معلومات = *(حرف *)(fifo->کتار + (u32)fifo->لکۍ * fifo->ثبت_اندازه);
		اكسر;
	حالة 2:
		*(u16 *)معلومات = *(u16 *)(fifo->کتار + (u32)fifo->لکۍ * fifo->ثبت_اندازه);
		اكسر;
	حالة 4:
		*(u32 *)معلومات = *(u32 *)(fifo->کتار + (u32)fifo->لکۍ * fifo->ثبت_اندازه);
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)معلومات = *(u64 *)(fifo->کتار + (u32)fifo->لکۍ * fifo->ثبت_اندازه);
		اكسر;
#د_که_پای
	افتراضي:
		sbi_memcpy(معلومات, fifo->کتار + (u32)fifo->لکۍ * fifo->ثبت_اندازه,
			   fifo->ثبت_اندازه);
		اكسر;
	}

skip_معلومات_copy:
	fifo->avail--;
	fifo->لکۍ++;
	إذا (fifo->لکۍ >= fifo->عدد_entries)
		fifo->لکۍ = 0;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن bool __sbi_fifo_is_empty(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	أرجع (fifo->avail == 0) ? true : false;
}

صحيح sbi_fifo_is_تشه(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_قلف(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_reset(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t اندازه = (size_t)fifo->عدد_entries * fifo->ثبت_اندازه;

	fifo->avail = 0;
	fifo->لکۍ  = 0;
	sbi_memset(fifo->کتار, 0, اندازه);
}

bool sbi_fifo_reset(بنية sbi_fifo *fifo)
{
	إذا (!fifo)
		أرجع false;

	spin_قلف(&fifo->qlock);
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
صحيح sbi_fifo_inplace_update(بنية sbi_fifo *fifo, فراغ *دننه,
			    صحيح (*fptr)(فراغ *دننه, فراغ *معلومات))
{
	صحيح i, شاخص_2;
	صحيح ret = SBI_FIFO_UNCHANGED;
	فراغ *ثبت_2;

	إذا (!fifo || !دننه)
		أرجع ret;

	spin_قلف(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع ret;
	}

	لكل (i = 0; i < fifo->avail; i++) {
		شاخص_2 = fifo->لکۍ + i;
		إذا (شاخص_2 >= fifo->عدد_entries)
			شاخص_2 -= fifo->عدد_entries;
		ثبت_2 = (حرف *)fifo->کتار + (u32)شاخص_2 * fifo->ثبت_اندازه;
		ret = fptr(دننه, ثبت_2);

		إذا (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			اكسر;
		}
	}
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_کتار_ته_ورزیاتول(بنية sbi_fifo *fifo, فراغ *معلومات, bool force)
{
	إذا (!fifo || !معلومات)
		أرجع SBI_EINVAL;

	spin_قلف(&fifo->qlock);

	إذا (__sbi_fifo_is_full(fifo)) {
		إذا (!force) {
			spin_unlock(&fifo->qlock);
			أرجع SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, عدم);
	}

	__sbi_fifo_enqueue(fifo, معلومات);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}

صحيح sbi_fifo_له_کتار_ایستل(بنية sbi_fifo *fifo, فراغ *معلومات)
{
	إذا (!fifo || !معلومات)
		أرجع SBI_EINVAL;

	spin_قلف(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, معلومات);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}
