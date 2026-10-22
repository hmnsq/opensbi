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

#ifdef OPENSBI_CC_SUPPORT_VECTOR

#define MASK_BUFFLEN 1024

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ set_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong size, ಧ್ರುವ uint8_t *bytes)
{
	pos += (which % 8) * vlenb;
	bytes -= pos;

	asm ಚಞ್ಚಲ (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + size));

	csr_write(CSR_VSTART, pos);

	ವಿಕಲ್ಪ (which / 8) {
	ಪಕ್ಷ 0:
		asm ಚಞ್ಚಲ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (bytes) : "memory");
		ಭಙ್ಗ;
	ಪಕ್ಷ 1:
		asm ಚಞ್ಚಲ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (bytes) : "memory");
		ಭಙ್ಗ;
	ಪಕ್ಷ 2:
		asm ಚಞ್ಚಲ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v16,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (bytes) : "memory");
		ಭಙ್ಗ;
	ಪಕ್ಷ 3:
		asm ಚಞ್ಚಲ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v24,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (bytes) : "memory");
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ get_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong size, uint8_t *bytes)
{
	pos += (which % 8) * vlenb;
	bytes -= pos;

	asm ಚಞ್ಚಲ (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + size));

	csr_write(CSR_VSTART, pos);

	ವಿಕಲ್ಪ (which / 8) {
	ಪಕ್ಷ 0:
		asm ಚಞ್ಚಲ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (bytes) : "memory");
		ಭಙ್ಗ;
	ಪಕ್ಷ 1:
		asm ಚಞ್ಚಲ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (bytes) : "memory");
		ಭಙ್ಗ;
	ಪಕ್ಷ 2:
		asm ಚಞ್ಚಲ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v16, (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (bytes) : "memory");
		ಭಙ್ಗ;
	ಪಕ್ಷ 3:
		asm ಚಞ್ಚಲ (
				".option push\n\t"
				".option arch, +v\n\t"
				"vse8.v v24, (%0)\n\t"
				".option pop\n\t"
				:: "r" (bytes) : "memory");
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ vsetvl(ulong vl, ulong vtype)
{
	asm ಚಞ್ಚಲ (
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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ sbi_misaligned_v_tinst_fixup(ಸಂರಚನಾ sbi_trap_info *uptrap)
{
	/*
	 * The function is called in code path for handling a vector
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	ಯದಿ (uptrap->tinst == INSN_PSEUDO_VS_LOAD ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		ಪ್ರತ್ಯಾಗಮ;

	uptrap->tinst = 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_misaligned_v_ld_emulator(ulong insn, ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ಸಂರಚನಾ sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong base = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t mask[MASK_BUFFLEN / 8];
	uint8_t bytes[8 * ಪರಿಮಾಣ(uint64_t)];
	ulong mask_len = MASK_BUFFLEN < vlenb * 8 ? MASK_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	ಯದಿ (IS_UNIT_STRIDE_LOAD(insn) || IS_FAULT_ONLY_FIRST_LOAD(insn)) {
		stride = nf * len;
	} ಅನ್ಯಥಾ ಯದಿ (IS_WHOLE_REG_LOAD(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} ಅನ್ಯಥಾ ಯದಿ (IS_INDEXED_LOAD(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	ಯದಿ (illegal) {
		ಸಂರಚನಾ sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &trap);
	}

	ಕುರು {
		ಯದಿ (masked) {
			ಯದಿ (vstart == orig_vstart || vstart % mask_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / mask_len * mask_len,
					 mask_len, mask);

			ಯದಿ (~mask[vstart % mask_len / 8] & BIT(vstart % 8))
				ಅನುವರ್ತ;
		}

		/* compute element address */
		ulong addr = base + vstart * stride;

		ಯದಿ (IS_INDEXED_LOAD(insn)) {
			ulong offset = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&offset);
			addr = base + offset;
		}

		csr_write(CSR_VSTART, vstart);

		/* obtain load data from memory */
		ಪ್ರತಿ (ulong seg = 0; seg < nf; seg++) {
			sbi_load_loop(bytes + seg * len,
				       addr + seg * len, len, &uptrap);

			ಯದಿ (!uptrap.cause)
				ಅನುವರ್ತ;

			ಯದಿ (IS_FAULT_ONLY_FIRST_LOAD(insn) && vstart != 0) {
				vl = vstart;
				ಗಚ್ಛ done;
			}

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			ಯದಿ (vstart != orig_vstart)
				SET_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &uptrap);
		}

		/* write load data to regfile */
		ಪ್ರತಿ (ulong seg = 0; seg < nf; seg++)
			set_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &bytes[seg * len]);
	} ಯಾವತ್ (++vstart < vl);

done:
	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * At least 1 element is processed, or vl is changed above in
	 * the FAULT_ONLY_FIRST_LOAD path, thus set dirty.
	 */
	SET_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ sbi_misaligned_v_st_emulator(ulong insn, ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	ಸಂರಚನಾ sbi_trap_regs *regs = &tcntx->regs;
	ಸಂರಚನಾ sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong base = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t mask[MASK_BUFFLEN / 8];
	uint8_t bytes[8 * ಪರಿಮಾಣ(uint64_t)];
	ulong mask_len = MASK_BUFFLEN < vlenb * 8 ? MASK_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	ಯದಿ (IS_UNIT_STRIDE_STORE(insn)) {
		stride = nf * len;
	} ಅನ್ಯಥಾ ಯದಿ (IS_WHOLE_REG_STORE(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} ಅನ್ಯಥಾ ಯದಿ (IS_INDEXED_STORE(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	ಯದಿ (illegal) {
		ಸಂರಚನಾ sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &trap);
	}

	ಕುರು {
		ಯದಿ (masked) {
			ಯದಿ (vstart == orig_vstart || vstart % mask_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / mask_len * mask_len,
					 mask_len, mask);

			ಯದಿ (~mask[vstart % mask_len / 8] & BIT(vstart % 8))
				ಅನುವರ್ತ;
		}

		/* compute element address */
		ulong addr = base + vstart * stride;

		ಯದಿ (IS_INDEXED_STORE(insn)) {
			ulong offset = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&offset);
			addr = base + offset;
		}

		/* obtain store data from regfile */
		ಪ್ರತಿ (ulong seg = 0; seg < nf; seg++)
			get_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &bytes[seg * len]);

		/* write store data to memory */
		ಪ್ರತಿ (ulong seg = 0; seg < nf; seg++) {
			sbi_store_loop(bytes + seg * len,
					addr + seg * len, len, &uptrap);

			ಯದಿ (!uptrap.cause)
				ಅನುವರ್ತ;

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			ಯದಿ (vstart != orig_vstart)
				SET_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(regs, &uptrap);
		}
	} ಯಾವತ್ (++vstart < vl);

	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * No need to set dirty for memory store, but as VSTART resets to
	 * 0 above, need to set dirty if it's originally not 0.
	 */
	ಯದಿ (orig_vstart != 0)
		SET_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	ಪ್ರತ್ಯಾಗಮ 1;
}
#ಅನ್ಯಥಾ
ಪೂರ್ಣಾಙ್ಕ sbi_misaligned_v_ld_emulator(ulong insn, ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}

ಪೂರ್ಣಾಙ್ಕ sbi_misaligned_v_st_emulator(ulong insn, ಸಂರಚನಾ sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	ಪ್ರತ್ಯಾಗಮ sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}
#endif /* OPENSBI_CC_SUPPORT_VECTOR */
