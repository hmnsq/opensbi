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
ಸಂರಚನಾ hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	ಸಂರಚನಾ sbi_trap_context trap_ctx;

	/** Supervisor status register */
	unsigned long sstatus;
	/** Supervisor interrupt enable register */
	unsigned long sie;
	/** Supervisor trap vector base address register */
	unsigned long stvec;
	/** Supervisor scratch register for temporary storage */
	unsigned long sscratch;
	/** Supervisor exception program counter register */
	unsigned long sepc;
	/** Supervisor cause register */
	unsigned long scause;
	/** Supervisor trap value register */
	unsigned long stval;
	/** Supervisor interrupt pending register */
	unsigned long sip;
	/** Supervisor address translation and protection register */
	unsigned long satp;
	/** Counter-enable register */
	unsigned long scounteren;
	/** Supervisor environment configuration register */
	unsigned long senvcfg;
	/** Supervisor resource management configuration register */
	unsigned long srmcfg;

	/** Float context state */
	ಸಂರಚನಾ sbi_fp_context fp_ctx;
	/** Vector context state */
	ಸಂರಚನಾ sbi_vector_context *vec_ctx;

	/** Reference to the owning domain */
	ಸಂರಚನಾ sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	ಸಂರಚನಾ hart_context *prev_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

ಸ್ಥಿರ ಸಂರಚನಾ sbi_domain_state dcstate;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಸಂರಚನಾ hart_context *hart_context_get(ಸಂರಚನಾ sbi_domain *dom,
						    u32 hartindex)
{
	ಸಂರಚನಾ hart_context **dom_hartindex_to_context_table;

	dom_hartindex_to_context_table = sbi_domain_state_ptr(dom, &dcstate);
	ಯದಿ (!dom_hartindex_to_context_table || !sbi_hartindex_valid(hartindex))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ dom_hartindex_to_context_table[hartindex];
}

ಸ್ಥಿರ ಶೂನ್ಯ hart_context_set(ಸಂರಚನಾ sbi_domain *dom, u32 hartindex,
			     ಸಂರಚನಾ hart_context *hc)
{
	ಸಂರಚನಾ hart_context **dom_hartindex_to_context_table;

	dom_hartindex_to_context_table = sbi_domain_state_ptr(dom, &dcstate);
	ಯದಿ (!dom_hartindex_to_context_table || !sbi_hartindex_valid(hartindex))
		ಪ್ರತ್ಯಾಗಮ;

	dom_hartindex_to_context_table[hartindex] = hc;
}

