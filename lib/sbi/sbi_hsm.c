/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/riscv_atomic.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hartmask.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_init.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi/sbi_console.h>

#నిర్వచించు __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	దీర్ఘ state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	ఒకవేళ (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

స్థిర స్థిరరాశి నిర్మాణం sbi_hsm_device *hsm_dev = ఖాళీ;
స్థిర గుర్తులేని దీర్ఘ hart_దత్తాంశం_స్థానభ్రంశం;
స్థిర bool hsm_device_has_hart_hotplug(శూన్య);
స్థిర పూర్ణాంకం hsm_device_hart_stop(శూన్య);

/** Per hart specific data to manage state transition **/
నిర్మాణం sbi_hsm_దత్తాంశం {
	atomic_t state;
	గుర్తులేని దీర్ఘ suspend_type;
	గుర్తులేని దీర్ఘ saved_mie;
	గుర్తులేని దీర్ఘ saved_mip;
	గుర్తులేని దీర్ఘ saved_medeleg;
	గుర్తులేని దీర్ఘ saved_mideleg;
	u64 saved_menvcfg;
	atomic_t ప్రారంభం_ticket;
};

bool sbi_hsm_hart_change_state(నిర్మాణం sbi_scratch *scratch, దీర్ఘ oldstate,
			       దీర్ఘ newstate)
{
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);
	తిరిగి __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

పూర్ణాంకం __sbi_hsm_hart_get_state(u32 hartindex)
{
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata;
	నిర్మాణం sbi_scratch *scratch;

	scratch = sbi_hartindex_గమ్యం_scratch(hartindex);
	ఒకవేళ (!scratch)
		తిరిగి SBI_EINVAL;

	hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch, hart_దత్తాంశం_స్థానభ్రంశం);
	తిరిగి atomic_read(&hdata->state);
}

