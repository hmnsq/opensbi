/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_atomic.h>
#समावेश <sbi/riscv_barrier.h>

#अगर !defined(__riscv_atomic) && !defined(__riscv_zaamo) && !defined(__riscv_zalrsc)
#त्रुटि "opensbi strongly relies on the Zaamo or Zalrsc extensions of RISC-V"
#पूर्ण_अगर

दीर्घ atomic_read(atomic_t *atom)
{
	दीर्घ ret = atom->गणक;
	rmb();
	वापस ret;
}

व्योम atomic_write(atomic_t *atom, दीर्घ मूल्य)
{
	atom->गणक = मूल्य;
	wmb();
}

दीर्घ atomic_add_return(atomic_t *atom, दीर्घ मूल्य)
{
#अगर defined(__riscv_atomic) || defined(__riscv_zaamo)
	दीर्घ ret;
#अगर __SIZEOF_LONG__ == 4
	__asm__ __volatile__("	amoadd.w.aqrl  %1, %2, %0"
			     : "+A"(atom->गणक), "=r"(ret)
			     : "r"(मूल्य)
			     : "memory");
#या_अगर __SIZEOF_LONG__ == 8
	__asm__ __volatile__("	amoadd.d.aqrl  %1, %2, %0"
			     : "+A"(atom->गणक), "=r"(ret)
			     : "r"(मूल्य)
			     : "memory");
#पूर्ण_अगर
#या_अगर defined(__riscv_zalrsc)
	दीर्घ ret, अस्थायी;
#अगर __SIZEOF_LONG__ == 4
	__asm__ __volatile__("1:lr.w.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.w.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->गणक), "=&r"(ret), "=&r"(अस्थायी)
			     : "r"(मूल्य)
			     : "memory");
#या_अगर __SIZEOF_LONG__ == 8
	__asm__ __volatile__("1:lr.d.aqrl	%1,%0\n"
			     "  add	%2,%1,%3\n"
			     "  sc.d.aqrl	%2,%2,%0\n"
			     "  bnez	%2,1b"
			     : "+A"(atom->गणक), "=&r"(ret), "=&r"(अस्थायी)
			     : "r"(मूल्य)
			     : "memory");
#पूर्ण_अगर
#अन्यथा
#त्रुटि "need A or Zaamo or Zalrsc"
#पूर्ण_अगर

	वापस ret + मूल्य;
}

दीर्घ atomic_sub_return(atomic_t *atom, दीर्घ मूल्य)
{
	वापस atomic_add_return(atom, -मूल्य);
}

#अगर defined(__riscv_atomic) || defined(__riscv_zaamo)
#घोषणा __axchg(ptr, new, आकार)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret;					\
		चयन (आकार) {							\
		हाल 4:								\
			__asm__ __volatile__ (					\
				"	amoswap.w.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			अवरोध;							\
		हाल 8:								\
			__asm__ __volatile__ (					\
				"	amoswap.d.aqrl %0, %2, %1\n"		\
				: "=r" (__ret), "+A" (*__ptr)			\
				: "r" (__new)					\
				: "memory");					\
			अवरोध;							\
		शेष:							\
			अवरोध;							\
		}								\
		__ret;								\
	})
#या_अगर defined(__riscv_zalrsc)
#घोषणा __axchg(ptr, new, आकार)							\
	({									\
		__typeof__(ptr) __ptr = (ptr);					\
		__typeof__(new) __new = (new);					\
		__typeof__(*(ptr)) __ret, __temp;					\
		चयन (आकार) {							\
		हाल 4:								\
			__asm__ __volatile__ (					\
				"1:	lr.w.aqrl %0, %1\n"			\
				"	sc.w.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			अवरोध;							\
		हाल 8:								\
			__asm__ __volatile__ (					\
				"1:	lr.d.aqrl %0, %1\n"			\
				"	sc.d.aqrl %2, %3, %1\n"			\
				"	bnez	  %2, 1b\n"			\
				: "=&r" (__ret), "+A" (*__ptr), "=&r" (__temp)	\
				: "r" (__new)					\
				: "memory");					\
			अवरोध;							\
		शेष:							\
			अवरोध;							\
		}								\
		__ret;								\
	})
#अन्यथा
#त्रुटि "need A or Zaamo or Zalrsc"
#पूर्ण_अगर

#घोषणा axchg(ptr, x)								\
	({									\
		__typeof__(*(ptr)) _x_ = (x);					\
		(__typeof__(*(ptr))) __axchg((ptr), _x_, माप(*(ptr)));	\
	})

दीर्घ atomic_cmpxchg(atomic_t *atom, दीर्घ oldval, दीर्घ newval)
{
	वापस __sync_val_compare_and_swap(&atom->गणक, oldval, newval);
}

दीर्घ atomic_xchg(atomic_t *atom, दीर्घ newval)
{
	/* Atomically set new value and return old value. */
	वापस axchg(&atom->गणक, newval);
}

अचिन्हित पूर्णांक atomic_raw_xchg_uint(अस्थिर अचिन्हित पूर्णांक *ptr,
				  अचिन्हित पूर्णांक newval)
{
	/* Atomically set new value and return old value. */
	वापस axchg(ptr, newval);
}

अचिन्हित दीर्घ atomic_raw_xchg_ulong(अस्थिर अचिन्हित दीर्घ *ptr,
				    अचिन्हित दीर्घ newval)
{
	/* Atomically set new value and return old value. */
	वापस axchg(ptr, newval);
}

पूर्णांक atomic_raw_set_bit(पूर्णांक nr, अस्थिर अचिन्हित दीर्घ *addr)
{
	अचिन्हित दीर्घ res, mask = BIT_MASK(nr);
	res = __atomic_fetch_or(&addr[BIT_WORD(nr)], mask, __ATOMIC_RELAXED);
	वापस res & mask ? 1 : 0;
}

पूर्णांक atomic_raw_clear_bit(पूर्णांक nr, अस्थिर अचिन्हित दीर्घ *addr)
{
	अचिन्हित दीर्घ res, mask = BIT_MASK(nr);
	res = __atomic_fetch_and(&addr[BIT_WORD(nr)], ~mask, __ATOMIC_RELAXED);
	वापस res & mask ? 1 : 0;
}

पूर्णांक atomic_set_bit(पूर्णांक nr, atomic_t *atom)
{
	वापस atomic_raw_set_bit(nr, (अचिन्हित दीर्घ *)&atom->गणक);
}

पूर्णांक atomic_clear_bit(पूर्णांक nr, atomic_t *atom)
{
	वापस atomic_raw_clear_bit(nr, (अचिन्हित दीर्घ *)&atom->गणक);
}
