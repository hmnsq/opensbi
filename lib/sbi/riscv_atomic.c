/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_atomic.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>

#જો !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	પાછા ret;
}

શૂન્ય atomic_write(atomic_t *atom, long કિંમત)
{
	atom->counter = કિંમત;
	wmb();
}

long atomic_add_return(atomic_t *atom, long કિંમત)
{
#જો defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#જો __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(કિંમત)
			     : "memory");
#અથવાજો __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(કિંમત)
			     : "memory");
#endif
#અથવાજો defined(__riscv_zalrsc)
	long ret, temp;
#જો __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(કિંમત)
			     : "memory");
#અથવાજો __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(કિંમત)
			     : "memory");
#endif
#નહીંતર
#error "need A or Zaamo or Zalrsc"
#endif

	પાછા ret + કિંમત;
}

long atomic_sub_return(atomic_t *atom, long કિંમત)
{
	પાછા atomic_add_return(atom, -કિંમત);
}

#જો defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		પસંદગી (size) {							\
		સ્થિતિ 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			તોડો;							\
		સ્થિતિ 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			તોડો;							\
		મૂળભૂત:							\
			તોડો;							\
		}								\
		__ret;								\
	})
#અથવાજો defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		પસંદગી (size) {							\
		સ્થિતિ 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			તોડો;							\
		સ્થિતિ 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			તોડો;							\
		મૂળભૂત:							\
			તોડો;							\
		}								\
		__ret;								\
	})
#નહીંતર
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, કદ(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	પાછા __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	પાછા axchg(&atom->counter, newval);
}

unsigned પૂર્ણાંક atomic_raw_xchg_uint(અસ્થિર unsigned પૂર્ણાંક *ptr,
				  unsigned પૂર્ણાંક newval)
{
	/* Atomically set new value and return old value. */
	પાછા axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(અસ્થિર unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	પાછા axchg(ptr, newval);
}

પૂર્ણાંક atomic_raw_set_bit(પૂર્ણાંક nr, અસ્થિર unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	પાછા res & mask ? 1 : 0;
}

પૂર્ણાંક atomic_raw_clear_bit(પૂર્ણાંક nr, અસ્થિર unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	પાછા res & mask ? 1 : 0;
}

પૂર્ણાંક atomic_set_bit(પૂર્ણાંક nr, atomic_t *atom)
{
	પાછા atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

પૂર્ણાંક atomic_clear_bit(પૂર્ણાંક nr, atomic_t *atom)
{
	પાછા atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
