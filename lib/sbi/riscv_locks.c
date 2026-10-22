/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Christoph Müllner <cmuellner@linux.com>
 */

#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/riscv_locks.h>

स्थिर अन्तःस्थ bool spin_तालकम्_unlocked(spinlock_t तालकम्)
{
	प्रत्यागम तालकम्.owner == तालकम्.अग्र;
}

bool spin_तालकम्_check(spinlock_t *तालकम्)
{
	RISCV_FENCE(r, rw);
	प्रत्यागम !spin_तालकम्_unlocked(*तालकम्);
}

bool spin_trylock(spinlock_t *तालकम्)
{
	अचिह्नित दीर्घ inc = 1u << TICKET_सरणम्;
	अचिह्नित दीर्घ आवरणम् = 0xffffu << TICKET_सरणम्;
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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*तालकम्)
		: "r"(inc), "r"(आवरणम्), "I"(TICKET_सरणम्)
		: "memory");

	प्रत्यागम l0 == 0;
}

शून्य spin_तालकम्(spinlock_t *तालकम्) NO_THREAD_SAFETY_ANALYSIS
{
	अचिह्नित दीर्घ inc = 1u << TICKET_सरणम्;
	अचिह्नित दीर्घ आवरणम् = 0xffffu;
	u32 l0, tmp1, tmp2;

	__asm__ __volatile__(
		/* Atomically increment the next ticket. */
#यदि defined(__riscv_atomic) || defined(__riscv_zaamo)
		"	amoadd.w.aqrl	%0, %4, %3\n"
#नोचेद्यदि defined(__riscv_zalrsc)
		"3:	lr.w.aqrl	%0, %3\n"
#यदि __riscv_xlen == 64
		"	addw	%1, %0, %4\n"
#नोचेद्यदि __riscv_xlen == 32
		"	add	%1, %0, %4\n"
#यदि_अन्तः
		"	sc.w.aqrl	%1, %1, %3\n"
		"	bnez	%1, 3b\n"
#अन्यथा
#दोषः "need A or Zaamo or Zalrsc"
#यदि_अन्तः

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
		: "=&r"(l0), "=&r"(tmp1), "=&r"(tmp2), "+A"(*तालकम्)
		: "r"(inc), "r"(आवरणम्), "I"(TICKET_सरणम्)
		: "memory");
}

शून्य spin_unlock(spinlock_t *तालकम्) NO_THREAD_SAFETY_ANALYSIS
{
	__smp_store_release(&तालकम्->owner, तालकम्->owner + 1);
}
