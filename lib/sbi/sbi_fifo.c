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
	вернуть (fifo->avail == fifo->num_entries) ? true : false;
}

u16 sbi_fifo_avail(struct sbi_fifo *fifo)
{
	u16 ret;

	если (!fifo)
		вернуть 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	вернуть ret;
}

целое sbi_fifo_is_full(struct sbi_fifo *fifo)
{
	bool ret;

	если (!fifo)
		вернуть SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	вернуть ret;
}

/* Note: must be called with fifo->qlock held */
static inline void __sbi_fifo_enqueue(struct sbi_fifo *fifo, void *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	если (head >= fifo->num_entries)
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
#если __riscv_xlen > 32
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
	если (!data)
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
#если __riscv_xlen > 32
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
	если (fifo->tail >= fifo->num_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
static inline bool __sbi_fifo_is_empty(struct sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	вернуть (fifo->avail == 0) ? true : false;
}

целое sbi_fifo_is_empty(struct sbi_fifo *fifo)
{
	bool ret;

	если (!fifo)
		вернуть SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	вернуть ret;
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
	если (!fifo)
		вернуть false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	вернуть true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
целое sbi_fifo_inplace_update(struct sbi_fifo *fifo, void *in,
			    целое (*fptr)(void *in, void *data))
{
	целое i, index;
	целое ret = SBI_FIFO_UNCHANGED;
	void *entry;

	если (!fifo || !in)
		вернуть ret;

	spin_lock(&fifo->qlock);

	если (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		вернуть ret;
	}

	для (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		если (index >= fifo->num_entries)
			index -= fifo->num_entries;
		entry = (char *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		если (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			break;
		}
	}
	spin_unlock(&fifo->qlock);

	вернуть ret;
}

целое sbi_fifo_enqueue(struct sbi_fifo *fifo, void *data, bool force)
{
	если (!fifo || !data)
		вернуть SBI_EINVAL;

	spin_lock(&fifo->qlock);

	если (__sbi_fifo_is_full(fifo)) {
		если (!force) {
			spin_unlock(&fifo->qlock);
			вернуть SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, NULL);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	вернуть 0;
}

целое sbi_fifo_dequeue(struct sbi_fifo *fifo, void *data)
{
	если (!fifo || !data)
		вернуть SBI_EINVAL;

	spin_lock(&fifo->qlock);

	если (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		вернуть SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	вернуть 0;
}
