/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_fifo.h>
#अन्तर्भाव <sbi/sbi_string.h>

शून्य sbi_fifo_init(संरचना sbi_fifo *fifo, शून्य *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->अंक_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (माप_प्रकार)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
स्थिर ओळीत bool __sbi_fifo_is_full(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	परत (fifo->avail == fifo->अंक_entries) ? true : false;
}

u16 sbi_fifo_avail(संरचना sbi_fifo *fifo)
{
	u16 ret;

	जर (!fifo)
		परत 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	परत ret;
}

पूर्णांक sbi_fifo_is_full(संरचना sbi_fifo *fifo)
{
	bool ret;

	जर (!fifo)
		परत SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	परत ret;
}

/* Note: must be called with fifo->qlock held */
स्थिर ओळीत शून्य __sbi_fifo_enqueue(संरचना sbi_fifo *fifo, शून्य *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	जर (head >= fifo->अंक_entries)
		head = head - fifo->अंक_entries;

	निवड (fifo->entry_size) {
	स्थिती 1:
		*(अक्षर *)(fifo->queue + head * fifo->entry_size) = *(अक्षर *)data;
		थांबा;
	स्थिती 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		थांबा;
	स्थिती 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		थांबा;
#जर __riscv_xlen > 32
	स्थिती 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		थांबा;
#endif
	पूर्वनिर्धारित:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		थांबा;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
स्थिर ओळीत शून्य __sbi_fifo_dequeue(संरचना sbi_fifo *fifo, शून्य *data)
	MUST_HOLD(&fifo->qlock)
{
	जर (!data)
		जा skip_data_copy;

	निवड (fifo->entry_size) {
	स्थिती 1:
		*(अक्षर *)data = *(अक्षर *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		थांबा;
	स्थिती 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		थांबा;
	स्थिती 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		थांबा;
#जर __riscv_xlen > 32
	स्थिती 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		थांबा;
#endif
	पूर्वनिर्धारित:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		थांबा;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	जर (fifo->tail >= fifo->अंक_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
स्थिर ओळीत bool __sbi_fifo_is_empty(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	परत (fifo->avail == 0) ? true : false;
}

पूर्णांक sbi_fifo_is_empty(संरचना sbi_fifo *fifo)
{
	bool ret;

	जर (!fifo)
		परत SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	परत ret;
}

/* Note: must be called with fifo->qlock held */
स्थिर ओळीत शून्य __sbi_fifo_reset(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	माप_प्रकार size = (माप_प्रकार)fifo->अंक_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(संरचना sbi_fifo *fifo)
{
	जर (!fifo)
		परत false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	परत true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
पूर्णांक sbi_fifo_inplace_update(संरचना sbi_fifo *fifo, शून्य *in,
			    पूर्णांक (*fptr)(शून्य *in, शून्य *data))
{
	पूर्णांक i, index;
	पूर्णांक ret = SBI_FIFO_UNCHANGED;
	शून्य *entry;

	जर (!fifo || !in)
		परत ret;

	spin_lock(&fifo->qlock);

	जर (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		परत ret;
	}

	साठी (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		जर (index >= fifo->अंक_entries)
			index -= fifo->अंक_entries;
		entry = (अक्षर *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		जर (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			थांबा;
		}
	}
	spin_unlock(&fifo->qlock);

	परत ret;
}

पूर्णांक sbi_fifo_enqueue(संरचना sbi_fifo *fifo, शून्य *data, bool force)
{
	जर (!fifo || !data)
		परत SBI_EINVAL;

	spin_lock(&fifo->qlock);

	जर (__sbi_fifo_is_full(fifo)) {
		जर (!force) {
			spin_unlock(&fifo->qlock);
			परत SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, रिक्त);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	परत 0;
}

पूर्णांक sbi_fifo_dequeue(संरचना sbi_fifo *fifo, शून्य *data)
{
	जर (!fifo || !data)
		परत SBI_EINVAL;

	spin_lock(&fifo->qlock);

	जर (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		परत SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	परत 0;
}
