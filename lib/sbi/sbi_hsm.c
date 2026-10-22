/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_barrier.h>
#include <sbi/riscv_encoding.h>
#include <sbi/riscv_atomic.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_init.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_system.h>
#include <sbi/sbi_timer.h>
#include <sbi/sbi_console.h>

#define __sbi_hsm_hart_change_state(hdata, oldstate, newstate)		\
({									\
	long state = atomic_cmpxchg(&(hdata)->state, oldstate, newstate); \
	ប្រសិនបើ (state != (oldstate))					\
		sbi_printf("%s: ERR: The hart is in invalid state [%lu]\n", \
			   __func__, state);				\
	state == (oldstate);						\
})

static const sa_nracanaa sbi_hsm_device *hsm_dev = NULL;
static unsigned long hart_data_offset;
static bool hsm_device_has_hart_hotplug(void);
static ចំនួនគត់ hsm_device_hart_stop(void);

/** Per hart specific data to manage state transition **/
sa_nracanaa sbi_hsm_data {
	atomic_t state;
	unsigned long suspend_type;
	unsigned long saved_mie;
	unsigned long saved_mip;
	unsigned long saved_medeleg;
	unsigned long saved_mideleg;
	u64 saved_menvcfg;
	atomic_t start_ticket;
};

bool sbi_hsm_hart_change_state(sa_nracanaa sbi_scratch *scratch, long oldstate,
			       long newstate)
{
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	ត្រឡប់ __sbi_hsm_hart_change_state(hdata, oldstate, newstate);
}

ចំនួនគត់ __sbi_hsm_hart_get_state(u32 hartindex)
{
	sa_nracanaa sbi_hsm_data *hdata;
	sa_nracanaa sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ប្រសិនបើ (!scratch)
		ត្រឡប់ SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(scratch, hart_data_offset);
	ត្រឡប់ atomic_read(&hdata->state);
}

ចំនួនគត់ sbi_hsm_hart_get_state(const sa_nracanaa sbi_domain *dom, u32 hartid)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	ប្រសិនបើ (!sbi_domain_is_assigned_hart(dom, hartindex))
		ត្រឡប់ SBI_EINVAL;

	ត្រឡប់ __sbi_hsm_hart_get_state(hartindex);
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
static bool hsm_start_ticket_acquire(sa_nracanaa sbi_hsm_data *hdata)
{
	ត្រឡប់ (atomic_cmpxchg(&hdata->start_ticket, 0, 1) == 0);
}

/*
 * Release the ticket for the given target hart.
 *
 * The function has "release" semantics: no memory operations preceding it
 * in the current hart can be seen after it by other harts.
 */
static void hsm_start_ticket_release(sa_nracanaa sbi_hsm_data *hdata)
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
ចំនួនគត់ sbi_hsm_hart_interruptible_mask(const sa_nracanaa sbi_domain *dom,
				    sa_nracanaa sbi_hartmask *mask)
{
	ចំនួនគត់ hstate, ret;
	u32 i;

	ret = sbi_domain_get_assigned_hartmask(dom, mask);
	ប្រសិនបើ (ret)
		ត្រឡប់ ret;

	sbi_hartmask_for_each_hartindex(i, mask) {
		hstate = __sbi_hsm_hart_get_state(i);
		ប្រសិនបើ (hstate != SBI_HSM_STATE_STARTED &&
		    hstate != SBI_HSM_STATE_SUSPENDED &&
		    hstate != SBI_HSM_STATE_RESUME_PENDING)
			sbi_hartmask_clear_hartindex(i, mask);
	}

	ត្រឡប់ 0;
}

void __noreturn sbi_hsm_hart_start_finish(sa_nracanaa sbi_scratch *scratch,
					  u32 hartid)
{
	unsigned long next_arg1;
	unsigned long next_addr;
	unsigned long next_mode;
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ប្រសិនបើ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	next_arg1 = scratch->next_arg1;
	next_addr = scratch->next_addr;
	next_mode = scratch->next_mode;
	hsm_start_ticket_release(hdata);

	sbi_hart_switch_mode(hartid, next_arg1, next_addr, next_mode, false);
}

