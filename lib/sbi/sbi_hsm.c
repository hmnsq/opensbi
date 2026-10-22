/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_init.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	اگر (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

ایستا ثابت ساختار sbi_hsm_device *hsm_dev = پوچ;
ایستا unsigned long hart_data_offset;
ایستا bool hsm_device_has_hart_hotplug(تهی);
ایستا صحیح hsm_device_hart_stop(تهی);

/** Per hart specific data to manage state transition **/
ساختار sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(ساختار sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	بازگشت __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

صحیح __sbi_hsm_hart_get_state(u32 hartindex)
{
	ساختار sbi_hsm_data *hdata;
	ساختار sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	اگر (!scratch)
		بازگشت SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	بازگشت atomic_read(&hdata->state);
}

صحیح sbi_hsm_hart_get_state(ثابت ساختار sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	اگر (!sbi_domain_is_assigned_hart(dom, hartindex))
		بازگشت SBI_EINVAL;

	بازگشت __sbi_hsm_hart_get_state(hartindex);
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
ایستا bool hsm_start_ticket_acquire(ساختار sbi_hsm_data *hdata)
{
	بازگشت (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
ایستا تهی hsm_start_ticket_release(ساختار sbi_hsm_data *hdata)
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
صحیح sbi_hsm_hart_interruptible_mask(ثابت ساختار sbi_domain *dom,
				    ساختار sbi_hartmask *mask)
{
	صحیح hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	اگر (ret)
		بازگشت ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		اگر (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	بازگشت 0;
}

تهی __noreturn sbi_hsm_hart_start_finish(ساختار sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

ایستا تهی sbi_hsm_hart_wait(ساختار sbi_scratch *scratch)
{
	unsigned long saved_mie;
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	وقتی (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		اگر (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_شمار(scratch);
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

ثابت ساختار sbi_hsm_device *sbi_hsm_get_device(تهی)
{
	بازگشت hsm_dev;
}

تهی sbi_hsm_set_device(ثابت ساختار sbi_hsm_device *dev)
{
	اگر (!dev || hsm_dev)
		بازگشت;

	hsm_dev = dev;
}

ایستا bool hsm_device_has_hart_hotplug(تهی)
{
	اگر (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		بازگشت true;
	بازگشت false;
}

ایستا bool hsm_device_has_hart_secondary_boot(تهی)
{
	اگر (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		بازگشت true;
	بازگشت false;
}

ایستا صحیح hsm_device_hart_start(u32 hartid, ulong saddr)
{
	اگر (hsm_dev && hsm_dev->hart_start)
		بازگشت hsm_dev->hart_start(hartid, saddr);
	بازگشت SBI_ENOTSUPP;
}

ایستا صحیح hsm_device_hart_stop(تهی)
{
	اگر (hsm_dev && hsm_dev->hart_stop)
		بازگشت hsm_dev->hart_stop();
	بازگشت SBI_ENOTSUPP;
}

ایستا صحیح hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	اگر (hsm_dev && hsm_dev->hart_suspend)
		بازگشت hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	بازگشت SBI_ENOTSUPP;
}

ایستا تهی hsm_device_hart_resume(تهی)
{
	اگر (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

صحیح sbi_hsm_init(ساختار sbi_scratch *scratch, bool cold_boot)
{
	ساختار sbi_scratch *rscratch;
	ساختار sbi_hsm_data *hdata;

	اگر (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(اندازه(*hdata));
		اگر (!hart_data_offset)
			بازگشت SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			اگر (!rscratch)
				ادامه;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} وگرنه {
		sbi_hsm_hart_wait(scratch);
	}

	بازگشت 0;
}

تهی __noreturn sbi_hsm_exit(ساختار sbi_scratch *scratch)
{
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	تهی (*jump_warmboot)(تهی) = (تهی (*)(تهی))scratch->warmboot_addr;

	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		برو fail_exit;

	اگر (hsm_device_has_hart_hotplug()) {
		اگر (hsm_device_hart_stop() != SBI_ENOTSUPP)
			برو fail_exit;
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

صحیح sbi_hsm_hart_start(ساختار sbi_scratch *scratch,
		       ثابت ساختار sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_شمار, entry_شمار;
	unsigned صحیح hstate;
	ساختار sbi_scratch *rscratch;
	ساختار sbi_hsm_data *hdata;
	صحیح rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	اگر (smode != PRV_S && smode != PRV_U)
		بازگشت SBI_EINVAL;
	اگر (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		بازگشت SBI_EINVAL;
	اگر (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		بازگشت SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	اگر (!rscratch)
		بازگشت SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	اگر (!hsm_start_ticket_acquire(hdata))
		بازگشت SBI_EINVAL;

	init_شمار = sbi_init_شمار(hartindex);
	entry_شمار = sbi_entry_شمار(hartindex);

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
	اگر (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		برو err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	اگر (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		برو err;
	}

	اگر ((hsm_device_has_hart_hotplug() && (entry_شمار == init_شمار)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_شمار)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} وگرنه {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	اگر (!rc)
		بازگشت 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	بازگشت rc;
}

صحیح sbi_hsm_hart_stop(ساختار sbi_scratch *scratch, bool exitnow)
{
	ثابت ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	اگر (!dom)
		بازگشت SBI_EFAIL;

	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		بازگشت SBI_EFAIL;

	اگر (exitnow)
		sbi_exit(scratch);

	بازگشت 0;
}

ایستا صحیح __sbi_hsm_suspend_default(ساختار sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	بازگشت 0;
}

تهی __sbi_hsm_suspend_non_ret_save(ساختار sbi_scratch *scratch)
{
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
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
	اگر (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

ایستا تهی __sbi_hsm_suspend_non_ret_restore(ساختار sbi_scratch *scratch)
{
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	اگر (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

تهی sbi_hsm_hart_resume_start(ساختار sbi_scratch *scratch)
{
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	اگر (sbi_system_is_suspended())
		sbi_system_resume();
	وگرنه
		hsm_device_hart_resume();
}

تهی __noreturn sbi_hsm_hart_resume_finish(ساختار sbi_scratch *scratch,
					   u32 hartid)
{
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
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

صحیح sbi_hsm_hart_suspend(ساختار sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	صحیح ret;
	ثابت ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	ساختار sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	اگر (!dom)
		بازگشت SBI_EFAIL;

	/* Sanity check on suspend type */
	اگر (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		بازگشت SBI_EINVAL;
	اگر (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		بازگشت SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	اگر (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		اگر (rmode != PRV_S && rmode != PRV_U)
			بازگشت SBI_EFAIL;
		اگر (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			بازگشت SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		بازگشت SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	اگر (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	اگر (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		اگر (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	اگر (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		تهی (*jump_warmboot)(تهی) =
			(تهی (*)(تهی))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	اگر (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	بازگشت ret;
}
