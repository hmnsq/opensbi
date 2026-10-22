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
		   u16 प्रविष्टिः_आकार) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->अङ्कः_entries = entries;
	fifo->प्रविष्टिः_आकार  = प्रविष्टिः_आकार;
	SPIN_तालकम्_INIT(fifo->qlock);
	fifo->avail = fifo->पुच्छ = 0;
	sbi_memset(fifo->queue, 0, (size_t)entries * प्रविष्टिः_आकार);
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ bool __sbi_fifo_is_full(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	प्रत्यागम (fifo->avail == fifo->अङ्कः_entries) ? true : false;
}

u16 sbi_fifo_avail(संरचना sbi_fifo *fifo)
{
	u16 ret;

	यदि (!fifo)
		प्रत्यागम 0;

	spin_तालकम्(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_fifo_is_पूर्णम्(संरचना sbi_fifo *fifo)
{
	bool ret;

	यदि (!fifo)
		प्रत्यागम SBI_EINVAL;

	spin_तालकम्(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	प्रत्यागम ret;
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ शून्य __sbi_fifo_enqueue(संरचना sbi_fifo *fifo, शून्य *दत्त)
	MUST_HOLD(&fifo->qlock)
{
	u32 शीर्ष;

	शीर्ष = (u32)fifo->पुच्छ + fifo->avail;
	यदि (शीर्ष >= fifo->अङ्कः_entries)
		शीर्ष = शीर्ष - fifo->अङ्कः_entries;

	विकल्प (fifo->प्रविष्टिः_आकार) {
	पक्ष 1:
		*(वर्ण *)(fifo->queue + शीर्ष * fifo->प्रविष्टिः_आकार) = *(वर्ण *)दत्त;
		भङ्ग;
	पक्ष 2:
		*(u16 *)(fifo->queue + शीर्ष * fifo->प्रविष्टिः_आकार) = *(u16 *)दत्त;
		भङ्ग;
	पक्ष 4:
		*(u32 *)(fifo->queue + शीर्ष * fifo->प्रविष्टिः_आकार) = *(u32 *)दत्त;
		भङ्ग;
#यदि __riscv_xlen > 32
	पक्ष 8:
		*(u64 *)(fifo->queue + शीर्ष * fifo->प्रविष्टिः_आकार) = *(u64 *)दत्त;
		भङ्ग;
#यदि_अन्तः
	स्वभाव:
		sbi_memcpy(fifo->queue + शीर्ष * fifo->प्रविष्टिः_आकार,
			   दत्त, fifo->प्रविष्टिः_आकार);
		भङ्ग;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ शून्य __sbi_fifo_dequeue(संरचना sbi_fifo *fifo, शून्य *दत्त)
	MUST_HOLD(&fifo->qlock)
{
	यदि (!दत्त)
		गच्छ skip_दत्त_copy;

	विकल्प (fifo->प्रविष्टिः_आकार) {
	पक्ष 1:
		*(वर्ण *)दत्त = *(वर्ण *)(fifo->queue + (u32)fifo->पुच्छ * fifo->प्रविष्टिः_आकार);
		भङ्ग;
	पक्ष 2:
		*(u16 *)दत्त = *(u16 *)(fifo->queue + (u32)fifo->पुच्छ * fifo->प्रविष्टिः_आकार);
		भङ्ग;
	पक्ष 4:
		*(u32 *)दत्त = *(u32 *)(fifo->queue + (u32)fifo->पुच्छ * fifo->प्रविष्टिः_आकार);
		भङ्ग;
#यदि __riscv_xlen > 32
	पक्ष 8:
		*(u64 *)दत्त = *(u64 *)(fifo->queue + (u32)fifo->पुच्छ * fifo->प्रविष्टिः_आकार);
		भङ्ग;
#यदि_अन्तः
	स्वभाव:
		sbi_memcpy(दत्त, fifo->queue + (u32)fifo->पुच्छ * fifo->प्रविष्टिः_आकार,
			   fifo->प्रविष्टिः_आकार);
		भङ्ग;
	}

skip_दत्त_copy:
	fifo->avail--;
	fifo->पुच्छ++;
	यदि (fifo->पुच्छ >= fifo->अङ्कः_entries)
		fifo->पुच्छ = 0;
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ bool __sbi_fifo_is_empty(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	प्रत्यागम (fifo->avail == 0) ? true : false;
}

पूर्णाङ्क sbi_fifo_is_रिक्तानि(संरचना sbi_fifo *fifo)
{
	bool ret;

	यदि (!fifo)
		प्रत्यागम SBI_EINVAL;

	spin_तालकम्(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	प्रत्यागम ret;
}

/* Note: must be called with fifo->qlock held */
स्थिर अन्तःस्थ शून्य __sbi_fifo_reset(संरचना sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t आकार = (size_t)fifo->अङ्कः_entries * fifo->प्रविष्टिः_आकार;

	fifo->avail = 0;
	fifo->पुच्छ  = 0;
	sbi_memset(fifo->queue, 0, आकार);
}

bool sbi_fifo_reset(संरचना sbi_fifo *fifo)
{
	यदि (!fifo)
		प्रत्यागम false;

	spin_तालकम्(&fifo->qlock);
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
पूर्णाङ्क sbi_fifo_inplace_update(संरचना sbi_fifo *fifo, शून्य *अन्तर्,
			    पूर्णाङ्क (*fptr)(शून्य *अन्तर्, शून्य *दत्त))
{
	पूर्णाङ्क i, सूचकांक_2;
	पूर्णाङ्क ret = SBI_FIFO_UNCHANGED;
	शून्य *प्रविष्टिः_2;

	यदि (!fifo || !अन्तर्)
		प्रत्यागम ret;

	spin_तालकम्(&fifo->qlock);

	यदि (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		प्रत्यागम ret;
	}

	प्रति (i = 0; i < fifo->avail; i++) {
		सूचकांक_2 = fifo->पुच्छ + i;
		यदि (सूचकांक_2 >= fifo->अङ्कः_entries)
			सूचकांक_2 -= fifo->अङ्कः_entries;
		प्रविष्टिः_2 = (वर्ण *)fifo->queue + (u32)सूचकांक_2 * fifo->प्रविष्टिः_आकार;
		ret = fptr(अन्तर्, प्रविष्टिः_2);

		यदि (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			भङ्ग;
		}
	}
	spin_unlock(&fifo->qlock);

	प्रत्यागम ret;
}

पूर्णाङ्क sbi_fifo_पङ्क्तौ_निक्षिप(संरचना sbi_fifo *fifo, शून्य *दत्त, bool force)
{
	यदि (!fifo || !दत्त)
		प्रत्यागम SBI_EINVAL;

	spin_तालकम्(&fifo->qlock);

	यदि (__sbi_fifo_is_full(fifo)) {
		यदि (!force) {
			spin_unlock(&fifo->qlock);
			प्रत्यागम SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, रिक्त);
	}

	__sbi_fifo_enqueue(fifo, दत्त);

	spin_unlock(&fifo->qlock);

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_fifo_पङ्क्तेः_निष्कासय(संरचना sbi_fifo *fifo, शून्य *दत्त)
{
	यदि (!fifo || !दत्त)
		प्रत्यागम SBI_EINVAL;

	spin_तालकम्(&fifo->qlock);

	यदि (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		प्रत्यागम SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, दत्त);

	spin_unlock(&fifo->qlock);

	प्रत्यागम 0;
}
