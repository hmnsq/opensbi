/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#অন্তর্ভাৱ <sbi/riscv_locks.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_fifo.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>

শূন্য sbi_fifo_init(সংরচনা sbi_fifo *fifo, শূন্য *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->অঙ্ক_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃস্থ bool __sbi_fifo_is_full(সংরচনা sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ঘূৰাই (fifo->avail == fifo->অঙ্ক_entries) ? true : false;
}

u16 sbi_fifo_avail(সংরচনা sbi_fifo *fifo)
{
	u16 ret;

	যদি (!fifo)
		ঘূৰাই 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_fifo_is_full(সংরচনা sbi_fifo *fifo)
{
	bool ret;

	যদি (!fifo)
		ঘূৰাই SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	ঘূৰাই ret;
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃস্থ শূন্য __sbi_fifo_enqueue(সংরচনা sbi_fifo *fifo, শূন্য *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	যদি (head >= fifo->অঙ্ক_entries)
		head = head - fifo->অঙ্ক_entries;

	ৱিকল্প (fifo->entry_size) {
	পক্ষ 1:
		*(ৱর্ণ *)(fifo->queue + head * fifo->entry_size) = *(ৱর্ণ *)data;
		ভঙ্গ;
	পক্ষ 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		ভঙ্গ;
	পক্ষ 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		ভঙ্গ;
#যদি __riscv_xlen > 32
	পক্ষ 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		ভঙ্গ;
#endif
	স্ৱভাৱ:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		ভঙ্গ;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃস্থ শূন্য __sbi_fifo_dequeue(সংরচনা sbi_fifo *fifo, শূন্য *data)
	MUST_HOLD(&fifo->qlock)
{
	যদি (!data)
		গচ্ছ skip_data_copy;

	ৱিকল্প (fifo->entry_size) {
	পক্ষ 1:
		*(ৱর্ণ *)data = *(ৱর্ণ *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ভঙ্গ;
	পক্ষ 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ভঙ্গ;
	পক্ষ 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ভঙ্গ;
#যদি __riscv_xlen > 32
	পক্ষ 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ভঙ্গ;
#endif
	স্ৱভাৱ:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		ভঙ্গ;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	যদি (fifo->tail >= fifo->অঙ্ক_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃস্থ bool __sbi_fifo_is_empty(সংরচনা sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ঘূৰাই (fifo->avail == 0) ? true : false;
}

পূর্ণাঙ্ক sbi_fifo_is_empty(সংরচনা sbi_fifo *fifo)
{
	bool ret;

	যদি (!fifo)
		ঘূৰাই SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	ঘূৰাই ret;
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃস্থ শূন্য __sbi_fifo_reset(সংরচনা sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->অঙ্ক_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(সংরচনা sbi_fifo *fifo)
{
	যদি (!fifo)
		ঘূৰাই false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	ঘূৰাই true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
পূর্ণাঙ্ক sbi_fifo_inplace_update(সংরচনা sbi_fifo *fifo, শূন্য *in,
			    পূর্ণাঙ্ক (*fptr)(শূন্য *in, শূন্য *data))
{
	পূর্ণাঙ্ক i, index;
	পূর্ণাঙ্ক ret = SBI_FIFO_UNCHANGED;
	শূন্য *entry;

	যদি (!fifo || !in)
		ঘূৰাই ret;

	spin_lock(&fifo->qlock);

	যদি (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ঘূৰাই ret;
	}

	বাবে (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		যদি (index >= fifo->অঙ্ক_entries)
			index -= fifo->অঙ্ক_entries;
		entry = (ৱর্ণ *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		যদি (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			ভঙ্গ;
		}
	}
	spin_unlock(&fifo->qlock);

	ঘূৰাই ret;
}

পূর্ণাঙ্ক sbi_fifo_enqueue(সংরচনা sbi_fifo *fifo, শূন্য *data, bool force)
{
	যদি (!fifo || !data)
		ঘূৰাই SBI_EINVAL;

	spin_lock(&fifo->qlock);

	যদি (__sbi_fifo_is_full(fifo)) {
		যদি (!force) {
			spin_unlock(&fifo->qlock);
			ঘূৰাই SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, রিক্ত);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	ঘূৰাই 0;
}

পূর্ণাঙ্ক sbi_fifo_dequeue(সংরচনা sbi_fifo *fifo, শূন্য *data)
{
	যদি (!fifo || !data)
		ঘূৰাই SBI_EINVAL;

	spin_lock(&fifo->qlock);

	যদি (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ঘূৰাই SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	ঘূৰাই 0;
}
