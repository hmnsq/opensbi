/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool spin_ಬೀಗ_unlocked(spinlock_t ಬೀಗ_2)
{
	ಹಿಂತಿರುಗಿ ಬೀಗ_2.owner == ಬೀಗ_2.ಮುಂದಿನ;
}

bool spin_ಬೀಗ_check(spinlock_t *ಬೀಗ_2)
{
	RISCV_FENCE(r, rw);
	ಹಿಂತಿರುಗಿ !spin_ಬೀಗ_unlocked(*ಬೀಗ_2);
}

bool spin_trylock(spinlock_t *ಬೀಗ_2)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ inc = 1u << TICKET_ಸರಿಸು;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = 0xffffu << TICKET_ಸರಿಸು;
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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*ಬೀಗ_2)
		: "r"(inc), "r"(ಮುಖವಾಡ), "I"(TICKET_ಸರಿಸು)
		: "memory");

	ಹಿಂತಿರುಗಿ l0 == 0;
}

ಶೂನ್ಯ spin_ಬೀಗ(spinlock_t *ಬೀಗ_2) NO_THREAD_SAFETY_ANALYSIS
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ inc = 1u << TICKET_ಸರಿಸು;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = 0xffffu;
	u32 l0, tmp1, tmp2;

	__asm__ __volatile__(
		/* Atomically increment the next ticket. */
#ಆದರೆ defined(__riscv_atomic) || defined(__riscv_zaamo)
		"	amoadd.w.aqrl	%0, %4, %3\n"
#ಅಥವಾಆದರೆ defined(__riscv_zalrsc)
		"3:	lr.w.aqrl	%0, %3\n"
#ಆದರೆ __riscv_xlen == 64
		"	addw	%1, %0, %4\n"
#ಅಥವಾಆದರೆ __riscv_xlen == 32
		"	add	%1, %0, %4\n"
#ಆದರೆ_ಅಂತ್ಯ
		"	sc.w.aqrl	%1, %1, %3\n"
		"	bnez	%1, 3b\n"
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "need A or Zaamo or Zalrsc"
#ಆದರೆ_ಅಂತ್ಯ

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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*ಬೀಗ_2)
		: "r"(inc), "r"(ಮುಖವಾಡ), "I"(TICKET_ಸರಿಸು)
		: "memory");
}

ಶೂನ್ಯ spin_unlock(spinlock_t *ಬೀಗ_2) NO_THREAD_SAFETY_ANALYSIS
{
	__smp_store_release(&ಬೀಗ_2->owner, ಬೀಗ_2->owner + 1);
}
