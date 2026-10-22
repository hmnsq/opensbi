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

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap_ldst.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_unpriv.h>
#अन्तर्भाव <sbi/sbi_vector.h>

#यदि_परिभाषितम् OPENSBI_CC_SUPPORT_सदिशम्

#परिभाषय आवरणम्_BUFFLEN 1024

स्थिर अन्तःस्थ शून्य समुच्चयः_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong आकार, ध्रुव uint8_t *अष्टकानि)
{
	pos += (which % 8) * vlenb;
	अष्टकानि -= pos;

	asm चञ्चल (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + आकार));

	csr_write(CSR_VSTART, pos);

	विकल्प (which / 8) {
	पक्ष 0:
		asm चञ्चल (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (अष्टकानि) : "memory");
		भङ्ग;
	पक्ष 1:
		asm चञ्चल (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (अष्टकानि) : "memory");
		भङ्ग;
	पक्ष 2:
		asm चञ्चल (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v16,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (अष्टकानि) : "memory");
		भङ्ग;
	पक्ष 3:
		asm चञ्चल (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v24,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (अष्टकानि) : "memory");
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}
}

स्थिर अन्तःस्थ शून्य get_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong आकार, uint8_t *अष्टकानि)
{
	pos += (which % 8) * vlenb;
	अष्टकानि -= pos;

	asm चञ्चल (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + आकार));

	csr_write(CSR_VSTART, pos);

	विकल्प (which / 8) {
	पक्ष 0:
		asm चञ्चल (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (अष्टकानि) : "memory");
		भङ्ग;
	पक्ष 1:
		asm चञ्चल (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (अष्टकानि) : "memory");
		भङ्ग;
	पक्ष 2:
		asm चञ्चल (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v16, (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (अष्टकानि) : "memory");
		भङ्ग;
	पक्ष 3:
		asm चञ्चल (
				".option push\n\t"
				".option arch, +v\n\t"
				"vse8.v v24, (%0)\n\t"
				".option pop\n\t"
				:: "r" (अष्टकानि) : "memory");
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}
}

स्थिर अन्तःस्थ शून्य vsetvl(ulong vl, ulong vtype)
{
	asm चञ्चल (
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
स्थिर अन्तःस्थ शून्य sbi_misaligned_v_tinst_fixup(संरचना sbi_trap_info *uptrap)
{
	/*
	 * The function is called in code path for handling a vector
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	यदि (uptrap->tinst == INSN_PSEUDO_VS_आरोपय ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		प्रत्यागम;

	uptrap->tinst = 0;
}

पूर्णाङ्क sbi_misaligned_v_ld_emulator(ulong insn, संरचना sbi_trap_context *tcntx)
{
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong आधारः_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t आवरणम्[आवरणम्_BUFFLEN / 8];
	uint8_t अष्टकानि[8 * परिमाण(uint64_t)];
	ulong आवरणम्_len = आवरणम्_BUFFLEN < vlenb * 8 ? आवरणम्_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	यदि (IS_UNIT_STRIDE_आरोपय(insn) || IS_FAULT_ONLY_FIRST_आरोपय(insn)) {
		stride = nf * len;
	} अन्यथा यदि (IS_सम्पूर्णम्_REG_आरोपय(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} अन्यथा यदि (IS_अनुक्रमितम्_आरोपय(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	यदि (illegal) {
		संरचना sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		प्रत्यागम sbi_trap_redirect(regs, &trap);
	}

	कुरु {
		यदि (masked) {
			यदि (vstart == orig_vstart || vstart % आवरणम्_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / आवरणम्_len * आवरणम्_len,
					 आवरणम्_len, आवरणम्);

			यदि (~आवरणम्[vstart % आवरणम्_len / 8] & BIT(vstart % 8))
				अनुवर्त;
		}

		/* compute element address */
		ulong addr = आधारः_2 + vstart * stride;

		यदि (IS_अनुक्रमितम्_आरोपय(insn)) {
			ulong विस्थापनम् = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&विस्थापनम्);
			addr = आधारः_2 + विस्थापनम्;
		}

		csr_write(CSR_VSTART, vstart);

		/* obtain load data from memory */
		प्रति (ulong seg = 0; seg < nf; seg++) {
			sbi_आरोपय_loop(अष्टकानि + seg * len,
				       addr + seg * len, len, &uptrap);

			यदि (!uptrap.cause)
				अनुवर्त;

			यदि (IS_FAULT_ONLY_FIRST_आरोपय(insn) && vstart != 0) {
				vl = vstart;
				गच्छ समाप्तम्;
			}

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			यदि (vstart != orig_vstart)
				समुच्चयः_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			प्रत्यागम sbi_trap_redirect(regs, &uptrap);
		}

		/* write load data to regfile */
		प्रति (ulong seg = 0; seg < nf; seg++)
			समुच्चयः_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &अष्टकानि[seg * len]);
	} यावत् (++vstart < vl);

