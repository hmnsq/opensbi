/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_fifo.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>

ଶୂନ୍ୟ sbi_fifo_init(ସଂରଚନା sbi_fifo *fifo, ଶୂନ୍ୟ *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->ଅଙ୍କ_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool __sbi_fifo_is_full(ସଂରଚନା sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ଫେରନ୍ତୁ (fifo->avail == fifo->ଅଙ୍କ_entries) ? true : false;
}

u16 sbi_fifo_avail(ସଂରଚନା sbi_fifo *fifo)
{
	u16 ret;

	ଯଦି (!fifo)
		ଫେରନ୍ତୁ 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_fifo_is_full(ସଂରଚନା sbi_fifo *fifo)
{
	bool ret;

	ଯଦି (!fifo)
		ଫେରନ୍ତୁ SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	ଫେରନ୍ତୁ ret;
}

/* Note: must be called with fifo->qlock held */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ __sbi_fifo_enqueue(ସଂରଚନା sbi_fifo *fifo, ଶୂନ୍ୟ *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	ଯଦି (head >= fifo->ଅଙ୍କ_entries)
		head = head - fifo->ଅଙ୍କ_entries;

	ବାଛ (fifo->entry_size) {
	ସ୍ଥିତି 1:
		*(ଅକ୍ଷର *)(fifo->queue + head * fifo->entry_size) = *(ଅକ୍ଷର *)data;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		ଭାଙ୍ଗ;
#ଯଦି __riscv_xlen > 32
	ସ୍ଥିତି 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		ଭାଙ୍ଗ;
#endif
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		ଭାଙ୍ଗ;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ __sbi_fifo_dequeue(ସଂରଚନା sbi_fifo *fifo, ଶୂନ୍ୟ *data)
	MUST_HOLD(&fifo->qlock)
{
	ଯଦି (!data)
		ଯାଅ skip_data_copy;

	ବାଛ (fifo->entry_size) {
	ସ୍ଥିତି 1:
		*(ଅକ୍ଷର *)data = *(ଅକ୍ଷର *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ଭାଙ୍ଗ;
#ଯଦି __riscv_xlen > 32
	ସ୍ଥିତି 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ଭାଙ୍ଗ;
#endif
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		ଭାଙ୍ଗ;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	ଯଦି (fifo->tail >= fifo->ଅଙ୍କ_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool __sbi_fifo_is_empty(ସଂରଚନା sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ଫେରନ୍ତୁ (fifo->avail == 0) ? true : false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_fifo_is_empty(ସଂରଚନା sbi_fifo *fifo)
{
	bool ret;

	ଯଦି (!fifo)
		ଫେରନ୍ତୁ SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	ଫେରନ୍ତୁ ret;
}

/* Note: must be called with fifo->qlock held */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ __sbi_fifo_reset(ସଂରଚନା sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->ଅଙ୍କ_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(ସଂରଚନା sbi_fifo *fifo)
{
	ଯଦି (!fifo)
		ଫେରନ୍ତୁ false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	ଫେରନ୍ତୁ true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_fifo_inplace_update(ସଂରଚନା sbi_fifo *fifo, ଶୂନ୍ୟ *in,
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*fptr)(ଶୂନ୍ୟ *in, ଶୂନ୍ୟ *data))
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, index;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_FIFO_UNCHANGED;
	ଶୂନ୍ୟ *entry;

	ଯଦି (!fifo || !in)
		ଫେରନ୍ତୁ ret;

	spin_lock(&fifo->qlock);

	ଯଦି (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ଫେରନ୍ତୁ ret;
	}

	ପାଇଁ (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		ଯଦି (index >= fifo->ଅଙ୍କ_entries)
			index -= fifo->ଅଙ୍କ_entries;
		entry = (ଅକ୍ଷର *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		ଯଦି (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			ଭାଙ୍ଗ;
		}
	}
	spin_unlock(&fifo->qlock);

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_fifo_enqueue(ସଂରଚନା sbi_fifo *fifo, ଶୂନ୍ୟ *data, bool force)
{
	ଯଦି (!fifo || !data)
		ଫେରନ୍ତୁ SBI_EINVAL;

	spin_lock(&fifo->qlock);

	ଯଦି (__sbi_fifo_is_full(fifo)) {
		ଯଦି (!force) {
			spin_unlock(&fifo->qlock);
			ଫେରନ୍ତୁ SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, ଖାଲି);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_fifo_dequeue(ସଂରଚନା sbi_fifo *fifo, ଶୂନ୍ୟ *data)
{
	ଯଦି (!fifo || !data)
		ଫେରନ୍ତୁ SBI_EINVAL;

	spin_lock(&fifo->qlock);

	ଯଦି (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ଫେରନ୍ତୁ SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	ଫେରନ୍ତୁ 0;
}
