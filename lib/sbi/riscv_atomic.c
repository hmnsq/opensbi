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
#تېروتنه "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#د_که_پای

اوږد atomic_read(atomic_t *atom)
{
	اوږد ret = atom->counter;
	rmb();
	أرجع ret;
}

فراغ atomic_write(atomic_t *atom, اوږد ارزښت)
{
	atom->counter = ارزښت;
	wmb();
}

اوږد atomic_جمع_کړه_return(atomic_t *atom, اوږد ارزښت)
{
#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
	اوږد ret;
#إذا __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ارزښت)
			     : "memory");
#وإلاإذا __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ارزښت)
			     : "memory");
#د_که_پای
#وإلاإذا defined(__riscv_zalrsc)
	اوږد ret, لنډمهاله;
#إذا __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(لنډمهاله)
			     : "r"(ارزښت)
			     : "memory");
#وإلاإذا __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(لنډمهاله)
			     : "r"(ارزښت)
			     : "memory");
#د_که_پای
#وإلا
#تېروتنه "need A or Zaamo or Zalrsc"
#د_که_پای

	أرجع ret + ارزښت;
}

اوږد atomic_sub_return(atomic_t *atom, اوږد ارزښت)
{
	أرجع atomic_جمع_کړه_return(atom, -ارزښت);
}

#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
#تعریف_کړه __axchg(ptr, new, اندازه)							\
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
#تعریف_کړه __axchg(ptr, new, اندازه)							\
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
#تېروتنه "need A or Zaamo or Zalrsc"
#د_که_پای

#تعریف_کړه axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, حجم(*(ptr)));	\
	})

اوږد atomic_cmpxchg(atomic_t *atom, اوږد oldval, اوږد newval)
{
	أرجع __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

اوږد atomic_xchg(atomic_t *atom, اوږد newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(&atom->counter, newval);
}

بینښه صحيح atomic_raw_xchg_uint(متطاير بینښه صحيح *ptr,
				  بینښه صحيح newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

بینښه اوږد atomic_raw_xchg_ulong(متطاير بینښه اوږد *ptr,
				    بینښه اوږد newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

صحيح atomic_raw_سېټ_بټ(صحيح nr, متطاير بینښه اوږد *addr)
{
	بینښه اوږد res, ماسک = بټ_ماسک(nr);
	res = __atomic_fetch_or(&addr[بټ_کلمه(nr)], ماسک, __ATOMIC_RELAXED);
	أرجع res & ماسک ? 1 : 0;
}

صحيح atomic_raw_clear_بټ(صحيح nr, متطاير بینښه اوږد *addr)
{
	بینښه اوږد res, ماسک = بټ_ماسک(nr);
	res = __atomic_fetch_and(&addr[بټ_کلمه(nr)], ~ماسک, __ATOMIC_RELAXED);
	أرجع res & ماسک ? 1 : 0;
}

صحيح atomic_سېټ_بټ(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_سېټ_بټ(nr, (بینښه اوږد *)&atom->counter);
}

صحيح atomic_clear_بټ(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_clear_بټ(nr, (بینښه اوږد *)&atom->counter);
}
