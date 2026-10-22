/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra<atish.patra@wdc.com>
 *
 */
#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_fifo.h>
#అన్తర్భావ <sbi/sbi_string.h>

శూన్య sbi_fifo_init(నిర్మాణం sbi_fifo *fifo, శూన్య *వరుస_mem, u16 entries,
		   u16 నమోదు_కొలత) NO_THREAD_SAFETY_ANALYSIS
{
	fifo->వరుస	  = వరుస_mem;
	fifo->అంకె_entries = entries;
	fifo->నమోదు_కొలత  = నమోదు_కొలత;
	SPIN_తాళం_INIT(fifo->qlock);
	fifo->avail = fifo->తోక = 0;
	sbi_memset(fifo->వరుస, 0, (size_t)entries * నమోదు_కొలత);
}

/* Note: must be called with fifo->qlock held */
స్థిర అంతఃవరుస bool __sbi_fifo_is_full(నిర్మాణం sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	తిరిగి (fifo->avail == fifo->అంకె_entries) ? true : false;
}

u16 sbi_fifo_avail(నిర్మాణం sbi_fifo *fifo)
{
	u16 ret;

	ఒకవేళ (!fifo)
		తిరిగి 0;

	spin_తాళం(&fifo->qlock);
	ret = fifo->avail;
	spin_unlock(&fifo->qlock);

	తిరిగి ret;
}

పూర్ణాంకం sbi_fifo_is_నిండిన(నిర్మాణం sbi_fifo *fifo)
{
	bool ret;

	ఒకవేళ (!fifo)
		తిరిగి SBI_EINVAL;

	spin_తాళం(&fifo->qlock);
	ret = __sbi_fifo_is_full(fifo);
	spin_unlock(&fifo->qlock);

	తిరిగి ret;
}

/* Note: must be called with fifo->qlock held */
స్థిర అంతఃవరుస శూన్య __sbi_fifo_enqueue(నిర్మాణం sbi_fifo *fifo, శూన్య *దత్తాంశం)
	MUST_HOLD(&fifo->qlock)
{
	u32 తల;

	తల = (u32)fifo->తోక + fifo->avail;
	ఒకవేళ (తల >= fifo->అంకె_entries)
		తల = తల - fifo->అంకె_entries;

	ఎంపిక (fifo->నమోదు_కొలత) {
	స్థితి 1:
		*(అక్షరం *)(fifo->వరుస + తల * fifo->నమోదు_కొలత) = *(అక్షరం *)దత్తాంశం;
		ఆపు;
	స్థితి 2:
		*(u16 *)(fifo->వరుస + తల * fifo->నమోదు_కొలత) = *(u16 *)దత్తాంశం;
		ఆపు;
	స్థితి 4:
		*(u32 *)(fifo->వరుస + తల * fifo->నమోదు_కొలత) = *(u32 *)దత్తాంశం;
		ఆపు;
#ఒకవేళ __riscv_xlen > 32
	స్థితి 8:
		*(u64 *)(fifo->వరుస + తల * fifo->నమోదు_కొలత) = *(u64 *)దత్తాంశం;
		ఆపు;
#అయితే_ముగింపు
	అప్రమేయ:
		sbi_memcpy(fifo->వరుస + తల * fifo->నమోదు_కొలత,
			   దత్తాంశం, fifo->నమోదు_కొలత);
		ఆపు;
	}

	fifo->avail++;
}

/* Note: must be called with fifo->qlock held */
స్థిర అంతఃవరుస శూన్య __sbi_fifo_dequeue(నిర్మాణం sbi_fifo *fifo, శూన్య *దత్తాంశం)
	MUST_HOLD(&fifo->qlock)
{
	ఒకవేళ (!దత్తాంశం)
		వెళ్ళు skip_దత్తాంశం_copy;

	ఎంపిక (fifo->నమోదు_కొలత) {
	స్థితి 1:
		*(అక్షరం *)దత్తాంశం = *(అక్షరం *)(fifo->వరుస + (u32)fifo->తోక * fifo->నమోదు_కొలత);
		ఆపు;
	స్థితి 2:
		*(u16 *)దత్తాంశం = *(u16 *)(fifo->వరుస + (u32)fifo->తోక * fifo->నమోదు_కొలత);
		ఆపు;
	స్థితి 4:
		*(u32 *)దత్తాంశం = *(u32 *)(fifo->వరుస + (u32)fifo->తోక * fifo->నమోదు_కొలత);
		ఆపు;
#ఒకవేళ __riscv_xlen > 32
	స్థితి 8:
		*(u64 *)దత్తాంశం = *(u64 *)(fifo->వరుస + (u32)fifo->తోక * fifo->నమోదు_కొలత);
		ఆపు;
#అయితే_ముగింపు
	అప్రమేయ:
		sbi_memcpy(దత్తాంశం, fifo->వరుస + (u32)fifo->తోక * fifo->నమోదు_కొలత,
			   fifo->నమోదు_కొలత);
		ఆపు;
	}

skip_దత్తాంశం_copy:
	fifo->avail--;
	fifo->తోక++;
	ఒకవేళ (fifo->తోక >= fifo->అంకె_entries)
		fifo->తోక = 0;
}

