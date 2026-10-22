/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_fifo.h>
#समावेश <sbi/sbi_string.h>

व्योम sbi_fifo_init(काष्ठा sbi_fifo *fifo, व्योम *queue_mem, u16 entries,
		   u16 entry_आकार) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->queue	  = queue_mem;
	fifo->अंक_entries = entries;
	fifo->entry_आकार  = entry_आकार;
	SPIN_LOCK_INIT(fifo->qlock);
	fifo->avail = fifo->पुच्छ = 0;
	sbi_memset(fifo->queue, 0, (माप_प्रकार)entries * entry_आकार);
}

/* Note: must be called with fifo->qlock held */
अटल अंतरभूत bool __sbi_fifo_is_full(काष्ठा sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	वापस (fifo->avail == fifo->अंक_entries) ? true : false;
}

u16 sbi_fifo_avail(काष्ठा sbi_fifo *fifo)
{
	u16 ret;

	अगर (!fifo)
		वापस 0;

	spin_lock(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	वापस ret;
}

पूर्णांक sbi_fifo_is_full(काष्ठा sbi_fifo *fifo)
{
	bool ret;

	अगर (!fifo)
		वापस SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	वापस ret;
}

/* Note: must be called with fifo->qlock held */
अटल अंतरभूत व्योम __sbi_fifo_enqueue(काष्ठा sbi_fifo *fifo, व्योम *तथ्य)
	MUST_HOLD(&fifo->qlock)
{
	u32 शीर्ष;

	शीर्ष = (u32)fifo->पुच्छ + fifo->avail;
	अगर (शीर्ष >= fifo->अंक_entries)
		शीर्ष = शीर्ष - fifo->अंक_entries;

	चयन (fifo->entry_आकार) {
	हाल 1:
		*(अक्षर *)(fifo->queue + शीर्ष * fifo->entry_आकार) = *(अक्षर *)तथ्य;
		अवरोध;
	हाल 2:
		*(u16 *)(fifo->queue + शीर्ष * fifo->entry_आकार) = *(u16 *)तथ्य;
		अवरोध;
	हाल 4:
		*(u32 *)(fifo->queue + शीर्ष * fifo->entry_आकार) = *(u32 *)तथ्य;
		अवरोध;
#अगर __riscv_xlen > 32
	हाल 8:
		*(u64 *)(fifo->queue + शीर्ष * fifo->entry_आकार) = *(u64 *)तथ्य;
		अवरोध;
#पूर्ण_अगर
	शेष:
		sbi_memcpy(fifo->queue + शीर्ष * fifo->entry_आकार,
			   तथ्य, fifo->entry_आकार);
		अवरोध;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
अटल अंतरभूत व्योम __sbi_fifo_dequeue(काष्ठा sbi_fifo *fifo, व्योम *तथ्य)
	MUST_HOLD(&fifo->qlock)
{
	अगर (!तथ्य)
		जाओ skip_तथ्य_copy;

	चयन (fifo->entry_आकार) {
	हाल 1:
		*(अक्षर *)तथ्य = *(अक्षर *)(fifo->queue + (u32)fifo->पुच्छ * fifo->entry_आकार);
		अवरोध;
	हाल 2:
		*(u16 *)तथ्य = *(u16 *)(fifo->queue + (u32)fifo->पुच्छ * fifo->entry_आकार);
		अवरोध;
	हाल 4:
		*(u32 *)तथ्य = *(u32 *)(fifo->queue + (u32)fifo->पुच्छ * fifo->entry_आकार);
		अवरोध;
#अगर __riscv_xlen > 32
	हाल 8:
		*(u64 *)तथ्य = *(u64 *)(fifo->queue + (u32)fifo->पुच्छ * fifo->entry_आकार);
		अवरोध;
#पूर्ण_अगर
	शेष:
		sbi_memcpy(तथ्य, fifo->queue + (u32)fifo->पुच्छ * fifo->entry_आकार,
			   fifo->entry_आकार);
		अवरोध;
	}

skip_तथ्य_copy:
	fifo->avail--;
	fifo->पुच्छ++;
	अगर (fifo->पुच्छ >= fifo->अंक_entries)
		fifo->पुच्छ = 0;
}

/* Note: must be called with fifo->qlock held */
अटल अंतरभूत bool __sbi_fifo_is_empty(काष्ठा sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	वापस (fifo->avail == 0) ? true : false;
}

पूर्णांक sbi_fifo_is_empty(काष्ठा sbi_fifo *fifo)
{
	bool ret;

	अगर (!fifo)
		वापस SBI_EINVAL;

	spin_lock(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	वापस ret;
}

/* Note: must be called with fifo->qlock held */
अटल अंतरभूत व्योम __sbi_fifo_reset(काष्ठा sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	माप_प्रकार आकार = (माप_प्रकार)fifo->अंक_entries * fifo->entry_आकार;

	fifo->avail = 0;
	fifo->पुच्छ  = 0;
	sbi_memset(fifo->queue, 0, आकार);
}

bool sbi_fifo_reset(काष्ठा sbi_fifo *fifo)
{
	अगर (!fifo)
		वापस false;

	spin_lock(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	वापस true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
पूर्णांक sbi_fifo_inplace_update(काष्ठा sbi_fifo *fifo, व्योम *in,
			    पूर्णांक (*fptr)(व्योम *in, व्योम *तथ्य))
{
	पूर्णांक i, सूचकांक_2;
	पूर्णांक ret = SBI_FIFO_UNCHANGED;
	व्योम *entry;

	अगर (!fifo || !in)
		वापस ret;

	spin_lock(&fifo->qlock);

	अगर (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		वापस ret;
	}

	क्रम (i = 0; i < fifo->avail; i++) {
		सूचकांक_2 = fifo->पुच्छ + i;
		अगर (सूचकांक_2 >= fifo->अंक_entries)
			सूचकांक_2 -= fifo->अंक_entries;
		entry = (अक्षर *)fifo->queue + (u32)सूचकांक_2 * fifo->entry_आकार;
		ret = fptr(in, entry);

		अगर (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			अवरोध;
		}
	}
	spin_unlock(&fifo->qlock);

	वापस ret;
}

पूर्णांक sbi_fifo_enqueue(काष्ठा sbi_fifo *fifo, व्योम *तथ्य, bool force)
{
	अगर (!fifo || !तथ्य)
		वापस SBI_EINVAL;

	spin_lock(&fifo->qlock);

	अगर (__sbi_fifo_is_full(fifo)) {
		अगर (!force) {
			spin_unlock(&fifo->qlock);
			वापस SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, शून्य);
	}

	__sbi_fifo_enqueue(fifo, तथ्य);

	spin_unlock(&fifo->qlock);

	वापस 0;
}

पूर्णांक sbi_fifo_dequeue(काष्ठा sbi_fifo *fifo, व्योम *तथ्य)
{
	अगर (!fifo || !तथ्य)
		वापस SBI_EINVAL;

	spin_lock(&fifo->qlock);

	अगर (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		वापस SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, तथ्य);

	spin_unlock(&fifo->qlock);

	वापस 0;
}
