/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 SiFive Inc.
 *
 * Authors:
 *   Andrew Waterman <andrew@sifive.com>
 *   Nylon Chen <nylon.chen@sifive.com>
 *   Zong Li <nylon.chen@sifive.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap_ldst.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_unpriv.h>
#ضمن <sbi/sbi_vector.h>

#اگر_تعریف_شده OPENSBI_CC_SUPPORT_وکتور

#تعریف_کن ماسک_BUFFLEN 1024

ساكن مضمن فراغ مجموعه_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong اندازه, ثابت uint8_t *بایتها)
{
	pos += (which % 8) * vlenb;
	بایتها -= pos;

	asm متطاير (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + اندازه));

	csr_write(CSR_VSTART, pos);

	بدل (which / 8) {
	حالة 0:
		asm متطاير (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بایتها) : "memory");
		اكسر;
	حالة 1:
		asm متطاير (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بایتها) : "memory");
		اكسر;
	حالة 2:
		asm متطاير (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v16,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بایتها) : "memory");
		اكسر;
	حالة 3:
		asm متطاير (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v24,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بایتها) : "memory");
		اكسر;
	افتراضي:
		اكسر;
	}
}

ساكن مضمن فراغ get_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong اندازه, uint8_t *بایتها)
{
	pos += (which % 8) * vlenb;
	بایتها -= pos;

	asm متطاير (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + اندازه));

	csr_write(CSR_VSTART, pos);

	بدل (which / 8) {
	حالة 0:
		asm متطاير (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بایتها) : "memory");
		اكسر;
	حالة 1:
		asm متطاير (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بایتها) : "memory");
		اكسر;
	حالة 2:
		asm متطاير (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v16, (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بایتها) : "memory");
		اكسر;
	حالة 3:
		asm متطاير (
				".option push\n\t"
				".option arch, +v\n\t"
				"vse8.v v24, (%0)\n\t"
				".option pop\n\t"
				:: "r" (بایتها) : "memory");
		اكسر;
	افتراضي:
		اكسر;
	}
}

ساكن مضمن فراغ vsetvl(ulong vl, ulong vtype)
{
	asm متطاير (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvl x0, %0, %1\n\t"
		"	.option pop\n\t"
			:: "r" (vl), "r" (vtype));
}

/**
 * Handling of misaligned fault is done by a collection of smaller, but
 * aligned load/store(s). Another fault (load/store, page fault...) can
 * arise from any of them, then the handling gets aborted. We must fixup
 * the tinst to pretend the fault was rised from the original insn. For
 * vector insn, simply null out tinst if it's not a guest-page fault, as
 * there's no transformed insn for vector load/store
 */
