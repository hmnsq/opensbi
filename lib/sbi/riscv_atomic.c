/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_atomic.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>

#എങ്കിൽ !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	തിരികെ ret;
}

ശൂന്യം atomic_write(atomic_t *atom, long മൂല്യം)
{
	atom->counter = മൂല്യം;
	wmb();
}

long atomic_add_return(atomic_t *atom, long മൂല്യം)
{
#എങ്കിൽ defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#എങ്കിൽ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(മൂല്യം)
			     : "memory");
#അല്ലെങ്കിലെങ്കിൽ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(മൂല്യം)
			     : "memory");
#endif
#അല്ലെങ്കിലെങ്കിൽ defined(__riscv_zalrsc)
	long ret, temp;
#എങ്കിൽ __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(മൂല്യം)
			     : "memory");
#അല്ലെങ്കിലെങ്കിൽ __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(മൂല്യം)
			     : "memory");
#endif
#അല്ലെങ്കിൽ
#error "need A or Zaamo or Zalrsc"
#endif

	തിരികെ ret + മൂല്യം;
}

long atomic_sub_return(atomic_t *atom, long മൂല്യം)
{
	തിരികെ atomic_add_return(atom, -മൂല്യം);
}

#എങ്കിൽ defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		തിരഞ്ഞെടുപ്പ് (size) {							\
		അവസ്ഥ 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			നിർത്തുക;							\
		അവസ്ഥ 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			നിർത്തുക;							\
		സ്ഥിരസ്ഥിതി:							\
			നിർത്തുക;							\
		}								\
		__ret;								\
	})
#അല്ലെങ്കിലെങ്കിൽ defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		തിരഞ്ഞെടുപ്പ് (size) {							\
		അവസ്ഥ 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			നിർത്തുക;							\
		അവസ്ഥ 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			നിർത്തുക;							\
		സ്ഥിരസ്ഥിതി:							\
			നിർത്തുക;							\
		}								\
		__ret;								\
	})
#അല്ലെങ്കിൽ
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, വലുപ്പം(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	തിരികെ __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	തിരികെ axchg(&atom->counter, newval);
}

unsigned പൂർണ്ണസംഖ്യ atomic_raw_xchg_uint(അസ്ഥിര unsigned പൂർണ്ണസംഖ്യ *ptr,
				  unsigned പൂർണ്ണസംഖ്യ newval)
{
	/* Atomically set new value and return old value. */
	തിരികെ axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(അസ്ഥിര unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	തിരികെ axchg(ptr, newval);
}

പൂർണ്ണസംഖ്യ atomic_raw_set_bit(പൂർണ്ണസംഖ്യ nr, അസ്ഥിര unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	തിരികെ res & mask ? 1 : 0;
}

പൂർണ്ണസംഖ്യ atomic_raw_clear_bit(പൂർണ്ണസംഖ്യ nr, അസ്ഥിര unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	തിരികെ res & mask ? 1 : 0;
}

പൂർണ്ണസംഖ്യ atomic_set_bit(പൂർണ്ണസംഖ്യ nr, atomic_t *atom)
{
	തിരികെ atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

പൂർണ്ണസംഖ്യ atomic_clear_bit(പൂർണ്ണസംഖ്യ nr, atomic_t *atom)
{
	തിരികെ atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
