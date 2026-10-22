/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_double_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_illegal_insn.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_irqchip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap_ldst.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_sse.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>

ਸਥਿਰ ਖਾਲੀ sbi_trap_error_one(ਅਚਲ ਬਣਤਰ sbi_trap_context *tcntx,
			       ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, u32 hartid, u32 ਡੂੰਘਾਈ)
{
	ਅਚਲ ਬਣਤਰ sbi_trap_info *trap = &tcntx->trap;
	ਅਚਲ ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "mcause", trap->cause, "mtval", trap->tval);
	ਜੇ (misa_extension('H')) {
		sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
			   hartid, ਡੂੰਘਾਈ, "mtval2", trap->tval2, "mtinst", trap->tinst);
	}
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "mepc", regs->mepc, "mstatus", regs->mstatus);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "ra", regs->ra, "sp", regs->sp);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "gp", regs->gp, "tp", regs->tp);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "s0", regs->s0, "s1", regs->s1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "a0", regs->a0, "a1", regs->a1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "a2", regs->a2, "a3", regs->a3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "a4", regs->a4, "a5", regs->a5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "a6", regs->a6, "a7", regs->a7);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "s2", regs->s2, "s3", regs->s3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "s4", regs->s4, "s5", regs->s5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "s6", regs->s6, "s7", regs->s7);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "s8", regs->s8, "s9", regs->s9);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "s10", regs->s10, "s11", regs->s11);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "t0", regs->t0, "t1", regs->t1);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "t2", regs->t2, "t3", regs->t3);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "t4", regs->t4, "t5", regs->t5);
	sbi_printf("%s: hart%d: trap%d: %s=0x%" PRILX "\n", ਅਗੇਤਰ,
		   hartid, ਡੂੰਘਾਈ, "t6", regs->t6);
}

ਸਥਿਰ ਖਾਲੀ __noreturn sbi_trap_error(ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ, ਪੂਰਨ rc,
				      ਅਚਲ ਬਣਤਰ sbi_trap_context *tcntx)
{
	u32 ਡੂੰਘਾਈ = 0, hartid = current_hartid();
	ਅਚਲ ਬਣਤਰ sbi_trap_context *tc;

	ਲਈ (tc = tcntx; tc; tc = tc->prev_context)
		ਡੂੰਘਾਈ++;

	sbi_printf("\n");
	sbi_printf("%s: hart%d: trap%d: %s (error %d)\n", __func__,
		   hartid, ਡੂੰਘਾਈ - 1, ਸੁਨੇਹਾ, rc);
	ਲਈ (tc = tcntx; tc; tc = tc->prev_context)
		sbi_trap_error_one(tc, __func__, hartid, --ਡੂੰਘਾਈ);

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
ਪੂਰਨ sbi_trap_redirect(ਬਣਤਰ sbi_trap_regs *regs,
		      ਅਚਲ ਬਣਤਰ sbi_trap_info *trap)
{
	ulong hstatus, vsstatus, prev_ਬਹੁਲਕ;
	bool elp = false;
	bool prev_virt = sbi_regs_from_virt(regs);
	/* By default, we redirect to HS-mode */
	bool ਅਗਲਾ_virt = false;

	/* Sanity check on previous mode */
	prev_ਬਹੁਲਕ = sbi_mstatus_prev_ਬਹੁਲਕ(regs->mstatus);
	ਜੇ (prev_ਬਹੁਲਕ != PRV_S && prev_ਬਹੁਲਕ != PRV_U)
		ਵਾਪਸ SBI_ENOTSUPP;

	/* If hart support for zicfilp, clear MPELP because redirecting to VS or (H)S */
	ਜੇ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_ZICFILP)) {
#ਜੇ __riscv_xlen == 32
		elp = regs->mstatusH & MSTATUSH_MPELP;
		regs->mstatusH &= ~MSTATUSH_MPELP;
#ਨਹੀਂਤਾਂ
		elp = regs->mstatus & MSTATUS_MPELP;
		regs->mstatus &= ~MSTATUS_MPELP;
#ਜੇ_ਅੰਤ
	}

	/* If exceptions came from VS/VU-mode, redirect to VS-mode if
	 * delegated in hedeleg
	 */
	ਜੇ (misa_extension('H') && prev_virt) {
		ਜੇ ((trap->cause < __riscv_xlen) &&
		    (csr_read(CSR_HEDELEG) & ਦੋਆਧਾਰੀ_ਅੰਕ(trap->cause))) {
			ਅਗਲਾ_virt = true;
		}
	}

	/* Update MSTATUS MPV bits */