/* Note: must be called with fifo->qlock held */
స్థిర అంతఃవరుస bool __sbi_fifo_is_empty(నిర్మాణం sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	తిరిగి (fifo->avail == 0) ? true : false;
}

పూర్ణాంకం sbi_fifo_is_ఖాళీలు(నిర్మాణం sbi_fifo *fifo)
{
	bool ret;

	ఒకవేళ (!fifo)
		తిరిగి SBI_EINVAL;

	spin_తాళం(&fifo->qlock);
	ret = __sbi_fifo_is_empty(fifo);
	spin_unlock(&fifo->qlock);

	తిరిగి ret;
}

/* Note: must be called with fifo->qlock held */
స్థిర అంతఃవరుస శూన్య __sbi_fifo_reset(నిర్మాణం sbi_fifo *fifo)
	MUST_HOLD(&fifo->qlock)
{
	size_t కొలత = (size_t)fifo->అంకె_entries * fifo->నమోదు_కొలత;

	fifo->avail = 0;
	fifo->తోక  = 0;
	sbi_memset(fifo->వరుస, 0, కొలత);
}

bool sbi_fifo_reset(నిర్మాణం sbi_fifo *fifo)
{
	ఒకవేళ (!fifo)
		తిరిగి false;

	spin_తాళం(&fifo->qlock);
	__sbi_fifo_reset(fifo);
	spin_unlock(&fifo->qlock);

	తిరిగి true;
}

/**
 * Provide a helper function to do inplace update to the fifo.
 * Note: The callback function is called with lock being held.
 *
 * **Do not** invoke any other fifo function from callback. Otherwise, it will
 * lead to deadlock.
 */
పూర్ణాంకం sbi_fifo_inplace_update(నిర్మాణం sbi_fifo *fifo, శూన్య *లోపల,
			    పూర్ణాంకం (*fptr)(శూన్య *లోపల, శూన్య *దత్తాంశం))
{
	పూర్ణాంకం i, సూచీ_2;
	పూర్ణాంకం ret = SBI_FIFO_UNCHANGED;
	శూన్య *నమోదు_3;

	ఒకవేళ (!fifo || !లోపల)
		తిరిగి ret;

	spin_తాళం(&fifo->qlock);

	ఒకవేళ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		తిరిగి ret;
	}

	కొరకు (i = 0; i < fifo->avail; i++) {
		సూచీ_2 = fifo->తోక + i;
		ఒకవేళ (సూచీ_2 >= fifo->అంకె_entries)
			సూచీ_2 -= fifo->అంకె_entries;
		నమోదు_3 = (అక్షరం *)fifo->వరుస + (u32)సూచీ_2 * fifo->నమోదు_కొలత;
		ret = fptr(లోపల, నమోదు_3);

		ఒకవేళ (ret == SBI_FIFO_SKIP || ret == SBI_FIFO_UPDATED) {
			ఆపు;
		}
	}
	spin_unlock(&fifo->qlock);

	తిరిగి ret;
}

పూర్ణాంకం sbi_fifo_వరుసలో_చేర్చు(నిర్మాణం sbi_fifo *fifo, శూన్య *దత్తాంశం, bool force)
{
	ఒకవేళ (!fifo || !దత్తాంశం)
		తిరిగి SBI_EINVAL;

	spin_తాళం(&fifo->qlock);

	ఒకవేళ (__sbi_fifo_is_full(fifo)) {
		ఒకవేళ (!force) {
			spin_unlock(&fifo->qlock);
			తిరిగి SBI_ENOSPC;
		}
		__sbi_fifo_dequeue(fifo, ఖాళీ);
	}

	__sbi_fifo_enqueue(fifo, దత్తాంశం);

	spin_unlock(&fifo->qlock);

	తిరిగి 0;
}

పూర్ణాంకం sbi_fifo_వరుస_నుండి_తీయి(నిర్మాణం sbi_fifo *fifo, శూన్య *దత్తాంశం)
{
	ఒకవేళ (!fifo || !దత్తాంశం)
		తిరిగి SBI_EINVAL;

	spin_తాళం(&fifo->qlock);

	ఒకవేళ (__sbi_fifo_is_empty(fifo)) {
		spin_unlock(&fifo->qlock);
		తిరిగి SBI_ENOENT;
	}

	__sbi_fifo_dequeue(fifo, దత్తాంశం);

	spin_unlock(&fifo->qlock);

	తిరిగి 0;
}
