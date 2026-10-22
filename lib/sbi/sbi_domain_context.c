/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) IPADS@SJTU 2023. All rights reserved.
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hart_protection.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_domain_context.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_vector.h>
#ضمن <sbi/sbi_fp.h>

/** Context representation for a hart within a domain */
بنية hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	بنية sbi_trap_context trap_ctx;

	/** Supervisor status register */
	بنانشانی لما sstatus;
	/** Supervisor interrupt enable register */
	بنانشانی لما sie;
	/** Supervisor trap vector base address register */
	بنانشانی لما stvec;
	/** Supervisor scratch register for temporary storage */
	بنانشانی لما sscratch;
	/** Supervisor exception program counter register */
	بنانشانی لما sepc;
	/** Supervisor cause register */
	بنانشانی لما scause;
	/** Supervisor trap value register */
	بنانشانی لما stval;
	/** Supervisor interrupt pending register */
	بنانشانی لما sip;
	/** Supervisor address translation and protection register */
	بنانشانی لما satp;
	/** Counter-enable register */
	بنانشانی لما scounteren;
	/** Supervisor environment configuration register */
	بنانشانی لما senvcfg;
	/** Supervisor resource management configuration register */
	بنانشانی لما srmcfg;

	/** Float context state */
	بنية sbi_fp_context fp_ctx;
	/** Vector context state */
	بنية sbi_سمتیہ_context *vec_ctx;

	/** Reference to the owning domain */
	بنية sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	بنية hart_context *prev_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

ساكن بنية sbi_domain_state dcstate;

ساكن مضمن بنية hart_context *hart_context_get(بنية sbi_domain *dom,
						    u32 hartindex)
{
	بنية hart_context **dom_hartindex_منزل_context_جدول;

	dom_hartindex_منزل_context_جدول = sbi_domain_state_ptr(dom, &dcstate);
	إذا (!dom_hartindex_منزل_context_جدول || !sbi_hartindex_valid(hartindex))
		أرجع عدم;

	أرجع dom_hartindex_منزل_context_جدول[hartindex];
}

ساكن فراغ hart_context_سیٹ(بنية sbi_domain *dom, u32 hartindex,
			     بنية hart_context *hc)
{
	بنية hart_context **dom_hartindex_منزل_context_جدول;

	dom_hartindex_منزل_context_جدول = sbi_domain_state_ptr(dom, &dcstate);
	إذا (!dom_hartindex_منزل_context_جدول || !sbi_hartindex_valid(hartindex))
		أرجع;

	dom_hartindex_منزل_context_جدول[hartindex] = hc;
}