ساكن مضمن فراغ sbi_misaligned_v_tinst_fixup(بنية sbi_trap_info *uptrap)
{
	/*
	 * The function is called in code path for handling a vector
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	إذا (uptrap->tinst == INSN_PSEUDO_VS_بارگیری ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		أرجع;

	uptrap->tinst = 0;
}

صحيح sbi_misaligned_v_ld_emulator(ulong insn, بنية sbi_trap_context *tcntx)
{
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong پایه_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t ماسک[ماسک_BUFFLEN / 8];
	uint8_t بایتها[8 * حجم(uint64_t)];
	ulong ماسک_len = ماسک_BUFFLEN < vlenb * 8 ? ماسک_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	إذا (IS_UNIT_STRIDE_بارگیری(insn) || IS_FAULT_ONLY_اولی_بارگیری(insn)) {
		stride = nf * len;
	} وإلا إذا (IS_کامل_REG_بارگیری(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} وإلا إذا (IS_نمایهای_بارگیری(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	إذا (illegal) {
		بنية sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		أرجع sbi_trap_redirect(regs, &trap);
	}

	افعل {
		إذا (masked) {
			إذا (vstart == orig_vstart || vstart % ماسک_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / ماسک_len * ماسک_len,
					 ماسک_len, ماسک);

			إذا (~ماسک[vstart % ماسک_len / 8] & بیت(vstart % 8))
				استمر;
		}

		/* compute element address */
		ulong addr = پایه_2 + vstart * stride;

		إذا (IS_نمایهای_بارگیری(insn)) {
			ulong جابه_جایی = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&جابه_جایی);
			addr = پایه_2 + جابه_جایی;
		}

		csr_write(CSR_VSTART, vstart);

		/* obtain load data from memory */
		لكل (ulong seg = 0; seg < nf; seg++) {
			sbi_بارگیری_loop(بایتها + seg * len,
				       addr + seg * len, len, &uptrap);

			إذا (!uptrap.cause)
				استمر;

			إذا (IS_FAULT_ONLY_اولی_بارگیری(insn) && vstart != 0) {
				vl = vstart;
				اذهب انجامشده;
			}

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			إذا (vstart != orig_vstart)
				مجموعه_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			أرجع sbi_trap_redirect(regs, &uptrap);
		}

		/* write load data to regfile */
		لكل (ulong seg = 0; seg < nf; seg++)
			مجموعه_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &بایتها[seg * len]);
	} طالما (++vstart < vl);

انجامشده:
	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * At least 1 element is processed, or vl is changed above in
	 * the FAULT_ONLY_FIRST_LOAD path, thus set dirty.
	 */
	مجموعه_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	أرجع 1;
}

صحيح sbi_misaligned_v_st_emulator(ulong insn, بنية sbi_trap_context *tcntx)
{
	بنية sbi_trap_regs *regs = &tcntx->regs;
	بنية sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong پایه_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t ماسک[ماسک_BUFFLEN / 8];
	uint8_t بایتها[8 * حجم(uint64_t)];
	ulong ماسک_len = ماسک_BUFFLEN < vlenb * 8 ? ماسک_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	إذا (IS_UNIT_STRIDE_STORE(insn)) {
		stride = nf * len;
	} وإلا إذا (IS_کامل_REG_STORE(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} وإلا إذا (IS_نمایهای_STORE(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	إذا (illegal) {
		بنية sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		أرجع sbi_trap_redirect(regs, &trap);
	}

	افعل {
		إذا (masked) {
			إذا (vstart == orig_vstart || vstart % ماسک_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / ماسک_len * ماسک_len,
					 ماسک_len, ماسک);

			إذا (~ماسک[vstart % ماسک_len / 8] & بیت(vstart % 8))
				استمر;
		}

		/* compute element address */
		ulong addr = پایه_2 + vstart * stride;

		إذا (IS_نمایهای_STORE(insn)) {
			ulong جابه_جایی = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&جابه_جایی);
			addr = پایه_2 + جابه_جایی;
		}

		/* obtain store data from regfile */
		لكل (ulong seg = 0; seg < nf; seg++)
			get_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &بایتها[seg * len]);

		/* write store data to memory */
		لكل (ulong seg = 0; seg < nf; seg++) {
			sbi_store_loop(بایتها + seg * len,
					addr + seg * len, len, &uptrap);

			إذا (!uptrap.cause)
				استمر;

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			إذا (vstart != orig_vstart)
				مجموعه_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			أرجع sbi_trap_redirect(regs, &uptrap);
		}
	} طالما (++vstart < vl);

	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * No need to set dirty for memory store, but as VSTART resets to
	 * 0 above, need to set dirty if it's originally not 0.
	 */
	إذا (orig_vstart != 0)
		مجموعه_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	أرجع 1;
}
#وإلا
صحيح sbi_misaligned_v_ld_emulator(ulong insn, بنية sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	أرجع sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}

صحيح sbi_misaligned_v_st_emulator(ulong insn, بنية sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	أرجع sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}
#پایان_اگر /* OPENSBI_CC_SUPPORT_VECTOR */
