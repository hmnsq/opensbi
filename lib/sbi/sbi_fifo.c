/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#அந்தர்பாவ <sbi/riscv_locks.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_fifo.h>
#அந்தர்பாவ <sbi/sbi_string.h>

வெறுமை sbi_fifo_init(கட்டமைப்பு sbi_fifo *fifo, வெறுமை *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->இலக்கம்_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
நிலையான உள்வரி bool __sbi_fifo_is_full(கட்டமைப்பு sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	திரும்பு (fifo->avail == fifo->இலக்கம்_entries) ? true : false;
}

u16 sbi_fifo_avail(கட்டமைப்பு sbi_fifo *fifo)
{
	u16 ret;

	எனில் (!fifo)
		திரும்பு 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	திரும்பு ret;
}

முழு sbi_fifo_is_full(கட்டமைப்பு sbi_fifo *fifo)
{
	bool ret;

	எனில் (!fifo)
		திரும்பு SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	திரும்பு ret;
}

/* Note: must be called with fifo->qlock held */
நிலையான உள்வரி வெறுமை __sbi_fifo_enqueue(கட்டமைப்பு sbi_fifo *fifo, வெறுமை *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	எனில் (head >= fifo->இலக்கம்_entries)
		head = head - fifo->இலக்கம்_entries;

	தேர்வு (fifo->entry_size) {
	நிலை 1:
		*(எழுத்து *)(fifo->queue + head * fifo->entry_size) = *(எழுத்து *)data;
		நிறுத்து;
	நிலை 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		நிறுத்து;
	நிலை 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		நிறுத்து;
#எனில் __riscv_xlen > 32
	நிலை 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		நிறுத்து;
#endif
	இயல்பு:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		நிறுத்து;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
நிலையான உள்வரி வெறுமை __sbi_fifo_dequeue(கட்டமைப்பு sbi_fifo *fifo, வெறுமை *data)
	MUST_HOLD(&fifo->qlock)
{
	எனில் (!data)
		செல் skip_data_copy;

	தேர்வு (fifo->entry_size) {
	நிலை 1:
		*(எழுத்து *)data = *(எழுத்து *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		நிறுத்து;
	நிலை 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		நிறுத்து;
	நிலை 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		நிறுத்து;
#எனில் __riscv_xlen > 32
	நிலை 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		நிறுத்து;
#endif
	இயல்பு:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		நிறுத்து;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	எனில் (fifo->tail >= fifo->இலக்கம்_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
நிலையான உள்வரி bool __sbi_fifo_is_empty(கட்டமைப்பு sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	திரும்பு (fifo->avail == 0) ? true : false;
}

முழு sbi_fifo_is_empty(கட்டமைப்பு sbi_fifo *fifo)
{
	bool ret;

	எனில் (!fifo)
		திரும்பு SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	திரும்பு ret;
}

/* Note: must be called with fifo->qlock held */
நிலையான உள்வரி வெறுமை __sbi_fifo_reset(கட்டமைப்பு sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->இலக்கம்_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(கட்டமைப்பு sbi_fifo *fifo)
{
	எனில் (!fifo)
		திரும்பு false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	திரும்பு true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
முழு sbi_fifo_inplace_update(கட்டமைப்பு sbi_fifo *fifo, வெறுமை *in,
			    முழு (*fptr)(வெறுமை *in, வெறுமை *data))
{
	முழு i, index;
	முழு ret = SBI_FIFO_UNCHANGED;
	வெறுமை *entry;

	எனில் (!fifo || !in)
		திரும்பு ret;

	spin_lock(&fifo->qlock);

	எனில் (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		திரும்பு ret;
	}

	க்கு (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		எனில் (index >= fifo->இலக்கம்_entries)
			index -= fifo->இலக்கம்_entries;
		entry = (எழுத்து *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		எனில் (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			நிறுத்து;
		}
	}
	spin_unlock(&fifo->qlock);

	திரும்பு ret;
}

முழு sbi_fifo_enqueue(கட்டமைப்பு sbi_fifo *fifo, வெறுமை *data, bool force)
{
	எனில் (!fifo || !data)
		திரும்பு SBI_EINVAL;

	spin_lock(&fifo->qlock);

	எனில் (__sbi_fifo_is_full(fifo)) {
		எனில் (!force) {
			spin_unlock(&fifo->qlock);
			திரும்பு SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, வெற்று);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	திரும்பு 0;
}

முழு sbi_fifo_dequeue(கட்டமைப்பு sbi_fifo *fifo, வெறுமை *data)
{
	எனில் (!fifo || !data)
		திரும்பு SBI_EINVAL;

	spin_lock(&fifo->qlock);

	எனில் (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		திரும்பு SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	திரும்பு 0;
}
