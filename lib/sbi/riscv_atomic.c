/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_atomic.h>
#অন্তর্ভাব <sbi/riscv_barrier.h>

#যদি !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	প্রত্যাগম ret;
}

শূন্য atomic_write(atomic_t *atom, long মান)
{
	atom->counter = মান;
	wmb();
}

long atomic_add_return(atomic_t *atom, long মান)
{
#যদি defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#যদি __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(মান)
			     : "memory");
#নোচেদ্যদি __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(মান)
			     : "memory");
#endif
#নোচেদ্যদি defined(__riscv_zalrsc)
	long ret, temp;
#যদি __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(মান)
			     : "memory");
#নোচেদ্যদি __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(মান)
			     : "memory");
#endif
#অন্যথা
#error "need A or Zaamo or Zalrsc"
#endif

	প্রত্যাগম ret + মান;
}

long atomic_sub_return(atomic_t *atom, long মান)
{
	প্রত্যাগম atomic_add_return(atom, -মান);
}

#যদি defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		বিকল্প (size) {							\
		পক্ষ 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ভঙ্গ;							\
		পক্ষ 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ভঙ্গ;							\
		স্বভাব:							\
			ভঙ্গ;							\
		}								\
		__ret;								\
	})
#নোচেদ্যদি defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		বিকল্প (size) {							\
		পক্ষ 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ভঙ্গ;							\
		পক্ষ 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ভঙ্গ;							\
		স্বভাব:							\
			ভঙ্গ;							\
		}								\
		__ret;								\
	})
#অন্যথা
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, পরিমাণ(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	প্রত্যাগম __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	প্রত্যাগম axchg(&atom->counter, newval);
}

unsigned পূর্ণাঙ্ক atomic_raw_xchg_uint(চঞ্চল unsigned পূর্ণাঙ্ক *ptr,
				  unsigned পূর্ণাঙ্ক newval)
{
	/* Atomically set new value and return old value. */
	প্রত্যাগম axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(চঞ্চল unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	প্রত্যাগম axchg(ptr, newval);
}

পূর্ণাঙ্ক atomic_raw_set_bit(পূর্ণাঙ্ক nr, চঞ্চল unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	প্রত্যাগম res & mask ? 1 : 0;
}

পূর্ণাঙ্ক atomic_raw_clear_bit(পূর্ণাঙ্ক nr, চঞ্চল unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	প্রত্যাগম res & mask ? 1 : 0;
}

পূর্ণাঙ্ক atomic_set_bit(পূর্ণাঙ্ক nr, atomic_t *atom)
{
	প্রত্যাগম atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

পূর্ণাঙ্ক atomic_clear_bit(পূর্ণাঙ্ক nr, atomic_t *atom)
{
	প্রত্যাগম atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
