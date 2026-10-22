/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_fifo.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

ಶೂನ್ಯ sbi_fifo_init(ಸಂರಚನಾ sbi_fifo *fifo, ಶೂನ್ಯ *queue_mem, u16 entries,
		   u16 entry_size) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->ಅಂಕೆ_entries = entries;
	fifo->entry_size  = entry_size;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->tail = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * entry_size);
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool __sbi_fifo_is_full(ಸಂರಚನಾ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ಪ್ರತ್ಯಾಗಮ (fifo->avail == fifo->ಅಂಕೆ_entries) ? true : false;
}

u16 sbi_fifo_avail(ಸಂರಚನಾ sbi_fifo *fifo)
{
	u16 ret;

	ಯದಿ (!fifo)
		ಪ್ರತ್ಯಾಗಮ 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_fifo_is_full(ಸಂರಚನಾ sbi_fifo *fifo)
{
	bool ret;

	ಯದಿ (!fifo)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	ಪ್ರತ್ಯಾಗಮ ret;
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ __sbi_fifo_enqueue(ಸಂರಚನಾ sbi_fifo *fifo, ಶೂನ್ಯ *data)
	MUST_HOLD(&fifo->qlock)
{
	u32 head;

	head = (u32)fifo->tail + fifo->avail;
	ಯದಿ (head >= fifo->ಅಂಕೆ_entries)
		head = head - fifo->ಅಂಕೆ_entries;

	ವಿಕಲ್ಪ (fifo->entry_size) {
	ಪಕ್ಷ 1:
		*(ವರ್ಣ *)(fifo->queue + head * fifo->entry_size) = *(ವರ್ಣ *)data;
		ಭಙ್ಗ;
	ಪಕ್ಷ 2:
		*(u16 *)(fifo->queue + head * fifo->entry_size) = *(u16 *)data;
		ಭಙ್ಗ;
	ಪಕ್ಷ 4:
		*(u32 *)(fifo->queue + head * fifo->entry_size) = *(u32 *)data;
		ಭಙ್ಗ;
#ಯದಿ __riscv_xlen > 32
	ಪಕ್ಷ 8:
		*(u64 *)(fifo->queue + head * fifo->entry_size) = *(u64 *)data;
		ಭಙ್ಗ;
#endif
	ಸ್ವಭಾವ:
		sbi_memcpy(fifo->queue + head * fifo->entry_size,
			   data, fifo->entry_size);
		ಭಙ್ಗ;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ __sbi_fifo_dequeue(ಸಂರಚನಾ sbi_fifo *fifo, ಶೂನ್ಯ *data)
	MUST_HOLD(&fifo->qlock)
{
	ಯದಿ (!data)
		ಗಚ್ಛ skip_data_copy;

	ವಿಕಲ್ಪ (fifo->entry_size) {
	ಪಕ್ಷ 1:
		*(ವರ್ಣ *)data = *(ವರ್ಣ *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ಭಙ್ಗ;
	ಪಕ್ಷ 2:
		*(u16 *)data = *(u16 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ಭಙ್ಗ;
	ಪಕ್ಷ 4:
		*(u32 *)data = *(u32 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ಭಙ್ಗ;
#ಯದಿ __riscv_xlen > 32
	ಪಕ್ಷ 8:
		*(u64 *)data = *(u64 *)(fifo->queue + (u32)fifo->tail * fifo->entry_size);
		ಭಙ್ಗ;
#endif
	ಸ್ವಭಾವ:
		sbi_memcpy(data, fifo->queue + (u32)fifo->tail * fifo->entry_size,
			   fifo->entry_size);
		ಭಙ್ಗ;
	}

skip_data_copy:
	fifo->avail--;
	fifo->tail++;
	ಯದಿ (fifo->tail >= fifo->ಅಂಕೆ_entries)
		fifo->tail = 0;
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool __sbi_fifo_is_empty(ಸಂರಚನಾ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	ಪ್ರತ್ಯಾಗಮ (fifo->avail == 0) ? true : false;
}

ಪೂರ್ಣಾಙ್ಕ sbi_fifo_is_empty(ಸಂರಚನಾ sbi_fifo *fifo)
{
	bool ret;

	ಯದಿ (!fifo)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	ಪ್ರತ್ಯಾಗಮ ret;
}

/* Note: must be called with fifo->qlock held */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ __sbi_fifo_reset(ಸಂರಚನಾ sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t size = (size_t)fifo->ಅಂಕೆ_entries * fifo->entry_size;

	fifo->avail = 0;
	fifo->tail  = 0;
	sbi_memset(fifo->queue, 0, size);
}

bool sbi_fifo_reset(ಸಂರಚನಾ sbi_fifo *fifo)
{
	ಯದಿ (!fifo)
		ಪ್ರತ್ಯಾಗಮ false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	ಪ್ರತ್ಯಾಗಮ true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
ಪೂರ್ಣಾಙ್ಕ sbi_fifo_inplace_update(ಸಂರಚನಾ sbi_fifo *fifo, ಶೂನ್ಯ *in,
			    ಪೂರ್ಣಾಙ್ಕ (*fptr)(ಶೂನ್ಯ *in, ಶೂನ್ಯ *data))
{
	ಪೂರ್ಣಾಙ್ಕ i, index;
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_FIFO_UNCHANGED;
	ಶೂನ್ಯ *entry;

	ಯದಿ (!fifo || !in)
		ಪ್ರತ್ಯಾಗಮ ret;

	spin_lock(&fifo->qlock);

	ಯದಿ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತಿ (i = 0; i < fifo->avail; i++) {
		index = fifo->tail + i;
		ಯದಿ (index >= fifo->ಅಂಕೆ_entries)
			index -= fifo->ಅಂಕೆ_entries;
		entry = (ವರ್ಣ *)fifo->queue + (u32)index * fifo->entry_size;
		ret = fptr(in, entry);

		ಯದಿ (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			ಭಙ್ಗ;
		}
	}
	spin_unlock(&fifo->qlock);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ sbi_fifo_enqueue(ಸಂರಚನಾ sbi_fifo *fifo, ಶೂನ್ಯ *data, bool force)
{
	ಯದಿ (!fifo || !data)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	spin_lock(&fifo->qlock);

	ಯದಿ (__sbi_fifo_is_full(fifo)) {
		ಯದಿ (!force) {
			spin_unlock(&fifo->qlock);
			ಪ್ರತ್ಯಾಗಮ SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, ರಿಕ್ತ);
	}

	__sbi_fifo_enqueue(fifo, data);

	spin_unlock(&fifo->qlock);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_fifo_dequeue(ಸಂರಚನಾ sbi_fifo *fifo, ಶೂನ್ಯ *data)
{
	ಯದಿ (!fifo || !data)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	spin_lock(&fifo->qlock);

	ಯದಿ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, data);

	spin_unlock(&fifo->qlock);

	ಪ್ರತ್ಯಾಗಮ 0;
}
