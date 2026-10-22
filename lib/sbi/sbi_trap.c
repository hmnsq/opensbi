/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_double_trap.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_illegal_insn.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_irqchip.h>
#अन्तर्भाव <sbi/sbi_trap_ldst.h>
#अन्तर्भाव <sbi/sbi_pmu.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_sse.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi/sbi_trap.h>

स्थिर शून्य sbi_trap_error_one(ध्रुव संरचना sbi_trap_context *tcntx,
			       ध्रुव वर्ण *prefix, u32 hartid, u32 depth)
{
	ध्रुव संरचना sbi_trap_info *trap = &tcntx->trap;
	ध्रुव संरचना sbi_trap_regs *regs = &tcntx->regs;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", prefix,
		   hartid, depth, "mcause", trap->cause, "mtval", trap->tval);
	जइ (misa_extension('H')) {
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

स्थिर शून्य __noreturn sbi_trap_error(ध्रुव वर्ण *सन्देशः, पूर्णाङ्क rc,
				      ध्रुव संरचना sbi_trap_context *tcntx)
{
	u32 depth = 0, hartid = current_hartid();
	ध्रुव संरचना sbi_trap_context *tc;

	प्रति (tc = tcntx; tc; tc = tc->prev_context)
		depth++;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s (error %d)\n", __func__,
		   hartid, depth - 1, सन्देशः, rc);
	प्रति (tc = tcntx; tc; tc = tc->prev_context)
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
पूर्णाङ्क sbi_trap_redirect(संरचना sbi_trap_regs *regs,
		      ध्रुव संरचना sbi_trap_info *trap)
{
	ulong hstatus, vsstatus, prev_mode;
	bool elp = false;
	bool prev_virt = sbi_regs_from_virt(regs);
	/* By default, we redirect to HS-mode */
	bool next_virt = false;

	/* Sanity check on previous mode */
	prev_mode = sbi_mstatus_prev_mode(regs->mstatus);
	जइ (prev_mode != PRV_S && prev_mode != PRV_U)
		प्रत्यागम SBI_ENOTSUPP;

	/* If hart support for zicfilp, clear MPELP because redirecting to VS or (H)S */
	जइ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_ZICFILP)) {
#जइ __riscv_xlen == 32
		elp = regs->mstatusH & MSTATUSH_MPELP;
		regs->mstatusH &= ~MSTATUSH_MPELP;
#अण्णहा
		elp = regs->mstatus & MSTATUS_MPELP;
		regs->mstatus &= ~MSTATUS_MPELP;
#endif
	}

	/* If exceptions came from VS/VU-mode, redirect to VS-mode if
	 * delegated in hedeleg
	 */
	जइ (misa_extension('H') && prev_virt) {
		जइ ((trap->cause < __riscv_xlen) &&
		    (csr_read(CSR_HEDELEG) & BIT(trap->cause))) {
			next_virt = true;
		}
	}

	/* Update MSTATUS MPV bits */
#जइ __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
	regs->mstatusH |= (next_virt) ? MSTATUSH_MPV : 0UL;
#अण्णहा
	regs->mstatus &= ~MSTATUS_MPV;
	regs->mstatus |= (next_virt) ? MSTATUS_MPV : 0UL;
