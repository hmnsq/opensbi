/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#অন্তর্ভাব <sbi/riscv_locks.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_fifo.h>
#অন্তর্ভাব <sbi/sbi_string.h>

শূন্য sbi_fifo_init(কাঠামো sbi_fifo *fifo, শূন্য *queue_mem, u16 entries,
		   u16 entry_আকার) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->অঙ্ক_entries = entries;
	fifo->entry_আকার  = entry_আকার;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->লেজ = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_আকার);
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃরেখ bool __sbi_fifo_is_full(কাঠামো sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ফেরত (fifo->avail == fifo->অঙ্ক_entries) ? true : false;
}

u16 sbi_fifo_avail(কাঠামো sbi_fifo *fifo)
{
	u16 ret;

	যদি (!fifo)
		ফেরত 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	ফেরত ret;
}

পূর্ণসংখ্যা sbi_fifo_is_full(কাঠামো sbi_fifo *fifo)
{
	bool ret;

	যদি (!fifo)
		ফেরত SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	ফেরত ret;
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃরেখ শূন্য __sbi_fifo_enqueue(কাঠামো sbi_fifo *fifo, শূন্য *তথ্য)
	MUST_HOLD(&fifo->qlock)
{
	u32 শীর্ষ;

	শীর্ষ = (u32)fifo->লেজ + fifo->avail;
	যদি (শীর্ষ >= fifo->অঙ্ক_entries)
		শীর্ষ = শীর্ষ - fifo->অঙ্ক_entries;

	নির্বাচন (fifo->entry_আকার) {
	ক্ষেত্র 1:
		*(অক্ষর *)(fifo->queue + শীর্ষ * fifo->entry_আকার) = *(অক্ষর *)তথ্য;
		বিরতি;
	ক্ষেত্র 2:
		*(u16 *)(fifo->queue + শীর্ষ * fifo->entry_আকার) = *(u16 *)তথ্য;
		বিরতি;
	ক্ষেত্র 4:
		*(u32 *)(fifo->queue + শীর্ষ * fifo->entry_আকার) = *(u32 *)তথ্য;
		বিরতি;
#যদি __riscv_xlen > 32
	ক্ষেত্র 8:
		*(u64 *)(fifo->queue + শীর্ষ * fifo->entry_আকার) = *(u64 *)তথ্য;
		বিরতি;
#endif
	পূর্বনির্ধারিত:
		sbi_memcpy(fifo->queue + শীর্ষ * fifo->entry_আকার,
			   তথ্য, fifo->entry_আকার);
		বিরতি;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃরেখ শূন্য __sbi_fifo_dequeue(কাঠামো sbi_fifo *fifo, শূন্য *তথ্য)
	MUST_HOLD(&fifo->qlock)
{
	যদি (!তথ্য)
		যাও skip_তথ্য_copy;

	নির্বাচন (fifo->entry_আকার) {
	ক্ষেত্র 1:
		*(অক্ষর *)তথ্য = *(অক্ষর *)(fifo->queue + (u32)fifo->লেজ * fifo->entry_আকার);
		বিরতি;
	ক্ষেত্র 2:
		*(u16 *)তথ্য = *(u16 *)(fifo->queue + (u32)fifo->লেজ * fifo->entry_আকার);
		বিরতি;
	ক্ষেত্র 4:
		*(u32 *)তথ্য = *(u32 *)(fifo->queue + (u32)fifo->লেজ * fifo->entry_আকার);
		বিরতি;
#যদি __riscv_xlen > 32
	ক্ষেত্র 8:
		*(u64 *)তথ্য = *(u64 *)(fifo->queue + (u32)fifo->লেজ * fifo->entry_আকার);
		বিরতি;
#endif
	পূর্বনির্ধারিত:
		sbi_memcpy(তথ্য, fifo->queue + (u32)fifo->লেজ * fifo->entry_আকার,
			   fifo->entry_আকার);
		বিরতি;
	}

skip_তথ্য_copy:
	fifo->avail--;
	fifo->লেজ++;
	যদি (fifo->লেজ >= fifo->অঙ্ক_entries)
		fifo->লেজ = 0;
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃরেখ bool __sbi_fifo_is_empty(কাঠামো sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ফেরত (fifo->avail == 0) ? true : false;
}

পূর্ণসংখ্যা sbi_fifo_is_empty(কাঠামো sbi_fifo *fifo)
{
	bool ret;

	যদি (!fifo)
		ফেরত SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	ফেরত ret;
}

/* Note: must be called with fifo->qlock held */
স্থির অন্তঃরেখ শূন্য __sbi_fifo_reset(কাঠামো sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t আকার_2 = (size_t)fifo->অঙ্ক_entries * fifo->entry_আকার;

	fifo->avail = 0;
	fifo->লেজ  = 0;
	sbi_memset(fifo->queue, 0, আকার_2);
}

bool sbi_fifo_reset(কাঠামো sbi_fifo *fifo)
{
	যদি (!fifo)
		ফেরত false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	ফেরত true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
পূর্ণসংখ্যা sbi_fifo_inplace_update(কাঠামো sbi_fifo *fifo, শূন্য *in,
			    পূর্ণসংখ্যা (*fptr)(শূন্য *in, শূন্য *তথ্য))
{
	পূর্ণসংখ্যা i, সূচক_2;
	পূর্ণসংখ্যা ret = SBI_FIFO_UNCHANGED;
	শূন্য *entry;

	যদি (!fifo || !in)
		ফেরত ret;

	spin_lock(&fifo->qlock);

	যদি (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ফেরত ret;
	}

	জন্য (i = 0; i < fifo->avail; i++) {
		সূচক_2 = fifo->লেজ + i;
		যদি (সূচক_2 >= fifo->অঙ্ক_entries)
			সূচক_2 -= fifo->অঙ্ক_entries;
		entry = (অক্ষর *)fifo->queue + (u32)সূচক_2 * fifo->entry_আকার;
		ret = fptr(in, entry);

		যদি (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			বিরতি;
		}
	}
	spin_unlock(&fifo->qlock);

	ফেরত ret;
}

পূর্ণসংখ্যা sbi_fifo_enqueue(কাঠামো sbi_fifo *fifo, শূন্য *তথ্য, bool force)
{
	যদি (!fifo || !তথ্য)
		ফেরত SBI_EINVAL;

	spin_lock(&fifo->qlock);

	যদি (__sbi_fifo_is_full(fifo)) {
		যদি (!force) {
			spin_unlock(&fifo->qlock);
			ফেরত SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, ফাঁকা);
	}

	__sbi_fifo_enqueue(fifo, তথ্য);

	spin_unlock(&fifo->qlock);

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_fifo_dequeue(কাঠামো sbi_fifo *fifo, শূন্য *তথ্য)
{
	যদি (!fifo || !তথ্য)
		ফেরত SBI_EINVAL;

	spin_lock(&fifo->qlock);

	যদি (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ফেরত SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, তথ্য);

	spin_unlock(&fifo->qlock);

	ফেরত 0;
}
