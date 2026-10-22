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

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_trap_ldst.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_unpriv.h>
#అన్తర్భావ <sbi/sbi_vector.h>

#నిర్వచితమైతే OPENSBI_CC_SUPPORT_సదిశ

#నిర్వచించు ముసుగు_BUFFLEN 1024

స్థిర అంతఃవరుస శూన్య సమితి_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong కొలత, స్థిరరాశి uint8_t *బైట్లు)
{
	pos += (which % 8) * vlenb;
	బైట్లు -= pos;

	asm అస్థిర (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + కొలత));

	csr_write(CSR_VSTART, pos);

	ఎంపిక (which / 8) {
	స్థితి 0:
		asm అస్థిర (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (బైట్లు) : "memory");
		ఆపు;
	స్థితి 1:
		asm అస్థిర (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (బైట్లు) : "memory");
		ఆపు;
	స్థితి 2:
		asm అస్థిర (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v16,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (బైట్లు) : "memory");
		ఆపు;
	స్థితి 3:
		asm అస్థిర (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v24,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (బైట్లు) : "memory");
		ఆపు;
	అప్రమేయ:
		ఆపు;
	}
}

స్థిర అంతఃవరుస శూన్య get_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong కొలత, uint8_t *బైట్లు)
{
	pos += (which % 8) * vlenb;
	బైట్లు -= pos;

	asm అస్థిర (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + కొలత));

	csr_write(CSR_VSTART, pos);

	ఎంపిక (which / 8) {
	స్థితి 0:
		asm అస్థిర (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (బైట్లు) : "memory");
		ఆపు;
	స్థితి 1:
		asm అస్థిర (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (బైట్లు) : "memory");
		ఆపు;
	స్థితి 2:
		asm అస్థిర (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v16, (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (బైట్లు) : "memory");
		ఆపు;
	స్థితి 3:
		asm అస్థిర (
				".option push\n\t"
				".option arch, +v\n\t"
				"vse8.v v24, (%0)\n\t"
				".option pop\n\t"
				:: "r" (బైట్లు) : "memory");
		ఆపు;
	అప్రమేయ:
		ఆపు;
	}
}

స్థిర అంతఃవరుస శూన్య vsetvl(ulong vl, ulong vtype)
{
	asm అస్థిర (
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
స్థిర అంతఃవరుస శూన్య sbi_misaligned_v_tinst_fixup(నిర్మాణం sbi_trap_info *uptrap)
{
	/*
	 * The function is called in code path for handling a vector
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	ఒకవేళ (uptrap->tinst == INSN_PSEUDO_VS_ఎక్కించు ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		తిరిగి;

	uptrap->tinst = 0;
}

పూర్ణాంకం sbi_misaligned_v_ld_emulator(ulong insn, నిర్మాణం sbi_trap_context *tcntx)
{
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	నిర్మాణం sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong పునాది_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t ముసుగు[ముసుగు_BUFFLEN / 8];
	uint8_t బైట్లు[8 * పరిమాణం(uint64_t)];
	ulong ముసుగు_len = ముసుగు_BUFFLEN < vlenb * 8 ? ముసుగు_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	ఒకవేళ (IS_UNIT_STRIDE_ఎక్కించు(insn) || IS_FAULT_ONLY_మొదటి_ఎక్కించు(insn)) {
		stride = nf * len;
	} లేకపోతే ఒకవేళ (IS_సంపూర్ణ_REG_ఎక్కించు(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} లేకపోతే ఒకవేళ (IS_సూచీకృత_ఎక్కించు(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	ఒకవేళ (illegal) {
		నిర్మాణం sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		తిరిగి sbi_trap_redirect(regs, &trap);
	}

	చేయి {
		ఒకవేళ (masked) {
			ఒకవేళ (vstart == orig_vstart || vstart % ముసుగు_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / ముసుగు_len * ముసుగు_len,
					 ముసుగు_len, ముసుగు);

			ఒకవేళ (~ముసుగు[vstart % ముసుగు_len / 8] & బిట్టు(vstart % 8))
				కొనసాగు;
		}

		/* compute element address */
		ulong addr = పునాది_2 + vstart * stride;

		ఒకవేళ (IS_సూచీకృత_ఎక్కించు(insn)) {
			ulong స్థానభ్రంశం = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&స్థానభ్రంశం);
			addr = పునాది_2 + స్థానభ్రంశం;
		}

		csr_write(CSR_VSTART, vstart);

		/* obtain load data from memory */
		కొరకు (ulong seg = 0; seg < nf; seg++) {
			sbi_ఎక్కించు_loop(బైట్లు + seg * len,
				       addr + seg * len, len, &uptrap);

			ఒకవేళ (!uptrap.cause)
				కొనసాగు;

			ఒకవేళ (IS_FAULT_ONLY_మొదటి_ఎక్కించు(insn) && vstart != 0) {
				vl = vstart;
				వెళ్ళు పూర్తి;
			}

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			ఒకవేళ (vstart != orig_vstart)
				సమితి_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			తిరిగి sbi_trap_redirect(regs, &uptrap);
		}

		/* write load data to regfile */
		కొరకు (ulong seg = 0; seg < nf; seg++)
			సమితి_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &బైట్లు[seg * len]);
	} అయితే (++vstart < vl);

