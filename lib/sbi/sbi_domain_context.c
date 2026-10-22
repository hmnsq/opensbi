/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) IPADS@SJTU 2023. All rights reserved.
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hart_protection.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_domain_context.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_vector.h>
#समावेश <sbi/sbi_fp.h>

/** Context representation for a hart within a domain */
काष्ठा hart_context {
	/** Trap-related states such as GPRs, mepc, and mstatus */
	काष्ठा sbi_trap_context trap_ctx;

	/** Supervisor status register */
	अचिन्हित दीर्घ sstatus;
	/** Supervisor interrupt enable register */
	अचिन्हित दीर्घ sie;
	/** Supervisor trap vector base address register */
	अचिन्हित दीर्घ stvec;
	/** Supervisor scratch register for temporary storage */
	अचिन्हित दीर्घ sscratch;
	/** Supervisor exception program counter register */
	अचिन्हित दीर्घ sepc;
	/** Supervisor cause register */
	अचिन्हित दीर्घ scause;
	/** Supervisor trap value register */
	अचिन्हित दीर्घ stval;
	/** Supervisor interrupt pending register */
	अचिन्हित दीर्घ sip;
	/** Supervisor address translation and protection register */
	अचिन्हित दीर्घ satp;
	/** Counter-enable register */
	अचिन्हित दीर्घ scounteren;
	/** Supervisor environment configuration register */
	अचिन्हित दीर्घ senvcfg;
	/** Supervisor resource management configuration register */
	अचिन्हित दीर्घ srmcfg;

	/** Float context state */
	काष्ठा sbi_fp_context fp_ctx;
	/** Vector context state */
	काष्ठा sbi_vector_context *vec_ctx;

	/** Reference to the owning domain */
	काष्ठा sbi_domain *dom;
	/** Previous context (caller) to jump to during context exits */
	काष्ठा hart_context *पूर्व_ctx;
	/** Is context initialized and runnable */
	bool initialized;
};

अटल काष्ठा sbi_domain_state dcstate;

अटल अंतरभूत काष्ठा hart_context *hart_context_get(काष्ठा sbi_domain *dom,
						    u32 hartindex)
{
	काष्ठा hart_context **dom_hartindex_to_context_table;

	dom_hartindex_to_context_table = sbi_domain_state_ptr(dom, &dcstate);
	अगर (!dom_hartindex_to_context_table || !sbi_hartindex_valid(hartindex))
		वापस शून्य;

	वापस dom_hartindex_to_context_table[hartindex];
}

अटल व्योम hart_context_set(काष्ठा sbi_domain *dom, u32 hartindex,
			     काष्ठा hart_context *hc)
{
	काष्ठा hart_context **dom_hartindex_to_context_table;

	dom_hartindex_to_context_table = sbi_domain_state_ptr(dom, &dcstate);
	अगर (!dom_hartindex_to_context_table || !sbi_hartindex_valid(hartindex))
		वापस;

	dom_hartindex_to_context_table[hartindex] = hc;
}

/** Macro to obtain the current hart's context pointer */
#घोषणा hart_context_thishart_get()					\
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
अटल पूर्णांक switch_to_अग्र_domain_context(काष्ठा hart_context *ctx,
					  काष्ठा hart_context *dom_ctx)
	MUST_NOT_HOLD(&ctx->dom->assigned_harts_lock)
	MUST_NOT_HOLD(&dom_ctx->dom->assigned_harts_lock)
{
	u32 hartindex = current_hartindex();
	काष्ठा sbi_trap_context *trap_ctx;
	काष्ठा sbi_domain *current_dom, *target_dom;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	अगर (!ctx || !dom_ctx || ctx == dom_ctx)
		वापस SBI_EINVAL;

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
	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		ctx->scounteren = csr_swap(CSR_SCOUNTEREN, dom_ctx->scounteren);
	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		ctx->senvcfg	= csr_swap(CSR_SENVCFG, dom_ctx->senvcfg);
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSQOSID))
		ctx->srmcfg	= csr_swap(CSR_SRMCFG, dom_ctx->srmcfg);

	/* Eager context switch for float */
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_F) ||
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_D)) {
		sbi_fp_save(&ctx->fp_ctx);
		sbi_fp_restore(&dom_ctx->fp_ctx);
	}

	/* Eager context switch for vector */
	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_V)) {
		sbi_vector_save(ctx->vec_ctx);
		sbi_vector_restore(dom_ctx->vec_ctx);
	}

	/* Save current trap state and restore target domain's trap state */
	trap_ctx = sbi_trap_get_context(scratch);
	sbi_memcpy(&ctx->trap_ctx, trap_ctx, माप(*trap_ctx));
	sbi_memcpy(trap_ctx, &dom_ctx->trap_ctx, माप(*trap_ctx));

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
	अगर (!dom_ctx->initialized) {
		/* Startup boot HART of target domain */
		अगर (current_hartid() == target_dom->boot_hartid)
			sbi_hart_switch_mode(target_dom->boot_hartid,
					     target_dom->अग्र_arg1,
					     target_dom->अग्र_addr,
					     target_dom->अग्र_mode,
					     false);
		अन्यथा
			sbi_hsm_hart_stop(scratch, true);
	}
	वापस 0;
}

