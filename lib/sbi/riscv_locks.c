/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>

ਸਥਿਰ ਸਤਰਅੰਦਰ bool spin_ਤਾਲਾ_unlocked(spinlock_t ਤਾਲਾ)
{
	ਵਾਪਸ ਤਾਲਾ.owner == ਤਾਲਾ.ਅਗਲਾ;
}

bool spin_ਤਾਲਾ_check(spinlock_t *ਤਾਲਾ)
{
	RISCV_FENCE(r, rw);
	ਵਾਪਸ !spin_ਤਾਲਾ_unlocked(*ਤਾਲਾ);
}

bool spin_trylock(spinlock_t *ਤਾਲਾ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ inc = 1u << TICKET_ਖਿਸਕਾਓ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = 0xffffu << TICKET_ਖਿਸਕਾਓ;
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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*ਤਾਲਾ)
		: "r"(inc), "r"(ਨਕਾਬ), "I"(TICKET_ਖਿਸਕਾਓ)
		: "memory");

	ਵਾਪਸ l0 == 0;
}

ਖਾਲੀ spin_ਤਾਲਾ(spinlock_t *ਤਾਲਾ) NO_THREAD_SAFETY_ANALYSIS
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ inc = 1u << TICKET_ਖਿਸਕਾਓ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = 0xffffu;
	u32 l0, tmp1, tmp2;

	__asm__ __volatile__(
		/* Atomically increment the next ticket. */
#ਜੇ defined(__riscv_atomic) || defined(__riscv_zaamo)
		"	amoadd.w.aqrl	%0, %4, %3\n"
#ਜਾਂਜੇ defined(__riscv_zalrsc)
		"3:	lr.w.aqrl	%0, %3\n"
#ਜੇ __riscv_xlen == 64
		"	addw	%1, %0, %4\n"
#ਜਾਂਜੇ __riscv_xlen == 32
		"	add	%1, %0, %4\n"
#ਜੇ_ਅੰਤ
		"	sc.w.aqrl	%1, %1, %3\n"
		"	bnez	%1, 3b\n"
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "need A or Zaamo or Zalrsc"
#ਜੇ_ਅੰਤ

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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*ਤਾਲਾ)
		: "r"(inc), "r"(ਨਕਾਬ), "I"(TICKET_ਖਿਸਕਾਓ)
		: "memory");
}

ਖਾਲੀ spin_unlock(spinlock_t *ਤਾਲਾ) NO_THREAD_SAFETY_ANALYSIS
{
	__smp_store_release(&ਤਾਲਾ->owner, ਤਾਲਾ->owner + 1);
}
