/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_locks.h>

ساكن مضمن bool spin_قلف_unlocked(spinlock_t قلف)
{
	أرجع قلف.owner == قلف.بل;
}

bool spin_قلف_check(spinlock_t *قلف)
{
	RISCV_FENCE(r, rw);
	أرجع !spin_قلف_unlocked(*قلف);
}

bool spin_trylock(spinlock_t *قلف)
{
	بینښه اوږد inc = 1u << TICKET_ښويدنه;
	بینښه اوږد ماسک = 0xffffu << TICKET_ښويدنه;
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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*قلف)
		: "r"(inc), "r"(ماسک), "I"(TICKET_ښويدنه)
		: "memory");

	أرجع l0 == 0;
}

فراغ spin_قلف(spinlock_t *قلف) NO_THREAD_SAFETY_ANALYSIS
{
	بینښه اوږد inc = 1u << TICKET_ښويدنه;
	بینښه اوږد ماسک = 0xffffu;
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
#د_که_پای
		"	sc.w.aqrl	%1, %1, %3\n"
		"	bnez	%1, 3b\n"
#وإلا
#تېروتنه "need A or Zaamo or Zalrsc"
#د_که_پای

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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*قلف)
		: "r"(inc), "r"(ماسک), "I"(TICKET_ښويدنه)
		: "memory");
}

فراغ spin_unlock(spinlock_t *قلف) NO_THREAD_SAFETY_ANALYSIS
{
	__smp_store_release(&قلف->owner, قلف->owner + 1);
}