समाप्तम्:
	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * At least 1 element is processed, or vl is changed above in
	 * the FAULT_ONLY_FIRST_LOAD path, thus set dirty.
	 */
	समुच्चयः_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	प्रत्यागम 1;
}

पूर्णाङ्क sbi_misaligned_v_st_emulator(ulong insn, संरचना sbi_trap_context *tcntx)
{
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	संरचना sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong आधारः_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t आवरणम्[आवरणम्_BUFFLEN / 8];
	uint8_t अष्टकानि[8 * परिमाण(uint64_t)];
	ulong आवरणम्_len = आवरणम्_BUFFLEN < vlenb * 8 ? आवरणम्_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	यदि (IS_UNIT_STRIDE_STORE(insn)) {
		stride = nf * len;
	} अन्यथा यदि (IS_सम्पूर्णम्_REG_STORE(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} अन्यथा यदि (IS_अनुक्रमितम्_STORE(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	यदि (illegal) {
		संरचना sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		प्रत्यागम sbi_trap_redirect(regs, &trap);
	}

	कुरु {
		यदि (masked) {
			यदि (vstart == orig_vstart || vstart % आवरणम्_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / आवरणम्_len * आवरणम्_len,
					 आवरणम्_len, आवरणम्);

			यदि (~आवरणम्[vstart % आवरणम्_len / 8] & BIT(vstart % 8))
				अनुवर्त;
		}

		/* compute element address */
		ulong addr = आधारः_2 + vstart * stride;

		यदि (IS_अनुक्रमितम्_STORE(insn)) {
			ulong विस्थापनम् = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&विस्थापनम्);
			addr = आधारः_2 + विस्थापनम्;
		}

		/* obtain store data from regfile */
		प्रति (ulong seg = 0; seg < nf; seg++)
			get_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &अष्टकानि[seg * len]);

		/* write store data to memory */
		प्रति (ulong seg = 0; seg < nf; seg++) {
			sbi_store_loop(अष्टकानि + seg * len,
					addr + seg * len, len, &uptrap);

			यदि (!uptrap.cause)
				अनुवर्त;

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			यदि (vstart != orig_vstart)
				समुच्चयः_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			प्रत्यागम sbi_trap_redirect(regs, &uptrap);
		}
	} यावत् (++vstart < vl);

	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * No need to set dirty for memory store, but as VSTART resets to
	 * 0 above, need to set dirty if it's originally not 0.
	 */
	यदि (orig_vstart != 0)
		समुच्चयः_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	प्रत्यागम 1;
}
#अन्यथा
पूर्णाङ्क sbi_misaligned_v_ld_emulator(ulong insn, संरचना sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	प्रत्यागम sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}

पूर्णाङ्क sbi_misaligned_v_st_emulator(ulong insn, संरचना sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	प्रत्यागम sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}
#यदि_अन्तः /* OPENSBI_CC_SUPPORT_VECTOR */
