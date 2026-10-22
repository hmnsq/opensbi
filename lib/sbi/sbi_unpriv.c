/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_unpriv.h>

ஒன்றிப்பு sbi_unpriv_data {
	u8 b;
	u16 h;
	u32 w;
#எனில் __riscv_xlen == 64
	u64 d;
#endif
	u8 bytes[__riscv_xlen / 8];
};

/**
 * a3 must a pointer to the sbi_trap_info and a4 is used as a temporary
 * register in the trap handler. Make sure that compiler doesn't use a3 & a4.
 */
#define DEFINE_UNPRIVILEGED_LOAD_FUNCTION(type, insn)                         \
	type sbi_load_##type(மாறிலி type *addr,                                \
			     கட்டமைப்பு sbi_trap_info *trap)                      \
	{                                                                     \
		பதிவு ulong tinfo asm("a3") = (ulong)trap;                 \
		பதிவு ulong mstatus = 0;                                   \
		பதிவு ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		type ret = 0;                                                 \
		trap->cause = 0;                                              \
		asm நிலையற்ற(                                                 \
			"csrrw %[mtvec], " STR(CSR_MTVEC) ", %[mtvec]\n"      \
			"csrrs %[mstatus], " STR(CSR_MSTATUS) ", %[mprv]\n"   \
			".option push\n"                                      \
			".option norvc\n"                                     \
			#insn " %[ret], %[addr]\n"                            \
			".option pop\n"                                       \
			"csrw " STR(CSR_MSTATUS) ", %[mstatus]\n"             \
			"csrw " STR(CSR_MTVEC) ", %[mtvec]"                   \
		    : [mstatus] "+&r"(mstatus), [mtvec] "+&r"(mtvec),         \
		      [tinfo] "+&r"(tinfo), [ret] "=&r"(ret)                  \
		    : [addr] "m"(*addr), [mprv] "r"(MSTATUS_MPRV)             \
		    : "a4", "memory");                                        \
		திரும்பு ret;                                                   \
	}

#define DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, insn)                        \
	வெறுமை sbi_store_##type(type *addr, type val,                           \
			      கட்டமைப்பு sbi_trap_info *trap)                     \
	{                                                                     \
		பதிவு ulong tinfo asm("a3") = (ulong)trap;                 \
		பதிவு ulong mstatus = 0;                                   \
		பதிவு ulong mtvec = (ulong)sbi_hart_expected_trap;         \
		trap->cause = 0;                                              \
		asm நிலையற்ற(                                                 \
			"csrrw %[mtvec], " STR(CSR_MTVEC) ", %[mtvec]\n"      \
			"csrrs %[mstatus], " STR(CSR_MSTATUS) ", %[mprv]\n"   \
			".option push\n"                                      \
			".option norvc\n"                                     \
			#insn " %[val], %[addr]\n"                            \
			".option pop\n"                                       \
			"csrw " STR(CSR_MSTATUS) ", %[mstatus]\n"             \
			"csrw " STR(CSR_MTVEC) ", %[mtvec]"                   \
		    : [mstatus] "+&r"(mstatus), [mtvec] "+&r"(mtvec),         \
		      [tinfo] "+&r"(tinfo)                                    \
		    : [addr] "m"(*addr), [mprv] "r"(MSTATUS_MPRV),            \
		      [val] "r"(val)                                          \
		    : "a4", "memory");                                        \
	}

DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u8, lbu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u16, lhu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s8, lb)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s16, lh)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s32, lw)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, sb)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, sh)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, sw)
#எனில் __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lwu)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u64, ld)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, sd)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, ld)
#அல்லதெனில் __riscv_xlen == 32
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, lw)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, lw)

u64 sbi_load_u64(மாறிலி u64 *addr,
		 கட்டமைப்பு sbi_trap_info *trap)
{
	u64 ret = sbi_load_u32((u32 *)addr, trap);

	எனில் (trap->cause)
		திரும்பு 0;
	ret |= ((u64)sbi_load_u32((u32 *)addr + 1, trap) << 32);
	எனில் (trap->cause)
		திரும்பு 0;

	திரும்பு ret;
}

வெறுமை sbi_store_u64(u64 *addr, u64 val,
		   கட்டமைப்பு sbi_trap_info *trap)
{
	sbi_store_u32((u32 *)addr, val, trap);
	எனில் (trap->cause)
		திரும்பு;

	sbi_store_u32((u32 *)addr + 1, val >> 32, trap);
	எனில் (trap->cause)
		திரும்பு;
}
#இல்லையெனில்
# error "Unexpected __riscv_xlen"
#endif

