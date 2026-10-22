/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_fifo.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>

ശൂന്യം sbi_fifo_init(ഘടന sbi_fifo *fifo, ശൂന്യം *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->അക്കം_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
സ്ഥിതം വരിയിൽ bool __sbi_fifo_is_full(ഘടന sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	തിരികെ (fifo->avail == fifo->അക്കം_entries) ? true : false;
}

u16 sbi_fifo_avail(ഘടന sbi_fifo *fifo)
{
	u16 ret;

	എങ്കിൽ (!fifo)
		തിരികെ 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_fifo_is_full(ഘടന sbi_fifo *fifo)
{
	bool ret;

	എങ്കിൽ (!fifo)
		തിരികെ SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	തിരികെ ret;
}

/* Note: must be called with fifo->qlock held */
സ്ഥിതം വരിയിൽ ശൂന്യം __sbi_fifo_enqueue(ഘടന sbi_fifo *fifo, ശൂന്യം *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	എങ്കിൽ (head >= fifo->അക്കം_entries)
		head = head - fifo->അക്കം_entries;

	തിരഞ്ഞെടുപ്പ് (fifo->entry_size) {
	അവസ്ഥ 1:
		*(അക്ഷരം *)(fifo->queue + head * fifo->entry_size) = *(അക്ഷരം *)data;
		നിർത്തുക;
	അവസ്ഥ 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		നിർത്തുക;
	അവസ്ഥ 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		നിർത്തുക;
#എങ്കിൽ __riscv_xlen > 32
	അവസ്ഥ 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		നിർത്തുക;
#endif
	സ്ഥിരസ്ഥിതി:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		നിർത്തുക;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
സ്ഥിതം വരിയിൽ ശൂന്യം __sbi_fifo_dequeue(ഘടന sbi_fifo *fifo, ശൂന്യം *data)
	MUST_HOLD(&fifo->qlock)
{
	എങ്കിൽ (!data)
		പോകുക skip_data_copy;

	തിരഞ്ഞെടുപ്പ് (fifo->entry_size) {
	അവസ്ഥ 1:
		*(അക്ഷരം *)data = *(അക്ഷരം *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		നിർത്തുക;
	അവസ്ഥ 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		നിർത്തുക;
	അവസ്ഥ 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		നിർത്തുക;
#എങ്കിൽ __riscv_xlen > 32
	അവസ്ഥ 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		നിർത്തുക;
#endif
	സ്ഥിരസ്ഥിതി:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		നിർത്തുക;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	എങ്കിൽ (fifo->tail >= fifo->അക്കം_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
സ്ഥിതം വരിയിൽ bool __sbi_fifo_is_empty(ഘടന sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	തിരികെ (fifo->avail == 0) ? true : false;
}

പൂർണ്ണസംഖ്യ sbi_fifo_is_empty(ഘടന sbi_fifo *fifo)
{
	bool ret;

	എങ്കിൽ (!fifo)
		തിരികെ SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	തിരികെ ret;
}

/* Note: must be called with fifo->qlock held */
സ്ഥിതം വരിയിൽ ശൂന്യം __sbi_fifo_reset(ഘടന sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->അക്കം_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(ഘടന sbi_fifo *fifo)
{
	എങ്കിൽ (!fifo)
		തിരികെ false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	തിരികെ true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
പൂർണ്ണസംഖ്യ sbi_fifo_inplace_update(ഘടന sbi_fifo *fifo, ശൂന്യം *in,
			    പൂർണ്ണസംഖ്യ (*fptr)(ശൂന്യം *in, ശൂന്യം *data))
{
	പൂർണ്ണസംഖ്യ i, index;
	പൂർണ്ണസംഖ്യ ret = SBI_FIFO_UNCHANGED;
	ശൂന്യം *entry;

	എങ്കിൽ (!fifo || !in)
		തിരികെ ret;

	spin_lock(&fifo->qlock);

	എങ്കിൽ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		തിരികെ ret;
	}

	വേണ്ടി (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		എങ്കിൽ (index >= fifo->അക്കം_entries)
			index -= fifo->അക്കം_entries;
		entry = (അക്ഷരം *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		എങ്കിൽ (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			നിർത്തുക;
		}
	}
	spin_unlock(&fifo->qlock);

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ sbi_fifo_enqueue(ഘടന sbi_fifo *fifo, ശൂന്യം *data, bool force)
{
	എങ്കിൽ (!fifo || !data)
		തിരികെ SBI_EINVAL;

	spin_lock(&fifo->qlock);

	എങ്കിൽ (__sbi_fifo_is_full(fifo)) {
		എങ്കിൽ (!force) {
			spin_unlock(&fifo->qlock);
			തിരികെ SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, ശൂന്യ);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_fifo_dequeue(ഘടന sbi_fifo *fifo, ശൂന്യം *data)
{
	എങ്കിൽ (!fifo || !data)
		തിരികെ SBI_EINVAL;

	spin_lock(&fifo->qlock);

	എങ്കിൽ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		തിരികെ SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	തിരികെ 0;
}