పూర్తి:
	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * At least 1 element is processed, or vl is changed above in
	 * the FAULT_ONLY_FIRST_LOAD path, thus set dirty.
	 */
	సమితి_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	తిరిగి 1;
}

పూర్ణాంకం sbi_misaligned_v_st_emulator(ulong insn, నిర్మాణం sbi_trap_context *tcntx)
{
	నిర్మాణం sbi_trap_regs *regs = &tcntx->regs;
	నిర్మాణం sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong పునాది_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t ముసుగు[ముసుగు_BUFFLEN / 8];
	uint8_t బైట్లు[8 * పరిమాణం(uint64_t)];
	ulong ముసుగు_len = ముసుగు_BUFFLEN < vlenb * 8 ? ముసుగు_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	ఒకవేళ (IS_UNIT_STRIDE_STORE(insn)) {
		stride = nf * len;
	} లేకపోతే ఒకవేళ (IS_సంపూర్ణ_REG_STORE(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} లేకపోతే ఒకవేళ (IS_సూచీకృత_STORE(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	ఒకవేళ (illegal) {
		నిర్మాణం sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		తిరిగి sbi_trap_redirect(regs, &trap);
	}

	చేయి {
		ఒకవేళ (masked) {
			ఒకవేళ (vstart == orig_vstart || vstart % ముసుగు_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / ముసుగు_len * ముసుగు_len,
					 ముసుగు_len, ముసుగు);

			ఒకవేళ (~ముసుగు[vstart % ముసుగు_len / 8] & బిట్టు(vstart % 8))
				కొనసాగు;
		}

		/* compute element address */
		ulong addr = పునాది_2 + vstart * stride;

		ఒకవేళ (IS_సూచీకృత_STORE(insn)) {
			ulong స్థానభ్రంశం = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&స్థానభ్రంశం);
			addr = పునాది_2 + స్థానభ్రంశం;
		}

		/* obtain store data from regfile */
		కొరకు (ulong seg = 0; seg < nf; seg++)
			get_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &బైట్లు[seg * len]);

		/* write store data to memory */
		కొరకు (ulong seg = 0; seg < nf; seg++) {
			sbi_store_loop(బైట్లు + seg * len,
					addr + seg * len, len, &uptrap);

			ఒకవేళ (!uptrap.cause)
				కొనసాగు;

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			ఒకవేళ (vstart != orig_vstart)
				సమితి_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			తిరిగి sbi_trap_redirect(regs, &uptrap);
		}
	} అయితే (++vstart < vl);

	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * No need to set dirty for memory store, but as VSTART resets to
	 * 0 above, need to set dirty if it's originally not 0.
	 */
	ఒకవేళ (orig_vstart != 0)
		సమితి_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	తిరిగి 1;
}
#లేకపోతే
పూర్ణాంకం sbi_misaligned_v_ld_emulator(ulong insn, నిర్మాణం sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	తిరిగి sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}

పూర్ణాంకం sbi_misaligned_v_st_emulator(ulong insn, నిర్మాణం sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	తిరిగి sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}
#అయితే_ముగింపు /* OPENSBI_CC_SUPPORT_VECTOR */
