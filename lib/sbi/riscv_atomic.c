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

#ಯದಿ !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ atomic_write(atomic_t *atom, long ಮೌಲ್ಯ)
{
	atom->counter = ಮೌಲ್ಯ;
	wmb();
}

long atomic_add_return(atomic_t *atom, long ಮೌಲ್ಯ)
{
#ಯದಿ defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#ಯದಿ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ಮೌಲ್ಯ)
			     : "memory");
#ನೋಚೇದ್ಯದಿ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ಮೌಲ್ಯ)
			     : "memory");
#endif
#ನೋಚೇದ್ಯದಿ defined(__riscv_zalrsc)
	long ret, temp;
#ಯದಿ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(ಮೌಲ್ಯ)
			     : "memory");
#ನೋಚೇದ್ಯದಿ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(ಮೌಲ್ಯ)
			     : "memory");
#endif
#ಅನ್ಯಥಾ
#error "need A or Zaamo or Zalrsc"
#endif

	ಪ್ರತ್ಯಾಗಮ ret + ಮೌಲ್ಯ;
}

long atomic_sub_return(atomic_t *atom, long ಮೌಲ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ atomic_add_return(atom, -ಮೌಲ್ಯ);
}

#ಯದಿ defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		ವಿಕಲ್ಪ (size) {							\
		ಪಕ್ಷ 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ಭಙ್ಗ;							\
		ಪಕ್ಷ 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ಭಙ್ಗ;							\
		ಸ್ವಭಾವ:							\
			ಭಙ್ಗ;							\
		}								\
		__ret;								\
	})
#ನೋಚೇದ್ಯದಿ defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		ವಿಕಲ್ಪ (size) {							\
		ಪಕ್ಷ 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ಭಙ್ಗ;							\
		ಪಕ್ಷ 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ಭಙ್ಗ;							\
		ಸ್ವಭಾವ:							\
			ಭಙ್ಗ;							\
		}								\
		__ret;								\
	})
#ಅನ್ಯಥಾ
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, ಪರಿಮಾಣ(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	ಪ್ರತ್ಯಾಗಮ __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	ಪ್ರತ್ಯಾಗಮ axchg(&atom->counter, newval);
}

unsigned ಪೂರ್ಣಾಙ್ಕ atomic_raw_xchg_uint(ಚಞ್ಚಲ unsigned ಪೂರ್ಣಾಙ್ಕ *ptr,
				  unsigned ಪೂರ್ಣಾಙ್ಕ newval)
{
	/* Atomically set new value and return old value. */
	ಪ್ರತ್ಯಾಗಮ axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(ಚಞ್ಚಲ unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	ಪ್ರತ್ಯಾಗಮ axchg(ptr, newval);
}

ಪೂರ್ಣಾಙ್ಕ atomic_raw_set_bit(ಪೂರ್ಣಾಙ್ಕ nr, ಚಞ್ಚಲ unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	ಪ್ರತ್ಯಾಗಮ res & mask ? 1 : 0;
}

ಪೂರ್ಣಾಙ್ಕ atomic_raw_clear_bit(ಪೂರ್ಣಾಙ್ಕ nr, ಚಞ್ಚಲ unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	ಪ್ರತ್ಯಾಗಮ res & mask ? 1 : 0;
}

ಪೂರ್ಣಾಙ್ಕ atomic_set_bit(ಪೂರ್ಣಾಙ್ಕ nr, atomic_t *atom)
{
	ಪ್ರತ್ಯಾಗಮ atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

ಪೂರ್ಣಾಙ್ಕ atomic_clear_bit(ಪೂರ್ಣಾಙ್ಕ nr, atomic_t *atom)
{
	ಪ್ರತ್ಯಾಗಮ atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