/** Macro to obtain the current hart's context pointer */
#define hart_context_thishart_get()					\
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ switch_to_next_domain_context(ಸಂರಚನಾ hart_context *ctx,
					  ಸಂರಚನಾ hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_lock)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_lock)
{
	u32 hartindex = current_hartindex();
	ಸಂರಚನಾ sbi_trap_context *trap_ctx;
	ಸಂರಚನಾ sbi_domain *current_dom, *target_dom;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	ಯದಿ (!ctx || !dom_ctx || ctx == dom_ctx)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	current_dom = ctx->dom;
	target_dom = dom_ctx->dom;
	/* Assign current hart to target domain */
	spin_lock(&current_dom->assigned_harts_lock);
	sbi_hartmask_clear_hartindex(hartindex, &current_dom->assigned_harts);
	spin_unlock(&current_dom->assigned_harts_lock);

	sbi_update_hartindex_to_domain(hartindex, target_dom);

	spin_lock(&target_dom->assigned_harts_lock);
	sbi_hartmask_set_hartindex(hartindex, &target_dom->assigned_harts);
	spin_unlock(&target_dom->assigned_harts_lock);

	/* Save current CSR context and restore target domain's CSR context */
	ctx->sstatus	= csr_swap(CSR_SSTATUS, dom_ctx->sstatus);
	ctx->sie	= csr_swap(CSR_SIE, dom_ctx->sie);
	ctx->stvec	= csr_swap(CSR_STVEC, dom_ctx->stvec);
	ctx->sscratch	= csr_swap(CSR_SSCRATCH, dom_ctx->sscratch);
	ctx->sepc	= csr_swap(CSR_SEPC, dom_ctx->sepc);
	ctx->scause	= csr_swap(CSR_SCAUSE, dom_ctx->scause);
	ctx->stval	= csr_swap(CSR_STVAL, dom_ctx->stval);
	ctx->sip	= csr_swap(CSR_SIP, dom_ctx->sip);
	ctx->satp	= csr_swap(CSR_SATP, dom_ctx->satp);
	ಯದಿ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_swap(CSR_SCOUNTEREN, dom_ctx->scounteren);
	ಯದಿ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_swap(CSR_SENVCFG, dom_ctx->senvcfg);
	ಯದಿ (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_swap(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	ಯದಿ (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	ಯದಿ (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_vector_save(ctx->vec_ctx);
		sbi_vector_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, ಪರಿಮಾಣ(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, ಪರಿಮಾಣ(*trap_ctx));

	/*
	 * Re-configure PMP settings for the new domain
	 *
	 * This will internally perform full SFENCE / HFENCE which
	 * is also required for some of the above CSR updates (such
	 * as satp CSR).
	 */
	sbi_hart_protection_reconfigure(scratch, current_dom, target_dom);

	/* Mark current context structure initialized because context saved */
	ctx->initialized = true;

	/* If target domain context is not initialized or runnable */
	ಯದಿ (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		ಯದಿ (current_hartid() == target_dom->boot_hartid)
			sbi_hart_switch_mode(target_dom->boot_hartid,
					     target_dom->next_arg1,
					     target_dom->next_addr,
					     target_dom->next_mode,
					     false);
		ಅನ್ಯಥಾ
			sbi_hsm_hart_stop(scratch, true);
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hart_context_init(u32 hartindex)
{
	size_t vec_size;
	ಸಂರಚನಾ hart_context *ctx;
	ಸಂರಚನಾ sbi_domain *dom;

	sbi_domain_for_each(dom) {
		ಯದಿ (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			ಅನುವರ್ತ;

		ctx = sbi_zalloc(ಪರಿಮಾಣ(ಸಂರಚನಾ hart_context));
		ಯದಿ (!ctx)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		ಯದಿ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_size = sbi_vector_context_size();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_size);
			ಯದಿ (!ctx->vec_ctx) {
				sbi_free(ctx);
				ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_set(dom, hartindex, ctx);
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_domain_context_enter(ಸಂರಚನಾ sbi_domain *dom)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ hart_context *dom_ctx;
	ಸಂರಚನಾ hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	ಯದಿ (!dom || dom == sbi_domain_thishart_ptr())
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	ಯದಿ (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	ಯದಿ (!ctx) {
		rc = hart_context_init(hartindex);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		ctx = hart_context_thishart_get();
		ಯದಿ (!ctx)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	ಯದಿ (!dom_ctx)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->prev_ctx = ctx;

	ಪ್ರತ್ಯಾಗಮ switch_to_next_domain_context(ctx, dom_ctx);
}

ಪೂರ್ಣಾಙ್ಕ sbi_domain_context_exit(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	u32 hartindex = current_hartindex();
	ಸಂರಚನಾ sbi_domain *dom;
	ಸಂರಚನಾ hart_context *ctx = hart_context_thishart_get();
	ಸಂರಚನಾ hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	ಯದಿ (!ctx) {
		rc = hart_context_init(hartindex);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		ctx = hart_context_thishart_get();
		ಯದಿ (!ctx)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	dom_ctx = ctx->prev_ctx;
	ctx->prev_ctx = ರಿಕ್ತ;

	/* If no previous caller context */
	ಯದಿ (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = ರಿಕ್ತ;
		sbi_domain_for_each(dom) {
			ಯದಿ (dom == sbi_domain_thishart_ptr())
				ಅನುವರ್ತ;

			ಯದಿ (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				ಅನುವರ್ತ;

			tmp = hart_context_get(dom, hartindex);
			ಯದಿ (tmp && tmp->initialized)
				ಅನುವರ್ತ;

			ಯದಿ (!dom_ctx || tmp->dom->init_order < dom_ctx->dom->init_order)
				dom_ctx = tmp;
		}
	}
	ಯದಿ (!dom_ctx)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	ಪ್ರತ್ಯಾಗಮ switch_to_next_domain_context(ctx, dom_ctx);
}

ಪೂರ್ಣಾಙ್ಕ sbi_domain_context_init(ಶೂನ್ಯ)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_size = ಪರಿಮಾಣ(ಸಂರಚನಾ hart_context *) * sbi_hart_ಎಣಿಕೆ();

	ಪ್ರತ್ಯಾಗಮ sbi_domain_register_state(&dcstate);
}

ಶೂನ್ಯ sbi_domain_context_deinit(ಶೂನ್ಯ)
{
	sbi_domain_unregister_state(&dcstate);
}
