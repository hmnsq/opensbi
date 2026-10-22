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
#غلطی "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#اگر_ختم

طویل atomic_read(atomic_t *atom)
{
	طویل ret = atom->شمار_2;
	rmb();
	واپس ret;
}

خالی atomic_write(atomic_t *atom, طویل قدر)
{
	atom->شمار_2 = قدر;
	wmb();
}

طویل atomic_جوڑو_return(atomic_t *atom, طویل قدر)
{
#اگر defined(__riscv_atomic) || defined(__riscv_zaamo)
	طویل ret;
#اگر __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->شمار_2), "=r"(ret)
			     : "r"(قدر)
			     : "memory");
#وگرنہ_اگر __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->شمار_2), "=r"(ret)
			     : "r"(قدر)
			     : "memory");
#اگر_ختم
#وگرنہ_اگر defined(__riscv_zalrsc)
	طویل ret, عارضی;
#اگر __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->شمار_2), "=&r"(ret), "=&r"(عارضی)
			     : "r"(قدر)
			     : "memory");
#وگرنہ_اگر __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->شمار_2), "=&r"(ret), "=&r"(عارضی)
			     : "r"(قدر)
			     : "memory");
#اگر_ختم
#ورنہ
#غلطی "need A or Zaamo or Zalrsc"
#اگر_ختم

	واپس ret + قدر;
}

طویل atomic_sub_return(atomic_t *atom, طویل قدر)
{
	واپس atomic_جوڑو_return(atom, -قدر);
}

#اگر defined(__riscv_atomic) || defined(__riscv_zaamo)
#تعریف_کرو __axchg(ptr, new, سائز)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		انتخاب (سائز) {							\
		حالت 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			توڑو;							\
		حالت 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			توڑو;							\
		طےشدہ:							\
			توڑو;							\
		}								\
		__ret;								\
	})
#وگرنہ_اگر defined(__riscv_zalrsc)
#تعریف_کرو __axchg(ptr, new, سائز)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		انتخاب (سائز) {							\
		حالت 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			توڑو;							\
		حالت 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			توڑو;							\
		طےشدہ:							\
			توڑو;							\
		}								\
		__ret;								\
	})
#ورنہ
#غلطی "need A or Zaamo or Zalrsc"
#اگر_ختم

#تعریف_کرو axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, حجم(*(ptr)));	\
	})

طویل atomic_cmpxchg(atomic_t *atom, طویل oldval, طویل newval)
{
	واپس __sync_val_compare_and_swap(&atom->شمار_2, oldval, newval);
}

طویل atomic_xchg(atomic_t *atom, طویل newval)
{
	/* Atomically set new value and return old value. */
	واپس axchg(&atom->شمار_2, newval);
}

غیرعلامتی صحیح atomic_raw_xchg_uint(متلون غیرعلامتی صحیح *ptr,
				  غیرعلامتی صحیح newval)
{
	/* Atomically set new value and return old value. */
	واپس axchg(ptr, newval);
}

غیرعلامتی طویل atomic_raw_xchg_ulong(متلون غیرعلامتی طویل *ptr,
				    غیرعلامتی طویل newval)
{
	/* Atomically set new value and return old value. */
	واپس axchg(ptr, newval);
}

صحیح atomic_raw_سیٹ_ثنائی_ہندسہ(صحیح nr, متلون غیرعلامتی طویل *addr)
{
	غیرعلامتی طویل res, نقاب = ثنائی_ہندسہ_نقاب(nr);
	res = __atomic_fetch_or(&addr[ثنائی_ہندسہ_لفظ(nr)], نقاب, __ATOMIC_RELAXED);
	واپس res & نقاب ? 1 : 0;
}

صحیح atomic_raw_clear_ثنائی_ہندسہ(صحیح nr, متلون غیرعلامتی طویل *addr)
{
	غیرعلامتی طویل res, نقاب = ثنائی_ہندسہ_نقاب(nr);
	res = __atomic_fetch_and(&addr[ثنائی_ہندسہ_لفظ(nr)], ~نقاب, __ATOMIC_RELAXED);
	واپس res & نقاب ? 1 : 0;
}

صحیح atomic_سیٹ_ثنائی_ہندسہ(صحیح nr, atomic_t *atom)
{
	واپس atomic_raw_سیٹ_ثنائی_ہندسہ(nr, (غیرعلامتی طویل *)&atom->شمار_2);
}

صحیح atomic_clear_ثنائی_ہندسہ(صحیح nr, atomic_t *atom)
{
	واپس atomic_raw_clear_ثنائی_ہندسہ(nr, (غیرعلامتی طویل *)&atom->شمار_2);
}
