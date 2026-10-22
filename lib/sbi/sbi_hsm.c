/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/riscv_atomic.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hartmask.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_init.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi/sbi_console.h>

#घोषणा __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	दीर्घ state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	अगर (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

अटल स्थिर काष्ठा sbi_hsm_device *hsm_dev = शून्य;
अटल अचिन्हित दीर्घ hart_तथ्य_offset;
अटल bool hsm_device_has_hart_hotplug(व्योम);
अटल पूर्णांक hsm_device_hart_stop(व्योम);

/** Per hart specific data to manage state transition **/
काष्ठा sbi_hsm_तथ्य {
	atomic_t state;
	अचिन्हित दीर्घ suspend_type;
	अचिन्हित दीर्घ saved_mie;
	अचिन्हित दीर्घ saved_mip;
	अचिन्हित दीर्घ saved_medeleg;
	अचिन्हित दीर्घ saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(काष्ठा sbi_scratch *scratch, दीर्घ oldstate,
			       दीर्घ newstate)
{
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);
	वापस __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

पूर्णांक __sbi_hsm_hart_get_state(u32 hartindex)
{
	काष्ठा sbi_hsm_तथ्य *hdata;
	काष्ठा sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	अगर (!scratch)
		वापस SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_तथ्य_offset);
	वापस atomic_read(&hdata->state);
}

पूर्णांक sbi_hsm_hart_get_state(स्थिर काष्ठा sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	अगर (!sbi_domain_is_assigned_hart(dom, hartindex))
		वापस SBI_EINVAL;

	वापस __sbi_hsm_hart_get_state(hartindex);
}

/*
 * Try to acquire the ticket for the given target hart to make sure only
 * one hart prepares the start of the target hart.
 * Returns true if the ticket has been acquired, false otherwise.
 *
 * The function has "acquire" semantics: no memory operations following it
 * in the current hart can be seen before it by other harts.
 * atomic_cmpxchg() provides the memory barriers needed for that.
 */
