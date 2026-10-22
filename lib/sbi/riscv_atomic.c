/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_atomic.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>

#ଯଦି !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ atomic_write(atomic_t *atom, long ମୂଲ୍ୟ)
{
	atom->counter = ମୂଲ୍ୟ;
	wmb();
}

long atomic_add_return(atomic_t *atom, long ମୂଲ୍ୟ)
{
#ଯଦି defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#ଯଦି __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ମୂଲ୍ୟ)
			     : "memory");
#ନଚେତଯଦି __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(ମୂଲ୍ୟ)
			     : "memory");
#endif
#ନଚେତଯଦି defined(__riscv_zalrsc)
	long ret, temp;
#ଯଦି __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(ମୂଲ୍ୟ)
			     : "memory");
#ନଚେତଯଦି __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(ମୂଲ୍ୟ)
			     : "memory");
#endif
#ଅନ୍ୟଥା
#error "need A or Zaamo or Zalrsc"
#endif

	ଫେରନ୍ତୁ ret + ମୂଲ୍ୟ;
}

long atomic_sub_return(atomic_t *atom, long ମୂଲ୍ୟ)
{
	ଫେରନ୍ତୁ atomic_add_return(atom, -ମୂଲ୍ୟ);
}

#ଯଦି defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		ବାଛ (size) {							\
		ସ୍ଥିତି 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ଭାଙ୍ଗ;							\
		ସ୍ଥିତି 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			ଭାଙ୍ଗ;							\
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:							\
			ଭାଙ୍ଗ;							\
		}								\
		__ret;								\
	})
#ନଚେତଯଦି defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		ବାଛ (size) {							\
		ସ୍ଥିତି 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ଭାଙ୍ଗ;							\
		ସ୍ଥିତି 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			ଭାଙ୍ଗ;							\
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:							\
			ଭାଙ୍ଗ;							\
		}								\
		__ret;								\
	})
#ଅନ୍ୟଥା
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, ଆକାର(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	ଫେରନ୍ତୁ __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	ଫେରନ୍ତୁ axchg(&atom->counter, newval);
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା atomic_raw_xchg_uint(ଅସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ptr,
				  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା newval)
{
	/* Atomically set new value and return old value. */
	ଫେରନ୍ତୁ axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(ଅସ୍ଥିର unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	ଫେରନ୍ତୁ axchg(ptr, newval);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା atomic_raw_set_bit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, ଅସ୍ଥିର unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	ଫେରନ୍ତୁ res & mask ? 1 : 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା atomic_raw_clear_bit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, ଅସ୍ଥିର unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	ଫେରନ୍ତୁ res & mask ? 1 : 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା atomic_set_bit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, atomic_t *atom)
{
	ଫେରନ୍ତୁ atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା atomic_clear_bit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, atomic_t *atom)
{
	ଫେରନ୍ତୁ atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
