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

تهی sbi_fifo_init(ساختار sbi_fifo *fifo, تهی *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->رقم_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
ایستا درونخطی bool __sbi_fifo_is_full(ساختار sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	بازگشت (fifo->avail == fifo->رقم_entries) ? true : false;
}

u16 sbi_fifo_avail(ساختار sbi_fifo *fifo)
{
	u16 ret;

	اگر (!fifo)
		بازگشت 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	بازگشت ret;
}

صحیح sbi_fifo_is_full(ساختار sbi_fifo *fifo)
{
	bool ret;

	اگر (!fifo)
		بازگشت SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	بازگشت ret;
}

/* Note: must be called with fifo->qlock held */
ایستا درونخطی تهی __sbi_fifo_enqueue(ساختار sbi_fifo *fifo, تهی *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	اگر (head >= fifo->رقم_entries)
		head = head - fifo->رقم_entries;

	انتخاب (fifo->entry_size) {
	حالت 1:
		*(نویسه *)(fifo->queue + head * fifo->entry_size) = *(نویسه *)data;
		بشکن;
	حالت 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		بشکن;
	حالت 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		بشکن;
#اگر __riscv_xlen > 32
	حالت 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		بشکن;
#endif
	پیشفرض:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		بشکن;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ایستا درونخطی تهی __sbi_fifo_dequeue(ساختار sbi_fifo *fifo, تهی *data)
	MUST_HOLD(&fifo->qlock)
{
	اگر (!data)
		برو skip_data_copy;

	انتخاب (fifo->entry_size) {
	حالت 1:
		*(نویسه *)data = *(نویسه *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		بشکن;
	حالت 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		بشکن;
	حالت 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		بشکن;
#اگر __riscv_xlen > 32
	حالت 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		بشکن;
#endif
	پیشفرض:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		بشکن;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	اگر (fifo->tail >= fifo->رقم_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
ایستا درونخطی bool __sbi_fifo_is_empty(ساختار sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	بازگشت (fifo->avail == 0) ? true : false;
}

صحیح sbi_fifo_is_empty(ساختار sbi_fifo *fifo)
{
	bool ret;

	اگر (!fifo)
		بازگشت SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	بازگشت ret;
}

/* Note: must be called with fifo->qlock held */
ایستا درونخطی تهی __sbi_fifo_reset(ساختار sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->رقم_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(ساختار sbi_fifo *fifo)
{
	اگر (!fifo)
		بازگشت false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	بازگشت true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
صحیح sbi_fifo_inplace_update(ساختار sbi_fifo *fifo, تهی *in,
			    صحیح (*fptr)(تهی *in, تهی *data))
{
	صحیح i, index;
	صحیح ret = SBI_FIFO_UNCHANGED;
	تهی *entry;

	اگر (!fifo || !in)
		بازگشت ret;

	spin_lock(&fifo->qlock);

	اگر (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		بازگشت ret;
	}

	برای (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		اگر (index >= fifo->رقم_entries)
			index -= fifo->رقم_entries;
		entry = (نویسه *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		اگر (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			بشکن;
		}
	}
	spin_unlock(&fifo->qlock);

	بازگشت ret;
}

صحیح sbi_fifo_enqueue(ساختار sbi_fifo *fifo, تهی *data, bool force)
{
	اگر (!fifo || !data)
		بازگشت SBI_EINVAL;

	spin_lock(&fifo->qlock);

	اگر (__sbi_fifo_is_full(fifo)) {
		اگر (!force) {
			spin_unlock(&fifo->qlock);
			بازگشت SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, پوچ);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	بازگشت 0;
}

صحیح sbi_fifo_dequeue(ساختار sbi_fifo *fifo, تهی *data)
{
	اگر (!fifo || !data)
		بازگشت SBI_EINVAL;

	spin_lock(&fifo->qlock);

	اگر (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		بازگشت SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	بازگشت 0;
}
