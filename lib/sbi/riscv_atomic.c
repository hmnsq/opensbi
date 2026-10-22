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
#خطا "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#پایان_اگر

دراز atomic_read(atomic_t *atom)
{
	دراز ret = atom->counter;
	rmb();
	أرجع ret;
}

فراغ atomic_write(atomic_t *atom, دراز مقدار)
{
	atom->counter = مقدار;
	wmb();
}

دراز atomic_جمع_کن_return(atomic_t *atom, دراز مقدار)
{
#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
	دراز ret;
#إذا __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(مقدار)
			     : "memory");
#وإلاإذا __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(مقدار)
			     : "memory");
#پایان_اگر
#وإلاإذا defined(__riscv_zalrsc)
	دراز ret, موقت;
#إذا __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(موقت)
			     : "r"(مقدار)
			     : "memory");
#وإلاإذا __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(موقت)
			     : "r"(مقدار)
			     : "memory");
#پایان_اگر
#وإلا
#خطا "need A or Zaamo or Zalrsc"
#پایان_اگر

	أرجع ret + مقدار;
}

دراز atomic_sub_return(atomic_t *atom, دراز مقدار)
{
	أرجع atomic_جمع_کن_return(atom, -مقدار);
}

#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
#تعریف_کن __axchg(ptr, new, اندازه)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		بدل (اندازه) {							\
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
#تعریف_کن __axchg(ptr, new, اندازه)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		بدل (اندازه) {							\
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
#خطا "need A or Zaamo or Zalrsc"
#پایان_اگر

#تعریف_کن axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, حجم(*(ptr)));	\
	})

دراز atomic_cmpxchg(atomic_t *atom, دراز oldval, دراز newval)
{
	أرجع __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

دراز atomic_xchg(atomic_t *atom, دراز newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(&atom->counter, newval);
}

بیعلامت صحيح atomic_raw_xchg_uint(متطاير بیعلامت صحيح *ptr,
				  بیعلامت صحيح newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

بیعلامت دراز atomic_raw_xchg_ulong(متطاير بیعلامت دراز *ptr,
				    بیعلامت دراز newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

صحيح atomic_raw_مجموعه_بیت(صحيح nr, متطاير بیعلامت دراز *addr)
{
	بیعلامت دراز res, ماسک = بیت_ماسک(nr);
	res = __atomic_fetch_or(&addr[بیت_کلمه(nr)], ماسک, __ATOMIC_RELAXED);
	أرجع res & ماسک ? 1 : 0;
}

صحيح atomic_raw_clear_بیت(صحيح nr, متطاير بیعلامت دراز *addr)
{
	بیعلامت دراز res, ماسک = بیت_ماسک(nr);
	res = __atomic_fetch_and(&addr[بیت_کلمه(nr)], ~ماسک, __ATOMIC_RELAXED);
	أرجع res & ماسک ? 1 : 0;
}

صحيح atomic_مجموعه_بیت(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_مجموعه_بیت(nr, (بیعلامت دراز *)&atom->counter);
}

صحيح atomic_clear_بیت(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_clear_بیت(nr, (بیعلامت دراز *)&atom->counter);
}
