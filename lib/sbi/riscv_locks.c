/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_locks.h>

ساكن مضمن bool spin_تالو_unlocked(spinlock_t تالو)
{
	أرجع تالو.owner == تالو.ايندڙ;
}

bool spin_تالو_check(spinlock_t *تالو)
{
	RISCV_FENCE(r, rw);
	أرجع !spin_تالو_unlocked(*تالو);
}

bool spin_trylock(spinlock_t *تالو)
{
	بنانشان ڊگهو inc = 1u << TICKET_سرڪايو;
	بنانشان ڊگهو نقاب = 0xffffu << TICKET_سرڪايو;
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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*تالو)
		: "r"(inc), "r"(نقاب), "I"(TICKET_سرڪايو)
		: "memory");

	أرجع l0 == 0;
}

فراغ spin_تالو(spinlock_t *تالو) NO_THREAD_SAFETY_ANALYSIS
{
	بنانشان ڊگهو inc = 1u << TICKET_سرڪايو;
	بنانشان ڊگهو نقاب = 0xffffu;
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
#جيڪڏهن_پورو
		"	sc.w.aqrl	%1, %1, %3\n"
		"	bnez	%1, 3b\n"
#وإلا
#غلطي "need A or Zaamo or Zalrsc"
#جيڪڏهن_پورو

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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*تالو)
		: "r"(inc), "r"(نقاب), "I"(TICKET_سرڪايو)
		: "memory");
}

فراغ spin_unlock(spinlock_t *تالو) NO_THREAD_SAFETY_ANALYSIS
{
	__smp_store_release(&تالو->owner, تالو->owner + 1);
}
