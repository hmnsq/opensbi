/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_locks.h>

ساكن مضمن bool spin_قفل_unlocked(spinlock_t قفل_2)
{
	أرجع قفل_2.owner == قفل_2.بعدی;
}

bool spin_قفل_check(spinlock_t *قفل_2)
{
	RISCV_FENCE(r, rw);
	أرجع !spin_قفل_unlocked(*قفل_2);
}

bool spin_trylock(spinlock_t *قفل_2)
{
	بیعلامت دراز inc = 1u << TICKET_لغزش;
	بیعلامت دراز ماسک = 0xffffu << TICKET_لغزش;
	u32 l0, tmp1, tmp2;

	__asm__ __volatile__(
		/* Get the current lock counters. */
		"1:	lr.w.aq	%0, %3\n"
		"	slli	%2, %0, %6\n"
		"	and	%2, %2, %5\n"
		"	and	%1, %0, %5\n"
		/* Is the lock free right now? */
		"	bne	%1, %2, 2f\n"
		"	add	%0, %0, %4\n"
		/* Acquire the lock. */
		"	sc.w.rl	%0, %0, %3\n"
		"	bnez	%0, 1b\n"
		"2:"
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*قفل_2)
		: "r"(inc), "r"(ماسک), "I"(TICKET_لغزش)
		: "memory");

	أرجع l0 == 0;
}

فراغ spin_قفل(spinlock_t *قفل_2) NO_THREAD_SAFETY_ANALYSIS
{
	بیعلامت دراز inc = 1u << TICKET_لغزش;
	بیعلامت دراز ماسک = 0xffffu;
	u32 l0, tmp1, tmp2;

	__asm__ __volatile__(
		/* Atomically increment the next ticket. */
#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
		"	amoadd.w.aqrl	%0, %4, %3\n"
#وإلاإذا defined(__riscv_zalrsc)
		"3:	lr.w.aqrl	%0, %3\n"
#إذا __riscv_xlen == 64
		"	addw	%1, %0, %4\n"
#وإلاإذا __riscv_xlen == 32
		"	add	%1, %0, %4\n"
#پایان_اگر
		"	sc.w.aqrl	%1, %1, %3\n"
		"	bnez	%1, 3b\n"
#وإلا
#خطا "need A or Zaamo or Zalrsc"
#پایان_اگر

		/* Did we get the lock? */
		"	srli	%1, %0, %6\n"
		"	and	%1, %1, %5\n"
		"1:	and	%2, %0, %5\n"
		"	beq	%1, %2, 2f\n"

		/* If not, then spin on the lock. */
		"	lw	%0, %3\n"
		RISCV_ACQUIRE_BARRIER
		"	j	1b\n"
		"2:"
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*قفل_2)
		: "r"(inc), "r"(ماسک), "I"(TICKET_لغزش)
		: "memory");
}

فراغ spin_unlock(spinlock_t *قفل_2) NO_THREAD_SAFETY_ANALYSIS
{
	__smp_store_release(&قفل_2->owner, قفل_2->owner + 1);
}
