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
#غلطي "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#جيڪڏهن_پورو

ڊگهو atomic_read(atomic_t *atom)
{
	ڊگهو ret = atom->counter;
	rmb();
	أرجع ret;
}

فراغ atomic_write(atomic_t *atom, ڊگهو قدر)
{
	atom->counter = قدر;
	wmb();
}

ڊگهو atomic_جوڙيو_return(atomic_t *atom, ڊگهو قدر)
{
#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
	ڊگهو ret;
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
#جيڪڏهن_پورو
#وإلاإذا defined(__riscv_zalrsc)
	ڊگهو ret, عارضي;
#إذا __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(عارضي)
			     : "r"(قدر)
			     : "memory");
#وإلاإذا __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(عارضي)
			     : "r"(قدر)
			     : "memory");
#جيڪڏهن_پورو
#وإلا
#غلطي "need A or Zaamo or Zalrsc"
#جيڪڏهن_پورو

	أرجع ret + قدر;
}

ڊگهو atomic_sub_return(atomic_t *atom, ڊگهو قدر)
{
	أرجع atomic_جوڙيو_return(atom, -قدر);
}

#إذا defined(__riscv_atomic) || defined(__riscv_zaamo)
#وصف_ڪريو __axchg(ptr, new, سائيز)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		بدل (سائيز) {							\
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
#وصف_ڪريو __axchg(ptr, new, سائيز)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		بدل (سائيز) {							\
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
#غلطي "need A or Zaamo or Zalrsc"
#جيڪڏهن_پورو

#وصف_ڪريو axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, حجم(*(ptr)));	\
	})

ڊگهو atomic_cmpxchg(atomic_t *atom, ڊگهو oldval, ڊگهو newval)
{
	أرجع __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

ڊگهو atomic_xchg(atomic_t *atom, ڊگهو newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(&atom->counter, newval);
}

بنانشان صحيح atomic_raw_xchg_uint(متطاير بنانشان صحيح *ptr,
				  بنانشان صحيح newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

بنانشان ڊگهو atomic_raw_xchg_ulong(متطاير بنانشان ڊگهو *ptr,
				    بنانشان ڊگهو newval)
{
	/* Atomically set new value and return old value. */
	أرجع axchg(ptr, newval);
}

صحيح atomic_raw_سيٽ_ٻٽو_انگ(صحيح nr, متطاير بنانشان ڊگهو *addr)
{
	بنانشان ڊگهو res, نقاب = ٻٽو_انگ_نقاب(nr);
	res = __atomic_fetch_or(&addr[ٻٽو_انگ_لفظ(nr)], نقاب, __ATOMIC_RELAXED);
	أرجع res & نقاب ? 1 : 0;
}

صحيح atomic_raw_clear_ٻٽو_انگ(صحيح nr, متطاير بنانشان ڊگهو *addr)
{
	بنانشان ڊگهو res, نقاب = ٻٽو_انگ_نقاب(nr);
	res = __atomic_fetch_and(&addr[ٻٽو_انگ_لفظ(nr)], ~نقاب, __ATOMIC_RELAXED);
	أرجع res & نقاب ? 1 : 0;
}

صحيح atomic_سيٽ_ٻٽو_انگ(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_سيٽ_ٻٽو_انگ(nr, (بنانشان ڊگهو *)&atom->counter);
}

صحيح atomic_clear_ٻٽو_انگ(صحيح nr, atomic_t *atom)
{
	أرجع atomic_raw_clear_ٻٽو_انگ(nr, (بنانشان ڊگهو *)&atom->counter);
}
