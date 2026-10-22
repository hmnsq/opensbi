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

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap_ldst.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_unpriv.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_vector.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSBI_CC_SUPPORT_ಸದಿಶ

#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_BUFFLEN 1024

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು)
{
	pos += (which % 8) * vlenb;
	ಬೈಟುಗಳು -= pos;

	asm ಅಸ್ಥಿರ (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + ಗಾತ್ರ_2));

	csr_write(CSR_VSTART, pos);

	ಆಯ್ಕೆ (which / 8) {
	ಸ್ಥಿತಿ 0:
		asm ಅಸ್ಥಿರ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ಬೈಟುಗಳು) : "memory");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 1:
		asm ಅಸ್ಥಿರ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ಬೈಟುಗಳು) : "memory");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2:
		asm ಅಸ್ಥಿರ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v16,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ಬೈಟುಗಳು) : "memory");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 3:
		asm ಅಸ್ಥಿರ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v24,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ಬೈಟುಗಳು) : "memory");
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ get_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong ಗಾತ್ರ_2, uint8_t *ಬೈಟುಗಳು)
{
	pos += (which % 8) * vlenb;
	ಬೈಟುಗಳು -= pos;

	asm ಅಸ್ಥಿರ (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + ಗಾತ್ರ_2));

	csr_write(CSR_VSTART, pos);

	ಆಯ್ಕೆ (which / 8) {
	ಸ್ಥಿತಿ 0:
		asm ಅಸ್ಥಿರ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ಬೈಟುಗಳು) : "memory");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 1:
		asm ಅಸ್ಥಿರ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ಬೈಟುಗಳು) : "memory");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2:
		asm ಅಸ್ಥಿರ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v16, (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ಬೈಟುಗಳು) : "memory");
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 3:
		asm ಅಸ್ಥಿರ (
				".option push\n\t"
				".option arch, +v\n\t"
				"vse8.v v24, (%0)\n\t"
				".option pop\n\t"
				:: "r" (ಬೈಟುಗಳು) : "memory");
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ vsetvl(ulong vl, ulong vtype)
{
	asm ಅಸ್ಥಿರ (
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ sbi_misaligned_v_tinst_fixup(ರಚನೆ sbi_trap_info *uptrap)
{
	/*
	 * The function is called in code path for handling a vector
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	ಆದರೆ (uptrap->tinst == INSN_PSEUDO_VS_ತುಂಬು ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		ಹಿಂತಿರುಗಿ;

	uptrap->tinst = 0;
}

ಪೂರ್ಣಾಂಕ sbi_misaligned_v_ld_emulator(ulong insn, ರಚನೆ sbi_trap_context *tcntx)
{
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ರಚನೆ sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong ತಳ_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t ಮುಖವಾಡ[ಮುಖವಾಡ_BUFFLEN / 8];
	uint8_t ಬೈಟುಗಳು[8 * ಗಾತ್ರ(uint64_t)];
	ulong ಮುಖವಾಡ_len = ಮುಖವಾಡ_BUFFLEN < vlenb * 8 ? ಮುಖವಾಡ_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	ಆದರೆ (IS_UNIT_STRIDE_ತುಂಬು(insn) || IS_FAULT_ONLY_ಮೊದಲ_ತುಂಬು(insn)) {
		stride = nf * len;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IS_ಸಂಪೂರ್ಣ_REG_ತುಂಬು(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IS_ಸೂಚ್ಯಂಕಿತ_ತುಂಬು(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	ಆದರೆ (illegal) {
		ರಚನೆ sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, &trap);
	}

	ಮಾಡು {
		ಆದರೆ (masked) {
			ಆದರೆ (vstart == orig_vstart || vstart % ಮುಖವಾಡ_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / ಮುಖವಾಡ_len * ಮುಖವಾಡ_len,
					 ಮುಖವಾಡ_len, ಮುಖವಾಡ);

			ಆದರೆ (~ಮುಖವಾಡ[vstart % ಮುಖವಾಡ_len / 8] & ಬಿಟ್(vstart % 8))
				ಮುಂದುವರಿ;
		}

		/* compute element address */
		ulong addr = ತಳ_2 + vstart * stride;

		ಆದರೆ (IS_ಸೂಚ್ಯಂಕಿತ_ತುಂಬು(insn)) {
			ulong ಸ್ಥಾನಾಂತರ = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&ಸ್ಥಾನಾಂತರ);
			addr = ತಳ_2 + ಸ್ಥಾನಾಂತರ;
		}

		csr_write(CSR_VSTART, vstart);

		/* obtain load data from memory */
		ಗಾಗಿ (ulong seg = 0; seg < nf; seg++) {
			sbi_ತುಂಬು_loop(ಬೈಟುಗಳು + seg * len,
				       addr + seg * len, len, &uptrap);

			ಆದರೆ (!uptrap.cause)
				ಮುಂದುವರಿ;

			ಆದರೆ (IS_FAULT_ONLY_ಮೊದಲ_ತುಂಬು(insn) && vstart != 0) {
				vl = vstart;
				ಹೋಗು ಮುಗಿದ;
			}

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			ಆದರೆ (vstart != orig_vstart)
				ಗಣ_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, &uptrap);
		}

		/* write load data to regfile */
		ಗಾಗಿ (ulong seg = 0; seg < nf; seg++)
			ಗಣ_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &ಬೈಟುಗಳು[seg * len]);
	} ಆಗ (++vstart < vl);

