/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_fifo.h>
#شامل <sbi/sbi_string.h>

خالی sbi_fifo_init(ساخت sbi_fifo *fifo, خالی *قطار_mem, u16 entries,
		   u16 اندراج_سائز) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->قطار	  = قطار_mem;
	fifo->عدد_entries = entries;
	fifo->اندراج_سائز  = اندراج_سائز;
	SPIN_تالا_INIT(fifo->qlock);
	fifo->avail = fifo->دم = 0;
	sbi_memset(fifo->قطار, 0, (size_t)entries * اندراج_سائز);
}

/* Note: must be called with fifo->qlock held */
ساکن اندرونی bool __sbi_fifo_is_full(ساخت sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	واپس (fifo->avail == fifo->عدد_entries) ? true : false;
}

u16 sbi_fifo_avail(ساخت sbi_fifo *fifo)
{
	u16 ret;

	اگر (!fifo)
		واپس 0;

	spin_تالا(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	واپس ret;
}

صحیح sbi_fifo_is_بھرا(ساخت sbi_fifo *fifo)
{
	bool ret;

	اگر (!fifo)
		واپس SBI_EINVAL;

	spin_تالا(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	واپس ret;
}

/* Note: must be called with fifo->qlock held */
ساکن اندرونی خالی __sbi_fifo_enqueue(ساخت sbi_fifo *fifo, خالی *ڈیٹا)
	MUST_HOLD(&fifo->qlock)
{
	u32 سر;

	سر = (u32)fifo->دم + fifo->avail;
	اگر (سر >= fifo->عدد_entries)
		سر = سر - fifo->عدد_entries;

	انتخاب (fifo->اندراج_سائز) {
	حالت 1:
		*(حرف *)(fifo->قطار + سر * fifo->اندراج_سائز) = *(حرف *)ڈیٹا;
		توڑو;
	حالت 2:
		*(u16 *)(fifo->قطار + سر * fifo->اندراج_سائز) = *(u16 *)ڈیٹا;
		توڑو;
	حالت 4:
		*(u32 *)(fifo->قطار + سر * fifo->اندراج_سائز) = *(u32 *)ڈیٹا;
		توڑو;
#اگر __riscv_xlen > 32
	حالت 8:
		*(u64 *)(fifo->قطار + سر * fifo->اندراج_سائز) = *(u64 *)ڈیٹا;
		توڑو;
#اگر_ختم
	طےشدہ:
		sbi_memcpy(fifo->قطار + سر * fifo->اندراج_سائز,
			   ڈیٹا, fifo->اندراج_سائز);
		توڑو;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ساکن اندرونی خالی __sbi_fifo_dequeue(ساخت sbi_fifo *fifo, خالی *ڈیٹا)
	MUST_HOLD(&fifo->qlock)
{
	اگر (!ڈیٹا)
		جاؤ skip_ڈیٹا_copy;

	انتخاب (fifo->اندراج_سائز) {
	حالت 1:
		*(حرف *)ڈیٹا = *(حرف *)(fifo->قطار + (u32)fifo->دم * fifo->اندراج_سائز);
		توڑو;
	حالت 2:
		*(u16 *)ڈیٹا = *(u16 *)(fifo->قطار + (u32)fifo->دم * fifo->اندراج_سائز);
		توڑو;
	حالت 4:
		*(u32 *)ڈیٹا = *(u32 *)(fifo->قطار + (u32)fifo->دم * fifo->اندراج_سائز);
		توڑو;
#اگر __riscv_xlen > 32
	حالت 8:
		*(u64 *)ڈیٹا = *(u64 *)(fifo->قطار + (u32)fifo->دم * fifo->اندراج_سائز);
		توڑو;
#اگر_ختم
	طےشدہ:
		sbi_memcpy(ڈیٹا, fifo->قطار + (u32)fifo->دم * fifo->اندراج_سائز,
			   fifo->اندراج_سائز);
		توڑو;
	}

skip_ڈیٹا_copy:
	fifo->avail--;
	fifo->دم++;
	اگر (fifo->دم >= fifo->عدد_entries)
		fifo->دم = 0;
}

/* Note: must be called with fifo->qlock held */
ساکن اندرونی bool __sbi_fifo_is_empty(ساخت sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	واپس (fifo->avail == 0) ? true : false;
}

صحیح sbi_fifo_is_خالی_خانے(ساخت sbi_fifo *fifo)
{
	bool ret;

	اگر (!fifo)
		واپس SBI_EINVAL;

	spin_تالا(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	واپس ret;
}

/* Note: must be called with fifo->qlock held */
ساکن اندرونی خالی __sbi_fifo_reset(ساخت sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t سائز = (size_t)fifo->عدد_entries * fifo->اندراج_سائز;

	fifo->avail = 0;
	fifo->دم  = 0;
	sbi_memset(fifo->قطار, 0, سائز);
}

bool sbi_fifo_reset(ساخت sbi_fifo *fifo)
{
	اگر (!fifo)
		واپس false;

	spin_تالا(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	واپس true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
صحیح sbi_fifo_inplace_update(ساخت sbi_fifo *fifo, خالی *اندر,
			    صحیح (*fptr)(خالی *اندر, خالی *ڈیٹا))
{
	صحیح i, اشاریہ_2;
	صحیح ret = SBI_FIFO_UNCHANGED;
	خالی *اندراج_3;

	اگر (!fifo || !اندر)
		واپس ret;

	spin_تالا(&fifo->qlock);

	اگر (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		واپس ret;
	}

	برائے (i = 0; i < fifo->avail; i++) {
		اشاریہ_2 = fifo->دم + i;
		اگر (اشاریہ_2 >= fifo->عدد_entries)
			اشاریہ_2 -= fifo->عدد_entries;
		اندراج_3 = (حرف *)fifo->قطار + (u32)اشاریہ_2 * fifo->اندراج_سائز;
		ret = fptr(اندر, اندراج_3);

		اگر (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			توڑو;
		}
	}
	spin_unlock(&fifo->qlock);

	واپس ret;
}

صحیح sbi_fifo_قطار_میں_ڈالو(ساخت sbi_fifo *fifo, خالی *ڈیٹا, bool force)
{
	اگر (!fifo || !ڈیٹا)
		واپس SBI_EINVAL;

	spin_تالا(&fifo->qlock);

	اگر (__sbi_fifo_is_full(fifo)) {
		اگر (!force) {
			spin_unlock(&fifo->qlock);
			واپس SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, تہی);
	}

	__sbi_fifo_enqueue(fifo, ڈیٹا);

	spin_unlock(&fifo->qlock);

	واپس 0;
}

صحیح sbi_fifo_قطار_سے_نکالو(ساخت sbi_fifo *fifo, خالی *ڈیٹا)
{
	اگر (!fifo || !ڈیٹا)
		واپس SBI_EINVAL;

	spin_تالا(&fifo->qlock);

	اگر (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		واپس SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, ڈیٹا);

	spin_unlock(&fifo->qlock);

	واپس 0;
}
