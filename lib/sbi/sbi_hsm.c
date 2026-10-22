/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/riscv_atomic.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hartmask.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_init.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	यदि (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

स्थिर ध्रुव संरचना sbi_hsm_device *hsm_dev = रिक्त;
स्थिर unsigned long hart_data_offset;
स्थिर bool hsm_device_has_hart_hotplug(शून्य);
स्थिर पूर्णाङ्क hsm_device_hart_stop(शून्य);

/** Per hart specific data to manage state transition **/
संरचना sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(संरचना sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	प्रत्यागम __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

पूर्णाङ्क __sbi_hsm_hart_get_state(u32 hartindex)
{
	संरचना sbi_hsm_data *hdata;
	संरचना sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	यदि (!scratch)
		प्रत्यागम SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	प्रत्यागम atomic_read(&hdata->state);
}

पूर्णाङ्क sbi_hsm_hart_get_state(ध्रुव संरचना sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	यदि (!sbi_domain_is_assigned_hart(dom, hartindex))
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम __sbi_hsm_hart_get_state(hartindex);
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
स्थिर bool hsm_start_ticket_acquire(संरचना sbi_hsm_data *hdata)
{
	प्रत्यागम (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
स्थिर शून्य hsm_start_ticket_release(संरचना sbi_hsm_data *hdata)
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
पूर्णाङ्क sbi_hsm_hart_interruptible_mask(ध्रुव संरचना sbi_domain *dom,
				    संरचना sbi_hartmask *mask)
{
	पूर्णाङ्क hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	यदि (ret)
		प्रत्यागम ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		यदि (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	प्रत्यागम 0;
}

शून्य __noreturn sbi_hsm_hart_start_finish(संरचना sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	यदि (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

स्थिर शून्य sbi_hsm_hart_wait(संरचना sbi_scratch *scratch)
{
	unsigned long saved_mie;
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	यावत् (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		यदि (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_मोजणी(scratch);
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

ध्रुव संरचना sbi_hsm_device *sbi_hsm_get_device(शून्य)
{
	प्रत्यागम hsm_dev;
}

शून्य sbi_hsm_set_device(ध्रुव संरचना sbi_hsm_device *dev)
{
	यदि (!dev || hsm_dev)
		प्रत्यागम;

	hsm_dev = dev;
}

स्थिर bool hsm_device_has_hart_hotplug(शून्य)
{
	यदि (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		प्रत्यागम true;
	प्रत्यागम false;
}

स्थिर bool hsm_device_has_hart_secondary_boot(शून्य)
{
	यदि (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		प्रत्यागम true;
	प्रत्यागम false;
}

स्थिर पूर्णाङ्क hsm_device_hart_start(u32 hartid, ulong saddr)
{
	यदि (hsm_dev && hsm_dev->hart_start)
		प्रत्यागम hsm_dev->hart_start(hartid, saddr);
	प्रत्यागम SBI_ENOTSUPP;
}

स्थिर पूर्णाङ्क hsm_device_hart_stop(शून्य)
{
	यदि (hsm_dev && hsm_dev->hart_stop)
		प्रत्यागम hsm_dev->hart_stop();
	प्रत्यागम SBI_ENOTSUPP;
}

स्थिर पूर्णाङ्क hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	यदि (hsm_dev && hsm_dev->hart_suspend)
		प्रत्यागम hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	प्रत्यागम SBI_ENOTSUPP;
}

स्थिर शून्य hsm_device_hart_resume(शून्य)
{
	यदि (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

पूर्णाङ्क sbi_hsm_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	संरचना sbi_scratch *rscratch;
	संरचना sbi_hsm_data *hdata;

	यदि (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(परिमाण(*hdata));
		यदि (!hart_data_offset)
			प्रत्यागम SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			यदि (!rscratch)
				अनुवर्त;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} अन्यथा {
		sbi_hsm_hart_wait(scratch);
	}

	प्रत्यागम 0;
}

शून्य __noreturn sbi_hsm_exit(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	शून्य (*jump_warmboot)(शून्य) = (शून्य (*)(शून्य))scratch->warmboot_addr;

	यदि (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		गच्छ fail_exit;

	यदि (hsm_device_has_hart_hotplug()) {
		यदि (hsm_device_hart_stop() != SBI_ENOTSUPP)
			गच्छ fail_exit;
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

पूर्णाङ्क sbi_hsm_hart_start(संरचना sbi_scratch *scratch,
		       ध्रुव संरचना sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_मोजणी, entry_मोजणी;
	unsigned पूर्णाङ्क hstate;
	संरचना sbi_scratch *rscratch;
	संरचना sbi_hsm_data *hdata;
	पूर्णाङ्क rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	यदि (smode != PRV_S && smode != PRV_U)
		प्रत्यागम SBI_EINVAL;
	यदि (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		प्रत्यागम SBI_EINVAL;
	यदि (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		प्रत्यागम SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	यदि (!rscratch)
		प्रत्यागम SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	यदि (!hsm_start_ticket_acquire(hdata))
		प्रत्यागम SBI_EINVAL;

	init_मोजणी = sbi_init_मोजणी(hartindex);
	entry_मोजणी = sbi_entry_मोजणी(hartindex);

	rscratch->next_arg1 = arg1;
	rscratch->next_addr = saddr;
	rscratch->next_mode = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_START_PENDING);
	यदि (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		गच्छ err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	यदि (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		गच्छ err;
	}

	यदि ((hsm_device_has_hart_hotplug() && (entry_मोजणी == init_मोजणी)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_मोजणी)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} अन्यथा {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	यदि (!rc)
		प्रत्यागम 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	प्रत्यागम rc;
}

पूर्णाङ्क sbi_hsm_hart_stop(संरचना sbi_scratch *scratch, bool exitnow)
{
	ध्रुव संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	यदि (!dom)
		प्रत्यागम SBI_EFAIL;

	यदि (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		प्रत्यागम SBI_EFAIL;

	यदि (exitnow)
		sbi_exit(scratch);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क __sbi_hsm_suspend_default(संरचना sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	प्रत्यागम 0;
}

शून्य __sbi_hsm_suspend_non_ret_save(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

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
	यदि (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

स्थिर शून्य __sbi_hsm_suspend_non_ret_restore(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	यदि (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

शून्य sbi_hsm_hart_resume_start(संरचना sbi_scratch *scratch)
{
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	यदि (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	यदि (sbi_system_is_suspended())
		sbi_system_resume();
	अन्यथा
		hsm_device_hart_resume();
}

शून्य __noreturn sbi_hsm_hart_resume_finish(संरचना sbi_scratch *scratch,
					   u32 hartid)
{
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	यदि (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_mode(hartid, scratch->next_arg1,
			     scratch->next_addr,
			     scratch->next_mode, false);
}

पूर्णाङ्क sbi_hsm_hart_suspend(संरचना sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	पूर्णाङ्क ret;
	ध्रुव संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	संरचना sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	यदि (!dom)
		प्रत्यागम SBI_EFAIL;

	/* Sanity check on suspend type */
	यदि (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		प्रत्यागम SBI_EINVAL;
	यदि (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		प्रत्यागम SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	यदि (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		यदि (rmode != PRV_S && rmode != PRV_U)
			प्रत्यागम SBI_EFAIL;
		यदि (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			प्रत्यागम SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	यदि (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		प्रत्यागम SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	यदि (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	यदि (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		यदि (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	यदि (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		शून्य (*jump_warmboot)(शून्य) =
			(शून्य (*)(शून्य))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	यदि (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	प्रत्यागम ret;
}
