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

فراغ sbi_fifo_init(بنية sbi_fifo *fifo, فراغ *صف_mem, u16 entries,
		   u16 مدخل_اندازه) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->صف	  = صف_mem;
	fifo->رقم_entries = entries;
	fifo->مدخل_اندازه  = مدخل_اندازه;
	SPIN_قفل_INIT(fifo->qlock);
	fifo->avail = fifo->دم = 0;
	sbi_memset(fifo->صف, 0, (size_t)entries * مدخل_اندازه);
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن bool __sbi_fifo_is_full(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	أرجع (fifo->avail == fifo->رقم_entries) ? true : false;
}

u16 sbi_fifo_avail(بنية sbi_fifo *fifo)
{
	u16 ret;

	إذا (!fifo)
		أرجع 0;

	spin_قفل(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_is_پر(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_قفل(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_enqueue(بنية sbi_fifo *fifo, فراغ *داده)
	MUST_HOLD(&fifo->qlock)
{
	u32 سر;

	سر = (u32)fifo->دم + fifo->avail;
	إذا (سر >= fifo->رقم_entries)
		سر = سر - fifo->رقم_entries;

	بدل (fifo->مدخل_اندازه) {
	حالة 1:
		*(حرف *)(fifo->صف + سر * fifo->مدخل_اندازه) = *(حرف *)داده;
		اكسر;
	حالة 2:
		*(u16 *)(fifo->صف + سر * fifo->مدخل_اندازه) = *(u16 *)داده;
		اكسر;
	حالة 4:
		*(u32 *)(fifo->صف + سر * fifo->مدخل_اندازه) = *(u32 *)داده;
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)(fifo->صف + سر * fifo->مدخل_اندازه) = *(u64 *)داده;
		اكسر;
#پایان_اگر
	افتراضي:
		sbi_memcpy(fifo->صف + سر * fifo->مدخل_اندازه,
			   داده, fifo->مدخل_اندازه);
		اكسر;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_dequeue(بنية sbi_fifo *fifo, فراغ *داده)
	MUST_HOLD(&fifo->qlock)
{
	إذا (!داده)
		اذهب skip_داده_copy;

	بدل (fifo->مدخل_اندازه) {
	حالة 1:
		*(حرف *)داده = *(حرف *)(fifo->صف + (u32)fifo->دم * fifo->مدخل_اندازه);
		اكسر;
	حالة 2:
		*(u16 *)داده = *(u16 *)(fifo->صف + (u32)fifo->دم * fifo->مدخل_اندازه);
		اكسر;
	حالة 4:
		*(u32 *)داده = *(u32 *)(fifo->صف + (u32)fifo->دم * fifo->مدخل_اندازه);
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)داده = *(u64 *)(fifo->صف + (u32)fifo->دم * fifo->مدخل_اندازه);
		اكسر;
#پایان_اگر
	افتراضي:
		sbi_memcpy(داده, fifo->صف + (u32)fifo->دم * fifo->مدخل_اندازه,
			   fifo->مدخل_اندازه);
		اكسر;
	}

skip_داده_copy:
	fifo->avail--;
	fifo->دم++;
	إذا (fifo->دم >= fifo->رقم_entries)
		fifo->دم = 0;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن bool __sbi_fifo_is_empty(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	أرجع (fifo->avail == 0) ? true : false;
}

صحيح sbi_fifo_is_خالی(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_قفل(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_reset(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t اندازه = (size_t)fifo->رقم_entries * fifo->مدخل_اندازه;

	fifo->avail = 0;
	fifo->دم  = 0;
	sbi_memset(fifo->صف, 0, اندازه);
}

bool sbi_fifo_reset(بنية sbi_fifo *fifo)
{
	إذا (!fifo)
		أرجع false;

	spin_قفل(&fifo->qlock);
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
صحيح sbi_fifo_inplace_update(بنية sbi_fifo *fifo, فراغ *درون,
			    صحيح (*fptr)(فراغ *درون, فراغ *داده))
{
	صحيح i, شاخص_2;
	صحيح ret = SBI_FIFO_UNCHANGED;
	فراغ *مدخل_2;

	إذا (!fifo || !درون)
		أرجع ret;

	spin_قفل(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع ret;
	}

	لكل (i = 0; i < fifo->avail; i++) {
		شاخص_2 = fifo->دم + i;
		إذا (شاخص_2 >= fifo->رقم_entries)
			شاخص_2 -= fifo->رقم_entries;
		مدخل_2 = (حرف *)fifo->صف + (u32)شاخص_2 * fifo->مدخل_اندازه;
		ret = fptr(درون, مدخل_2);

		إذا (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			اكسر;
		}
	}
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_افزودن_به_صف(بنية sbi_fifo *fifo, فراغ *داده, bool force)
{
	إذا (!fifo || !داده)
		أرجع SBI_EINVAL;

	spin_قفل(&fifo->qlock);

	إذا (__sbi_fifo_is_full(fifo)) {
		إذا (!force) {
			spin_unlock(&fifo->qlock);
			أرجع SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, عدم);
	}

	__sbi_fifo_enqueue(fifo, داده);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}

صحيح sbi_fifo_خارج_از_صف(بنية sbi_fifo *fifo, فراغ *داده)
{
	إذا (!fifo || !داده)
		أرجع SBI_EINVAL;

	spin_قفل(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, داده);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}
