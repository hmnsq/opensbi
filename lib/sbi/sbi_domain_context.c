/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) IPADS@SJTU 2023. All rights reserved.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain_context.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_vector.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_fp.h>

/** Context representation for a hart within a domain */
ರಚನೆ hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	ರಚನೆ sbi_trap_context trap_ctx;

	/** Supervisor status register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sstatus;
	/** Supervisor interrupt enable register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sie;
	/** Supervisor trap vector base address register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ stvec;
	/** Supervisor scratch register for temporary storage */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sscratch;
	/** Supervisor exception program counter register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sepc;
	/** Supervisor cause register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ scause;
	/** Supervisor trap value register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ stval;
	/** Supervisor interrupt pending register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sip;
	/** Supervisor address translation and protection register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ satp;
	/** Counter-enable register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ scounteren;
	/** Supervisor environment configuration register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ senvcfg;
	/** Supervisor resource management configuration register */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ srmcfg;

	/** Float context state */
	ರಚನೆ sbi_fp_context fp_ctx;
	/** Vector context state */
	ರಚನೆ sbi_ಸದಿಶ_context *vec_ctx;

	/** Reference to the owning domain */
	ರಚನೆ sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	ರಚನೆ hart_context *prev_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

ಸ್ಥಿರ ರಚನೆ sbi_domain_state dcstate;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ರಚನೆ hart_context *hart_context_get(ರಚನೆ sbi_domain *dom,
						    u32 hartindex)
{
	ರಚನೆ hart_context **dom_hartindex_ಗಮ್ಯ_context_ಕೋಷ್ಟಕ;

	dom_hartindex_ಗಮ್ಯ_context_ಕೋಷ್ಟಕ = sbi_domain_state_ptr(dom, &dcstate);
	ಆದರೆ (!dom_hartindex_ಗಮ್ಯ_context_ಕೋಷ್ಟಕ || !sbi_hartindex_valid(hartindex))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ dom_hartindex_ಗಮ್ಯ_context_ಕೋಷ್ಟಕ[hartindex];
}

ಸ್ಥಿರ ಶೂನ್ಯ hart_context_ಗಣ(ರಚನೆ sbi_domain *dom, u32 hartindex,
			     ರಚನೆ hart_context *hc)
{
	ರಚನೆ hart_context **dom_hartindex_ಗಮ್ಯ_context_ಕೋಷ್ಟಕ;

	dom_hartindex_ಗಮ್ಯ_context_ಕೋಷ್ಟಕ = sbi_domain_state_ptr(dom, &dcstate);
	ಆದರೆ (!dom_hartindex_ಗಮ್ಯ_context_ಕೋಷ್ಟಕ || !sbi_hartindex_valid(hartindex))
		ಹಿಂತಿರುಗಿ;

	dom_hartindex_ಗಮ್ಯ_context_ಕೋಷ್ಟಕ[hartindex] = hc;
}

/** Macro to obtain the current hart's context pointer */
#ವ್ಯಾಖ್ಯಾನಿಸು hart_context_thishart_get()					\
	hart_context_get(sbi_domain_thishart_ptr(),			\
			 current_hartindex())

