/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_atomic.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>

#எனில் !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

long atomic_read(atomic_t *atom)
{
	long ret = atom->counter;
	rmb();
	திரும்பு ret;
}

வெறுமை atomic_write(atomic_t *atom, long மதிப்பு)
{
	atom->counter = மதிப்பு;
	wmb();
}

long atomic_add_return(atomic_t *atom, long மதிப்பு)
{
#எனில் defined(__riscv_atomic) || defined(__riscv_zaamo)
	long ret;
#எனில் __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(மதிப்பு)
			     : "memory");
#அல்லதெனில் __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(மதிப்பு)
			     : "memory");
#endif
#அல்லதெனில் defined(__riscv_zalrsc)
	long ret, temp;
#எனில் __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(மதிப்பு)
			     : "memory");
#அல்லதெனில் __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(மதிப்பு)
			     : "memory");
#endif
#இல்லையெனில்
#error "need A or Zaamo or Zalrsc"
#endif

	திரும்பு ret + மதிப்பு;
}

long atomic_sub_return(atomic_t *atom, long மதிப்பு)
{
	திரும்பு atomic_add_return(atom, -மதிப்பு);
}

#எனில் defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		தேர்வு (size) {							\
		நிலை 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			நிறுத்து;							\
		நிலை 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			நிறுத்து;							\
		இயல்பு:							\
			நிறுத்து;							\
		}								\
		__ret;								\
	})
#அல்லதெனில் defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		தேர்வு (size) {							\
		நிலை 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			நிறுத்து;							\
		நிலை 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			நிறுத்து;							\
		இயல்பு:							\
			நிறுத்து;							\
		}								\
		__ret;								\
	})
#இல்லையெனில்
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, அளவு(*(ptr)));	\
	})

long atomic_cmpxchg(atomic_t *atom, long oldval, long newval)
{
	திரும்பு __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

long atomic_xchg(atomic_t *atom, long newval)
{
	/* Atomically set new value and return old value. */
	திரும்பு axchg(&atom->counter, newval);
}

unsigned முழு atomic_raw_xchg_uint(நிலையற்ற unsigned முழு *ptr,
				  unsigned முழு newval)
{
	/* Atomically set new value and return old value. */
	திரும்பு axchg(ptr, newval);
}

unsigned long atomic_raw_xchg_ulong(நிலையற்ற unsigned long *ptr,
				    unsigned long newval)
{
	/* Atomically set new value and return old value. */
	திரும்பு axchg(ptr, newval);
}

முழு atomic_raw_set_bit(முழு nr, நிலையற்ற unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	திரும்பு res & mask ? 1 : 0;
}

முழு atomic_raw_clear_bit(முழு nr, நிலையற்ற unsigned long *addr)
{
	unsigned long res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	திரும்பு res & mask ? 1 : 0;
}

முழு atomic_set_bit(முழு nr, atomic_t *atom)
{
	திரும்பு atomic_raw_set_bit(nr, (unsigned long *)&atom->counter);
}

முழு atomic_clear_bit(முழு nr, atomic_t *atom)
{
	திரும்பு atomic_raw_clear_bit(nr, (unsigned long *)&atom->counter);
}
