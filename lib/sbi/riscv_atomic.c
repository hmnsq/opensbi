/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_atomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>

#ਜੇ !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#ਗਲਤੀ "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#ਜੇ_ਅੰਤ

ਲੰਮਾ atomic_read(atomic_t *atom)
{
	ਲੰਮਾ ret = atom->counter;
	rmb();
	ਵਾਪਸ ret;
}

ਖਾਲੀ atomic_write(atomic_t *atom, ਲੰਮਾ ਮੁੱਲ)
{
	atom->counter = ਮੁੱਲ;
	wmb();
}

ਲੰਮਾ atomic_ਜੋੜੋ_return(atomic_t *atom, ਲੰਮਾ ਮੁੱਲ)
{
#ਜੇ defined(__riscv_atomic) || defined(__riscv_zaamo)
	ਲੰਮਾ ret;
#ਜੇ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ਮੁੱਲ)
			     : "memory");
#ਜਾਂਜੇ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ਮੁੱਲ)
			     : "memory");
#ਜੇ_ਅੰਤ
#ਜਾਂਜੇ defined(__riscv_zalrsc)
	ਲੰਮਾ ret, ਆਰਜ਼ੀ;
#ਜੇ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(ਆਰਜ਼ੀ)
			     : "r"(ਮੁੱਲ)
			     : "memory");
#ਜਾਂਜੇ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(ਆਰਜ਼ੀ)
			     : "r"(ਮੁੱਲ)
			     : "memory");
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "need A or Zaamo or Zalrsc"
#ਜੇ_ਅੰਤ

	ਵਾਪਸ ret + ਮੁੱਲ;
}

ਲੰਮਾ atomic_sub_return(atomic_t *atom, ਲੰਮਾ ਮੁੱਲ)
{
	ਵਾਪਸ atomic_ਜੋੜੋ_return(atom, -ਮੁੱਲ);
}

#ਜੇ defined(__riscv_atomic) || defined(__riscv_zaamo)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __axchg(ptr, new, ਮਾਪ)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		ਚੋਣ (ਮਾਪ) {							\
		ਹਾਲਤ 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ਰੋਕੋ;							\
		ਹਾਲਤ 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ਰੋਕੋ;							\
		ਮੂਲ:							\
			ਰੋਕੋ;							\
		}								\
		__ret;								\
	})
#ਜਾਂਜੇ defined(__riscv_zalrsc)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __axchg(ptr, new, ਮਾਪ)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		ਚੋਣ (ਮਾਪ) {							\
		ਹਾਲਤ 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ਰੋਕੋ;							\
		ਹਾਲਤ 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ਰੋਕੋ;							\
		ਮੂਲ:							\
			ਰੋਕੋ;							\
		}								\
		__ret;								\
	})
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "need A or Zaamo or Zalrsc"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, ਆਕਾਰ(*(ptr)));	\
	})

ਲੰਮਾ atomic_cmpxchg(atomic_t *atom, ਲੰਮਾ oldval, ਲੰਮਾ newval)
{
	ਵਾਪਸ __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

ਲੰਮਾ atomic_xchg(atomic_t *atom, ਲੰਮਾ newval)
{
	/* Atomically set new value and return old value. */
	ਵਾਪਸ axchg(&atom->counter, newval);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ atomic_raw_xchg_uint(ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *ptr,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ newval)
{
	/* Atomically set new value and return old value. */
	ਵਾਪਸ axchg(ptr, newval);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ atomic_raw_xchg_ulong(ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ptr,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ newval)
{
	/* Atomically set new value and return old value. */
	ਵਾਪਸ axchg(ptr, newval);
}

ਪੂਰਨ atomic_raw_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਨ nr, ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ res, ਨਕਾਬ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(nr);
	res = __atomic_fetch_or(&addr[ਦੋਆਧਾਰੀ_ਅੰਕ_ਸ਼ਬਦ(nr)], ਨਕਾਬ, __ATOMIC_RELAXED);
	ਵਾਪਸ res & ਨਕਾਬ ? 1 : 0;
}

ਪੂਰਨ atomic_raw_clear_ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਨ nr, ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ res, ਨਕਾਬ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(nr);
	res = __atomic_fetch_and(&addr[ਦੋਆਧਾਰੀ_ਅੰਕ_ਸ਼ਬਦ(nr)], ~ਨਕਾਬ, __ATOMIC_RELAXED);
	ਵਾਪਸ res & ਨਕਾਬ ? 1 : 0;
}

ਪੂਰਨ atomic_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਨ nr, atomic_t *atom)
{
	ਵਾਪਸ atomic_raw_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(nr, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *)&atom->counter);
}

ਪੂਰਨ atomic_clear_ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੂਰਨ nr, atomic_t *atom)
{
	ਵਾਪਸ atomic_raw_clear_ਦੋਆਧਾਰੀ_ਅੰਕ(nr, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *)&atom->counter);
}