/**
 * Switches the HART context from the current domain to the target domain.
 * This includes changing domain assignments and reconfiguring PMP, as well
 * as saving and restoring CSRs and trap states.
 *
 * @param ctx pointer to the current HART context
 * @param dom_ctx pointer to the target domain context
 *
 * @return 0 on success and negative error code on failure
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ switch_ಗಮ್ಯ_ಮುಂದಿನ_domain_context(ರಚನೆ hart_context *ctx,
					  ರಚನೆ hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_ಬೀಗ)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_ಬೀಗ)
{
	u32 hartindex = current_hartindex();
	ರಚನೆ sbi_trap_context *trap_ctx;
	ರಚನೆ sbi_domain *current_dom, *ಗುರಿ_dom;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಆದರೆ (!ctx || !dom_ctx || ctx == dom_ctx)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	current_dom = ctx->dom;
	ಗುರಿ_dom = dom_ctx->dom;
	/* Assign current hart to target domain */
	spin_ಬೀಗ(&current_dom->assigned_harts_ಬೀಗ);
	sbi_hartmask_clear_hartindex(hartindex, &current_dom->assigned_harts);
	spin_unlock(&current_dom->assigned_harts_ಬೀಗ);

	sbi_update_hartindex_ಗಮ್ಯ_domain(hartindex, ಗುರಿ_dom);

	spin_ಬೀಗ(&ಗುರಿ_dom->assigned_harts_ಬೀಗ);
	sbi_hartmask_ಗಣ_hartindex(hartindex, &ಗುರಿ_dom->assigned_harts);
	spin_unlock(&ಗುರಿ_dom->assigned_harts_ಬೀಗ);

	/* Save current CSR context and restore target domain's CSR context */
	ctx->sstatus	= csr_ಅದಲುಬದಲು(CSR_SSTATUS, dom_ctx->sstatus);
	ctx->sie	= csr_ಅದಲುಬದಲು(CSR_SIE, dom_ctx->sie);
	ctx->stvec	= csr_ಅದಲುಬದಲು(CSR_STVEC, dom_ctx->stvec);
	ctx->sscratch	= csr_ಅದಲುಬದಲು(CSR_SSCRATCH, dom_ctx->sscratch);
	ctx->sepc	= csr_ಅದಲುಬದಲು(CSR_SEPC, dom_ctx->sepc);
	ctx->scause	= csr_ಅದಲುಬದಲು(CSR_SCAUSE, dom_ctx->scause);
	ctx->stval	= csr_ಅದಲುಬದಲು(CSR_STVAL, dom_ctx->stval);
	ctx->sip	= csr_ಅದಲುಬದಲು(CSR_SIP, dom_ctx->sip);
	ctx->satp	= csr_ಅದಲುಬದಲು(CSR_SATP, dom_ctx->satp);
	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_ಅದಲುಬದಲು(CSR_SCOUNTEREN, dom_ctx->scounteren);
	ಆದರೆ (sbi_hart_priv_ಆವೃತ್ತಿ(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_ಅದಲುಬದಲು(CSR_SENVCFG, dom_ctx->senvcfg);
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_ಅದಲುಬದಲು(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_ಸದಿಶ_save(ctx->vec_ctx);
		sbi_ಸದಿಶ_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, ಗಾತ್ರ(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, ಗಾತ್ರ(*trap_ctx));

	/*
	 * Re-configure PMP settings for the new domain
	 *
	 * This will internally perform full SFENCE / HFENCE which
	 * is also required for some of the above CSR updates (such
	 * as satp CSR).
	 */
	sbi_hart_protection_reconfigure(scratch, current_dom, ಗುರಿ_dom);

	/* Mark current context structure initialized because context saved */
	ctx->initialized = true;

	/* If target domain context is not initialized or runnable */
	ಆದರೆ (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		ಆದರೆ (current_hartid() == ಗುರಿ_dom->boot_hartid)
			sbi_hart_switch_ಬಹುಲಕ(ಗುರಿ_dom->boot_hartid,
					     ಗುರಿ_dom->ಮುಂದಿನ_arg1,
					     ಗುರಿ_dom->ಮುಂದಿನ_addr,
					     ಗುರಿ_dom->ಮುಂದಿನ_ಬಹುಲಕ,
					     false);
		ಇಲ್ಲದಿದ್ದರೆ
			sbi_hsm_hart_stop(scratch, true);
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hart_context_init(u32 hartindex)
{
	size_t vec_ಗಾತ್ರ;
	ರಚನೆ hart_context *ctx;
	ರಚನೆ sbi_domain *dom;

	sbi_domain_for_each(dom) {
		ಆದರೆ (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			ಮುಂದುವರಿ;

		ctx = sbi_zalloc(ಗಾತ್ರ(ರಚನೆ hart_context));
		ಆದರೆ (!ctx)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		ಆದರೆ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_ಗಾತ್ರ = sbi_ಸದಿಶ_context_ಗಾತ್ರ();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_ಗಾತ್ರ);
			ಆದರೆ (!ctx->vec_ctx) {
				sbi_free(ctx);
				ಹಿಂತಿರುಗಿ SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_ಗಣ(dom, hartindex, ctx);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_domain_context_enter(ರಚನೆ sbi_domain *dom)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ hart_context *dom_ctx;
	ರಚನೆ hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	ಆದರೆ (!dom || dom == sbi_domain_thishart_ptr())
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	ಆದರೆ (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	ಆದರೆ (!ctx) {
		rc = hart_context_init(hartindex);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		ctx = hart_context_thishart_get();
		ಆದರೆ (!ctx)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	ಆದರೆ (!dom_ctx)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->prev_ctx = ctx;

	ಹಿಂತಿರುಗಿ switch_ಗಮ್ಯ_ಮುಂದಿನ_domain_context(ctx, dom_ctx);
}

ಪೂರ್ಣಾಂಕ sbi_domain_context_exit(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc;
	u32 hartindex = current_hartindex();
	ರಚನೆ sbi_domain *dom;
	ರಚನೆ hart_context *ctx = hart_context_thishart_get();
	ರಚನೆ hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	ಆದರೆ (!ctx) {
		rc = hart_context_init(hartindex);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		ctx = hart_context_thishart_get();
		ಆದರೆ (!ctx)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	dom_ctx = ctx->prev_ctx;
	ctx->prev_ctx = ಖಾಲಿ;

	/* If no previous caller context */
	ಆದರೆ (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = ಖಾಲಿ;
		sbi_domain_for_each(dom) {
			ಆದರೆ (dom == sbi_domain_thishart_ptr())
				ಮುಂದುವರಿ;

			ಆದರೆ (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				ಮುಂದುವರಿ;

			tmp = hart_context_get(dom, hartindex);
			ಆದರೆ (tmp && tmp->initialized)
				ಮುಂದುವರಿ;

			ಆದರೆ (!dom_ctx || tmp->dom->init_ಕ್ರಮ < dom_ctx->dom->init_ಕ್ರಮ)
				dom_ctx = tmp;
		}
	}
	ಆದರೆ (!dom_ctx)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	ಹಿಂತಿರುಗಿ switch_ಗಮ್ಯ_ಮುಂದಿನ_domain_context(ctx, dom_ctx);
}

ಪೂರ್ಣಾಂಕ sbi_domain_context_init(ಶೂನ್ಯ)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ hart_context *) * sbi_hart_ಎಣಿಕೆ();

	ಹಿಂತಿರುಗಿ sbi_domain_ರಿಜಿಸ್ಟರ್_state(&dcstate);
}

ಶೂನ್ಯ sbi_domain_context_deinit(ಶೂನ್ಯ)
{
	sbi_domain_unregister_state(&dcstate);
}
