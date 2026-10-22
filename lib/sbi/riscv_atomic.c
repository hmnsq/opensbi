/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_bitops.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/riscv_barrier.h>

#اگر !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	بازگشت ret;
}

تهی atomic_write(atomic_t *atom, long مقدار)
{
	atom->counter = مقدار;
	wmb();
}

long atomic_add_return(atomic_t *atom, long مقدار)
{
#اگر defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#اگر __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(مقدار)
			     : "memory");
#وگرنهاگر __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(مقدار)
			     : "memory");
#endif
#وگرنهاگر defined(__riscv_zalrsc)
	long ret, temp;
#اگر __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(مقدار)
			     : "memory");
#وگرنهاگر __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(مقدار)
			     : "memory");
#endif
#وگرنه
#error "need A or Zaamo or Zalrsc"
#endif

	بازگشت ret + مقدار;
}

long atomic_sub_return(atomic_t *atom, long مقدار)
{
	بازگشت atomic_add_return(atom, -مقدار);
}

#اگر defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		انتخاب (size) {							\
		حالت 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			بشکن;							\
		حالت 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			بشکن;							\
		پیشفرض:							\
			بشکن;							\
		}								\
		__ret;								\
	})
#وگرنهاگر defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		انتخاب (size) {							\
		حالت 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			بشکن;							\
		حالت 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			بشکن;							\
		پیشفرض:							\
			بشکن;							\
		}								\
		__ret;								\
	})
#وگرنه
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, اندازه(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	بازگشت __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	بازگشت axchg(&atom->counter, newval);
}

unsigned صحیح atomic_raw_xchg_uint(فرار unsigned صحیح *ptr,
				  unsigned صحیح newval)
{
	/* Atomically set new value and return old value. */
	بازگشت axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(فرار unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	بازگشت axchg(ptr, newval);
}

صحیح atomic_raw_set_bit(صحیح nr, فرار unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	بازگشت res & mask ? 1 : 0;
}

صحیح atomic_raw_clear_bit(صحیح nr, فرار unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	بازگشت res & mask ? 1 : 0;
}

صحیح atomic_set_bit(صحیح nr, atomic_t *atom)
{
	بازگشت atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

صحیح atomic_clear_bit(صحیح nr, atomic_t *atom)
{
	بازگشت atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