अटल bool hsm_start_ticket_acquire(काष्ठा sbi_hsm_तथ्य *hdata)
{
	वापस (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
अटल व्योम hsm_start_ticket_release(काष्ठा sbi_hsm_तथ्य *hdata)
{
	RISCV_FENCE(rw, w);
	atomic_write(&hdata->start_ticket, 0);
}

/**
 * Get the mask of harts which are valid IPI targets
 * @param dom the domain to be used for output HART mask
 * @param mask the output hartmask to fill
 * @return 0 on success and SBI_Exxx (< 0) on failure
 */
पूर्णांक sbi_hsm_hart_interruptible_mask(स्थिर काष्ठा sbi_domain *dom,
				    काष्ठा sbi_hartmask *mask)
{
	पूर्णांक hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	अगर (ret)
		वापस ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		अगर (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	वापस 0;
}

व्योम __noreturn sbi_hsm_hart_start_finish(काष्ठा sbi_scratch *scratch,
					  u32 hartid)
{
	अचिन्हित दीर्घ अग्र_arg1;
	अचिन्हित दीर्घ अग्र_addr;
	अचिन्हित दीर्घ अग्र_mode;
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);

	अगर (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	अग्र_arg1 = scratch->अग्र_arg1;
	अग्र_addr = scratch->अग्र_addr;
	अग्र_mode = scratch->अग्र_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, अग्र_arg1, अग्र_addr, अग्र_mode, false);
}

अटल व्योम sbi_hsm_hart_wait(काष्ठा sbi_scratch *scratch)
{
	अचिन्हित दीर्घ saved_mie;
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	जबतक (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		अगर (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_गिनती(scratch);
			hsm_device_hart_stop();
		}

		wfi();
	}

	/* Restore MIE CSR */
	csr_write(CSR_MIE, saved_mie);

	/*
	 * No need to clear IPI here because the sbi_ipi_init() will
	 * clear it for current HART.
	 */
}

स्थिर काष्ठा sbi_hsm_device *sbi_hsm_get_device(व्योम)
{
	वापस hsm_dev;
}

व्योम sbi_hsm_set_device(स्थिर काष्ठा sbi_hsm_device *dev)
{
	अगर (!dev || hsm_dev)
		वापस;

	hsm_dev = dev;
}

अटल bool hsm_device_has_hart_hotplug(व्योम)
{
	अगर (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		वापस true;
	वापस false;
}

अटल bool hsm_device_has_hart_secondary_boot(व्योम)
{
	अगर (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		वापस true;
	वापस false;
}

अटल पूर्णांक hsm_device_hart_start(u32 hartid, ulong saddr)
{
	अगर (hsm_dev && hsm_dev->hart_start)
		वापस hsm_dev->hart_start(hartid, saddr);
	वापस SBI_ENOTSUPP;
}

अटल पूर्णांक hsm_device_hart_stop(व्योम)
{
	अगर (hsm_dev && hsm_dev->hart_stop)
		वापस hsm_dev->hart_stop();
	वापस SBI_ENOTSUPP;
}

अटल पूर्णांक hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	अगर (hsm_dev && hsm_dev->hart_suspend)
		वापस hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	वापस SBI_ENOTSUPP;
}

अटल व्योम hsm_device_hart_resume(व्योम)
{
	अगर (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

पूर्णांक sbi_hsm_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	काष्ठा sbi_scratch *rscratch;
	काष्ठा sbi_hsm_तथ्य *hdata;

	अगर (cold_boot) {
		hart_तथ्य_offset = sbi_scratch_alloc_offset(माप(*hdata));
		अगर (!hart_तथ्य_offset)
			वापस SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			अगर (!rscratch)
				जारी;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_तथ्य_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} अन्यथा {
		sbi_hsm_hart_wait(scratch);
	}

	वापस 0;
}

व्योम __noreturn sbi_hsm_exit(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);
	व्योम (*jump_warmboot)(व्योम) = (व्योम (*)(व्योम))scratch->warmboot_addr;

	अगर (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		जाओ fail_exit;

	अगर (hsm_device_has_hart_hotplug()) {
		अगर (hsm_device_hart_stop() != SBI_ENOTSUPP)
			जाओ fail_exit;
	}

	/**
	 * As platform is lacking support for hotplug, directly jump to warmboot
	 * and wait for interrupts in warmboot. We do it preemptively in order
	 * preserve the hart states and reuse the code path for hotplug.
	 */
	jump_warmboot();

fail_exit:
	/* It should never reach here */
	sbi_printf("ERR: Failed stop hart [%u]\n", current_hartid());
	sbi_hart_hang();
}

पूर्णांक sbi_hsm_hart_start(काष्ठा sbi_scratch *scratch,
		       स्थिर काष्ठा sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	अचिन्हित दीर्घ init_गिनती, entry_गिनती;
	अचिन्हित पूर्णांक hstate;
	काष्ठा sbi_scratch *rscratch;
	काष्ठा sbi_hsm_तथ्य *hdata;
	पूर्णांक rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	अगर (smode != PRV_S && smode != PRV_U)
		वापस SBI_EINVAL;
	अगर (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		वापस SBI_EINVAL;
	अगर (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		वापस SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	अगर (!rscratch)
		वापस SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_तथ्य_offset);
	अगर (!hsm_start_ticket_acquire(hdata))
		वापस SBI_EINVAL;

	init_गिनती = sbi_init_गिनती(hartindex);
	entry_गिनती = sbi_entry_गिनती(hartindex);

	rscratch->अग्र_arg1 = arg1;
	rscratch->अग्र_addr = saddr;
	rscratch->अग्र_mode = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_START_PENDING);
	अगर (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		जाओ err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	अगर (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		जाओ err;
	}

	अगर ((hsm_device_has_hart_hotplug() && (entry_गिनती == init_गिनती)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_गिनती)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} अन्यथा {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	अगर (!rc)
		वापस 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	वापस rc;
}

पूर्णांक sbi_hsm_hart_stop(काष्ठा sbi_scratch *scratch, bool exitnow)
{
	स्थिर काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);

	अगर (!dom)
		वापस SBI_EFAIL;

	अगर (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		वापस SBI_EFAIL;

	अगर (exitnow)
		sbi_exit(scratch);

	वापस 0;
}

अटल पूर्णांक __sbi_hsm_suspend_default(काष्ठा sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	वापस 0;
}

व्योम __sbi_hsm_suspend_non_ret_save(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);

	/*
	 * We will be resuming in warm-boot path so the MIE and MIP CSRs
	 * will be back to initial state. It is possible that HART has
	 * configured timer event before going to suspend state so we
	 * should save MIE and MIP CSRs and restore it after resuming.
	 *
	 * Further, the M-mode bits in MIP CSR are read-only and set by
	 * external devices (such as interrupt controller) whereas all
	 * VS-mode bits in MIP are read-only alias of bits in HVIP CSR.
	 *
	 * This means we should only save/restore S-mode bits of MIP CSR
	 * such as MIP.SSIP and MIP.STIP.
	 */

	hdata->saved_mie = csr_read(CSR_MIE);
	hdata->saved_mip = csr_read(CSR_MIP) & (MIP_SSIP | MIP_STIP);
	hdata->saved_medeleg = csr_read(CSR_MEDELEG);
	hdata->saved_mideleg = csr_read(CSR_MIDELEG);
	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

अटल व्योम __sbi_hsm_suspend_non_ret_restore(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);

	अगर (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

व्योम sbi_hsm_hart_resume_start(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	अगर (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	अगर (sbi_system_is_suspended())
		sbi_system_resume();
	अन्यथा
		hsm_device_hart_resume();
}

व्योम __noreturn sbi_hsm_hart_resume_finish(काष्ठा sbi_scratch *scratch,
					   u32 hartid)
{
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	अगर (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_mode(hartid, scratch->अग्र_arg1,
			     scratch->अग्र_addr,
			     scratch->अग्र_mode, false);
}

पूर्णांक sbi_hsm_hart_suspend(काष्ठा sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	पूर्णांक ret;
	स्थिर काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	काष्ठा sbi_hsm_तथ्य *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_तथ्य_offset);

	/* Sanity check on domain assigned to current HART */
	अगर (!dom)
		वापस SBI_EFAIL;

	/* Sanity check on suspend type */
	अगर (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		वापस SBI_EINVAL;
	अगर (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		वापस SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	अगर (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		अगर (rmode != PRV_S && rmode != PRV_U)
			वापस SBI_EFAIL;
		अगर (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			वापस SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->अग्र_arg1 = arg1;
	scratch->अग्र_addr = raddr;
	scratch->अग्र_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	अगर (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		वापस SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	अगर (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	अगर (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		अगर (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
		    suspend_type == SBI_HSM_SUSPEND_NON_RET_DEFAULT) {
			ret = __sbi_hsm_suspend_default(scratch);
		}
	}

	/*
	 * The platform may have coordinated a retentive suspend, or it may
	 * have exited early from a non-retentive suspend. Either way, the
	 * caller is not expecting a successful return, so jump to the warm
	 * boot entry point to simulate resume from a non-retentive suspend.
	 */
	अगर (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		व्योम (*jump_warmboot)(व्योम) =
			(व्योम (*)(व्योम))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	अगर (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	वापस ret;
}