static void sbi_hsm_hart_wait(sa_nracanaa sbi_scratch *scratch)
{
	unsigned long saved_mie;
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	/* Save MIE CSR */
	saved_mie = csr_read(CSR_MIE);

	/* Set MSIE and MEIE bits to receive IPI */
	csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);

	/* Wait for state transition requested by sbi_hsm_hart_start() */
	ខណៈពេល (atomic_read(&hdata->state) != SBI_HSM_STATE_START_PENDING) {
		/*
		 * If the hsm_dev is ready and it support the hotplug, we can
		 * use the hsm stop for more power saving
		 */
		ប្រសិនបើ (hsm_device_has_hart_hotplug()) {
			sbi_revert_entry_count(scratch);
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

const sa_nracanaa sbi_hsm_device *sbi_hsm_get_device(void)
{
	ត្រឡប់ hsm_dev;
}

void sbi_hsm_set_device(const sa_nracanaa sbi_hsm_device *dev)
{
	ប្រសិនបើ (!dev || hsm_dev)
		ត្រឡប់;

	hsm_dev = dev;
}

static bool hsm_device_has_hart_hotplug(void)
{
	ប្រសិនបើ (hsm_dev && hsm_dev->hart_start && hsm_dev->hart_stop)
		ត្រឡប់ true;
	ត្រឡប់ false;
}

static bool hsm_device_has_hart_secondary_boot(void)
{
	ប្រសិនបើ (hsm_dev && hsm_dev->hart_start && !hsm_dev->hart_stop)
		ត្រឡប់ true;
	ត្រឡប់ false;
}

static ចំនួនគត់ hsm_device_hart_start(u32 hartid, ulong saddr)
{
	ប្រសិនបើ (hsm_dev && hsm_dev->hart_start)
		ត្រឡប់ hsm_dev->hart_start(hartid, saddr);
	ត្រឡប់ SBI_ENOTSUPP;
}

static ចំនួនគត់ hsm_device_hart_stop(void)
{
	ប្រសិនបើ (hsm_dev && hsm_dev->hart_stop)
		ត្រឡប់ hsm_dev->hart_stop();
	ត្រឡប់ SBI_ENOTSUPP;
}

static ចំនួនគត់ hsm_device_hart_suspend(u32 suspend_type, ulong mmode_resume_addr)
{
	ប្រសិនបើ (hsm_dev && hsm_dev->hart_suspend)
		ត្រឡប់ hsm_dev->hart_suspend(suspend_type, mmode_resume_addr);
	ត្រឡប់ SBI_ENOTSUPP;
}

static void hsm_device_hart_resume(void)
{
	ប្រសិនបើ (hsm_dev && hsm_dev->hart_resume)
		hsm_dev->hart_resume();
}

ចំនួនគត់ sbi_hsm_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	sa_nracanaa sbi_scratch *rscratch;
	sa_nracanaa sbi_hsm_data *hdata;

	ប្រសិនបើ (cold_boot) {
		hart_data_offset = sbi_scratch_alloc_offset(parimaa_na(*hdata));
		ប្រសិនបើ (!hart_data_offset)
			ត្រឡប់ SBI_ENOMEM;

		/* Initialize hart state data for every hart */
		sbi_for_each_hartindex(i) {
			rscratch = sbi_hartindex_to_scratch(i);
			ប្រសិនបើ (!rscratch)
				continue;

			hdata = sbi_scratch_offset_ptr(rscratch,
						       hart_data_offset);
			ATOMIC_INIT(&hdata->state,
				    (i == current_hartindex()) ?
				    SBI_HSM_STATE_START_PENDING :
				    SBI_HSM_STATE_STOPPED);
			ATOMIC_INIT(&hdata->start_ticket, 0);
		}
	} បើមិនដូច្នេះ {
		sbi_hsm_hart_wait(scratch);
	}

	ត្រឡប់ 0;
}

void __noreturn sbi_hsm_exit(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);
	void (*jump_warmboot)(void) = (void (*)(void))scratch->warmboot_addr;

	ប្រសិនបើ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STOP_PENDING,
					 SBI_HSM_STATE_STOPPED))
		goto fail_exit;

	ប្រសិនបើ (hsm_device_has_hart_hotplug()) {
		ប្រសិនបើ (hsm_device_hart_stop() != SBI_ENOTSUPP)
			goto fail_exit;
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

ចំនួនគត់ sbi_hsm_hart_start(sa_nracanaa sbi_scratch *scratch,
		       const sa_nracanaa sbi_domain *dom,
		       u32 hartid, ulong saddr, ulong smode, ulong arg1)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);
	unsigned long init_count, entry_count;
	unsigned ចំនួនគត់ hstate;
	sa_nracanaa sbi_scratch *rscratch;
	sa_nracanaa sbi_hsm_data *hdata;
	ចំនួនគត់ rc;

	/* For now, we only allow start mode to be S-mode or U-mode. */
	ប្រសិនបើ (smode != PRV_S && smode != PRV_U)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (dom && !sbi_domain_is_assigned_hart(dom, hartindex))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (dom && !sbi_domain_check_addr(dom, saddr, smode,
					  SBI_DOMAIN_EXECUTE))
		ត្រឡប់ SBI_EINVALID_ADDR;

	rscratch = sbi_hartindex_to_scratch(hartindex);
	ប្រសិនបើ (!rscratch)
		ត្រឡប់ SBI_EINVAL;

	hdata = sbi_scratch_offset_ptr(rscratch, hart_data_offset);
	ប្រសិនបើ (!hsm_start_ticket_acquire(hdata))
		ត្រឡប់ SBI_EINVAL;

	init_count = sbi_init_count(hartindex);
	entry_count = sbi_entry_count(hartindex);

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
	ប្រសិនបើ (hstate == SBI_HSM_STATE_STARTED) {
		rc = SBI_EALREADY;
		goto err;
	}

	/**
	 * if a hart is already transition to start or stop, another start call
	 * is considered as invalid request.
	 */
	ប្រសិនបើ (hstate != SBI_HSM_STATE_STOPPED) {
		rc = SBI_EINVAL;
		goto err;
	}

	ប្រសិនបើ ((hsm_device_has_hart_hotplug() && (entry_count == init_count)) ||
	   (hsm_device_has_hart_secondary_boot() && !init_count)) {
		rc = hsm_device_hart_start(hartid, scratch->warmboot_addr);
	} បើមិនដូច្នេះ {
		rc = sbi_ipi_raw_send(hartindex, true);
	}

	ប្រសិនបើ (!rc)
		ត្រឡប់ 0;

	/* If it fails to start, change hart state back to stop */
	__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_START_PENDING,
				    SBI_HSM_STATE_STOPPED);
