/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_double_trap.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_illegal_insn.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi/sbi_irqchip.h>
#அந்தர்பாவ <sbi/sbi_trap_ldst.h>
#அந்தர்பாவ <sbi/sbi_pmu.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_sse.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi/sbi_trap.h>

நிலையான வெறுமை sbi_trap_error_one(மாறிலி கட்டமைப்பு sbi_trap_context *tcntx,
			       மாறிலி எழுத்து *prefix, u32 hartid, u32 depth)
{
	மாறிலி கட்டமைப்பு sbi_trap_info *trap = &tcntx->trap;
	மாறிலி கட்டமைப்பு sbi_trap_regs *regs = &tcntx->regs;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "mcause", trap->cause, "mtval", trap->tval);
	எனில் (misa_extension('H')) {
		sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
			   hartid, depth, "mtval2", trap->tval2, "mtinst", trap->tinst);
	}
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "mepc", regs->mepc, "mstatus", regs->mstatus);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "ra", regs->ra, "sp", regs->sp);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "gp", regs->gp, "tp", regs->tp);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "s0", regs->s0, "s1", regs->s1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "a0", regs->a0, "a1", regs->a1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "a2", regs->a2, "a3", regs->a3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "a4", regs->a4, "a5", regs->a5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "a6", regs->a6, "a7", regs->a7);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "s2", regs->s2, "s3", regs->s3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "s4", regs->s4, "s5", regs->s5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "s6", regs->s6, "s7", regs->s7);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "s8", regs->s8, "s9", regs->s9);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "s10", regs->s10, "s11", regs->s11);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "t0", regs->t0, "t1", regs->t1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "t2", regs->t2, "t3", regs->t3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "t4", regs->t4, "t5", regs->t5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "t6", regs->t6);
}

நிலையான வெறுமை __noreturn sbi_trap_error(மாறிலி எழுத்து *செய்தி, முழு rc,
				      மாறிலி கட்டமைப்பு sbi_trap_context *tcntx)
{
	u32 depth = 0, hartid = current_hartid();
	மாறிலி கட்டமைப்பு sbi_trap_context *tc;

	க்கு (tc = tcntx; tc; tc = tc->prev_context)
		depth++;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s (error %d)\n", __func__,
		   hartid, depth - 1, செய்தி, rc);
	க்கு (tc = tcntx; tc; tc = tc->prev_context)
		sbi_trap_error_one(tc, __func__, hartid, --depth);

	sbi_hart_hang();
}

/**
 * Redirect trap to lower privledge mode (S-mode or U-mode)
 *
 * @param regs pointer to register state
 * @param trap pointer to trap details
 *
 * @return 0 on success and negative error code on failure
 */
முழு sbi_trap_redirect(கட்டமைப்பு sbi_trap_regs *regs,
		      மாறிலி கட்டமைப்பு sbi_trap_info *trap)
{
	ulong hstatus, vsstatus, prev_mode;
	bool elp = false;
	bool prev_virt = sbi_regs_from_virt(regs);
	/* By default, we redirect to HS-mode */
	bool next_virt = false;

	/* Sanity check on previous mode */
	prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	எனில் (prev_mode != PRV_S && prev_mode != PRV_U)
		திரும்பு SBI_ENOTSUPP;

	/* If hart support for zicfilp, clear MPELP because redirecting to VS or (H)S */
	எனில் (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_ZICFILP)) {
#எனில் __riscv_xlen == 32
		elp = regs->mstatusH & MSTATUSH_MPELP;
		regs->mstatusH &= ~MSTATUSH_MPELP;
#இல்லையெனில்
		elp = regs->mstatus & MSTATUS_MPELP;
		regs->mstatus &= ~MSTATUS_MPELP;
#endif
	}

	/* If exceptions came from VS/VU-mode, redirect to VS-mode if
	 * delegated in hedeleg
	 */
	எனில் (misa_extension('H') && prev_virt) {
		எனில் ((trap->cause < __riscv_xlen) &&
		    (csr_read(CSR_HEDELEG) & BIT(trap->cause))) {
			next_virt = true;
		}
	}

	/* Update MSTATUS MPV bits */
#எனில் __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
	regs->mstatusH |= (next_virt) ? MSTATUSH_MPV : 0UL;
