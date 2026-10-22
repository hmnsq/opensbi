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
	ပြန်ပေး (fifo->avail == fifo->num_entries) ? true : false;
}

u16 sbi_fifo_avail(sa_nracanaa sbi_fifo *fifo)
{
	u16 ret;

	အကယ်၍ (!fifo)
		ပြန်ပေး 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	ပြန်ပေး ret;
}

ကိန်းပြည့် sbi_fifo_is_full(sa_nracanaa sbi_fifo *fifo)
{
	bool ret;

	အကယ်၍ (!fifo)
		ပြန်ပေး SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	ပြန်ပေး ret;
}

/* Note: must be called with fifo->qlock held */
static anta_hstha void __sbi_fifo_enqueue(sa_nracanaa sbi_fifo *fifo, void *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	အကယ်၍ (head >= fifo->num_entries)
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
#အကယ်၍ __riscv_xlen > 32
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
	အကယ်၍ (!data)
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
#အကယ်၍ __riscv_xlen > 32
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
	အကယ်၍ (fifo->tail >= fifo->num_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
static anta_hstha bool __sbi_fifo_is_empty(sa_nracanaa sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ပြန်ပေး (fifo->avail == 0) ? true : false;
}

ကိန်းပြည့် sbi_fifo_is_empty(sa_nracanaa sbi_fifo *fifo)
{
	bool ret;

	အကယ်၍ (!fifo)
		ပြန်ပေး SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	ပြန်ပေး ret;
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
	အကယ်၍ (!fifo)
		ပြန်ပေး false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	ပြန်ပေး true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
ကိန်းပြည့် sbi_fifo_inplace_update(sa_nracanaa sbi_fifo *fifo, void *in,
			    ကိန်းပြည့် (*fptr)(void *in, void *data))
{
	ကိန်းပြည့် i, index;
	ကိန်းပြည့် ret = SBI_FIFO_UNCHANGED;
	void *entry;

	အကယ်၍ (!fifo || !in)
		ပြန်ပေး ret;

	spin_lock(&fifo->qlock);

	အကယ်၍ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ပြန်ပေး ret;
	}

	အတွက် (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		အကယ်၍ (index >= fifo->num_entries)
			index -= fifo->num_entries;
		entry = (var_na *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		အကယ်၍ (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			bha_nga;
		}
	}
	spin_unlock(&fifo->qlock);

	ပြန်ပေး ret;
}

ကိန်းပြည့် sbi_fifo_enqueue(sa_nracanaa sbi_fifo *fifo, void *data, bool force)
{
	အကယ်၍ (!fifo || !data)
		ပြန်ပေး SBI_EINVAL;

	spin_lock(&fifo->qlock);

	အကယ်၍ (__sbi_fifo_is_full(fifo)) {
		အကယ်၍ (!force) {
			spin_unlock(&fifo->qlock);
			ပြန်ပေး SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, NULL);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	ပြန်ပေး 0;
}

ကိန်းပြည့် sbi_fifo_dequeue(sa_nracanaa sbi_fifo *fifo, void *data)
{
	အကယ်၍ (!fifo || !data)
		ပြန်ပေး SBI_EINVAL;

	spin_lock(&fifo->qlock);

	အကယ်၍ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ပြန်ပေး SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	ပြန်ပေး 0;
}
