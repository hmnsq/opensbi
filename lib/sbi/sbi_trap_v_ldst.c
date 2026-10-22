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

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap_ldst.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_unpriv.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_vector.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSBI_CC_SUPPORT_ਸਦਿਸ਼

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_BUFFLEN 1024

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong ਮਾਪ, ਅਚਲ uint8_t *ਬਾਈਟਾਂ)
{
	pos += (which % 8) * vlenb;
	ਬਾਈਟਾਂ -= pos;

	asm ਅਸਥਿਰ (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + ਮਾਪ));

	csr_write(CSR_VSTART, pos);

	ਚੋਣ (which / 8) {
	ਹਾਲਤ 0:
		asm ਅਸਥਿਰ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ਬਾਈਟਾਂ) : "memory");
		ਰੋਕੋ;
	ਹਾਲਤ 1:
		asm ਅਸਥਿਰ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ਬਾਈਟਾਂ) : "memory");
		ਰੋਕੋ;
	ਹਾਲਤ 2:
		asm ਅਸਥਿਰ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v16,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ਬਾਈਟਾਂ) : "memory");
		ਰੋਕੋ;
	ਹਾਲਤ 3:
		asm ਅਸਥਿਰ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vle8.v v24,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ਬਾਈਟਾਂ) : "memory");
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ get_vreg(ulong vlenb, ulong which,
			    ulong pos, ulong ਮਾਪ, uint8_t *ਬਾਈਟਾਂ)
{
	pos += (which % 8) * vlenb;
	ਬਾਈਟਾਂ -= pos;

	asm ਅਸਥਿਰ (
		"	.option push\n\t"
		"	.option arch, +v\n\t"
		"	vsetvli x0, %0, e8, m8, tu, ma\n\t"
		"	.option pop\n\t"
		:: "r" (pos + ਮਾਪ));

	csr_write(CSR_VSTART, pos);

	ਚੋਣ (which / 8) {
	ਹਾਲਤ 0:
		asm ਅਸਥਿਰ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v0,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ਬਾਈਟਾਂ) : "memory");
		ਰੋਕੋ;
	ਹਾਲਤ 1:
		asm ਅਸਥਿਰ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v8,  (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ਬਾਈਟਾਂ) : "memory");
		ਰੋਕੋ;
	ਹਾਲਤ 2:
		asm ਅਸਥਿਰ (
			"	.option push\n\t"
			"	.option arch, +v\n\t"
			"	vse8.v v16, (%0)\n\t"
			"	.option pop\n\t"
			:: "r" (ਬਾਈਟਾਂ) : "memory");
		ਰੋਕੋ;
	ਹਾਲਤ 3:
		asm ਅਸਥਿਰ (
				".option push\n\t"
				".option arch, +v\n\t"
				"vse8.v v24, (%0)\n\t"
				".option pop\n\t"
				:: "r" (ਬਾਈਟਾਂ) : "memory");
		ਰੋਕੋ;
	ਮੂਲ:
		ਰੋਕੋ;
	}
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ vsetvl(ulong vl, ulong vtype)
{
	asm ਅਸਥਿਰ (
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
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ sbi_misaligned_v_tinst_fixup(ਬਣਤਰ sbi_trap_info *uptrap)
{
	/*
	 * The function is called in code path for handling a vector
	 * load/store misaligned fault, thus the new uptrap can't have
	 * custom value of tinst
	 */
	ਜੇ (uptrap->tinst == INSN_PSEUDO_VS_ਲੱਦੋ ||
	    uptrap->tinst == INSN_PSEUDO_VS_STORE)
		/* Use uptrap as-is for guest-page faults */
		ਵਾਪਸ;

	uptrap->tinst = 0;
}

ਪੂਰਨ sbi_misaligned_v_ld_emulator(ulong insn, ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ਬਣਤਰ sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong ਅਧਾਰ_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t ਨਕਾਬ[ਨਕਾਬ_BUFFLEN / 8];
	uint8_t ਬਾਈਟਾਂ[8 * ਆਕਾਰ(uint64_t)];
	ulong ਨਕਾਬ_len = ਨਕਾਬ_BUFFLEN < vlenb * 8 ? ਨਕਾਬ_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	ਜੇ (IS_UNIT_STRIDE_ਲੱਦੋ(insn) || IS_FAULT_ONLY_ਪਹਿਲਾ_ਲੱਦੋ(insn)) {
		stride = nf * len;
	} ਨਹੀਂਤਾਂ ਜੇ (IS_ਸਾਰਾ_REG_ਲੱਦੋ(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} ਨਹੀਂਤਾਂ ਜੇ (IS_ਸੂਚਕਾਂਕਿਤ_ਲੱਦੋ(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	ਜੇ (illegal) {
		ਬਣਤਰ sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		ਵਾਪਸ sbi_trap_redirect(regs, &trap);
	}

	ਕਰੋ {
		ਜੇ (masked) {
			ਜੇ (vstart == orig_vstart || vstart % ਨਕਾਬ_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / ਨਕਾਬ_len * ਨਕਾਬ_len,
					 ਨਕਾਬ_len, ਨਕਾਬ);

			ਜੇ (~ਨਕਾਬ[vstart % ਨਕਾਬ_len / 8] & ਦੋਆਧਾਰੀ_ਅੰਕ(vstart % 8))
				ਜਾਰੀ;
		}

		/* compute element address */
		ulong addr = ਅਧਾਰ_2 + vstart * stride;

		ਜੇ (IS_ਸੂਚਕਾਂਕਿਤ_ਲੱਦੋ(insn)) {
			ulong ਵਿੱਥ = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&ਵਿੱਥ);
			addr = ਅਧਾਰ_2 + ਵਿੱਥ;
		}

		csr_write(CSR_VSTART, vstart);

		/* obtain load data from memory */
		ਲਈ (ulong seg = 0; seg < nf; seg++) {
			sbi_ਲੱਦੋ_loop(ਬਾਈਟਾਂ + seg * len,
				       addr + seg * len, len, &uptrap);

			ਜੇ (!uptrap.cause)
				ਜਾਰੀ;

			ਜੇ (IS_FAULT_ONLY_ਪਹਿਲਾ_ਲੱਦੋ(insn) && vstart != 0) {
				vl = vstart;
				ਜਾਓ ਪੂਰਾ;
			}

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			ਜੇ (vstart != orig_vstart)
				ਸੈੱਟ_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			ਵਾਪਸ sbi_trap_redirect(regs, &uptrap);
		}

		/* write load data to regfile */
		ਲਈ (ulong seg = 0; seg < nf; seg++)
			ਸੈੱਟ_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &ਬਾਈਟਾਂ[seg * len]);
	} ਜਦੋਂ (++vstart < vl);

ਪੂਰਾ:
	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * At least 1 element is processed, or vl is changed above in
	 * the FAULT_ONLY_FIRST_LOAD path, thus set dirty.
	 */
	ਸੈੱਟ_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	ਵਾਪਸ 1;
}

ਪੂਰਨ sbi_misaligned_v_st_emulator(ulong insn, ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ਬਣਤਰ sbi_trap_info uptrap;
	ulong vl = csr_read(CSR_VL);
	ulong vtype = csr_read(CSR_VTYPE);
	ulong vlenb = csr_read(CSR_VLENB);
	ulong vstart = csr_read(CSR_VSTART), orig_vstart = vstart;
	ulong ਅਧਾਰ_2 = GET_RS1(insn, regs);
	ulong stride = GET_RS2(insn, regs);
	ulong vd = GET_VD(insn);
	ulong vs2 = GET_VS2(insn);
	ulong view = GET_VIEW(insn);
	ulong vsew = GET_VSEW(vtype);
	ulong vlmul = GET_VLMUL(vtype);
	bool illegal = GET_MEW(insn);
	bool masked = IS_MASKED(insn);
	uint8_t ਨਕਾਬ[ਨਕਾਬ_BUFFLEN / 8];
	uint8_t ਬਾਈਟਾਂ[8 * ਆਕਾਰ(uint64_t)];
	ulong ਨਕਾਬ_len = ਨਕਾਬ_BUFFLEN < vlenb * 8 ? ਨਕਾਬ_BUFFLEN : vlenb * 8;
	ulong len = GET_LEN(view);
	ulong nf = GET_NF(insn);
	ulong vemul = GET_VEMUL(vlmul, view, vsew);
	ulong emul = GET_EMUL(vemul);

	ਜੇ (IS_UNIT_STRIDE_STORE(insn)) {
		stride = nf * len;
	} ਨਹੀਂਤਾਂ ਜੇ (IS_ਸਾਰਾ_REG_STORE(insn)) {
		vl = (nf * vlenb) >> view;
		nf = 1;
		vemul = 0;
		emul = 1;
		stride = nf * len;
	} ਨਹੀਂਤਾਂ ਜੇ (IS_ਸੂਚਕਾਂਕਿਤ_STORE(insn)) {
		len = 1 << vsew;
		vemul = (vlmul + vsew - vsew) & 7;
		emul = 1 << ((vemul & 4) ? 0 : vemul);
		stride = nf * len;
	}

	ਜੇ (illegal) {
		ਬਣਤਰ sbi_trap_info trap = {
			uptrap.cause = CAUSE_ILLEGAL_INSTRUCTION,
			uptrap.tval = insn,
		};
		ਵਾਪਸ sbi_trap_redirect(regs, &trap);
	}

	ਕਰੋ {
		ਜੇ (masked) {
			ਜੇ (vstart == orig_vstart || vstart % ਨਕਾਬ_len == 0)
				/* Fetch a mask_len chunk of mask */
				get_vreg(vlenb, 0, vstart / ਨਕਾਬ_len * ਨਕਾਬ_len,
					 ਨਕਾਬ_len, ਨਕਾਬ);

			ਜੇ (~ਨਕਾਬ[vstart % ਨਕਾਬ_len / 8] & ਦੋਆਧਾਰੀ_ਅੰਕ(vstart % 8))
				ਜਾਰੀ;
		}

		/* compute element address */
		ulong addr = ਅਧਾਰ_2 + vstart * stride;

		ਜੇ (IS_ਸੂਚਕਾਂਕਿਤ_STORE(insn)) {
			ulong ਵਿੱਥ = 0;

			get_vreg(vlenb, vs2, vstart << view, 1 << view, (uint8_t *)&ਵਿੱਥ);
			addr = ਅਧਾਰ_2 + ਵਿੱਥ;
		}

		/* obtain store data from regfile */
		ਲਈ (ulong seg = 0; seg < nf; seg++)
			get_vreg(vlenb, vd + seg * emul, vstart * len,
				 len, &ਬਾਈਟਾਂ[seg * len]);

		/* write store data to memory */
		ਲਈ (ulong seg = 0; seg < nf; seg++) {
			sbi_store_loop(ਬਾਈਟਾਂ + seg * len,
					addr + seg * len, len, &uptrap);

			ਜੇ (!uptrap.cause)
				ਜਾਰੀ;

			vsetvl(vl, vtype);
			csr_write(CSR_VSTART, vstart);
			/* Don't forget to set dirty if vstart has changed */
			ਜੇ (vstart != orig_vstart)
				ਸੈੱਟ_VS_DIRTY(regs);
			sbi_misaligned_v_tinst_fixup(&uptrap);
			ਵਾਪਸ sbi_trap_redirect(regs, &uptrap);
		}
	} ਜਦੋਂ (++vstart < vl);

	/* restore clobbered vl/vtype */
	vsetvl(vl, vtype); // VSTART resets to 0

	/*
	 * No need to set dirty for memory store, but as VSTART resets to
	 * 0 above, need to set dirty if it's originally not 0.
	 */
	ਜੇ (orig_vstart != 0)
		ਸੈੱਟ_VS_DIRTY(regs);

	/* Return a >0 value for the caller to advance mepc */
	ਵਾਪਸ 1;
}
#ਨਹੀਂਤਾਂ
ਪੂਰਨ sbi_misaligned_v_ld_emulator(ulong insn, ਬਣਤਰ sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	ਵਾਪਸ sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}

ਪੂਰਨ sbi_misaligned_v_st_emulator(ulong insn, ਬਣਤਰ sbi_trap_context *tcntx)
{
	/* Unable to emulate, send trap to previous mode. */
	ਵਾਪਸ sbi_trap_redirect(&tcntx->regs, &tcntx->trap);
}
#ਜੇ_ਅੰਤ /* OPENSBI_CC_SUPPORT_VECTOR */