#endif

	/* Update hypervisor CSRs if going to HS-mode */
	जइ (misa_extension('H') && !next_virt) {
		hstatus = csr_read(CSR_HSTATUS);
		जइ (prev_virt) {
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
	जइ (next_virt) {
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
		जइ (elp)
			vsstatus |= MSTATUS_SPELP;

		/* Set SPP for VS-mode */
		vsstatus &= ~SSTATUS_SPP;
		जइ (prev_mode == PRV_S)
			vsstatus |= (1UL << SSTATUS_SPP_SHIFT);

		/* Set SPIE for VS-mode */
		vsstatus &= ~SSTATUS_SPIE;
		जइ (vsstatus & SSTATUS_SIE)
			vsstatus |= (1UL << SSTATUS_SPIE_SHIFT);

		/* Clear SIE for VS-mode */
		vsstatus &= ~SSTATUS_SIE;

		/* Update VS-mode SSTATUS CSR */
		csr_write(CSR_VSSTATUS, vsstatus);
	} अण्णहा {
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
		जइ (prev_mode == PRV_S)
			regs->mstatus |= (1UL << MSTATUS_SPP_SHIFT);

		/* Set SPIE for S-mode */
		regs->mstatus &= ~MSTATUS_SPIE;
		जइ (regs->mstatus & MSTATUS_SIE)
			regs->mstatus |= (1UL << MSTATUS_SPIE_SHIFT);

		/* Clear SIE for S-mode */
		regs->mstatus &= ~MSTATUS_SIE;

		/* If elp was set, set it back in mstatus */
		जइ (elp)
			regs->mstatus |= MSTATUS_SPELP;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sbi_trap_nonaia_irq(unsigned long irq)
{
	विकल्प (irq) {
	पक्ष IRQ_M_TIMER:
		sbi_timer_process();
		भङ्ग;
	पक्ष IRQ_M_SOFT:
		sbi_ipi_process();
		भङ्ग;
	पक्ष IRQ_M_EXT:
		प्रत्यागम sbi_irqchip_process();
	स्वभाव:
		जइ (irq == sbi_pmu_irq_bit()) {
			sbi_pmu_ovf_irq();
			प्रत्यागम 0;
		}
		प्रत्यागम SBI_ENOENT;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sbi_trap_aia_irq(शून्य)
{
	पूर्णाङ्क rc;
	unsigned long mtopi;

	जाव ((mtopi = csr_read(CSR_MTOPI))) {
		mtopi = mtopi >> TOPI_IID_SHIFT;
		विकल्प (mtopi) {
		पक्ष IRQ_M_TIMER:
			sbi_timer_process();
			भङ्ग;
		पक्ष IRQ_M_SOFT:
			sbi_ipi_process();
			भङ्ग;
		पक्ष IRQ_M_EXT:
			rc = sbi_irqchip_process();
			जइ (rc)
				प्रत्यागम rc;
			भङ्ग;
		स्वभाव:
			जइ (mtopi == sbi_pmu_irq_bit()) {
				sbi_pmu_ovf_irq();
				भङ्ग;
			}

			प्रत्यागम SBI_ENOENT;
		}
	}

	प्रत्यागम 0;
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
संरचना sbi_trap_context *sbi_trap_handler(संरचना sbi_trap_context *tcntx)
{
	पूर्णाङ्क rc = SBI_ENOTSUPP;
	ध्रुव वर्ण *सन्देशः = "trap handler failed";
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ध्रुव संरचना sbi_trap_info *trap = &tcntx->trap;
	संरचना sbi_trap_regs *regs = &tcntx->regs;
	ulong mcause = tcntx->trap.cause;

	/* Update trap context pointer */
	tcntx->prev_context = sbi_trap_get_context(scratch);
	sbi_trap_set_context(scratch, tcntx);

	जइ (mcause & MCAUSE_IRQ_MASK) {
		जइ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_SMAIA))
			rc = sbi_trap_aia_irq();
		अण्णहा
			rc = sbi_trap_nonaia_irq(mcause & ~MCAUSE_IRQ_MASK);
		सन्देशः = "unhandled local interrupt";
		गच्छ trap_done;
	}

	विकल्प (mcause) {
	पक्ष CAUSE_ILLEGAL_INSTRUCTION:
		rc  = sbi_illegal_insn_handler(tcntx);
		सन्देशः = "illegal instruction handler failed";
		भङ्ग;
	पक्ष CAUSE_MISALIGNED_LOAD:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_LOAD);
		rc  = sbi_misaligned_load_handler(tcntx);
		सन्देशः = "misaligned load handler failed";
		भङ्ग;
	पक्ष CAUSE_MISALIGNED_STORE:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_STORE);
		rc  = sbi_misaligned_store_handler(tcntx);
		सन्देशः = "misaligned store handler failed";
		भङ्ग;
	पक्ष CAUSE_SUPERVISOR_ECALL:
	पक्ष CAUSE_MACHINE_ECALL:
		rc  = sbi_ecall_handler(tcntx);
		सन्देशः = "ecall handler failed";
		भङ्ग;
	पक्ष CAUSE_LOAD_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_LOAD);
		rc  = sbi_load_access_handler(tcntx);
		सन्देशः = "load fault handler failed";
		भङ्ग;
	पक्ष CAUSE_STORE_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_STORE);
		rc  = sbi_store_access_handler(tcntx);
		सन्देशः = "store fault handler failed";
		भङ्ग;
	पक्ष CAUSE_DOUBLE_TRAP:
		rc  = sbi_double_trap_handler(tcntx);
		सन्देशः = "double trap handler failed";
		भङ्ग;
	स्वभाव:
		/* If the trap came from S or U mode, redirect it there */
		सन्देशः = "trap redirect failed";
		rc  = sbi_trap_redirect(regs, trap);
		भङ्ग;
	}

trap_done:
	जइ (rc)
		sbi_trap_error(सन्देशः, rc, tcntx);

	जइ (sbi_mstatus_prev_mode(regs->mstatus) != PRV_M)
		sbi_sse_process_pending_events(regs);

	sbi_trap_set_context(scratch, tcntx->prev_context);
	प्रत्यागम tcntx;
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
संरचना sbi_trap_context *sbi_trap_rnmi_handler(संरचना sbi_trap_context *tcntx)
{
	पूर्णाङ्क rc;
	ध्रुव संरचना sbi_platform *plat = sbi_platform_thishart_ptr();
	ध्रुव संरचना sbi_platform_operations *ops = sbi_platform_ops(plat);

	/* Call platform-specific NMI handler if registered */
	जइ (ops && ops->rnmi_handler) {
		rc = ops->rnmi_handler(tcntx);
		जइ (rc) {
			/* Platform handler failed to handle NMI */
			sbi_trap_error("platform NMI handler failed", rc, tcntx);
		}
		प्रत्यागम tcntx;
	}

	/* No platform handler - treat as unhandled NMI */
	sbi_trap_error("unhandled NMI (no platform rnmi_handler)",
		       SBI_ENOTSUPP, tcntx);

	/* Never returns */
	प्रत्यागम tcntx;
}

स्थिर पूर्णाङ्क delegate_traps(संरचना sbi_scratch *scratch)
{
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	unsigned long interrupts, exceptions;

	जइ (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist */
		प्रत्यागम 0;

	/* Send M-mode interrupts and most exceptions to S-mode */
	interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;
	interrupts |= sbi_pmu_irq_mask();

	exceptions = (1U << CAUSE_MISALIGNED_FETCH) | (1U << CAUSE_BREAKPOINT) |
		     (1U << CAUSE_USER_ECALL);
	जइ (sbi_platform_has_mfaults_delegation(plat))
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
	जइ (misa_extension('H')) {
		exceptions |= (1U << CAUSE_VIRTUAL_SUPERVISOR_ECALL);
		exceptions |= (1U << CAUSE_FETCH_GUEST_PAGE_FAULT);
		exceptions |= (1U << CAUSE_LOAD_GUEST_PAGE_FAULT);
		exceptions |= (1U << CAUSE_VIRTUAL_INST_FAULT);
		exceptions |= (1U << CAUSE_STORE_GUEST_PAGE_FAULT);
	}

	csr_write(CSR_MIDELEG, interrupts);
	csr_write(CSR_MEDELEG, exceptions);

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_trap_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	प्रत्यागम delegate_traps(scratch);
}
