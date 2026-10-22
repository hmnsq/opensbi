/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hartmask.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_init.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	ಯದಿ (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_hsm_device *hsm_dev = ರಿಕ್ತ;
ಸ್ಥಿರ unsigned long hart_data_offset;
ಸ್ಥಿರ bool hsm_device_has_hart_hotplug(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hsm_device_hart_stop(ಶೂನ್ಯ);

/** Per hart specific data to manage state transition **/
ಸಂರಚನಾ sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(ಸಂರಚನಾ sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	ಪ್ರತ್ಯಾಗಮ __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

ಪೂರ್ಣಾಙ್ಕ __sbi_hsm_hart_get_state(u32 hartindex)
{
	ಸಂರಚನಾ sbi_hsm_data *hdata;
	ಸಂರಚನಾ sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ಯದಿ (!scratch)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	ಪ್ರತ್ಯಾಗಮ atomic_read(&hdata->state);
}

ಪೂರ್ಣಾಙ್ಕ sbi_hsm_hart_get_state(ಧ್ರುವ ಸಂರಚನಾ sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	ಯದಿ (!sbi_domain_is_assigned_hart(dom, hartindex))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತ್ಯಾಗಮ __sbi_hsm_hart_get_state(hartindex);
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
ಸ್ಥಿರ bool hsm_start_ticket_acquire(ಸಂರಚನಾ sbi_hsm_data *hdata)
{
	ಪ್ರತ್ಯಾಗಮ (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
ಸ್ಥಿರ ಶೂನ್ಯ hsm_start_ticket_release(ಸಂರಚನಾ sbi_hsm_data *hdata)
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
ಪೂರ್ಣಾಙ್ಕ sbi_hsm_hart_interruptible_mask(ಧ್ರುವ ಸಂರಚನಾ sbi_domain *dom,
				    ಸಂರಚನಾ sbi_hartmask *mask)
{
	ಪೂರ್ಣಾಙ್ಕ hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		ಯದಿ (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ __noreturn sbi_hsm_hart_start_finish(ಸಂರಚನಾ sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ಯದಿ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_hsm_hart_wait(ಸಂರಚನಾ sbi_scratch *scratch)
{
	unsigned long saved_mie;
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	ಯಾವತ್ (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		ಯದಿ (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_ಎಣಿಕೆ(scratch);
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

ಧ್ರುವ ಸಂರಚನಾ sbi_hsm_device *sbi_hsm_get_device(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ hsm_dev;
}

ಶೂನ್ಯ sbi_hsm_set_device(ಧ್ರುವ ಸಂರಚನಾ sbi_hsm_device *dev)
{
	ಯದಿ (!dev || hsm_dev)
		ಪ್ರತ್ಯಾಗಮ;

	hsm_dev = dev;
}

ಸ್ಥಿರ bool hsm_device_has_hart_hotplug(ಶೂನ್ಯ)
{
	ಯದಿ (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		ಪ್ರತ್ಯಾಗಮ true;
	ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ bool hsm_device_has_hart_secondary_boot(ಶೂನ್ಯ)
{
	ಯದಿ (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		ಪ್ರತ್ಯಾಗಮ true;
	ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hsm_device_hart_start(u32 hartid, ulong saddr)
{
	ಯದಿ (hsm_dev && hsm_dev->hart_start)
		ಪ್ರತ್ಯಾಗಮ hsm_dev->hart_start(hartid, saddr);
	ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hsm_device_hart_stop(ಶೂನ್ಯ)
{
	ಯದಿ (hsm_dev && hsm_dev->hart_stop)
		ಪ್ರತ್ಯಾಗಮ hsm_dev->hart_stop();
	ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	ಯದಿ (hsm_dev && hsm_dev->hart_suspend)
		ಪ್ರತ್ಯಾಗಮ hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಶೂನ್ಯ hsm_device_hart_resume(ಶೂನ್ಯ)
{
	ಯದಿ (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

ಪೂರ್ಣಾಙ್ಕ sbi_hsm_init(ಸಂರಚನಾ sbi_scratch *scratch, bool cold_boot)
{
	ಸಂರಚನಾ sbi_scratch *rscratch;
	ಸಂರಚನಾ sbi_hsm_data *hdata;

	ಯದಿ (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(ಪರಿಮಾಣ(*hdata));
		ಯದಿ (!hart_data_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			ಯದಿ (!rscratch)
				ಅನುವರ್ತ;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} ಅನ್ಯಥಾ {
		sbi_hsm_hart_wait(scratch);
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ __noreturn sbi_hsm_exit(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	ಶೂನ್ಯ (*jump_warmboot)(ಶೂನ್ಯ) = (ಶೂನ್ಯ (*)(ಶೂನ್ಯ))scratch->warmboot_addr;

	ಯದಿ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		ಗಚ್ಛ fail_exit;

	ಯದಿ (hsm_device_has_hart_hotplug()) {
		ಯದಿ (hsm_device_hart_stop() != SBI_ENOTSUPP)
			ಗಚ್ಛ fail_exit;
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

ಪೂರ್ಣಾಙ್ಕ sbi_hsm_hart_start(ಸಂರಚನಾ sbi_scratch *scratch,
		       ಧ್ರುವ ಸಂರಚನಾ sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_ಎಣಿಕೆ, entry_ಎಣಿಕೆ;
	unsigned ಪೂರ್ಣಾಙ್ಕ hstate;
	ಸಂರಚನಾ sbi_scratch *rscratch;
	ಸಂರಚನಾ sbi_hsm_data *hdata;
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	ಯದಿ (smode != PRV_S && smode != PRV_U)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	ಯದಿ (!rscratch)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	ಯದಿ (!hsm_start_ticket_acquire(hdata))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	init_ಎಣಿಕೆ = sbi_init_ಎಣಿಕೆ(hartindex);
	entry_ಎಣಿಕೆ = sbi_entry_ಎಣಿಕೆ(hartindex);

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
	ಯದಿ (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		ಗಚ್ಛ err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	ಯದಿ (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		ಗಚ್ಛ err;
	}

	ಯದಿ ((hsm_device_has_hart_hotplug() && (entry_ಎಣಿಕೆ == init_ಎಣಿಕೆ)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_ಎಣಿಕೆ)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} ಅನ್ಯಥಾ {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	ಯದಿ (!rc)
		ಪ್ರತ್ಯಾಗಮ 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಪೂರ್ಣಾಙ್ಕ sbi_hsm_hart_stop(ಸಂರಚನಾ sbi_scratch *scratch, bool exitnow)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ಯದಿ (!dom)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ಯದಿ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	ಯದಿ (exitnow)
		sbi_exit(scratch);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __sbi_hsm_suspend_default(ಸಂರಚನಾ sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ __sbi_hsm_suspend_non_ret_save(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
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
	ಯದಿ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

ಸ್ಥಿರ ಶೂನ್ಯ __sbi_hsm_suspend_non_ret_restore(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ಯದಿ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

ಶೂನ್ಯ sbi_hsm_hart_resume_start(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	ಯದಿ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	ಯದಿ (sbi_system_is_suspended())
		sbi_system_resume();
	ಅನ್ಯಥಾ
		hsm_device_hart_resume();
}

ಶೂನ್ಯ __noreturn sbi_hsm_hart_resume_finish(ಸಂರಚನಾ sbi_scratch *scratch,
					   u32 hartid)
{
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	ಯದಿ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
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

ಪೂರ್ಣಾಙ್ಕ sbi_hsm_hart_suspend(ಸಂರಚನಾ sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	ಧ್ರುವ ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಸಂರಚನಾ sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	ಯದಿ (!dom)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	/* Sanity check on suspend type */
	ಯದಿ (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	ಯದಿ (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		ಯದಿ (rmode != PRV_S && rmode != PRV_U)
			ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
		ಯದಿ (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	ಯದಿ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	ಯದಿ (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	ಯದಿ (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		ಯದಿ (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	ಯದಿ (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		ಶೂನ್ಯ (*jump_warmboot)(ಶೂನ್ಯ) =
			(ಶೂನ್ಯ (*)(ಶೂನ್ಯ))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	ಯದಿ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	ಪ್ರತ್ಯಾಗಮ ret;
}
