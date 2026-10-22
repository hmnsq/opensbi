/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_atomic.h>
#అన్తర్భావ <sbi/riscv_barrier.h>

#ఒకవేళ !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#దోషం "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#అయితే_ముగింపు

దీర్ఘ atomic_read(atomic_t *atom)
{
	దీర్ఘ ret = atom->counter;
	rmb();
	తిరిగి ret;
}

శూన్య atomic_write(atomic_t *atom, దీర్ఘ విలువ)
{
	atom->counter = విలువ;
	wmb();
}

దీర్ఘ atomic_కూడు_return(atomic_t *atom, దీర్ఘ విలువ)
{
#ఒకవేళ defined(__riscv_atomic) || defined(__riscv_zaamo)
	దీర్ఘ ret;
#ఒకవేళ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(విలువ)
			     : "memory");
#లేదావేళ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(విలువ)
			     : "memory");
#అయితే_ముగింపు
#లేదావేళ defined(__riscv_zalrsc)
	దీర్ఘ ret, తాత్కాలిక;
#ఒకవేళ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(తాత్కాలిక)
			     : "r"(విలువ)
			     : "memory");
#లేదావేళ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(తాత్కాలిక)
			     : "r"(విలువ)
			     : "memory");
#అయితే_ముగింపు
#లేకపోతే
#దోషం "need A or Zaamo or Zalrsc"
#అయితే_ముగింపు

	తిరిగి ret + విలువ;
}

దీర్ఘ atomic_sub_return(atomic_t *atom, దీర్ఘ విలువ)
{
	తిరిగి atomic_కూడు_return(atom, -విలువ);
}

#ఒకవేళ defined(__riscv_atomic) || defined(__riscv_zaamo)
#నిర్వచించు __axchg(ptr, new, కొలత)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		ఎంపిక (కొలత) {							\
		స్థితి 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ఆపు;							\
		స్థితి 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ఆపు;							\
		అప్రమేయ:							\
			ఆపు;							\
		}								\
		__ret;								\
	})
#లేదావేళ defined(__riscv_zalrsc)
#నిర్వచించు __axchg(ptr, new, కొలత)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		ఎంపిక (కొలత) {							\
		స్థితి 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ఆపు;							\
		స్థితి 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ఆపు;							\
		అప్రమేయ:							\
			ఆపు;							\
		}								\
		__ret;								\
	})
#లేకపోతే
#దోషం "need A or Zaamo or Zalrsc"
#అయితే_ముగింపు

#నిర్వచించు axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, పరిమాణం(*(ptr)));	\
	})

దీర్ఘ atomic_cmpxchg(atomic_t *atom, దీర్ఘ oldval, దీర్ఘ newval)
{
	తిరిగి __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

దీర్ఘ atomic_xchg(atomic_t *atom, దీర్ఘ newval)
{
	/* Atomically set new value and return old value. */
	తిరిగి axchg(&atom->counter, newval);
}

గుర్తులేని పూర్ణాంకం atomic_raw_xchg_uint(అస్థిర గుర్తులేని పూర్ణాంకం *ptr,
				  గుర్తులేని పూర్ణాంకం newval)
{
	/* Atomically set new value and return old value. */
	తిరిగి axchg(ptr, newval);
}

గుర్తులేని దీర్ఘ atomic_raw_xchg_ulong(అస్థిర గుర్తులేని దీర్ఘ *ptr,
				    గుర్తులేని దీర్ఘ newval)
{
	/* Atomically set new value and return old value. */
	తిరిగి axchg(ptr, newval);
}

పూర్ణాంకం atomic_raw_సమితి_బిట్టు(పూర్ణాంకం nr, అస్థిర గుర్తులేని దీర్ఘ *addr)
{
	గుర్తులేని దీర్ఘ res, ముసుగు = బిట్టు_ముసుగు(nr);
	res = __atomic_fetch_or(&addr[బిట్టు_మాట(nr)], ముసుగు, __ATOMIC_RELAXED);
	తిరిగి res & ముసుగు ? 1 : 0;
}

పూర్ణాంకం atomic_raw_clear_బిట్టు(పూర్ణాంకం nr, అస్థిర గుర్తులేని దీర్ఘ *addr)
{
	గుర్తులేని దీర్ఘ res, ముసుగు = బిట్టు_ముసుగు(nr);
	res = __atomic_fetch_and(&addr[బిట్టు_మాట(nr)], ~ముసుగు, __ATOMIC_RELAXED);
	తిరిగి res & ముసుగు ? 1 : 0;
}

పూర్ణాంకం atomic_సమితి_బిట్టు(పూర్ణాంకం nr, atomic_t *atom)
{
	తిరిగి atomic_raw_సమితి_బిట్టు(nr, (గుర్తులేని దీర్ఘ *)&atom->counter);
}

పూర్ణాంకం atomic_clear_బిట్టు(పూర్ణాంకం nr, atomic_t *atom)
{
	తిరిగి atomic_raw_clear_బిట్టు(nr, (గుర్తులేని దీర్ఘ *)&atom->counter);
}
