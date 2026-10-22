/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_atomic.h>
#अन्तर्भाव <sbi/riscv_barrier.h>

#यदि !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#error "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#endif

दीर्घ atomic_read(atomic_t *atom)
{
	दीर्घ ret = atom->counter;
	rmb();
	प्रत्यागम ret;
}

शून्य atomic_write(atomic_t *atom, दीर्घ मूल्य)
{
	atom->counter = मूल्य;
	wmb();
}

दीर्घ atomic_add_return(atomic_t *atom, दीर्घ मूल्य)
{
#यदि defined(__riscv_atomic) || defined(__riscv_zaamo)
	दीर्घ ret;
#यदि __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(मूल्य)
			     : "memory");
#नोचेद्यदि __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->counter), "=r"(ret)
			     : "r"(मूल्य)
			     : "memory");
#endif
#नोचेद्यदि defined(__riscv_zalrsc)
	दीर्घ ret, temp;
#यदि __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(मूल्य)
			     : "memory");
#नोचेद्यदि __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->counter), "=&r"(ret), "=&r"(temp)
			     : "r"(मूल्य)
			     : "memory");
#endif
#अन्यथा
#error "need A or Zaamo or Zalrsc"
#endif

	प्रत्यागम ret + मूल्य;
}

दीर्घ atomic_sub_return(atomic_t *atom, दीर्घ मूल्य)
{
	प्रत्यागम atomic_add_return(atom, -मूल्य);
}

#यदि defined(__riscv_atomic) || defined(__riscv_zaamo)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		विकल्प (size) {							\
		पक्ष 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			भङ्ग;							\
		पक्ष 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			भङ्ग;							\
		स्वभाव:							\
			भङ्ग;							\
		}								\
		__ret;								\
	})
#नोचेद्यदि defined(__riscv_zalrsc)
#define __axchg(ptr, new, size)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		विकल्प (size) {							\
		पक्ष 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			भङ्ग;							\
		पक्ष 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			भङ्ग;							\
		स्वभाव:							\
			भङ्ग;							\
		}								\
		__ret;								\
	})
#अन्यथा
#error "need A or Zaamo or Zalrsc"
#endif

#define axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, परिमाण(*(ptr)));	\
	})

दीर्घ atomic_cmpxchg(atomic_t *atom, दीर्घ oldval, दीर्घ newval)
{
	प्रत्यागम __sync_val_compare_and_swap(&atom->counter, oldval, newval);
}

दीर्घ atomic_xchg(atomic_t *atom, दीर्घ newval)
{
	/* Atomically set new value and return old value. */
	प्रत्यागम axchg(&atom->counter, newval);
}

अचिन्हित पूर्णाङ्क atomic_raw_xchg_uint(चञ्चल अचिन्हित पूर्णाङ्क *ptr,
				  अचिन्हित पूर्णाङ्क newval)
{
	/* Atomically set new value and return old value. */
	प्रत्यागम axchg(ptr, newval);
}

अचिन्हित दीर्घ atomic_raw_xchg_ulong(चञ्चल अचिन्हित दीर्घ *ptr,
				    अचिन्हित दीर्घ newval)
{
	/* Atomically set new value and return old value. */
	प्रत्यागम axchg(ptr, newval);
}

पूर्णाङ्क atomic_raw_set_bit(पूर्णाङ्क nr, चञ्चल अचिन्हित दीर्घ *addr)
{
	अचिन्हित दीर्घ res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	प्रत्यागम res & mask ? 1 : 0;
}

पूर्णाङ्क atomic_raw_clear_bit(पूर्णाङ्क nr, चञ्चल अचिन्हित दीर्घ *addr)
{
	अचिन्हित दीर्घ res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	प्रत्यागम res & mask ? 1 : 0;
}

पूर्णाङ्क atomic_set_bit(पूर्णाङ्क nr, atomic_t *atom)
{
	प्रत्यागम atomic_raw_set_bit(nr, (अचिन्हित दीर्घ *)&atom->counter);
}

पूर्णाङ्क atomic_clear_bit(पूर्णाङ्क nr, atomic_t *atom)
{
	प्रत्यागम atomic_raw_clear_bit(nr, (अचिन्हित दीर्घ *)&atom->counter);
}