ಮುಗಿದ:
	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * At least 1 element is processed, or vl is changed above in
	 * the FAULT_ONLY_FIRST_LOAD path, thus set dirty.
	 */
	ಗಣ_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ sbi_misaligned_v_st_emulator(ulong insn, ರಚನೆ sbi_trap_context *tcntx)
{
	ರಚನೆ sbi_trap_regs *regs = &tcntx->regs;
	ರಚನೆ sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong ತಳ_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t ಮುಖವಾಡ[ಮುಖವಾಡ_BUFFLEN / 8];
	uint8_t ಬೈಟುಗಳು[8 * ಗಾತ್ರ(uint64_t)];
	ulong ಮುಖವಾಡ_len = ಮುಖವಾಡ_BUFFLEN < vlenb * 8 ? ಮುಖವಾಡ_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	ಆದರೆ (IS_UNIT_STRIDE_STORE(insn)) {
		stride = nf * len;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IS_ಸಂಪೂರ್ಣ_REG_STORE(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IS_ಸೂಚ್ಯಂಕಿತ_STORE(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	ಆದರೆ (illegal) {
		ರಚನೆ sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, &trap);
	}

	ಮಾಡು {
		ಆದರೆ (masked) {
			ಆದರೆ (vstart == orig_vstart || vstart % ಮುಖವಾಡ_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / ಮುಖವಾಡ_len * ಮುಖವಾಡ_len,
					 ಮುಖವಾಡ_len, ಮುಖವಾಡ);

			ಆದರೆ (~ಮುಖವಾಡ[vstart % ಮುಖವಾಡ_len / 8] & ಬಿಟ್(vstart % 8))
				ಮುಂದುವರಿ;
		}

		/* compute element address */
		ulong addr = ತಳ_2 + vstart * stride;

		ಆದರೆ (IS_ಸೂಚ್ಯಂಕಿತ_STORE(insn)) {
			ulong ಸ್ಥಾನಾಂತರ = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&ಸ್ಥಾನಾಂತರ);
			addr = ತಳ_2 + ಸ್ಥಾನಾಂತರ;
		}

		/* obtain store data from regfile */
		ಗಾಗಿ (ulong seg = 0; seg < nf; seg++)
			get_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &ಬೈಟುಗಳು[seg * len]);

		/* write store data to memory */
		ಗಾಗಿ (ulong seg = 0; seg < nf; seg++) {
			sbi_store_loop(ಬೈಟುಗಳು + seg * len,
					addr + seg * len, len, &uptrap);

			ಆದರೆ (!uptrap.cause)
				ಮುಂದುವರಿ;

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			ಆದರೆ (vstart != orig_vstart)
				ಗಣ_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			ಹಿಂತಿರುಗಿ sbi_trap_redirect(regs, &uptrap);
		}
	} ಆಗ (++vstart < vl);

	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * No need to set dirty for memory store, but as VSTART resets to
	 * 0 above, need to set dirty if it's originally not 0.
	 */
	ಆದರೆ (orig_vstart != 0)
		ಗಣ_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	ಹಿಂತಿರುಗಿ 1;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಪೂರ್ಣಾಂಕ sbi_misaligned_v_ld_emulator(ulong insn, ರಚನೆ sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	ಹಿಂತಿರುಗಿ sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}

ಪೂರ್ಣಾಂಕ sbi_misaligned_v_st_emulator(ulong insn, ರಚನೆ sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	ಹಿಂತಿರುಗಿ sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}
#ಆದರೆ_ಅಂತ್ಯ /* OPENSBI_CC_SUPPORT_VECTOR */
