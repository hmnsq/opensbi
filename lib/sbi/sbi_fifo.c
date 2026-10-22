/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#include <sbi/riscv_locks.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_fifo.h>
#include <sbi/sbi_string.h>

void sbi_fifo_init(struct sbi_fifo *fifo, void *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->num_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
static inline bool __sbi_fifo_is_full(struct sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	върни (fifo->avail == fifo->num_entries) ? true : false;
}

u16 sbi_fifo_avail(struct sbi_fifo *fifo)
{
	u16 ret;

	ако (!fifo)
		върни 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	върни ret;
}

цяло sbi_fifo_is_full(struct sbi_fifo *fifo)
{
	bool ret;

	ако (!fifo)
		върни SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	върни ret;
}

/* Note: must be called with fifo->qlock held */
static inline void __sbi_fifo_enqueue(struct sbi_fifo *fifo, void *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	ако (head >= fifo->num_entries)
		head = head - fifo->num_entries;

	switch (fifo->entry_size) {
	case 1:
		*(char *)(fifo->queue + head * fifo->entry_size) = *(char *)data;
		break;
	case 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		break;
	case 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		break;
#ако __riscv_xlen > 32
	case 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		break;
#endif
	default:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		break;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
static inline void __sbi_fifo_dequeue(struct sbi_fifo *fifo, void *data)
	MUST_HOLD(&fifo->qlock)
{
	ако (!data)
		goto skip_data_copy;

	switch (fifo->entry_size) {
	case 1:
		*(char *)data = *(char *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		break;
	case 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		break;
	case 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		break;
#ако __riscv_xlen > 32
	case 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		break;
#endif
	default:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		break;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	ако (fifo->tail >= fifo->num_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
static inline bool __sbi_fifo_is_empty(struct sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	върни (fifo->avail == 0) ? true : false;
}

цяло sbi_fifo_is_empty(struct sbi_fifo *fifo)
{
	bool ret;

	ако (!fifo)
		върни SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	върни ret;
}

/* Note: must be called with fifo->qlock held */
static inline void __sbi_fifo_reset(struct sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->num_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(struct sbi_fifo *fifo)
{
	ако (!fifo)
		върни false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	върни true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
цяло sbi_fifo_inplace_update(struct sbi_fifo *fifo, void *in,
			    цяло (*fptr)(void *in, void *data))
{
	цяло i, index;
	цяло ret = SBI_FIFO_UNCHANGED;
	void *entry;

	ако (!fifo || !in)
		върни ret;

	spin_lock(&fifo->qlock);

	ако (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		върни ret;
	}

	за (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		ако (index >= fifo->num_entries)
			index -= fifo->num_entries;
		entry = (char *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		ако (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			break;
		}
	}
	spin_unlock(&fifo->qlock);

	върни ret;
}

цяло sbi_fifo_enqueue(struct sbi_fifo *fifo, void *data, bool force)
{
	ако (!fifo || !data)
		върни SBI_EINVAL;

	spin_lock(&fifo->qlock);

	ако (__sbi_fifo_is_full(fifo)) {
		ако (!force) {
			spin_unlock(&fifo->qlock);
			върни SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, NULL);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	върни 0;
}

цяло sbi_fifo_dequeue(struct sbi_fifo *fifo, void *data)
{
	ако (!fifo || !data)
		върни SBI_EINVAL;

	spin_lock(&fifo->qlock);

	ако (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		върни SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	върни 0;
}