పూర్ణాంకం sbi_hsm_hart_get_state(స్థిరరాశి నిర్మాణం sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_గమ్యం_hartindex(hartid);

	ఒకవేళ (!sbi_domain_is_assigned_hart(dom, hartindex))
		తిరిగి SBI_EINVAL;

	తిరిగి __sbi_hsm_hart_get_state(hartindex);
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
స్థిర bool hsm_ప్రారంభం_ticket_acquire(నిర్మాణం sbi_hsm_దత్తాంశం *hdata)
{
	తిరిగి (atomic_cmpxchg(&hdata->ప్రారంభం_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
స్థిర శూన్య hsm_ప్రారంభం_ticket_release(నిర్మాణం sbi_hsm_దత్తాంశం *hdata)
{
	RISCV_FENCE(rw, w);
	atomic_write(&hdata->ప్రారంభం_ticket, 0);
}

/**
 * Get the mask of harts which are valid IPI targets
 * @param dom the domain to be used for output HART mask
 * @param mask the output hartmask to fill
 * @return 0 on success and SBI_Exxx (< 0) on failure
 */
పూర్ణాంకం sbi_hsm_hart_interruptible_ముసుగు(స్థిరరాశి నిర్మాణం sbi_domain *dom,
				    నిర్మాణం sbi_hartmask *ముసుగు)
{
	పూర్ణాంకం hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, ముసుగు);
	ఒకవేళ (ret)
		తిరిగి ret;

	sbi_hartmask_for_each_hartindex(i, ముసుగు) {
		hstate = __sbi_hsm_hart_get_state(i);
		ఒకవేళ (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, ముసుగు);
	}

	తిరిగి 0;
}

శూన్య __noreturn sbi_hsm_hart_ప్రారంభం_ముగింపు(నిర్మాణం sbi_scratch *scratch,
					  u32 hartid)
{
	గుర్తులేని దీర్ఘ తదుపరి_arg1;
	గుర్తులేని దీర్ఘ తదుపరి_addr;
	గుర్తులేని దీర్ఘ తదుపరి_బహుళకం;
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);

	ఒకవేళ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_ప్రారంభం_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	తదుపరి_arg1 = scratch->తదుపరి_arg1;
	తదుపరి_addr = scratch->తదుపరి_addr;
	తదుపరి_బహుళకం = scratch->తదుపరి_బహుళకం;
	hsm_ప్రారంభం_ticket_release(hdata);

	sbi_hart_switch_బహుళకం(hartid, తదుపరి_arg1, తదుపరి_addr, తదుపరి_బహుళకం, false);
}

స్థిర శూన్య sbi_hsm_hart_wait(నిర్మాణం sbi_scratch *scratch)
{
	గుర్తులేని దీర్ఘ saved_mie;
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_సమితి(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	అయితే (atomic_read(&hdata->state) != SBI_HSM_STATE_ప్రారంభం_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		ఒకవేళ (hsm_device_has_hart_hotplug()) {
			sbi_revert_నమోదు_లెక్క(scratch);
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

స్థిరరాశి నిర్మాణం sbi_hsm_device *sbi_hsm_get_device(శూన్య)
{
	తిరిగి hsm_dev;
}

శూన్య sbi_hsm_సమితి_device(స్థిరరాశి నిర్మాణం sbi_hsm_device *dev)
{
	ఒకవేళ (!dev || hsm_dev)
		తిరిగి;

	hsm_dev = dev;
}

స్థిర bool hsm_device_has_hart_hotplug(శూన్య)
{
	ఒకవేళ (hsm_dev && hsm_dev->hart_ప్రారంభం && hsm_dev->hart_stop)
		తిరిగి true;
	తిరిగి false;
}

స్థిర bool hsm_device_has_hart_secondary_boot(శూన్య)
{
	ఒకవేళ (hsm_dev && hsm_dev->hart_ప్రారంభం && !hsm_dev->hart_stop)
		తిరిగి true;
	తిరిగి false;
}

స్థిర పూర్ణాంకం hsm_device_hart_ప్రారంభం(u32 hartid, ulong saddr)
{
	ఒకవేళ (hsm_dev && hsm_dev->hart_ప్రారంభం)
		తిరిగి hsm_dev->hart_ప్రారంభం(hartid, saddr);
	తిరిగి SBI_ENOTSUPP;
}

స్థిర పూర్ణాంకం hsm_device_hart_stop(శూన్య)
{
	ఒకవేళ (hsm_dev && hsm_dev->hart_stop)
		తిరిగి hsm_dev->hart_stop();
	తిరిగి SBI_ENOTSUPP;
}

స్థిర పూర్ణాంకం hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	ఒకవేళ (hsm_dev && hsm_dev->hart_suspend)
		తిరిగి hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	తిరిగి SBI_ENOTSUPP;
}

స్థిర శూన్య hsm_device_hart_resume(శూన్య)
{
	ఒకవేళ (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

పూర్ణాంకం sbi_hsm_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	నిర్మాణం sbi_scratch *rscratch;
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata;

	ఒకవేళ (cold_boot) {
		hart_దత్తాంశం_స్థానభ్రంశం = sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(*hdata));
		ఒకవేళ (!hart_దత్తాంశం_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_గమ్యం_scratch(i);
			ఒకవేళ (!rscratch)
				కొనసాగు;

			hdata = sbi_scratch_స్థానభ్రంశం_ptr(rscratch,
						       hart_దత్తాంశం_స్థానభ్రంశం);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_ప్రారంభం_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->ప్రారంభం_ticket, 0);
		}
	} లేకపోతే {
		sbi_hsm_hart_wait(scratch);
	}

	తిరిగి 0;
}

శూన్య __noreturn sbi_hsm_exit(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);
	శూన్య (*jump_warmboot)(శూన్య) = (శూన్య (*)(శూన్య))scratch->warmboot_addr;

	ఒకవేళ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		వెళ్ళు fail_exit;

	ఒకవేళ (hsm_device_has_hart_hotplug()) {
		ఒకవేళ (hsm_device_hart_stop() != SBI_ENOTSUPP)
			వెళ్ళు fail_exit;
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

పూర్ణాంకం sbi_hsm_hart_ప్రారంభం(నిర్మాణం sbi_scratch *scratch,
		       స్థిరరాశి నిర్మాణం sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_గమ్యం_hartindex(hartid);
	గుర్తులేని దీర్ఘ init_లెక్క, నమోదు_లెక్క;
	గుర్తులేని పూర్ణాంకం hstate;
	నిర్మాణం sbi_scratch *rscratch;
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata;
	పూర్ణాంకం rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	ఒకవేళ (smode != PRV_S && smode != PRV_U)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		తిరిగి SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_గమ్యం_scratch(hartindex);
	ఒకవేళ (!rscratch)
		తిరిగి SBI_EINVAL;

	hdata = sbi_scratch_స్థానభ్రంశం_ptr(rscratch, hart_దత్తాంశం_స్థానభ్రంశం);
	ఒకవేళ (!hsm_ప్రారంభం_ticket_acquire(hdata))
		తిరిగి SBI_EINVAL;

	init_లెక్క = sbi_init_లెక్క(hartindex);
	నమోదు_లెక్క = sbi_నమోదు_లెక్క(hartindex);

	rscratch->తదుపరి_arg1 = arg1;
	rscratch->తదుపరి_addr = saddr;
	rscratch->తదుపరి_బహుళకం = smode;

	/*
	 * atomic_cmpxchg() is an implicit barrier. It makes sure that
	 * other harts see reading of init_count and writing to *rscratch
	 * before hdata->state is set to SBI_HSM_STATE_START_PENDING.
	 */
	hstate = atomic_cmpxchg(&hdata->state, SBI_HSM_STATE_STOPPED,
				SBI_HSM_STATE_ప్రారంభం_PENDING);
	ఒకవేళ (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		వెళ్ళు err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	ఒకవేళ (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		వెళ్ళు err;
	}

	ఒకవేళ ((hsm_device_has_hart_hotplug() && (నమోదు_లెక్క == init_లెక్క)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_లెక్క)) {
		rc = hsm_device_hart_ప్రారంభం(hartid, scratch->warmboot_addr);
	} లేకపోతే {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	ఒకవేళ (!rc)
		తిరిగి 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_ప్రారంభం_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_ప్రారంభం_ticket_release(hdata);
	తిరిగి rc;
}

పూర్ణాంకం sbi_hsm_hart_stop(నిర్మాణం sbi_scratch *scratch, bool exitnow)
{
	స్థిరరాశి నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);

	ఒకవేళ (!dom)
		తిరిగి SBI_EFAIL;

	ఒకవేళ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		తిరిగి SBI_EFAIL;

	ఒకవేళ (exitnow)
		sbi_exit(scratch);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం __sbi_hsm_suspend_default(నిర్మాణం sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	తిరిగి 0;
}

శూన్య __sbi_hsm_suspend_non_ret_save(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);

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
	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

స్థిర శూన్య __sbi_hsm_suspend_non_ret_restore(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);

	ఒకవేళ (sbi_hart_priv_సంస్కరణ(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_సమితి(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

శూన్య sbi_hsm_hart_resume_ప్రారంభం(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	ఒకవేళ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	ఒకవేళ (sbi_system_is_suspended())
		sbi_system_resume();
	లేకపోతే
		hsm_device_hart_resume();
}

శూన్య __noreturn sbi_hsm_hart_resume_ముగింపు(నిర్మాణం sbi_scratch *scratch,
					   u32 hartid)
{
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);

	/* If current HART was RESUME_PENDING then set STARTED state */
	ఒకవేళ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	/*
	 * Restore some of the M-mode CSRs which we are re-configured by
	 * the warm-boot sequence.
	 */
	__sbi_hsm_suspend_non_ret_restore(scratch);

	sbi_hart_switch_బహుళకం(hartid, scratch->తదుపరి_arg1,
			     scratch->తదుపరి_addr,
			     scratch->తదుపరి_బహుళకం, false);
}

పూర్ణాంకం sbi_hsm_hart_suspend(నిర్మాణం sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	పూర్ణాంకం ret;
	స్థిరరాశి నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	నిర్మాణం sbi_hsm_దత్తాంశం *hdata = sbi_scratch_స్థానభ్రంశం_ptr(scratch,
							    hart_దత్తాంశం_స్థానభ్రంశం);

	/* Sanity check on domain assigned to current HART */
	ఒకవేళ (!dom)
		తిరిగి SBI_EFAIL;

	/* Sanity check on suspend type */
	ఒకవేళ (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		తిరిగి SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	ఒకవేళ (suspend_type & SBI_HSM_SUSP_NON_RET_బిట్టు) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		ఒకవేళ (rmode != PRV_S && rmode != PRV_U)
			తిరిగి SBI_EFAIL;
		ఒకవేళ (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			తిరిగి SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->తదుపరి_arg1 = arg1;
	scratch->తదుపరి_addr = raddr;
	scratch->తదుపరి_బహుళకం = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	ఒకవేళ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		తిరిగి SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	ఒకవేళ (suspend_type & SBI_HSM_SUSP_NON_RET_బిట్టు)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	ఒకవేళ (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		ఒకవేళ (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	ఒకవేళ (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_బిట్టు)) {
		శూన్య (*jump_warmboot)(శూన్య) =
			(శూన్య (*)(శూన్య))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	ఒకవేళ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	తిరిగి ret;
}
