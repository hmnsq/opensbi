/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_atomic.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hartmask.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_init.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	ଯଦି (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_hsm_device *hsm_dev = ଖାଲି;
ସ୍ଥିର unsigned long hart_data_offset;
ସ୍ଥିର bool hsm_device_has_hart_hotplug(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hsm_device_hart_stop(ଶୂନ୍ୟ);

/** Per hart specific data to manage state transition **/
ସଂରଚନା sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(ସଂରଚନା sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	ଫେରନ୍ତୁ __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __sbi_hsm_hart_get_state(u32 hartindex)
{
	ସଂରଚନା sbi_hsm_data *hdata;
	ସଂରଚନା sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ଯଦି (!scratch)
		ଫେରନ୍ତୁ SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	ଫେରନ୍ତୁ atomic_read(&hdata->state);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hsm_hart_get_state(ଅଚଳ ସଂରଚନା sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	ଯଦି (!sbi_domain_is_assigned_hart(dom, hartindex))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ __sbi_hsm_hart_get_state(hartindex);
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
ସ୍ଥିର bool hsm_start_ticket_acquire(ସଂରଚନା sbi_hsm_data *hdata)
{
	ଫେରନ୍ତୁ (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
ସ୍ଥିର ଶୂନ୍ୟ hsm_start_ticket_release(ସଂରଚନା sbi_hsm_data *hdata)
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hsm_hart_interruptible_mask(ଅଚଳ ସଂରଚନା sbi_domain *dom,
				    ସଂରଚନା sbi_hartmask *mask)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		ଯଦି (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ __noreturn sbi_hsm_hart_start_finish(ସଂରଚନା sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ଯଦି (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_hsm_hart_wait(ସଂରଚନା sbi_scratch *scratch)
{
	unsigned long saved_mie;
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	ଯେତେବେଳେ (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		ଯଦି (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_ଗଣନା(scratch);
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

ଅଚଳ ସଂରଚନା sbi_hsm_device *sbi_hsm_get_device(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ hsm_dev;
}

ଶୂନ୍ୟ sbi_hsm_set_device(ଅଚଳ ସଂରଚନା sbi_hsm_device *dev)
{
	ଯଦି (!dev || hsm_dev)
		ଫେରନ୍ତୁ;

	hsm_dev = dev;
}

ସ୍ଥିର bool hsm_device_has_hart_hotplug(ଶୂନ୍ୟ)
{
	ଯଦି (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		ଫେରନ୍ତୁ true;
	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର bool hsm_device_has_hart_secondary_boot(ଶୂନ୍ୟ)
{
	ଯଦି (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		ଫେରନ୍ତୁ true;
	ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hsm_device_hart_start(u32 hartid, ulong saddr)
{
	ଯଦି (hsm_dev && hsm_dev->hart_start)
		ଫେରନ୍ତୁ hsm_dev->hart_start(hartid, saddr);
	ଫେରନ୍ତୁ SBI_ENOTSUPP;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hsm_device_hart_stop(ଶୂନ୍ୟ)
{
	ଯଦି (hsm_dev && hsm_dev->hart_stop)
		ଫେରନ୍ତୁ hsm_dev->hart_stop();
	ଫେରନ୍ତୁ SBI_ENOTSUPP;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	ଯଦି (hsm_dev && hsm_dev->hart_suspend)
		ଫେରନ୍ତୁ hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	ଫେରନ୍ତୁ SBI_ENOTSUPP;
}

ସ୍ଥିର ଶୂନ୍ୟ hsm_device_hart_resume(ଶୂନ୍ୟ)
{
	ଯଦି (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hsm_init(ସଂରଚନା sbi_scratch *scratch, bool cold_boot)
{
	ସଂରଚନା sbi_scratch *rscratch;
	ସଂରଚନା sbi_hsm_data *hdata;

	ଯଦି (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(ଆକାର(*hdata));
		ଯଦି (!hart_data_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			ଯଦି (!rscratch)
				ଜାରି;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} ଅନ୍ୟଥା {
		sbi_hsm_hart_wait(scratch);
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ __noreturn sbi_hsm_exit(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	ଶୂନ୍ୟ (*jump_warmboot)(ଶୂନ୍ୟ) = (ଶୂନ୍ୟ (*)(ଶୂନ୍ୟ))scratch->warmboot_addr;

	ଯଦି (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		ଯାଅ fail_exit;

	ଯଦି (hsm_device_has_hart_hotplug()) {
		ଯଦି (hsm_device_hart_stop() != SBI_ENOTSUPP)
			ଯାଅ fail_exit;
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hsm_hart_start(ସଂରଚନା sbi_scratch *scratch,
		       ଅଚଳ ସଂରଚନା sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_ଗଣନା, entry_ଗଣନା;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hstate;
	ସଂରଚନା sbi_scratch *rscratch;
	ସଂରଚନା sbi_hsm_data *hdata;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	ଯଦି (smode != PRV_S && smode != PRV_U)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	ଯଦି (!rscratch)
		ଫେରନ୍ତୁ SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	ଯଦି (!hsm_start_ticket_acquire(hdata))
		ଫେରନ୍ତୁ SBI_EINVAL;

	init_ଗଣନା = sbi_init_ଗଣନା(hartindex);
	entry_ଗଣନା = sbi_entry_ଗଣନା(hartindex);

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
	ଯଦି (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		ଯାଅ err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	ଯଦି (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		ଯାଅ err;
	}

	ଯଦି ((hsm_device_has_hart_hotplug() && (entry_ଗଣନା == init_ଗଣନା)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_ଗଣନା)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} ଅନ୍ୟଥା {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	ଯଦି (!rc)
		ଫେରନ୍ତୁ 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	ଫେରନ୍ତୁ rc;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hsm_hart_stop(ସଂରଚନା sbi_scratch *scratch, bool exitnow)
{
	ଅଚଳ ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ଯଦି (!dom)
		ଫେରନ୍ତୁ SBI_EFAIL;

	ଯଦି (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		ଫେରନ୍ତୁ SBI_EFAIL;

	ଯଦି (exitnow)
		sbi_exit(scratch);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __sbi_hsm_suspend_default(ସଂରଚନା sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ __sbi_hsm_suspend_non_ret_save(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
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
	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

ସ୍ଥିର ଶୂନ୍ୟ __sbi_hsm_suspend_non_ret_restore(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ଯଦି (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

ଶୂନ୍ୟ sbi_hsm_hart_resume_start(ସଂରଚନା sbi_scratch *scratch)
{
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	ଯଦି (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	ଯଦି (sbi_system_is_suspended())
		sbi_system_resume();
	ଅନ୍ୟଥା
		hsm_device_hart_resume();
}

ଶୂନ୍ୟ __noreturn sbi_hsm_hart_resume_finish(ସଂରଚନା sbi_scratch *scratch,
					   u32 hartid)
{
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	ଯଦି (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_hsm_hart_suspend(ସଂରଚନା sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	ଅଚଳ ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	ସଂରଚନା sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	ଯଦି (!dom)
		ଫେରନ୍ତୁ SBI_EFAIL;

	/* Sanity check on suspend type */
	ଯଦି (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	ଯଦି (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		ଯଦି (rmode != PRV_S && rmode != PRV_U)
			ଫେରନ୍ତୁ SBI_EFAIL;
		ଯଦି (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			ଫେରନ୍ତୁ SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	ଯଦି (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		ଫେରନ୍ତୁ SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	ଯଦି (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	ଯଦି (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		ଯଦି (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	ଯଦି (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		ଶୂନ୍ୟ (*jump_warmboot)(ଶୂନ୍ୟ) =
			(ଶୂନ୍ୟ (*)(ଶୂନ୍ୟ))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	ଯଦି (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	ଫେରନ୍ତୁ ret;
}