#இல்லையெனில்
	regs->mstatus &= ~MSTATUS_MPV;
	regs->mstatus |= (next_virt) ? MSTATUS_MPV : 0UL;
#endif

	/* Update hypervisor CSRs if going to HS-mode */
	எனில் (misa_extension('H') && !next_virt) {
		hstatus = csr_read(CSR_HSTATUS);
		எனில் (prev_virt) {
			/* hstatus.SPVP is only updated if coming from VS/VU-mode */
			hstatus &= ~HSTATUS_SPVP;
			hstatus |= (prev_mode == PRV_S) ? HSTATUS_SPVP : 0;
		}
		hstatus &= ~HSTATUS_SPV;
		hstatus |= (prev_virt) ? HSTATUS_SPV : 0;
		hstatus &= ~HSTATUS_GVA;
		hstatus |= (trap->gva) ? HSTATUS_GVA : 0;
		csr_write(CSR_HSTATUS, hstatus);
		csr_write(CSR_HTVAL, trap->tval2);
		csr_write(CSR_HTINST, trap->tinst);
	}

	/* Update exception related CSRs */
	எனில் (next_virt) {
		/* Update VS-mode exception info */
		csr_write(CSR_VSTVAL, trap->tval);
		csr_write(CSR_VSEPC, regs->mepc);
		csr_write(CSR_VSCAUSE, trap->cause);

		/* Set MEPC to VS-mode exception vector base */
		regs->mepc = csr_read(CSR_VSTVEC) & ~MTVEC_MODE;

		/* Set MPP to VS-mode */
		regs->mstatus &= ~MSTATUS_MPP;
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

		/* Get VS-mode SSTATUS CSR */
		vsstatus = csr_read(CSR_VSSTATUS);

		/* If elp was set, set it back in vsstatus */
		எனில் (elp)
			vsstatus |= MSTATUS_SPELP;

		/* Set SPP for VS-mode */
		vsstatus &= ~SSTATUS_SPP;
		எனில் (prev_mode == PRV_S)
			vsstatus |= (1UL << SSTATUS_SPP_SHIFT);

		/* Set SPIE for VS-mode */
		vsstatus &= ~SSTATUS_SPIE;
		எனில் (vsstatus & SSTATUS_SIE)
			vsstatus |= (1UL << SSTATUS_SPIE_SHIFT);

		/* Clear SIE for VS-mode */
		vsstatus &= ~SSTATUS_SIE;

		/* Update VS-mode SSTATUS CSR */
		csr_write(CSR_VSSTATUS, vsstatus);
	} இல்லையெனில் {
		/* Update S-mode exception info */
		csr_write(CSR_STVAL, trap->tval);
		csr_write(CSR_SEPC, regs->mepc);
		csr_write(CSR_SCAUSE, trap->cause);

		/* Set MEPC to S-mode exception vector base */
		regs->mepc = csr_read(CSR_STVEC) & ~MTVEC_MODE;

		/* Set MPP to S-mode */
		regs->mstatus &= ~MSTATUS_MPP;
		regs->mstatus |= (PRV_S << MSTATUS_MPP_SHIFT);

		/* Set SPP for S-mode */
		regs->mstatus &= ~MSTATUS_SPP;
		எனில் (prev_mode == PRV_S)
			regs->mstatus |= (1UL << MSTATUS_SPP_SHIFT);

		/* Set SPIE for S-mode */
		regs->mstatus &= ~MSTATUS_SPIE;
		எனில் (regs->mstatus & MSTATUS_SIE)
			regs->mstatus |= (1UL << MSTATUS_SPIE_SHIFT);

		/* Clear SIE for S-mode */
		regs->mstatus &= ~MSTATUS_SIE;

		/* If elp was set, set it back in mstatus */
		எனில் (elp)
			regs->mstatus |= MSTATUS_SPELP;
	}

	திரும்பு 0;
}

நிலையான முழு sbi_trap_nonaia_irq(unsigned long irq)
{
	தேர்வு (irq) {
	நிலை IRQ_M_TIMER:
		sbi_timer_process();
		நிறுத்து;
	நிலை IRQ_M_SOFT:
		sbi_ipi_process();
		நிறுத்து;
	நிலை IRQ_M_EXT:
		திரும்பு sbi_irqchip_process();
	இயல்பு:
		எனில் (irq == sbi_pmu_irq_bit()) {
			sbi_pmu_ovf_irq();
			திரும்பு 0;
		}
		திரும்பு SBI_ENOENT;
	}

	திரும்பு 0;
}