अटल पूर्णांक hart_context_init(u32 hartindex)
{
	माप_प्रकार vec_आकार;
	काष्ठा hart_context *ctx;
	काष्ठा sbi_domain *dom;

	sbi_domain_for_each(dom) {
		अगर (!sbi_hartmask_test_hartindex(hartindex,
						 dom->possible_harts))
			जारी;

		ctx = sbi_zalloc(माप(काष्ठा hart_context));
		अगर (!ctx)
			वापस SBI_ENOMEM;

		अगर (sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
					   SBI_HART_EXT_V)) {
			vec_आकार = sbi_vector_context_आकार();

			/* Allocate the vector context pointer */
			ctx->vec_ctx = sbi_zalloc(vec_आकार);
			अगर (!ctx->vec_ctx) {
				sbi_free(ctx);
				वापस SBI_ENOMEM;
			}
		}

		/* Bind context and domain */
		ctx->dom = dom;
		hart_context_set(dom, hartindex, ctx);
	}

	वापस 0;
}

पूर्णांक sbi_domain_context_enter(काष्ठा sbi_domain *dom)
{
	पूर्णांक rc;
	काष्ठा hart_context *dom_ctx;
	काष्ठा hart_context *ctx = hart_context_thishart_get();
	u32 hartindex = current_hartindex();

	/* Target domain must not be same as the current domain */
	अगर (!dom || dom == sbi_domain_thishart_ptr())
		वापस SBI_EINVAL;

	/* Target domain must have current hart as a possible harts */
	अगर (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
		वापस SBI_EINVAL;

	/*
	 * If it's first time to call `enter` on the current hart, no
	 * context allocated before. Allocate context for each valid
	 * domain on the current hart.
	 */
	अगर (!ctx) {
		rc = hart_context_init(hartindex);
		अगर (rc)
			वापस rc;

		ctx = hart_context_thishart_get();
		अगर (!ctx)
			वापस SBI_EINVAL;
	}

	dom_ctx = hart_context_get(dom, hartindex);
	/* Validate the domain context existence */
	अगर (!dom_ctx)
		वापस SBI_EINVAL;

	/* Update target context's previous context to indicate the caller */
	dom_ctx->पूर्व_ctx = ctx;

	वापस switch_to_अग्र_domain_context(ctx, dom_ctx);
}

पूर्णांक sbi_domain_context_exit(व्योम)
{
	पूर्णांक rc;
	u32 hartindex = current_hartindex();
	काष्ठा sbi_domain *dom;
	काष्ठा hart_context *ctx = hart_context_thishart_get();
	काष्ठा hart_context *dom_ctx, *tmp;

	/*
	 * If it's first time to call `exit` on the current hart, no
	 * context allocated before. Loop through each domain to allocate
	 * its context on the current hart if valid.
	 */
	अगर (!ctx) {
		rc = hart_context_init(hartindex);
		अगर (rc)
			वापस rc;

		ctx = hart_context_thishart_get();
		अगर (!ctx)
			वापस SBI_EINVAL;
	}

	dom_ctx = ctx->पूर्व_ctx;
	ctx->पूर्व_ctx = शून्य;

	/* If no previous caller context */
	अगर (!dom_ctx) {
		/* Try to find next uninitialized domain with least initialization order */
		dom_ctx = शून्य;
		sbi_domain_for_each(dom) {
			अगर (dom == sbi_domain_thishart_ptr())
				जारी;

			अगर (!sbi_hartmask_test_hartindex(hartindex, dom->possible_harts))
				जारी;

			tmp = hart_context_get(dom, hartindex);
			अगर (tmp && tmp->initialized)
				जारी;

			अगर (!dom_ctx || tmp->dom->init_order < dom_ctx->dom->init_order)
				dom_ctx = tmp;
		}
	}
	अगर (!dom_ctx)
		वापस SBI_ENOENT;

	वापस switch_to_अग्र_domain_context(ctx, dom_ctx);
}

पूर्णांक sbi_domain_context_init(व्योम)
{
	/**
	 * Allocate per-domain and per-hart context data.
	 * The data type is "struct hart_context **" whose memory space will be
	 * dynamically allocated by domain_setup_state_one(). Calculate needed
	 * size of memory space here.
	 */
	dcstate.state_आकार = माप(काष्ठा hart_context *) * sbi_hart_गिनती();

	वापस sbi_domain_register_state(&dcstate);
}

व्योम sbi_domain_context_deinit(व्योम)
{
	sbi_domain_unregister_state(&dcstate);
}
