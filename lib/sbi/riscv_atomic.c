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
	long ret = atom->গণক;
	rmb();
	ফেরত ret;
}

শূন্য atomic_write(atomic_t *atom, long মান)
{
	atom->গণক = মান;
	wmb();
}

long atomic_add_return(atomic_t *atom, long মান)
{
#যদি defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#যদি __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->গণক), "=r"(ret)
			     : "r"(মান)
			     : "memory");
#নতুবা_যদি __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->গণক), "=r"(ret)
			     : "r"(মান)
			     : "memory");
#endif
#নতুবা_যদি defined(__riscv_zalrsc)
	long ret, অস্থায়ী;
#যদি __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->গণক), "=&r"(ret), "=&r"(অস্থায়ী)
			     : "r"(মান)
			     : "memory");
#নতুবা_যদি __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->গণক), "=&r"(ret), "=&r"(অস্থায়ী)
			     : "r"(মান)
			     : "memory");
#endif
#অন্যথা
#error "need A or Zaamo or Zalrsc"
#endif

	ফেরত ret + মান;
}

long atomic_sub_return(atomic_t *atom, long মান)
{
	ফেরত atomic_add_return(atom, -মান);
}

#যদি defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, আকার_2)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		নির্বাচন (আকার_2) {							\
		ক্ষেত্র 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			বিরতি;							\
		ক্ষেত্র 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			বিরতি;							\
		পূর্বনির্ধারিত:							\
			বিরতি;							\
		}								\
		__ret;								\
	})
#নতুবা_যদি defined(__riscv_zalrsc)
#define __axchg(ptr, new, আকার_2)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		নির্বাচন (আকার_2) {							\
		ক্ষেত্র 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			বিরতি;							\
		ক্ষেত্র 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			বিরতি;							\
		পূর্বনির্ধারিত:							\
			বিরতি;							\
		}								\
		__ret;								\
	})
#অন্যথা
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, আকার(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	ফেরত __sync_val_compare_and_swap(&atom->গণক, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	ফেরত axchg(&atom->গণক, newval);
}

unsigned পূর্ণসংখ্যা atomic_raw_xchg_uint(অস্থির unsigned পূর্ণসংখ্যা *ptr,
				  unsigned পূর্ণসংখ্যা newval)
{
	/* Atomically set new value and return old value. */
	ফেরত axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(অস্থির unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	ফেরত axchg(ptr, newval);
}

পূর্ণসংখ্যা atomic_raw_set_bit(পূর্ণসংখ্যা nr, অস্থির unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	ফেরত res & mask ? 1 : 0;
}

পূর্ণসংখ্যা atomic_raw_clear_bit(পূর্ণসংখ্যা nr, অস্থির unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	ফেরত res & mask ? 1 : 0;
}

পূর্ণসংখ্যা atomic_set_bit(পূর্ণসংখ্যা nr, atomic_t *atom)
{
	ফেরত atomic_raw_set_bit(nr, (unsigned long *)&atom->গণক);
}

পূর্ণসংখ্যা atomic_clear_bit(পূর্ণসংখ্যা nr, atomic_t *atom)
{
	ফেরত atomic_raw_clear_bit(nr, (unsigned long *)&atom->গণক);
}