நிலையான முழு sbi_trap_aia_irq(வெறுமை)
{
	முழு rc;
	unsigned long mtopi;

	போது ((mtopi = csr_read(CSR_MTOPI))) {
		mtopi = mtopi >> TOPI_IID_SHIFT;
		தேர்வு (mtopi) {
		நிலை IRQ_M_TIMER:
			sbi_timer_process();
			நிறுத்து;
		நிலை IRQ_M_SOFT:
			sbi_ipi_process();
			நிறுத்து;
		நிலை IRQ_M_EXT:
			rc = sbi_irqchip_process();
			எனில் (rc)
				திரும்பு rc;
			நிறுத்து;
		இயல்பு:
			எனில் (mtopi == sbi_pmu_irq_bit()) {
				sbi_pmu_ovf_irq();
				நிறுத்து;
			}

			திரும்பு SBI_ENOENT;
		}
	}

	திரும்பு 0;
}

/**
 * Handle trap/interrupt
 *
 * This function is called by firmware linked to OpenSBI
 * library for handling trap/interrupt. It expects the
 * following:
 * 1. The 'mscratch' CSR is pointing to sbi_scratch of current HART
 * 2. The 'mcause' CSR is having exception/interrupt cause
 * 3. The 'mtval' CSR is having additional trap information
 * 4. The 'mtval2' CSR is having additional trap information
 * 5. The 'mtinst' CSR is having decoded trap instruction
 * 6. Stack pointer (SP) is setup for current HART
 * 7. Interrupts are disabled in MSTATUS CSR
 *
 * @param tcntx pointer to trap context
 */
கட்டமைப்பு sbi_trap_context *sbi_trap_handler(கட்டமைப்பு sbi_trap_context *tcntx)
{
	முழு rc = SBI_ENOTSUPP;
	மாறிலி எழுத்து *செய்தி = "trap handler failed";
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	மாறிலி கட்டமைப்பு sbi_trap_info *trap = &tcntx->trap;
	கட்டமைப்பு sbi_trap_regs *regs = &tcntx->regs;
	ulong mcause = tcntx->trap.cause;

	/* Update trap context pointer */
	tcntx->prev_context = sbi_trap_get_context(scratch);
	sbi_trap_set_context(scratch, tcntx);

	எனில் (mcause & MCAUSE_IRQ_MASK) {
		எனில் (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_SMAIA))
			rc = sbi_trap_aia_irq();
		இல்லையெனில்
			rc = sbi_trap_nonaia_irq(mcause & ~MCAUSE_IRQ_MASK);
		செய்தி = "unhandled local interrupt";
		செல் trap_done;
	}

	தேர்வு (mcause) {
	நிலை CAUSE_ILLEGAL_INSTRUCTION:
		rc  = sbi_illegal_insn_handler(tcntx);
		செய்தி = "illegal instruction handler failed";
		நிறுத்து;
	நிலை CAUSE_MISALIGNED_LOAD:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_LOAD);
		rc  = sbi_misaligned_load_handler(tcntx);
		செய்தி = "misaligned load handler failed";
		நிறுத்து;
	நிலை CAUSE_MISALIGNED_STORE:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_STORE);
		rc  = sbi_misaligned_store_handler(tcntx);
		செய்தி = "misaligned store handler failed";
		நிறுத்து;
	நிலை CAUSE_SUPERVISOR_ECALL:
	நிலை CAUSE_MACHINE_ECALL:
		rc  = sbi_ecall_handler(tcntx);
		செய்தி = "ecall handler failed";
		நிறுத்து;
	நிலை CAUSE_LOAD_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_LOAD);
		rc  = sbi_load_access_handler(tcntx);
		செய்தி = "load fault handler failed";
		நிறுத்து;
	நிலை CAUSE_STORE_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_STORE);
		rc  = sbi_store_access_handler(tcntx);
		செய்தி = "store fault handler failed";
		நிறுத்து;
	நிலை CAUSE_DOUBLE_TRAP:
		rc  = sbi_double_trap_handler(tcntx);
		செய்தி = "double trap handler failed";
		நிறுத்து;
	இயல்பு:
		/* If the trap came from S or U mode, redirect it there */
		செய்தி = "trap redirect failed";
		rc  = sbi_trap_redirect(regs, trap);
		நிறுத்து;
	}

