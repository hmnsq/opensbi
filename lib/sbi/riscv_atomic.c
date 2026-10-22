/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_atomic.h>
#ضمن <sbi/riscv_barrier.h>

#إذا !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	أرجع ret;
}

فراغ atomic_write(atomic_t *atom, long قدر)
{
	atom->counter = قدر;
	wmb();
}

long atomic_add_return(atomic_t *atom, long قدر)
{
#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#إذا __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(قدر)
			     : "memory");
#وإلاإذا __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(قدر)
			     : "memory");
#endif
#وإلاإذا defined(__riscv_zalrsc)
	long ret, temp;
#إذا __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(قدر)
			     : "memory");
#وإلاإذا __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(قدر)
			     : "memory");
#endif
#وإلا
#error "need A or Zaamo or Zalrsc"
#endif

	أرجع ret + قدر;
}

long atomic_sub_return(atomic_t *atom, long قدر)
{
	أرجع atomic_add_return(atom, -قدر);
}

#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		بدل (size) {							\
		حالة 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			اكسر;							\
		حالة 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			اكسر;							\
		افتراضي:							\
			اكسر;							\
		}								\
		__ret;								\
	})
#وإلاإذا defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		بدل (size) {							\
		حالة 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			اكسر;							\
		حالة 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			اكسر;							\
		افتراضي:							\
			اكسر;							\
		}								\
		__ret;								\
	})
#وإلا
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, حجم(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	أرجع __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(&atom->counter, newval);
}

unsigned صحيح atomic_raw_xchg_uint(متطاير unsigned صحيح *ptr,
				  unsigned صحيح newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(متطاير unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

صحيح atomic_raw_set_bit(صحيح nr, متطاير unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	أرجع res & mask ? 1 : 0;
}

صحيح atomic_raw_clear_bit(صحيح nr, متطاير unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	أرجع res & mask ? 1 : 0;
}

صحيح atomic_set_bit(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

صحيح atomic_clear_bit(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
