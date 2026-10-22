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

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap_ldst.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_unpriv.h>
#شامل <sbi/sbi_vector.h>

#اگر_تعریف_شدہ OPENSBI_CC_SUPPORT_سمتیہ

#تعریف_کرو نقاب_BUFFLEN 1024

ساکن اندرونی خالی سیٹ_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong سائز, ثابت uint8_t *بائٹیں)
{
	pos += (which % 8) * vlenb;
	بائٹیں -= pos;

	asm متلون (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + سائز));

	csr_write(CSR_VSTART, pos);

	انتخاب (which / 8) {
	حالت 0:
		asm متلون (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بائٹیں) : "memory");
		توڑو;
	حالت 1:
		asm متلون (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بائٹیں) : "memory");
		توڑو;
	حالت 2:
		asm متلون (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v16,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بائٹیں) : "memory");
		توڑو;
	حالت 3:
		asm متلون (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v24,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بائٹیں) : "memory");
		توڑو;
	طےشدہ:
		توڑو;
	}
}

ساکن اندرونی خالی get_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong سائز, uint8_t *بائٹیں)
{
	pos += (which % 8) * vlenb;
	بائٹیں -= pos;

	asm متلون (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + سائز));

	csr_write(CSR_VSTART, pos);

	انتخاب (which / 8) {
	حالت 0:
		asm متلون (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بائٹیں) : "memory");
		توڑو;
	حالت 1:
		asm متلون (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بائٹیں) : "memory");
		توڑو;
	حالت 2:
		asm متلون (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v16, (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (بائٹیں) : "memory");
		توڑو;
	حالت 3:
		asm متلون (
				".option push\n\t"
				".option arch, +v\n\t"
				"vse8.v v24, (%0)\n\t"
				".option pop\n\t"
				:: "r" (بائٹیں) : "memory");
		توڑو;
	طےشدہ:
		توڑو;
	}
}

ساکن اندرونی خالی vsetvl(ulong vl, ulong vtype)
{
	asm متلون (
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
ساکن اندرونی خالی sbi_misaligned_v_tinst_fixup(ساخت sbi_trap_info *uptrap)
{
	/*
	 * The function is called in code path for handling a vector
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	اگر (uptrap->tinst == INSN_PSEUDO_VS_بھرو ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		واپس;

	uptrap->tinst = 0;
}

صحیح sbi_misaligned_v_ld_emulator(ulong insn, ساخت sbi_trap_context *tcntx)
{
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ساخت sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong بنیاد_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t نقاب[نقاب_BUFFLEN / 8];
	uint8_t بائٹیں[8 * حجم(uint64_t)];
	ulong نقاب_len = نقاب_BUFFLEN < vlenb * 8 ? نقاب_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	اگر (IS_UNIT_STRIDE_بھرو(insn) || IS_FAULT_ONLY_پہلا_بھرو(insn)) {
		stride = nf * len;
	} ورنہ اگر (IS_پورا_REG_بھرو(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} ورنہ اگر (IS_اشاریہ_دار_بھرو(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	اگر (illegal) {
		ساخت sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		واپس sbi_trap_redirect(regs, &trap);
	}

	کرو {
		اگر (masked) {
			اگر (vstart == orig_vstart || vstart % نقاب_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / نقاب_len * نقاب_len,
					 نقاب_len, نقاب);

			اگر (~نقاب[vstart % نقاب_len / 8] & ثنائی_ہندسہ(vstart % 8))
				جاری;
		}

		/* compute element address */
		ulong addr = بنیاد_2 + vstart * stride;

		اگر (IS_اشاریہ_دار_بھرو(insn)) {
			ulong انحراف = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&انحراف);
			addr = بنیاد_2 + انحراف;
		}

		csr_write(CSR_VSTART, vstart);

		/* obtain load data from memory */
		برائے (ulong seg = 0; seg < nf; seg++) {
			sbi_بھرو_loop(بائٹیں + seg * len,
				       addr + seg * len, len, &uptrap);

			اگر (!uptrap.cause)
				جاری;

			اگر (IS_FAULT_ONLY_پہلا_بھرو(insn) && vstart != 0) {
				vl = vstart;
				جاؤ مکمل;
			}

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			اگر (vstart != orig_vstart)
				سیٹ_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			واپس sbi_trap_redirect(regs, &uptrap);
		}

		/* write load data to regfile */
		برائے (ulong seg = 0; seg < nf; seg++)
			سیٹ_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &بائٹیں[seg * len]);
	} جب (++vstart < vl);

مکمل:
	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * At least 1 element is processed, or vl is changed above in
	 * the FAULT_ONLY_FIRST_LOAD path, thus set dirty.
	 */
	سیٹ_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	واپس 1;
}

صحیح sbi_misaligned_v_st_emulator(ulong insn, ساخت sbi_trap_context *tcntx)
{
	ساخت sbi_trap_regs *regs = &tcntx->regs;
	ساخت sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong بنیاد_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t نقاب[نقاب_BUFFLEN / 8];
	uint8_t بائٹیں[8 * حجم(uint64_t)];
	ulong نقاب_len = نقاب_BUFFLEN < vlenb * 8 ? نقاب_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	اگر (IS_UNIT_STRIDE_STORE(insn)) {
		stride = nf * len;
	} ورنہ اگر (IS_پورا_REG_STORE(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} ورنہ اگر (IS_اشاریہ_دار_STORE(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	اگر (illegal) {
		ساخت sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		واپس sbi_trap_redirect(regs, &trap);
	}

	کرو {
		اگر (masked) {
			اگر (vstart == orig_vstart || vstart % نقاب_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / نقاب_len * نقاب_len,
					 نقاب_len, نقاب);

			اگر (~نقاب[vstart % نقاب_len / 8] & ثنائی_ہندسہ(vstart % 8))
				جاری;
		}

		/* compute element address */
		ulong addr = بنیاد_2 + vstart * stride;

		اگر (IS_اشاریہ_دار_STORE(insn)) {
			ulong انحراف = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&انحراف);
			addr = بنیاد_2 + انحراف;
		}

		/* obtain store data from regfile */
		برائے (ulong seg = 0; seg < nf; seg++)
			get_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &بائٹیں[seg * len]);

		/* write store data to memory */
		برائے (ulong seg = 0; seg < nf; seg++) {
			sbi_store_loop(بائٹیں + seg * len,
					addr + seg * len, len, &uptrap);

			اگر (!uptrap.cause)
				جاری;

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			اگر (vstart != orig_vstart)
				سیٹ_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			واپس sbi_trap_redirect(regs, &uptrap);
		}
	} جب (++vstart < vl);

	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * No need to set dirty for memory store, but as VSTART resets to
	 * 0 above, need to set dirty if it's originally not 0.
	 */
	اگر (orig_vstart != 0)
		سیٹ_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	واپس 1;
}
#ورنہ
صحیح sbi_misaligned_v_ld_emulator(ulong insn, ساخت sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	واپس sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}

صحیح sbi_misaligned_v_st_emulator(ulong insn, ساخت sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	واپس sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}
#اگر_ختم /* OPENSBI_CC_SUPPORT_VECTOR */