#ਜੇ __riscv_xlen == 32
	regs->mstatusH &= ~MSTATUSH_MPV;
	regs->mstatusH |= (ਅਗਲਾ_virt) ? MSTATUSH_MPV : 0UL;
#ਨਹੀਂਤਾਂ
	regs->mstatus &= ~MSTATUS_MPV;
	regs->mstatus |= (ਅਗਲਾ_virt) ? MSTATUS_MPV : 0UL;
#ਜੇ_ਅੰਤ

	/* Update hypervisor CSRs if going to HS-mode */
	ਜੇ (misa_extension('H') && !ਅਗਲਾ_virt) {
		hstatus = csr_read(CSR_HSTATUS);
		ਜੇ (prev_virt) {
			/* hstatus.SPVP is only updated if coming from VS/VU-mode */
			hstatus &= ~HSTATUS_SPVP;
			hstatus |= (prev_ਬਹੁਲਕ == PRV_S) ? HSTATUS_SPVP : 0;
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
	ਜੇ (ਅਗਲਾ_virt) {
		/* Update VS-mode exception info */
		csr_write(CSR_VSTVAL, trap->tval);
		csr_write(CSR_VSEPC, regs->mepc);
		csr_write(CSR_VSCAUSE, trap->cause);

		/* Set MEPC to VS-mode exception vector base */
		regs->mepc = csr_read(CSR_VSTVEC) & ~MTVEC_ਬਹੁਲਕ;

		/* Set MPP to VS-mode */
		regs->mstatus &= ~MSTATUS_MPP;
		regs->mstatus |= (PRV_S << MSTATUS_MPP_ਖਿਸਕਾਓ);

		/* Get VS-mode SSTATUS CSR */
		vsstatus = csr_read(CSR_VSSTATUS);

		/* If elp was set, set it back in vsstatus */
		ਜੇ (elp)
			vsstatus |= MSTATUS_SPELP;

		/* Set SPP for VS-mode */
		vsstatus &= ~SSTATUS_SPP;
		ਜੇ (prev_ਬਹੁਲਕ == PRV_S)
			vsstatus |= (1UL << SSTATUS_SPP_ਖਿਸਕਾਓ);

		/* Set SPIE for VS-mode */
		vsstatus &= ~SSTATUS_SPIE;
		ਜੇ (vsstatus & SSTATUS_SIE)
			vsstatus |= (1UL << SSTATUS_SPIE_ਖਿਸਕਾਓ);

		/* Clear SIE for VS-mode */
		vsstatus &= ~SSTATUS_SIE;

		/* Update VS-mode SSTATUS CSR */
		csr_write(CSR_VSSTATUS, vsstatus);
	} ਨਹੀਂਤਾਂ {
		/* Update S-mode exception info */
		csr_write(CSR_STVAL, trap->tval);
		csr_write(CSR_SEPC, regs->mepc);
		csr_write(CSR_SCAUSE, trap->cause);

		/* Set MEPC to S-mode exception vector base */
		regs->mepc = csr_read(CSR_STVEC) & ~MTVEC_ਬਹੁਲਕ;

		/* Set MPP to S-mode */
		regs->mstatus &= ~MSTATUS_MPP;
		regs->mstatus |= (PRV_S << MSTATUS_MPP_ਖਿਸਕਾਓ);

		/* Set SPP for S-mode */
		regs->mstatus &= ~MSTATUS_SPP;
		ਜੇ (prev_ਬਹੁਲਕ == PRV_S)
			regs->mstatus |= (1UL << MSTATUS_SPP_ਖਿਸਕਾਓ);

		/* Set SPIE for S-mode */
		regs->mstatus &= ~MSTATUS_SPIE;
		ਜੇ (regs->mstatus & MSTATUS_SIE)
			regs->mstatus |= (1UL << MSTATUS_SPIE_ਖਿਸਕਾਓ);

		/* Clear SIE for S-mode */
		regs->mstatus &= ~MSTATUS_SIE;

		/* If elp was set, set it back in mstatus */
		ਜੇ (elp)
			regs->mstatus |= MSTATUS_SPELP;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sbi_trap_nonaia_irq(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ irq)
{
	ਚੋਣ (irq) {
	ਹਾਲਤ IRQ_M_TIMER:
		sbi_timer_process();
		ਰੋਕੋ;
	ਹਾਲਤ IRQ_M_SOFT:
		sbi_ipi_process();
		ਰੋਕੋ;
	ਹਾਲਤ IRQ_M_EXT:
		ਵਾਪਸ sbi_irqchip_process();
	ਮੂਲ:
		ਜੇ (irq == sbi_pmu_irq_ਦੋਆਧਾਰੀ_ਅੰਕ()) {
			sbi_pmu_ovf_irq();
			ਵਾਪਸ 0;
		}
		ਵਾਪਸ SBI_ENOENT;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sbi_trap_aia_irq(ਖਾਲੀ)
{
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mtopi;

	ਜਦੋਂ ((mtopi = csr_read(CSR_MTOPI))) {
		mtopi = mtopi >> TOPI_IID_ਖਿਸਕਾਓ;
		ਚੋਣ (mtopi) {
		ਹਾਲਤ IRQ_M_TIMER:
			sbi_timer_process();
			ਰੋਕੋ;
		ਹਾਲਤ IRQ_M_SOFT:
			sbi_ipi_process();
			ਰੋਕੋ;
		ਹਾਲਤ IRQ_M_EXT:
			rc = sbi_irqchip_process();
			ਜੇ (rc)
				ਵਾਪਸ rc;
			ਰੋਕੋ;
		ਮੂਲ:
			ਜੇ (mtopi == sbi_pmu_irq_ਦੋਆਧਾਰੀ_ਅੰਕ()) {
				sbi_pmu_ovf_irq();
				ਰੋਕੋ;
			}

			ਵਾਪਸ SBI_ENOENT;
		}
	}

	ਵਾਪਸ 0;
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
ਬਣਤਰ sbi_trap_context *sbi_trap_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਪੂਰਨ rc = SBI_ENOTSUPP;
	ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = "trap handler failed";
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਅਚਲ ਬਣਤਰ sbi_trap_info *trap = &tcntx->trap;
	ਬਣਤਰ sbi_trap_regs *regs = &tcntx->regs;
	ulong mcause = tcntx->trap.cause;

	/* Update trap context pointer */
	tcntx->prev_context = sbi_trap_get_context(scratch);
	sbi_trap_ਸੈੱਟ_context(scratch, tcntx);

	ਜੇ (mcause & MCAUSE_IRQ_ਨਕਾਬ) {
		ਜੇ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_SMAIA))
			rc = sbi_trap_aia_irq();
		ਨਹੀਂਤਾਂ
			rc = sbi_trap_nonaia_irq(mcause & ~MCAUSE_IRQ_ਨਕਾਬ);
		ਸੁਨੇਹਾ = "unhandled local interrupt";
		ਜਾਓ trap_ਪੂਰਾ;
	}

	ਚੋਣ (mcause) {
	ਹਾਲਤ CAUSE_ILLEGAL_INSTRUCTION:
		rc  = sbi_illegal_insn_handler(tcntx);
		ਸੁਨੇਹਾ = "illegal instruction handler failed";
		ਰੋਕੋ;
	ਹਾਲਤ CAUSE_MISALIGNED_ਲੱਦੋ:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_ਲੱਦੋ);
		rc  = sbi_misaligned_ਲੱਦੋ_handler(tcntx);
		ਸੁਨੇਹਾ = "misaligned load handler failed";
		ਰੋਕੋ;
	ਹਾਲਤ CAUSE_MISALIGNED_STORE:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_MISALIGNED_STORE);
		rc  = sbi_misaligned_store_handler(tcntx);
		ਸੁਨੇਹਾ = "misaligned store handler failed";
		ਰੋਕੋ;
	ਹਾਲਤ CAUSE_SUPERVISOR_ECALL:
	ਹਾਲਤ CAUSE_MACHINE_ECALL:
		rc  = sbi_ecall_handler(tcntx);
		ਸੁਨੇਹਾ = "ecall handler failed";
		ਰੋਕੋ;
	ਹਾਲਤ CAUSE_ਲੱਦੋ_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_ਲੱਦੋ);
		rc  = sbi_ਲੱਦੋ_access_handler(tcntx);
		ਸੁਨੇਹਾ = "load fault handler failed";
		ਰੋਕੋ;
	ਹਾਲਤ CAUSE_STORE_ACCESS:
		sbi_pmu_ctr_incr_fw(SBI_PMU_FW_ACCESS_STORE);
		rc  = sbi_store_access_handler(tcntx);
		ਸੁਨੇਹਾ = "store fault handler failed";
		ਰੋਕੋ;
	ਹਾਲਤ CAUSE_DOUBLE_TRAP:
		rc  = sbi_double_trap_handler(tcntx);
		ਸੁਨੇਹਾ = "double trap handler failed";
		ਰੋਕੋ;
	ਮੂਲ:
		/* If the trap came from S or U mode, redirect it there */
		ਸੁਨੇਹਾ = "trap redirect failed";
		rc  = sbi_trap_redirect(regs, trap);
		ਰੋਕੋ;
	}

trap_ਪੂਰਾ:
	ਜੇ (rc)
		sbi_trap_error(ਸੁਨੇਹਾ, rc, tcntx);

	ਜੇ (sbi_mstatus_prev_ਬਹੁਲਕ(regs->mstatus) != PRV_M)
		sbi_sse_process_pending_events(regs);

	sbi_trap_ਸੈੱਟ_context(scratch, tcntx->prev_context);
	ਵਾਪਸ tcntx;
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
ਬਣਤਰ sbi_trap_context *sbi_trap_rnmi_handler(ਬਣਤਰ sbi_trap_context *tcntx)
{
	ਪੂਰਨ rc;
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_thishart_ptr();
	ਅਚਲ ਬਣਤਰ sbi_platform_operations *ops = sbi_platform_ops(plat);

	/* Call platform-specific NMI handler if registered */
	ਜੇ (ops && ops->rnmi_handler) {
		rc = ops->rnmi_handler(tcntx);
		ਜੇ (rc) {
			/* Platform handler failed to handle NMI */
			sbi_trap_error("platform NMI handler failed", rc, tcntx);
		}
		ਵਾਪਸ tcntx;
	}

	/* No platform handler - treat as unhandled NMI */
	sbi_trap_error("unhandled NMI (no platform rnmi_handler)",
		       SBI_ENOTSUPP, tcntx);

	/* Never returns */
	ਵਾਪਸ tcntx;
}

ਸਥਿਰ ਪੂਰਨ delegate_traps(ਬਣਤਰ sbi_scratch *scratch)
{
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ interrupts, exceptions;

	ਜੇ (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist */
		ਵਾਪਸ 0;

	/* Send M-mode interrupts and most exceptions to S-mode */
	interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;
	interrupts |= sbi_pmu_irq_ਨਕਾਬ();

	exceptions = (1U << CAUSE_MISALIGNED_FETCH) | (1U << CAUSE_BREAKPOINT) |
		     (1U << CAUSE_USER_ECALL);
	ਜੇ (sbi_platform_has_mfaults_delegation(plat))
		exceptions |= (1U << CAUSE_FETCH_ਪੰਨਾ_FAULT) |
			      (1U << CAUSE_ਲੱਦੋ_ਪੰਨਾ_FAULT) |
			      (1U << CAUSE_STORE_ਪੰਨਾ_FAULT) |
			      (1U << CAUSE_SW_CHECK_EXCP);

	/*
	 * If hypervisor extension available then we only handle hypervisor
	 * calls (i.e. ecalls from HS-mode) in M-mode.
	 *
	 * The HS-mode will additionally handle supervisor calls (i.e. ecalls
	 * from VS-mode), Guest page faults and Virtual interrupts.
	 */
	ਜੇ (misa_extension('H')) {
		exceptions |= (1U << CAUSE_VIRTUAL_SUPERVISOR_ECALL);
		exceptions |= (1U << CAUSE_FETCH_GUEST_ਪੰਨਾ_FAULT);
		exceptions |= (1U << CAUSE_ਲੱਦੋ_GUEST_ਪੰਨਾ_FAULT);
		exceptions |= (1U << CAUSE_VIRTUAL_INST_FAULT);
		exceptions |= (1U << CAUSE_STORE_GUEST_ਪੰਨਾ_FAULT);
	}

	csr_write(CSR_MIDELEG, interrupts);
	csr_write(CSR_MEDELEG, exceptions);

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_trap_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਵਾਪਸ delegate_traps(scratch);
}