/** Macro to obtain the current hart's context pointer */
#تعریف_کرو hart_context_thishart_get()					\
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
ساكن صحيح switch_منزل_اگلا_domain_context(بنية hart_context *ctx,
					  بنية hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_جندرا)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_جندرا)
{
	u32 hartindex = current_hartindex();
	بنية sbi_trap_context *trap_ctx;
	بنية sbi_domain *current_dom, *نشانہ_dom;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (!ctx || !dom_ctx || ctx == dom_ctx)
		أرجع SBI_EINVAL;

	current_dom = ctx->dom;
	نشانہ_dom = dom_ctx->dom;
	/* Assign current hart to target domain */
	spin_جندرا(&current_dom->assigned_harts_جندرا);
	sbi_hartmask_clear_hartindex(hartindex, &current_dom->assigned_harts);
	spin_unlock(&current_dom->assigned_harts_جندرا);

	sbi_update_hartindex_منزل_domain(hartindex, نشانہ_dom);

	spin_جندرا(&نشانہ_dom->assigned_harts_جندرا);
	sbi_hartmask_سیٹ_hartindex(hartindex, &نشانہ_dom->assigned_harts);
	spin_unlock(&نشانہ_dom->assigned_harts_جندرا);

	/* Save current CSR context and restore target domain's CSR context */
	ctx->sstatus	= csr_وٹاندرا(CSR_SSTATUS, dom_ctx->sstatus);
	ctx->sie	= csr_وٹاندرا(CSR_SIE, dom_ctx->sie);
	ctx->stvec	= csr_وٹاندرا(CSR_STVEC, dom_ctx->stvec);
	ctx->sscratch	= csr_وٹاندرا(CSR_SSCRATCH, dom_ctx->sscratch);
	ctx->sepc	= csr_وٹاندرا(CSR_SEPC, dom_ctx->sepc);
	ctx->scause	= csr_وٹاندرا(CSR_SCAUSE, dom_ctx->scause);
	ctx->stval	= csr_وٹاندرا(CSR_STVAL, dom_ctx->stval);
	ctx->sip	= csr_وٹاندرا(CSR_SIP, dom_ctx->sip);
	ctx->satp	= csr_وٹاندرا(CSR_SATP, dom_ctx->satp);
	إذا (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_وٹاندرا(CSR_SCOUNTEREN, dom_ctx->scounteren);
	إذا (sbi_hart_priv_نسخہ(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_وٹاندرا(CSR_SENVCFG, dom_ctx->senvcfg);
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_وٹاندرا(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_سمتیہ_save(ctx->vec_ctx);
		sbi_سمتیہ_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, حجم(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, حجم(*trap_ctx));

	/*
	 * Re-configure PMP settings for the new domain
	 *
	 * This will internally perform full SFENCE / HFENCE which
	 * is also required for some of the above CSR updates (such
	 * as satp CSR).
	 */
	sbi_hart_protection_reconfigure(scratch, current_dom, نشانہ_dom);

	/* Mark current context structure initialized because context saved */
	ctx->initialized = true;

	/* If target domain context is not initialized or runnable */
	إذا (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		إذا (current_hartid() == نشانہ_dom->boot_hartid)
			sbi_hart_switch_منوال(نشانہ_dom->boot_hartid,
					     نشانہ_dom->اگلا_arg1,
					     نشانہ_dom->اگلا_addr,
					     نشانہ_dom->اگلا_منوال,
					     false);
		وإلا
			sbi_hsm_hart_stop(scratch, true);
	}
	أرجع 0;
}

ساكن صحيح hart_context_init(u32 hartindex)
{
	size_t vec_ناپ;
	بنية hart_context *ctx;
	بنية sbi_domain *dom;

	sbi_domain_for_each(dom) {
		إذا (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			استمر;

		ctx = sbi_zalloc(حجم(بنية hart_context));
		إذا (!ctx)
			أرجع SBI_ENOMEM;

		إذا (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_ناپ = sbi_سمتیہ_context_ناپ();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_ناپ);
			إذا (!ctx->vec_ctx) {
				sbi_free(ctx);
				أرجع SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_سیٹ(dom, hartindex, ctx);
	}

	أرجع 0;
}

صحيح sbi_domain_context_enter(بنية sbi_domain *dom)
{
	صحيح rc;
	بنية hart_context *dom_ctx;
	بنية hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	إذا (!dom || dom == sbi_domain_thishart_ptr())
		أرجع SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	إذا (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		أرجع SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	إذا (!ctx) {
		rc = hart_context_init(hartindex);
		إذا (rc)
			أرجع rc;

		ctx = hart_context_thishart_get();
		إذا (!ctx)
			أرجع SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	إذا (!dom_ctx)
		أرجع SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->prev_ctx = ctx;

	أرجع switch_منزل_اگلا_domain_context(ctx, dom_ctx);
}

صحيح sbi_domain_context_exit(فراغ)
{
	صحيح rc;
	u32 hartindex = current_hartindex();
	بنية sbi_domain *dom;
	بنية hart_context *ctx = hart_context_thishart_get();
	بنية hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	إذا (!ctx) {
		rc = hart_context_init(hartindex);
		إذا (rc)
			أرجع rc;

		ctx = hart_context_thishart_get();
		إذا (!ctx)
			أرجع SBI_EINVAL;
	}

	dom_ctx = ctx->prev_ctx;
	ctx->prev_ctx = عدم;

	/* If no previous caller context */
	إذا (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = عدم;
		sbi_domain_for_each(dom) {
			إذا (dom == sbi_domain_thishart_ptr())
				استمر;

			إذا (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				استمر;

			tmp = hart_context_get(dom, hartindex);
			إذا (tmp && tmp->initialized)
				استمر;

			إذا (!dom_ctx || tmp->dom->init_ترتیب < dom_ctx->dom->init_ترتیب)
				dom_ctx = tmp;
		}
	}
	إذا (!dom_ctx)
		أرجع SBI_ENOENT;

	أرجع switch_منزل_اگلا_domain_context(ctx, dom_ctx);
}

صحيح sbi_domain_context_init(فراغ)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_ناپ = حجم(بنية hart_context *) * sbi_hart_گنتی();

	أرجع sbi_domain_رجسٹر_state(&dcstate);
}

فراغ sbi_domain_context_deinit(فراغ)
{
	sbi_domain_unregister_state(&dcstate);
}
