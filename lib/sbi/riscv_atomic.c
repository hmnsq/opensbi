/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>

#ಆದರೆ !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#ದೋಷ "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#ಆದರೆ_ಅಂತ್ಯ

ದೀರ್ಘ atomic_read(atomic_t *atom)
{
	ದೀರ್ಘ ret = atom->counter;
	rmb();
	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ atomic_write(atomic_t *atom, ದೀರ್ಘ ಮೌಲ್ಯ)
{
	atom->counter = ಮೌಲ್ಯ;
	wmb();
}

ದೀರ್ಘ atomic_ಕೂಡಿಸು_return(atomic_t *atom, ದೀರ್ಘ ಮೌಲ್ಯ)
{
#ಆದರೆ defined(__riscv_atomic) || defined(__riscv_zaamo)
	ದೀರ್ಘ ret;
#ಆದರೆ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ಮೌಲ್ಯ)
			     : "memory");
#ಅಥವಾಆದರೆ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ಮೌಲ್ಯ)
			     : "memory");
#ಆದರೆ_ಅಂತ್ಯ
#ಅಥವಾಆದರೆ defined(__riscv_zalrsc)
	ದೀರ್ಘ ret, ತಾತ್ಕಾಲಿಕ;
#ಆದರೆ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(ತಾತ್ಕಾಲಿಕ)
			     : "r"(ಮೌಲ್ಯ)
			     : "memory");
#ಅಥವಾಆದರೆ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(ತಾತ್ಕಾಲಿಕ)
			     : "r"(ಮೌಲ್ಯ)
			     : "memory");
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "need A or Zaamo or Zalrsc"
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ ret + ಮೌಲ್ಯ;
}

ದೀರ್ಘ atomic_sub_return(atomic_t *atom, ದೀರ್ಘ ಮೌಲ್ಯ)
{
	ಹಿಂತಿರುಗಿ atomic_ಕೂಡಿಸು_return(atom, -ಮೌಲ್ಯ);
}

#ಆದರೆ defined(__riscv_atomic) || defined(__riscv_zaamo)
#ವ್ಯಾಖ್ಯಾನಿಸು __axchg(ptr, new, ಗಾತ್ರ_2)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		ಆಯ್ಕೆ (ಗಾತ್ರ_2) {							\
		ಸ್ಥಿತಿ 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ನಿಲ್ಲಿಸು;							\
		ಸ್ಥಿತಿ 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ನಿಲ್ಲಿಸು;							\
		ಪೂರ್ವನಿಯೋಜಿತ:							\
			ನಿಲ್ಲಿಸು;							\
		}								\
		__ret;								\
	})
#ಅಥವಾಆದರೆ defined(__riscv_zalrsc)
#ವ್ಯಾಖ್ಯಾನಿಸು __axchg(ptr, new, ಗಾತ್ರ_2)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		ಆಯ್ಕೆ (ಗಾತ್ರ_2) {							\
		ಸ್ಥಿತಿ 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ನಿಲ್ಲಿಸು;							\
		ಸ್ಥಿತಿ 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ನಿಲ್ಲಿಸು;							\
		ಪೂರ್ವನಿಯೋಜಿತ:							\
			ನಿಲ್ಲಿಸು;							\
		}								\
		__ret;								\
	})
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "need A or Zaamo or Zalrsc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, ಗಾತ್ರ(*(ptr)));	\
	})

ದೀರ್ಘ atomic_cmpxchg(atomic_t *atom, ದೀರ್ಘ oldval, ದೀರ್ಘ newval)
{
	ಹಿಂತಿರುಗಿ __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

ದೀರ್ಘ atomic_xchg(atomic_t *atom, ದೀರ್ಘ newval)
{
	/* Atomically set new value and return old value. */
	ಹಿಂತಿರುಗಿ axchg(&atom->counter, newval);
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ atomic_raw_xchg_uint(ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *ptr,
				  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ newval)
{
	/* Atomically set new value and return old value. */
	ಹಿಂತಿರುಗಿ axchg(ptr, newval);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ atomic_raw_xchg_ulong(ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ptr,
				    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ newval)
{
	/* Atomically set new value and return old value. */
	ಹಿಂತಿರುಗಿ axchg(ptr, newval);
}

ಪೂರ್ಣಾಂಕ atomic_raw_ಗಣ_ಬಿಟ್(ಪೂರ್ಣಾಂಕ nr, ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ res, ಮುಖವಾಡ = ಬಿಟ್_ಮುಖವಾಡ(nr);
	res = __atomic_fetch_or(&addr[ಬಿಟ್_ಶಬ್ದ(nr)], ಮುಖವಾಡ, __ATOMIC_RELAXED);
	ಹಿಂತಿರುಗಿ res & ಮುಖವಾಡ ? 1 : 0;
}

ಪೂರ್ಣಾಂಕ atomic_raw_clear_ಬಿಟ್(ಪೂರ್ಣಾಂಕ nr, ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ res, ಮುಖವಾಡ = ಬಿಟ್_ಮುಖವಾಡ(nr);
	res = __atomic_fetch_and(&addr[ಬಿಟ್_ಶಬ್ದ(nr)], ~ಮುಖವಾಡ, __ATOMIC_RELAXED);
	ಹಿಂತಿರುಗಿ res & ಮುಖವಾಡ ? 1 : 0;
}

ಪೂರ್ಣಾಂಕ atomic_ಗಣ_ಬಿಟ್(ಪೂರ್ಣಾಂಕ nr, atomic_t *atom)
{
	ಹಿಂತಿರುಗಿ atomic_raw_ಗಣ_ಬಿಟ್(nr, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *)&atom->counter);
}

ಪೂರ್ಣಾಂಕ atomic_clear_ಬಿಟ್(ಪೂರ್ಣಾಂಕ nr, atomic_t *atom)
{
	ಹಿಂತಿರುಗಿ atomic_raw_clear_ಬಿಟ್(nr, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *)&atom->counter);
}