வெறுமை sbi_load_loop(u8 *buffer, ulong addr, ulong len,
		   கட்டமைப்பு sbi_trap_info *trap)
{
	ஒன்றிப்பு sbi_unpriv_data data;

	trap->cause = 0;
	போது (len) {
		unsigned முழு width = __riscv_xlen / 8;
		வெறுமை *ptr = (வெறுமை*)addr;

		போது (len < width || (addr & (width - 1)))
			width /= 2;

		தேர்வு (width) {
		நிலை 1:
			data.b = sbi_load_u8(ptr, trap);
			நிறுத்து;
		நிலை 2:
			data.h = sbi_load_u16(ptr, trap);
			நிறுத்து;
		நிலை 4:
			data.w = sbi_load_u32(ptr, trap);
			நிறுத்து;
#எனில் __riscv_xlen == 64
		நிலை 8:
			data.d = sbi_load_u64(ptr, trap);
			நிறுத்து;
#endif
		}
		எனில் (trap->cause)
			திரும்பு;

		sbi_memcpy(buffer, data.bytes, width);
		len -= width;
		addr += width;
		buffer += width;
	}
}

வெறுமை sbi_store_loop(u8 *buffer, ulong addr, ulong len,
		    கட்டமைப்பு sbi_trap_info *trap)
{
	ஒன்றிப்பு sbi_unpriv_data data;

	trap->cause = 0;
	போது (len) {
		unsigned முழு width = __riscv_xlen / 8;
		வெறுமை *ptr = (வெறுமை*)addr;

		போது (len < width || (addr & (width - 1)))
			width /= 2;

		sbi_memcpy(data.bytes, buffer, width);
		தேர்வு (width) {
		நிலை 1:
			sbi_store_u8(ptr, data.b, trap);
			நிறுத்து;
		நிலை 2:
			sbi_store_u16(ptr, data.h, trap);
			நிறுத்து;
		நிலை 4:
			sbi_store_u32(ptr, data.w, trap);
			நிறுத்து;
#எனில் __riscv_xlen == 64
		நிலை 8:
			sbi_store_u64(ptr, data.d, trap);
			நிறுத்து;
#endif
		}
		எனில் (trap->cause)
			திரும்பு;

		len -= width;
		addr += width;
		buffer += width;
	}
}

ulong sbi_get_insn(ulong mepc, கட்டமைப்பு sbi_trap_info *trap)
{
	பதிவு ulong tinfo asm("a3");
	பதிவு ulong ttmp asm("a4");
	பதிவு ulong mstatus = 0;
	பதிவு ulong mtvec = (ulong)sbi_hart_expected_trap;
	ulong insn = 0;

	trap->cause = 0;

	asm நிலையற்ற(
	    "add %[tinfo], %[taddr], zero\n"
	    "csrrw %[mtvec], " STR(CSR_MTVEC) ", %[mtvec]\n"
	    "csrrs %[mstatus], " STR(CSR_MSTATUS) ", %[mprv]\n"
	    "lhu %[insn], (%[addr])\n"
	    "andi %[ttmp], %[insn], 3\n"
	    "addi %[ttmp], %[ttmp], -3\n"
	    "bne %[ttmp], zero, 2f\n"
	    "lhu %[ttmp], 2(%[addr])\n"
	    "sll %[ttmp], %[ttmp], 16\n"
	    "add %[insn], %[insn], %[ttmp]\n"
	    "2: csrw " STR(CSR_MSTATUS) ", %[mstatus]\n"
	    "csrw " STR(CSR_MTVEC) ", %[mtvec]"
	    : [mstatus] "+&r"(mstatus), [mtvec] "+&r"(mtvec),
	      [tinfo] "+&r"(tinfo), [ttmp] "+&r"(ttmp),
	      [insn] "=&r"(insn)
	    : [mprv] "r"(MSTATUS_MPRV | MSTATUS_MXR),
	      [taddr] "r"((ulong)trap), [addr] "r"(mepc)
	    : "memory");

	தேர்வு (trap->cause) {
	நிலை CAUSE_LOAD_ACCESS:
		trap->cause = CAUSE_FETCH_ACCESS;
		trap->tinst = 0UL;
		நிறுத்து;
	நிலை CAUSE_LOAD_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_PAGE_FAULT;
		trap->tinst = 0UL;
		நிறுத்து;
	நிலை CAUSE_LOAD_GUEST_PAGE_FAULT:
		trap->cause = CAUSE_FETCH_GUEST_PAGE_FAULT;
		எனில் (trap->tinst != INSN_PSEUDO_VS_LOAD &&
		    trap->tinst != INSN_PSEUDO_VS_STORE)
			trap->tinst = 0UL;
		நிறுத்து;
	இயல்பு:
		நிறுத்து;
	}

	திரும்பு insn;
}