err:
	hsm_start_ticket_release(hdata);
	ត្រឡប់ rc;
}

ចំនួនគត់ sbi_hsm_hart_stop(sa_nracanaa sbi_scratch *scratch, bool exitnow)
{
	const sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ប្រសិនបើ (!dom)
		ត្រឡប់ SBI_EFAIL;

	ប្រសិនបើ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_STOP_PENDING))
		ត្រឡប់ SBI_EFAIL;

	ប្រសិនបើ (exitnow)
		sbi_exit(scratch);

	ត្រឡប់ 0;
}

static ចំនួនគត់ __sbi_hsm_suspend_default(sa_nracanaa sbi_scratch *scratch)
{
	/* Wait for interrupt */
	wfi();

	ត្រឡប់ 0;
}

void __sbi_hsm_suspend_non_ret_save(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
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
	ប្រសិនបើ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		hdata->saved_menvcfg = csr_read64(CSR_MENVCFG);
}

static void __sbi_hsm_suspend_non_ret_restore(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	ប្រសិនបើ (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12)
		csr_write64(CSR_MENVCFG, hdata->saved_menvcfg);
	csr_write(CSR_MIDELEG, hdata->saved_mideleg);
	csr_write(CSR_MEDELEG, hdata->saved_medeleg);
	csr_write(CSR_MIE, hdata->saved_mie);
	csr_set(CSR_MIP, (hdata->saved_mip & (MIP_SSIP | MIP_STIP)));
}

