/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#شامل <sbi/riscv_barrier.h>
#شامل <sbi/riscv_locks.h>

ساکن اندرونی bool spin_تالا_unlocked(spinlock_t تالا_2)
{
	واپس تالا_2.owner == تالا_2.اگلا;
}

bool spin_تالا_check(spinlock_t *تالا_2)
{
	RISCV_FENCE(r, rw);
	واپس !spin_تالا_unlocked(*تالا_2);
}

bool spin_trylock(spinlock_t *تالا_2)
{
	غیرعلامتی طویل inc = 1u << TICKET_سرکاؤ;
	غیرعلامتی طویل نقاب = 0xffffu << TICKET_سرکاؤ;
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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*تالا_2)
		: "r"(inc), "r"(نقاب), "I"(TICKET_سرکاؤ)
		: "memory");

	واپس l0 == 0;
}

خالی spin_تالا(spinlock_t *تالا_2) NO_THREAD_SAFETY_ANALYSIS
{
	غیرعلامتی طویل inc = 1u << TICKET_سرکاؤ;
	غیرعلامتی طویل نقاب = 0xffffu;
	u32 l0, tmp1, tmp2;

	__asm__ __volatile__(
		/* Atomically increment the next ticket. */
#اگر defined(__riscv_atomic) || defined(__riscv_zaamo)
		"	amoadd.w.aqrl	%0, %4, %3\n"
#وگرنہ_اگر defined(__riscv_zalrsc)
		"3:	lr.w.aqrl	%0, %3\n"
#اگر __riscv_xlen == 64
		"	addw	%1, %0, %4\n"
#وگرنہ_اگر __riscv_xlen == 32
		"	add	%1, %0, %4\n"
#اگر_ختم
		"	sc.w.aqrl	%1, %1, %3\n"
		"	bnez	%1, 3b\n"
#ورنہ
#غلطی "need A or Zaamo or Zalrsc"
#اگر_ختم

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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*تالا_2)
		: "r"(inc), "r"(نقاب), "I"(TICKET_سرکاؤ)
		: "memory");
}

خالی spin_unlock(spinlock_t *تالا_2) NO_THREAD_SAFETY_ANALYSIS
{
	__smp_store_release(&تالا_2->owner, تالا_2->owner + 1);
}
