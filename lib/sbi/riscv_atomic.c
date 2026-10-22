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
#غلطی "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#جے_انت

لما atomic_read(atomic_t *atom)
{
	لما ret = atom->counter;
	rmb();
	أرجع ret;
}

فراغ atomic_write(atomic_t *atom, لما قدر)
{
	atom->counter = قدر;
	wmb();
}

لما atomic_جوڑو_return(atomic_t *atom, لما قدر)
{
#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
	لما ret;
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
#جے_انت
#وإلاإذا defined(__riscv_zalrsc)
	لما ret, عارضی;
#إذا __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(عارضی)
			     : "r"(قدر)
			     : "memory");
#وإلاإذا __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(عارضی)
			     : "r"(قدر)
			     : "memory");
#جے_انت
#وإلا
#غلطی "need A or Zaamo or Zalrsc"
#جے_انت

	أرجع ret + قدر;
}

لما atomic_sub_return(atomic_t *atom, لما قدر)
{
	أرجع atomic_جوڑو_return(atom, -قدر);
}

#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
#تعریف_کرو __axchg(ptr, new, ناپ)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		بدل (ناپ) {							\
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
#تعریف_کرو __axchg(ptr, new, ناپ)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		بدل (ناپ) {							\
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
#غلطی "need A or Zaamo or Zalrsc"
#جے_انت

#تعریف_کرو axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, حجم(*(ptr)));	\
	})

لما atomic_cmpxchg(atomic_t *atom, لما oldval, لما newval)
{
	أرجع __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

لما atomic_xchg(atomic_t *atom, لما newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(&atom->counter, newval);
}

بنانشانی صحيح atomic_raw_xchg_uint(متطاير بنانشانی صحيح *ptr,
				  بنانشانی صحيح newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

بنانشانی لما atomic_raw_xchg_ulong(متطاير بنانشانی لما *ptr,
				    بنانشانی لما newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

صحيح atomic_raw_سیٹ_ثنائی_ہندسہ(صحيح nr, متطاير بنانشانی لما *addr)
{
	بنانشانی لما res, نقاب = ثنائی_ہندسہ_نقاب(nr);
	res = __atomic_fetch_or(&addr[ثنائی_ہندسہ_لفظ(nr)], نقاب, __ATOMIC_RELAXED);
	أرجع res & نقاب ? 1 : 0;
}

صحيح atomic_raw_clear_ثنائی_ہندسہ(صحيح nr, متطاير بنانشانی لما *addr)
{
	بنانشانی لما res, نقاب = ثنائی_ہندسہ_نقاب(nr);
	res = __atomic_fetch_and(&addr[ثنائی_ہندسہ_لفظ(nr)], ~نقاب, __ATOMIC_RELAXED);
	أرجع res & نقاب ? 1 : 0;
}

صحيح atomic_سیٹ_ثنائی_ہندسہ(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_سیٹ_ثنائی_ہندسہ(nr, (بنانشانی لما *)&atom->counter);
}

صحيح atomic_clear_ثنائی_ہندسہ(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_clear_ثنائی_ہندسہ(nr, (بنانشانی لما *)&atom->counter);
}
