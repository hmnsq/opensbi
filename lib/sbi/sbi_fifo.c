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
		   u16 داخلا_سائيز) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->قطار	  = قطار_mem;
	fifo->انگ_entries = entries;
	fifo->داخلا_سائيز  = داخلا_سائيز;
	SPIN_تالو_INIT(fifo->qlock);
	fifo->avail = fifo->پڇ = 0;
	sbi_memset(fifo->قطار, 0, (size_t)entries * داخلا_سائيز);
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن bool __sbi_fifo_is_full(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	أرجع (fifo->avail == fifo->انگ_entries) ? true : false;
}

u16 sbi_fifo_avail(بنية sbi_fifo *fifo)
{
	u16 ret;

	إذا (!fifo)
		أرجع 0;

	spin_تالو(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_is_ڀريل(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_تالو(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_enqueue(بنية sbi_fifo *fifo, فراغ *معلومات)
	MUST_HOLD(&fifo->qlock)
{
	u32 سر;

	سر = (u32)fifo->پڇ + fifo->avail;
	إذا (سر >= fifo->انگ_entries)
		سر = سر - fifo->انگ_entries;

	بدل (fifo->داخلا_سائيز) {
	حالة 1:
		*(حرف *)(fifo->قطار + سر * fifo->داخلا_سائيز) = *(حرف *)معلومات;
		اكسر;
	حالة 2:
		*(u16 *)(fifo->قطار + سر * fifo->داخلا_سائيز) = *(u16 *)معلومات;
		اكسر;
	حالة 4:
		*(u32 *)(fifo->قطار + سر * fifo->داخلا_سائيز) = *(u32 *)معلومات;
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)(fifo->قطار + سر * fifo->داخلا_سائيز) = *(u64 *)معلومات;
		اكسر;
#جيڪڏهن_پورو
	افتراضي:
		sbi_memcpy(fifo->قطار + سر * fifo->داخلا_سائيز,
			   معلومات, fifo->داخلا_سائيز);
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

	بدل (fifo->داخلا_سائيز) {
	حالة 1:
		*(حرف *)معلومات = *(حرف *)(fifo->قطار + (u32)fifo->پڇ * fifo->داخلا_سائيز);
		اكسر;
	حالة 2:
		*(u16 *)معلومات = *(u16 *)(fifo->قطار + (u32)fifo->پڇ * fifo->داخلا_سائيز);
		اكسر;
	حالة 4:
		*(u32 *)معلومات = *(u32 *)(fifo->قطار + (u32)fifo->پڇ * fifo->داخلا_سائيز);
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)معلومات = *(u64 *)(fifo->قطار + (u32)fifo->پڇ * fifo->داخلا_سائيز);
		اكسر;
#جيڪڏهن_پورو
	افتراضي:
		sbi_memcpy(معلومات, fifo->قطار + (u32)fifo->پڇ * fifo->داخلا_سائيز,
			   fifo->داخلا_سائيز);
		اكسر;
	}

skip_معلومات_copy:
	fifo->avail--;
	fifo->پڇ++;
	إذا (fifo->پڇ >= fifo->انگ_entries)
		fifo->پڇ = 0;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن bool __sbi_fifo_is_empty(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	أرجع (fifo->avail == 0) ? true : false;
}

صحيح sbi_fifo_is_خالي_جايون(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_تالو(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_reset(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t سائيز = (size_t)fifo->انگ_entries * fifo->داخلا_سائيز;

	fifo->avail = 0;
	fifo->پڇ  = 0;
	sbi_memset(fifo->قطار, 0, سائيز);
}

bool sbi_fifo_reset(بنية sbi_fifo *fifo)
{
	إذا (!fifo)
		أرجع false;

	spin_تالو(&fifo->qlock);
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
صحيح sbi_fifo_inplace_update(بنية sbi_fifo *fifo, فراغ *اندريون,
			    صحيح (*fptr)(فراغ *اندريون, فراغ *معلومات))
{
	صحيح i, اشاريو_2;
	صحيح ret = SBI_FIFO_UNCHANGED;
	فراغ *داخلا_2;

	إذا (!fifo || !اندريون)
		أرجع ret;

	spin_تالو(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع ret;
	}

	لكل (i = 0; i < fifo->avail; i++) {
		اشاريو_2 = fifo->پڇ + i;
		إذا (اشاريو_2 >= fifo->انگ_entries)
			اشاريو_2 -= fifo->انگ_entries;
		داخلا_2 = (حرف *)fifo->قطار + (u32)اشاريو_2 * fifo->داخلا_سائيز;
		ret = fptr(اندريون, داخلا_2);

		إذا (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			اكسر;
		}
	}
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_قطار_اندر_وجهو(بنية sbi_fifo *fifo, فراغ *معلومات, bool force)
{
	إذا (!fifo || !معلومات)
		أرجع SBI_EINVAL;

	spin_تالو(&fifo->qlock);

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

صحيح sbi_fifo_قطار_مان_ڪڍو(بنية sbi_fifo *fifo, فراغ *معلومات)
{
	إذا (!fifo || !معلومات)
		أرجع SBI_EINVAL;

	spin_تالو(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, معلومات);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}
