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
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ bool __sbi_fifo_is_full(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	प्रत्यागम (fifo->avail == fifo->अंक_entries) ? true : false;
}

u16 sbi_fifo_avail(संरचना sbi_fifo *fifo)
{
	u16 ret;

	यदि (!fifo)
		प्रत्यागम 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_fifo_is_full(संरचना sbi_fifo *fifo)
{
	bool ret;

	यदि (!fifo)
		प्रत्यागम SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	प्रत्यागम ret;
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ शून्य __sbi_fifo_enqueue(संरचना sbi_fifo *fifo, शून्य *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	यदि (head >= fifo->अंक_entries)
		head = head - fifo->अंक_entries;

	विकल्प (fifo->entry_size) {
	पक्ष 1:
		*(वर्ण *)(fifo->queue + head * fifo->entry_size) = *(वर्ण *)data;
		भङ्ग;
	पक्ष 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		भङ्ग;
	पक्ष 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		भङ्ग;
#यदि __riscv_xlen > 32
	पक्ष 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		भङ्ग;
#endif
	स्वभाव:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		भङ्ग;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ शून्य __sbi_fifo_dequeue(संरचना sbi_fifo *fifo, शून्य *data)
	MUST_HOLD(&fifo->qlock)
{
	यदि (!data)
		गच्छ skip_data_copy;

	विकल्प (fifo->entry_size) {
	पक्ष 1:
		*(वर्ण *)data = *(वर्ण *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		भङ्ग;
	पक्ष 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		भङ्ग;
	पक्ष 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		भङ्ग;
#यदि __riscv_xlen > 32
	पक्ष 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		भङ्ग;
#endif
	स्वभाव:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		भङ्ग;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	यदि (fifo->tail >= fifo->अंक_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ bool __sbi_fifo_is_empty(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	प्रत्यागम (fifo->avail == 0) ? true : false;
}

पूर्णाङ्क sbi_fifo_is_empty(संरचना sbi_fifo *fifo)
{
	bool ret;

	यदि (!fifo)
		प्रत्यागम SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	प्रत्यागम ret;
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ शून्य __sbi_fifo_reset(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->अंक_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(संरचना sbi_fifo *fifo)
{
	यदि (!fifo)
		प्रत्यागम false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	प्रत्यागम true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
पूर्णाङ्क sbi_fifo_inplace_update(संरचना sbi_fifo *fifo, शून्य *in,
			    पूर्णाङ्क (*fptr)(शून्य *in, शून्य *data))
{
	पूर्णाङ्क i, index;
	पूर्णाङ्क ret = SBI_FIFO_UNCHANGED;
	शून्य *entry;

	यदि (!fifo || !in)
		प्रत्यागम ret;

	spin_lock(&fifo->qlock);

	यदि (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		प्रत्यागम ret;
	}

	प्रति (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		यदि (index >= fifo->अंक_entries)
			index -= fifo->अंक_entries;
		entry = (वर्ण *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		यदि (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			भङ्ग;
		}
	}
	spin_unlock(&fifo->qlock);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_fifo_enqueue(संरचना sbi_fifo *fifo, शून्य *data, bool force)
{
	यदि (!fifo || !data)
		प्रत्यागम SBI_EINVAL;

	spin_lock(&fifo->qlock);

	यदि (__sbi_fifo_is_full(fifo)) {
		यदि (!force) {
			spin_unlock(&fifo->qlock);
			प्रत्यागम SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, रिक्त);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_fifo_dequeue(संरचना sbi_fifo *fifo, शून्य *data)
{
	यदि (!fifo || !data)
		प्रत्यागम SBI_EINVAL;

	spin_lock(&fifo->qlock);

	यदि (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		प्रत्यागम SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	प्रत्यागम 0;
}
