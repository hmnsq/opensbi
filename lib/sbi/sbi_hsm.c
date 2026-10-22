/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/riscv_atomic.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hartmask.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_init.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	எனில் (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

நிலையான மாறிலி கட்டமைப்பு sbi_hsm_device *hsm_dev = வெற்று;
நிலையான unsigned long hart_data_offset;
நிலையான bool hsm_device_has_hart_hotplug(வெறுமை);
நிலையான முழு hsm_device_hart_stop(வெறுமை);

/** Per hart specific data to manage state transition **/
கட்டமைப்பு sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(கட்டமைப்பு sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	திரும்பு __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

முழு __sbi_hsm_hart_get_state(u32 hartindex)
{
	கட்டமைப்பு sbi_hsm_data *hdata;
	கட்டமைப்பு sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	எனில் (!scratch)
		திரும்பு SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	திரும்பு atomic_read(&hdata->state);
}

முழு sbi_hsm_hart_get_state(மாறிலி கட்டமைப்பு sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	எனில் (!sbi_domain_is_assigned_hart(dom, hartindex))
		திரும்பு SBI_EINVAL;

	திரும்பு __sbi_hsm_hart_get_state(hartindex);
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
நிலையான bool hsm_start_ticket_acquire(கட்டமைப்பு sbi_hsm_data *hdata)
{
	திரும்பு (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
நிலையான வெறுமை hsm_start_ticket_release(கட்டமைப்பு sbi_hsm_data *hdata)
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
முழு sbi_hsm_hart_interruptible_mask(மாறிலி கட்டமைப்பு sbi_domain *dom,
				    கட்டமைப்பு sbi_hartmask *mask)
{
	முழு hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	எனில் (ret)
		திரும்பு ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		எனில் (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	திரும்பு 0;
}

வெறுமை __noreturn sbi_hsm_hart_start_finish(கட்டமைப்பு sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	எனில் (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

நிலையான வெறுமை sbi_hsm_hart_wait(கட்டமைப்பு sbi_scratch *scratch)
{
	unsigned long saved_mie;
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	போது (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		எனில் (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_மொத்தம்(scratch);
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

மாறிலி கட்டமைப்பு sbi_hsm_device *sbi_hsm_get_device(வெறுமை)
{
	திரும்பு hsm_dev;
}

வெறுமை sbi_hsm_set_device(மாறிலி கட்டமைப்பு sbi_hsm_device *dev)
{
	எனில் (!dev || hsm_dev)
		திரும்பு;

	hsm_dev = dev;
}

நிலையான bool hsm_device_has_hart_hotplug(வெறுமை)
{
	எனில் (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		திரும்பு true;
	திரும்பு false;
}

நிலையான bool hsm_device_has_hart_secondary_boot(வெறுமை)
{
	எனில் (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		திரும்பு true;
	திரும்பு false;
}

நிலையான முழு hsm_device_hart_start(u32 hartid, ulong saddr)
{
	எனில் (hsm_dev && hsm_dev->hart_start)
		திரும்பு hsm_dev->hart_start(hartid, saddr);
	திரும்பு SBI_ENOTSUPP;
}

நிலையான முழு hsm_device_hart_stop(வெறுமை)
{
	எனில் (hsm_dev && hsm_dev->hart_stop)
		திரும்பு hsm_dev->hart_stop();
	திரும்பு SBI_ENOTSUPP;
}

நிலையான முழு hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	எனில் (hsm_dev && hsm_dev->hart_suspend)
		திரும்பு hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	திரும்பு SBI_ENOTSUPP;
}

நிலையான வெறுமை hsm_device_hart_resume(வெறுமை)
{
	எனில் (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

முழு sbi_hsm_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	கட்டமைப்பு sbi_scratch *rscratch;
	கட்டமைப்பு sbi_hsm_data *hdata;

	எனில் (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(அளவு(*hdata));
		எனில் (!hart_data_offset)
			திரும்பு SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			எனில் (!rscratch)
				தொடர்;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} இல்லையெனில் {
		sbi_hsm_hart_wait(scratch);
	}

	திரும்பு 0;
}

வெறுமை __noreturn sbi_hsm_exit(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	வெறுமை (*jump_warmboot)(வெறுமை) = (வெறுமை (*)(வெறுமை))scratch->warmboot_addr;

	எனில் (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		செல் fail_exit;

	எனில் (hsm_device_has_hart_hotplug()) {
		எனில் (hsm_device_hart_stop() != SBI_ENOTSUPP)
			செல் fail_exit;
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

முழு sbi_hsm_hart_start(கட்டமைப்பு sbi_scratch *scratch,
		       மாறிலி கட்டமைப்பு sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_மொத்தம், entry_மொத்தம்;
	unsigned முழு hstate;
	கட்டமைப்பு sbi_scratch *rscratch;
	கட்டமைப்பு sbi_hsm_data *hdata;
	முழு rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	எனில் (smode != PRV_S && smode != PRV_U)
		திரும்பு SBI_EINVAL;
	எனில் (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		திரும்பு SBI_EINVAL;
	எனில் (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		திரும்பு SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	எனில் (!rscratch)
		திரும்பு SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	எனில் (!hsm_start_ticket_acquire(hdata))
		திரும்பு SBI_EINVAL;

	init_மொத்தம் = sbi_init_மொத்தம்(hartindex);
	entry_மொத்தம் = sbi_entry_மொத்தம்(hartindex);

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
	எனில் (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		செல் err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	எனில் (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		செல் err;
	}

	எனில் ((hsm_device_has_hart_hotplug() && (entry_மொத்தம் == init_மொத்தம்)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_மொத்தம்)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} இல்லையெனில் {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	எனில் (!rc)
		திரும்பு 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	திரும்பு rc;
}

முழு sbi_hsm_hart_stop(கட்டமைப்பு sbi_scratch *scratch, bool exitnow)
{
	மாறிலி கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	எனில் (!dom)
		திரும்பு SBI_EFAIL;

	எனில் (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		திரும்பு SBI_EFAIL;

	எனில் (exitnow)
		sbi_exit(scratch);

	திரும்பு 0;
}

நிலையான முழு __sbi_hsm_suspend_default(கட்டமைப்பு sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	திரும்பு 0;
}

வெறுமை __sbi_hsm_suspend_non_ret_save(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
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
	எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

நிலையான வெறுமை __sbi_hsm_suspend_non_ret_restore(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	எனில் (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

வெறுமை sbi_hsm_hart_resume_start(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	எனில் (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	எனில் (sbi_system_is_suspended())
		sbi_system_resume();
	இல்லையெனில்
		hsm_device_hart_resume();
}

வெறுமை __noreturn sbi_hsm_hart_resume_finish(கட்டமைப்பு sbi_scratch *scratch,
					   u32 hartid)
{
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	எனில் (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
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

முழு sbi_hsm_hart_suspend(கட்டமைப்பு sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	முழு ret;
	மாறிலி கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	கட்டமைப்பு sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	எனில் (!dom)
		திரும்பு SBI_EFAIL;

	/* Sanity check on suspend type */
	எனில் (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		திரும்பு SBI_EINVAL;
	எனில் (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		திரும்பு SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	எனில் (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		எனில் (rmode != PRV_S && rmode != PRV_U)
			திரும்பு SBI_EFAIL;
		எனில் (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			திரும்பு SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	எனில் (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		திரும்பு SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	எனில் (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	எனில் (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		எனில் (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	எனில் (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		வெறுமை (*jump_warmboot)(வெறுமை) =
			(வெறுமை (*)(வெறுமை))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	எனில் (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	திரும்பு ret;
}
