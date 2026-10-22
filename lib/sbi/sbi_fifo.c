/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_fifo.h>
#અન્તર્ભાવ <sbi/sbi_string.h>

શૂન્ય sbi_fifo_init(સંરચના sbi_fifo *fifo, શૂન્ય *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->અંક_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
સ્થિર અંતઃરેખ bool __sbi_fifo_is_full(સંરચના sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	પાછા (fifo->avail == fifo->અંક_entries) ? true : false;
}

u16 sbi_fifo_avail(સંરચના sbi_fifo *fifo)
{
	u16 ret;

	જો (!fifo)
		પાછા 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	પાછા ret;
}

પૂર્ણાંક sbi_fifo_is_full(સંરચના sbi_fifo *fifo)
{
	bool ret;

	જો (!fifo)
		પાછા SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	પાછા ret;
}

/* Note: must be called with fifo->qlock held */
સ્થિર અંતઃરેખ શૂન્ય __sbi_fifo_enqueue(સંરચના sbi_fifo *fifo, શૂન્ય *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	જો (head >= fifo->અંક_entries)
		head = head - fifo->અંક_entries;

	પસંદગી (fifo->entry_size) {
	સ્થિતિ 1:
		*(અક્ષર *)(fifo->queue + head * fifo->entry_size) = *(અક્ષર *)data;
		તોડો;
	સ્થિતિ 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		તોડો;
	સ્થિતિ 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		તોડો;
#જો __riscv_xlen > 32
	સ્થિતિ 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		તોડો;
#endif
	મૂળભૂત:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		તોડો;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
સ્થિર અંતઃરેખ શૂન્ય __sbi_fifo_dequeue(સંરચના sbi_fifo *fifo, શૂન્ય *data)
	MUST_HOLD(&fifo->qlock)
{
	જો (!data)
		જાઓ skip_data_copy;

	પસંદગી (fifo->entry_size) {
	સ્થિતિ 1:
		*(અક્ષર *)data = *(અક્ષર *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		તોડો;
	સ્થિતિ 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		તોડો;
	સ્થિતિ 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		તોડો;
#જો __riscv_xlen > 32
	સ્થિતિ 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		તોડો;
#endif
	મૂળભૂત:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		તોડો;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	જો (fifo->tail >= fifo->અંક_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
સ્થિર અંતઃરેખ bool __sbi_fifo_is_empty(સંરચના sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	પાછા (fifo->avail == 0) ? true : false;
}

પૂર્ણાંક sbi_fifo_is_empty(સંરચના sbi_fifo *fifo)
{
	bool ret;

	જો (!fifo)
		પાછા SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	પાછા ret;
}

/* Note: must be called with fifo->qlock held */
સ્થિર અંતઃરેખ શૂન્ય __sbi_fifo_reset(સંરચના sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->અંક_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(સંરચના sbi_fifo *fifo)
{
	જો (!fifo)
		પાછા false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	પાછા true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
પૂર્ણાંક sbi_fifo_inplace_update(સંરચના sbi_fifo *fifo, શૂન્ય *in,
			    પૂર્ણાંક (*fptr)(શૂન્ય *in, શૂન્ય *data))
{
	પૂર્ણાંક i, index;
	પૂર્ણાંક ret = SBI_FIFO_UNCHANGED;
	શૂન્ય *entry;

	જો (!fifo || !in)
		પાછા ret;

	spin_lock(&fifo->qlock);

	જો (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		પાછા ret;
	}

	માટે (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		જો (index >= fifo->અંક_entries)
			index -= fifo->અંક_entries;
		entry = (અક્ષર *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		જો (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			તોડો;
		}
	}
	spin_unlock(&fifo->qlock);

	પાછા ret;
}

પૂર્ણાંક sbi_fifo_enqueue(સંરચના sbi_fifo *fifo, શૂન્ય *data, bool force)
{
	જો (!fifo || !data)
		પાછા SBI_EINVAL;

	spin_lock(&fifo->qlock);

	જો (__sbi_fifo_is_full(fifo)) {
		જો (!force) {
			spin_unlock(&fifo->qlock);
			પાછા SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, ખાલી);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	પાછા 0;
}

પૂર્ણાંક sbi_fifo_dequeue(સંરચના sbi_fifo *fifo, શૂન્ય *data)
{
	જો (!fifo || !data)
		પાછા SBI_EINVAL;

	spin_lock(&fifo->qlock);

	જો (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		પાછા SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	પાછા 0;
}