trap_done:
	எனில் (rc)
		sbi_trap_error(செய்தி, rc, tcntx);

	எனில் (sbi_mstatus_prev_mode(regs->mstatus) != PRV_M)
		sbi_sse_process_pending_events(regs);

	sbi_trap_set_context(scratch, tcntx->prev_context);
	திரும்பு tcntx;
}

/**
 * Default Resumable NMI (RNMI) handler
 *
 * This function is called from the _trap_rnmi_handler assembly code.
 * It provides a simple wrapper that calls the platform-specific
 * NMI handler if registered. If no handler is registered, it prints
 * diagnostic information and hangs, similar to unhandled traps.
 *
 * Note: The trap context stores NMI CSR values (MNCAUSE, MNEPC, MNSTATUS)
 * in the generic trap context fields (cause, mepc, mstatus).
 *
 * @param tcntx Pointer to trap context (saved on stack)
 * @return Same trap context pointer (needed for restore macros)
 */
கட்டமைப்பு sbi_trap_context *sbi_trap_rnmi_handler(கட்டமைப்பு sbi_trap_context *tcntx)
{
	முழு rc;
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_thishart_ptr();
	மாறிலி கட்டமைப்பு sbi_platform_operations *ops = sbi_platform_ops(plat);

	/* Call platform-specific NMI handler if registered */
	எனில் (ops && ops->rnmi_handler) {
		rc = ops->rnmi_handler(tcntx);
		எனில் (rc) {
			/* Platform handler failed to handle NMI */
			sbi_trap_error("platform NMI handler failed", rc, tcntx);
		}
		திரும்பு tcntx;
	}

	/* No platform handler - treat as unhandled NMI */
	sbi_trap_error("unhandled NMI (no platform rnmi_handler)",
		       SBI_ENOTSUPP, tcntx);

	/* Never returns */
	திரும்பு tcntx;
}

நிலையான முழு delegate_traps(கட்டமைப்பு sbi_scratch *scratch)
{
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_ptr(scratch);
	unsigned long interrupts, exceptions;

	எனில் (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist */
		திரும்பு 0;

	/* Send M-mode interrupts and most exceptions to S-mode */
	interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;
	interrupts |= sbi_pmu_irq_mask();

	exceptions = (1U << CAUSE_MISALIGNED_FETCH) | (1U << CAUSE_BREAKPOINT) |
		     (1U << CAUSE_USER_ECALL);
	எனில் (sbi_platform_has_mfaults_delegation(plat))
		exceptions |= (1U << CAUSE_FETCH_PAGE_FAULT) |
			      (1U << CAUSE_LOAD_PAGE_FAULT) |
			      (1U << CAUSE_STORE_PAGE_FAULT) |
			      (1U << CAUSE_SW_CHECK_EXCP);

	/*
	 * If hypervisor extension available then we only handle hypervisor
	 * calls (i.e. ecalls from HS-mode) in M-mode.
	 *
	 * The HS-mode will additionally handle supervisor calls (i.e. ecalls
	 * from VS-mode), Guest page faults and Virtual interrupts.
	 */
	எனில் (misa_extension('H')) {
		exceptions |= (1U << CAUSE_VIRTUAL_SUPERVISOR_ECALL);
		exceptions |= (1U << CAUSE_FETCH_GUEST_PAGE_FAULT);
		exceptions |= (1U << CAUSE_LOAD_GUEST_PAGE_FAULT);
		exceptions |= (1U << CAUSE_VIRTUAL_INST_FAULT);
		exceptions |= (1U << CAUSE_STORE_GUEST_PAGE_FAULT);
	}

	csr_write(CSR_MIDELEG, interrupts);
	csr_write(CSR_MEDELEG, exceptions);

	திரும்பு 0;
}

முழு sbi_trap_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	திரும்பு delegate_traps(scratch);
}