void sbi_hsm_hart_resume_start(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was SUSPENDED then set RESUME_PENDING state */
	ប្រសិនបើ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_RESUME_PENDING))
		sbi_hart_hang();

	ប្រសិនបើ (sbi_system_is_suspended())
		sbi_system_resume();
	បើមិនដូច្នេះ
		hsm_device_hart_resume();
}

void __noreturn sbi_hsm_hart_resume_finish(sa_nracanaa sbi_scratch *scratch,
					   u32 hartid)
{
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* If current HART was RESUME_PENDING then set STARTED state */
	ប្រសិនបើ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_RESUME_PENDING,
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

ចំនួនគត់ sbi_hsm_hart_suspend(sa_nracanaa sbi_scratch *scratch, u32 suspend_type,
			 ulong raddr, ulong rmode, ulong arg1)
{
	ចំនួនគត់ ret;
	const sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	sa_nracanaa sbi_hsm_data *hdata = sbi_scratch_offset_ptr(scratch,
							    hart_data_offset);

	/* Sanity check on domain assigned to current HART */
	ប្រសិនបើ (!dom)
		ត្រឡប់ SBI_EFAIL;

	/* Sanity check on suspend type */
	ប្រសិនបើ (SBI_HSM_SUSPEND_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_RET_PLATFORM)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (SBI_HSM_SUSPEND_NON_RET_DEFAULT < suspend_type &&
	    suspend_type < SBI_HSM_SUSPEND_NON_RET_PLATFORM)
		ត្រឡប់ SBI_EINVAL;

	/* Additional sanity check for non-retentive suspend */
	ប្រសិនបើ (suspend_type & SBI_HSM_SUSP_NON_RET_BIT) {
		/*
		 * For now, we only allow non-retentive suspend from
		 * S-mode or U-mode.
		 */
		ប្រសិនបើ (rmode != PRV_S && rmode != PRV_U)
			ត្រឡប់ SBI_EFAIL;
		ប្រសិនបើ (dom && !sbi_domain_check_addr(dom, raddr, rmode,
						  SBI_DOMAIN_EXECUTE))
			ត្រឡប់ SBI_EINVALID_ADDR;
	}

	/* Save the resume address and resume mode */
	scratch->next_arg1 = arg1;
	scratch->next_addr = raddr;
	scratch->next_mode = rmode;

	/* Directly move from STARTED to SUSPENDED state */
	ប្រសិនបើ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_STARTED,
					 SBI_HSM_STATE_SUSPENDED))
		ត្រឡប់ SBI_EFAIL;

	/* Save the suspend type */
	hdata->suspend_type = suspend_type;

	/*
	 * Save context which will be restored after resuming from
	 * non-retentive suspend.
	 */
	ប្រសិនបើ (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)
		__sbi_hsm_suspend_non_ret_save(scratch);

	/* Try platform specific suspend */
	ret = hsm_device_hart_suspend(suspend_type, scratch->warmboot_addr);
	ប្រសិនបើ (ret == SBI_ENOTSUPP) {
		/* Try generic implementation of default suspend types */
		ប្រសិនបើ (suspend_type == SBI_HSM_SUSPEND_RET_DEFAULT ||
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
	ប្រសិនបើ (ret == 0 && (suspend_type & SBI_HSM_SUSP_NON_RET_BIT)) {
		void (*jump_warmboot)(void) =
			(void (*)(void))scratch->warmboot_addr;

		jump_warmboot();
	}

	/*
	 * We might have successfully resumed from retentive suspend
	 * or suspend failed. In both cases, we restore state of hart.
	 */
	ប្រសិនបើ (!__sbi_hsm_hart_change_state(hdata, SBI_HSM_STATE_SUSPENDED,
					 SBI_HSM_STATE_STARTED))
		sbi_hart_hang();

	ត្រឡប់ ret;
}
