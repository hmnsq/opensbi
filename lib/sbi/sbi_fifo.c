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

فراغ sbi_fifo_init(بنية sbi_fifo *fifo, فراغ *queue_mem, u16 entries,
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

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_is_full(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_enqueue(بنية sbi_fifo *fifo, فراغ *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	إذا (head >= fifo->رقم_entries)
		head = head - fifo->رقم_entries;

	بدل (fifo->entry_size) {
	حالة 1:
		*(حرف *)(fifo->queue + head * fifo->entry_size) = *(حرف *)data;
		اكسر;
	حالة 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		اكسر;
	حالة 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		اكسر;
#endif
	افتراضي:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		اكسر;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_dequeue(بنية sbi_fifo *fifo, فراغ *data)
	MUST_HOLD(&fifo->qlock)
{
	إذا (!data)
		اذهب skip_data_copy;

	بدل (fifo->entry_size) {
	حالة 1:
		*(حرف *)data = *(حرف *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		اكسر;
	حالة 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		اكسر;
	حالة 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		اكسر;
#إذا __riscv_xlen > 32
	حالة 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		اكسر;
#endif
	افتراضي:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		اكسر;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	إذا (fifo->tail >= fifo->رقم_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن bool __sbi_fifo_is_empty(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	أرجع (fifo->avail == 0) ? true : false;
}

صحيح sbi_fifo_is_empty(بنية sbi_fifo *fifo)
{
	bool ret;

	إذا (!fifo)
		أرجع SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

/* Note: must be called with fifo->qlock held */
ساكن مضمن فراغ __sbi_fifo_reset(بنية sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->رقم_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(بنية sbi_fifo *fifo)
{
	إذا (!fifo)
		أرجع false;

	spin_lock(&fifo->qlock);
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
صحيح sbi_fifo_inplace_update(بنية sbi_fifo *fifo, فراغ *in,
			    صحيح (*fptr)(فراغ *in, فراغ *data))
{
	صحيح i, index;
	صحيح ret = SBI_FIFO_UNCHANGED;
	فراغ *entry;

	إذا (!fifo || !in)
		أرجع ret;

	spin_lock(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع ret;
	}

	لكل (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		إذا (index >= fifo->رقم_entries)
			index -= fifo->رقم_entries;
		entry = (حرف *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		إذا (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			اكسر;
		}
	}
	spin_unlock(&fifo->qlock);

	أرجع ret;
}

صحيح sbi_fifo_enqueue(بنية sbi_fifo *fifo, فراغ *data, bool force)
{
	إذا (!fifo || !data)
		أرجع SBI_EINVAL;

	spin_lock(&fifo->qlock);

	إذا (__sbi_fifo_is_full(fifo)) {
		إذا (!force) {
			spin_unlock(&fifo->qlock);
			أرجع SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, عدم);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}

صحيح sbi_fifo_dequeue(بنية sbi_fifo *fifo, فراغ *data)
{
	إذا (!fifo || !data)
		أرجع SBI_EINVAL;

	spin_lock(&fifo->qlock);

	إذا (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		أرجع SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	أرجع 0;
}
