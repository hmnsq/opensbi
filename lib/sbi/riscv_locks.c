/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/riscv_locks.h>

స్థిర అంతఃవరుస bool spin_తాళం_unlocked(spinlock_t తాళం_2)
{
	తిరిగి తాళం_2.owner == తాళం_2.తదుపరి;
}

bool spin_తాళం_check(spinlock_t *తాళం_2)
{
	RISCV_FENCE(r, rw);
	తిరిగి !spin_తాళం_unlocked(*తాళం_2);
}

bool spin_trylock(spinlock_t *తాళం_2)
{
	గుర్తులేని దీర్ఘ inc = 1u << TICKET_జరుపు;
	గుర్తులేని దీర్ఘ ముసుగు = 0xffffu << TICKET_జరుపు;
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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*తాళం_2)
		: "r"(inc), "r"(ముసుగు), "I"(TICKET_జరుపు)
		: "memory");

	తిరిగి l0 == 0;
}

శూన్య spin_తాళం(spinlock_t *తాళం_2) NO_THREAD_SAFETY_ANALYSIS
{
	గుర్తులేని దీర్ఘ inc = 1u << TICKET_జరుపు;
	గుర్తులేని దీర్ఘ ముసుగు = 0xffffu;
	u32 l0, tmp1, tmp2;

	__asm__ __volatile__(
		/* Atomically increment the next ticket. */
#ఒకవేళ defined(__riscv_atomic) || defined(__riscv_zaamo)
		"	amoadd.w.aqrl	%0, %4, %3\n"
#లేదావేళ defined(__riscv_zalrsc)
		"3:	lr.w.aqrl	%0, %3\n"
#ఒకవేళ __riscv_xlen == 64
		"	addw	%1, %0, %4\n"
#లేదావేళ __riscv_xlen == 32
		"	add	%1, %0, %4\n"
#అయితే_ముగింపు
		"	sc.w.aqrl	%1, %1, %3\n"
		"	bnez	%1, 3b\n"
#లేకపోతే
#దోషం "need A or Zaamo or Zalrsc"
#అయితే_ముగింపు

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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*తాళం_2)
		: "r"(inc), "r"(ముసుగు), "I"(TICKET_జరుపు)
		: "memory");
}

శూన్య spin_unlock(spinlock_t *తాళం_2) NO_THREAD_SAFETY_ANALYSIS
{
	__smp_store_release(&తాళం_2->owner, తాళం_2->owner + 1);
}
