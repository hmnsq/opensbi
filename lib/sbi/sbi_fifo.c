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

void sbi_fifo_init(sa_nracanaa sbi_fifo *fifo, void *queue_mem, u16 entries,
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
static anta_hstha bool __sbi_fifo_is_full(sa_nracanaa sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ᯀᯮᯞ (fifo->avail == fifo->num_entries) ? true : false;
}

u16 sbi_fifo_avail(sa_nracanaa sbi_fifo *fifo)
{
	u16 ret;

	ᯀᯮᯝ (!fifo)
		ᯀᯮᯞ 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	ᯀᯮᯞ ret;
}

puur_naa_nka sbi_fifo_is_full(sa_nracanaa sbi_fifo *fifo)
{
	bool ret;

	ᯀᯮᯝ (!fifo)
		ᯀᯮᯞ SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	ᯀᯮᯞ ret;
}

/* Note: must be called with fifo->qlock held */
static anta_hstha void __sbi_fifo_enqueue(sa_nracanaa sbi_fifo *fifo, void *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	ᯀᯮᯝ (head >= fifo->num_entries)
		head = head - fifo->num_entries;

	switch (fifo->entry_size) {
	pak_sa 1:
		*(var_na *)(fifo->queue + head * fifo->entry_size) = *(var_na *)data;
		bha_nga;
	pak_sa 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		bha_nga;
	pak_sa 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		bha_nga;
#ᯀᯮᯝ __riscv_xlen > 32
	pak_sa 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		bha_nga;
#endif
	default:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		bha_nga;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
static anta_hstha void __sbi_fifo_dequeue(sa_nracanaa sbi_fifo *fifo, void *data)
	MUST_HOLD(&fifo->qlock)
{
	ᯀᯮᯝ (!data)
		goto skip_data_copy;

	switch (fifo->entry_size) {
	pak_sa 1:
		*(var_na *)data = *(var_na *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		bha_nga;
	pak_sa 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		bha_nga;
	pak_sa 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		bha_nga;
#ᯀᯮᯝ __riscv_xlen > 32
	pak_sa 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		bha_nga;
#endif
	default:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		bha_nga;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	ᯀᯮᯝ (fifo->tail >= fifo->num_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
static anta_hstha bool __sbi_fifo_is_empty(sa_nracanaa sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ᯀᯮᯞ (fifo->avail == 0) ? true : false;
}

puur_naa_nka sbi_fifo_is_empty(sa_nracanaa sbi_fifo *fifo)
{
	bool ret;

	ᯀᯮᯝ (!fifo)
		ᯀᯮᯞ SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	ᯀᯮᯞ ret;
}

/* Note: must be called with fifo->qlock held */
static anta_hstha void __sbi_fifo_reset(sa_nracanaa sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->num_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(sa_nracanaa sbi_fifo *fifo)
{
	ᯀᯮᯝ (!fifo)
		ᯀᯮᯞ false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	ᯀᯮᯞ true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
puur_naa_nka sbi_fifo_inplace_update(sa_nracanaa sbi_fifo *fifo, void *in,
			    puur_naa_nka (*fptr)(void *in, void *data))
{
	puur_naa_nka i, index;
	puur_naa_nka ret = SBI_FIFO_UNCHANGED;
	void *entry;

	ᯀᯮᯝ (!fifo || !in)
		ᯀᯮᯞ ret;

	spin_lock(&fifo->qlock);

	ᯀᯮᯝ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ᯀᯮᯞ ret;
	}

	for (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		ᯀᯮᯝ (index >= fifo->num_entries)
			index -= fifo->num_entries;
		entry = (var_na *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		ᯀᯮᯝ (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			bha_nga;
		}
	}
	spin_unlock(&fifo->qlock);

	ᯀᯮᯞ ret;
}

puur_naa_nka sbi_fifo_enqueue(sa_nracanaa sbi_fifo *fifo, void *data, bool force)
{
	ᯀᯮᯝ (!fifo || !data)
		ᯀᯮᯞ SBI_EINVAL;

	spin_lock(&fifo->qlock);

	ᯀᯮᯝ (__sbi_fifo_is_full(fifo)) {
		ᯀᯮᯝ (!force) {
			spin_unlock(&fifo->qlock);
			ᯀᯮᯞ SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, NULL);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	ᯀᯮᯞ 0;
}

puur_naa_nka sbi_fifo_dequeue(sa_nracanaa sbi_fifo *fifo, void *data)
{
	ᯀᯮᯝ (!fifo || !data)
		ᯀᯮᯞ SBI_EINVAL;

	spin_lock(&fifo->qlock);

	ᯀᯮᯝ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ᯀᯮᯞ SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	ᯀᯮᯞ 0;
}
