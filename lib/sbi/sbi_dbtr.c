/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_csr_detect.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_byteorder.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_dbtr.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hart_state_ptr_ಸ್ಥಾನಾಂತರ;

/** Device specific debug trigger operations */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_dbtr_device *dbtr_dev = ಖಾಲಿ;

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_dbtr_device *sbi_dbtr_get_device(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ dbtr_dev;
}

ಶೂನ್ಯ sbi_dbtr_ಗಣ_device(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_dbtr_device *dev)
{
	ಆದರೆ (!dev || dbtr_dev)
		ಹಿಂತಿರುಗಿ;

	dbtr_dev = dev;
}

#ವ್ಯಾಖ್ಯಾನಿಸು dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, hart_state_ptr_ಸ್ಥಾನಾಂತರ)

#ವ್ಯಾಖ್ಯಾನಿಸು dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#ವ್ಯಾಖ್ಯಾನಿಸು dbtr_ಗಣ_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, hart_state_ptr_ಸ್ಥಾನಾಂತರ, \
			       (__hart_state))

#ವ್ಯಾಖ್ಯಾನಿಸು tdata_read_ಸುರಕ್ಷಿತ(__csr)						\
	({								\
		ರಚನೆ sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#ವ್ಯಾಖ್ಯಾನಿಸು tdata_write_ಸುರಕ್ಷಿತ(__csr, __value)				\
	({								\
		ರಚನೆ sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#ವ್ಯಾಖ್ಯಾನಿಸು tdata_implemented(__csr)					\
	({								\
		ರಚನೆ sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(ಸೂಚ್ಯಂಕ)					\
	({								\
		ರಚನೆ sbi_dbtr_trigger *__trg = ಖಾಲಿ;			\
		ರಚನೆ sbi_dbtr_hart_triggers_state *__hs = ಖಾಲಿ;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[ಸೂಚ್ಯಂಕ];			\
		(__trg);						\
	})

#ವ್ಯಾಖ್ಯಾನಿಸು for_each_trig_ನಮೂದು(ತಳ, _max, _etype, ನಮೂದು)		\
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ _idx = 0; ನಮೂದು = ((_etype *)ತಳ + _idx),		\
	     _idx < _max;						\
	     _idx++, ನಮೂದು = ((_etype *)ತಳ + _idx))

#ವ್ಯಾಖ್ಯಾನಿಸು DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool sbi_dbtr_shmem_disabled(
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs)
{
	ಹಿಂತಿರುಗಿ (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ sbi_dbtr_disable_shmem(
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *hart_shmem_ತಳ(
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs)
{
	ಹಿಂತಿರುಗಿ ((ಶೂನ್ಯ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_trigger_init(ರಚನೆ sbi_dbtr_trigger *trig,
			     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ type_ಮುಖವಾಡ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ idx)
{
	trig->type_ಮುಖವಾಡ = type_ಮುಖವಾಡ;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->ಸೂಚ್ಯಂಕ_2 = idx;
}

ಸ್ಥಿರ bool dbtr_trigger_hw_supported(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ idx, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata1,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata2,
				      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata3)
{
	ಆದರೆ (dbtr_dev && dbtr_dev->trigger_supported)
		ಹಿಂತಿರುಗಿ dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool dbtr_trigger_any_hw_supported(
			ರಚನೆ sbi_dbtr_hart_triggers_state *hs,
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata1, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata2,
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata3)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ type = TDATA1_GET_TYPE(tdata1);
	ರಚನೆ sbi_dbtr_trigger *trig;
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ (i = 0; i < hs->ಒಟ್ಟು_trigs; i++) {
		trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(i);
		ಆದರೆ (__test_bit(type, &trig->type_ಮುಖವಾಡ) &&
		    dbtr_trigger_hw_supported(trig->ಸೂಚ್ಯಂಕ_2, tdata1, tdata2,
					      tdata3))
			ಹಿಂತಿರುಗಿ true;
	}

	ಹಿಂತಿರುಗಿ false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dbtr_ಕಂಡುಹಿಡಿ_free_ಸ್ಥಾನ(ರಚನೆ sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_ಮುಖವಾಡ,
			       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata1, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata2,
			       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata3)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ type = TDATA1_GET_TYPE(tdata1);
	ರಚನೆ sbi_dbtr_trigger *trig;
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ (i = 0; i < hs->ಒಟ್ಟು_trigs; i++) {
		trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(i);
		ಆದರೆ (trig->state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, MAPPED))
			ಮುಂದುವರಿ;
		ಆದರೆ (claimed_ಮುಖವಾಡ & ಬಿಟ್(i))
			ಮುಂದುವರಿ;
		ಆದರೆ (!__test_bit(type, &trig->type_ಮುಖವಾಡ))
			ಮುಂದುವರಿ;
		ಆದರೆ (!dbtr_trigger_hw_supported(trig->ಸೂಚ್ಯಂಕ_2, tdata1,
					       tdata2, tdata3))
			ಮುಂದುವರಿ;
		ಹಿಂತಿರುಗಿ i;
	}

	ಹಿಂತಿರುಗಿ SBI_ENOENT;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ರಚನೆ sbi_dbtr_trigger *sbi_alloc_trigger(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata1,
							 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata2,
							 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata3)
{
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ sbi_dbtr_trigger *f_trig;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	ಆದರೆ (!hart_state)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (hart_state->ಲಭ್ಯ_trigs <= 0)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	i = dbtr_ಕಂಡುಹಿಡಿ_free_ಸ್ಥಾನ(hart_state, 0, tdata1, tdata2, tdata3);
	ಆದರೆ (i < 0)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	f_trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(i);
	hart_state->ಲಭ್ಯ_trigs--;
	__set_bit(RV_DBTR_ಬಿಟ್(TS, MAPPED), &f_trig->state);

	ಹಿಂತಿರುಗಿ f_trig;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ sbi_free_trigger(ರಚನೆ sbi_dbtr_trigger *trig)
{
	ರಚನೆ sbi_dbtr_hart_triggers_state *hart_state;

	ಆದರೆ (trig == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;

	hart_state = dbtr_thishart_state_ptr();
	ಆದರೆ (!hart_state)
		ಹಿಂತಿರುಗಿ;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->ಲಭ್ಯ_trigs++;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_init(ರಚನೆ sbi_scratch *scratch, bool coldboot)
{
	ರಚನೆ sbi_trap_info trap = {0};
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata1;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val;
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hart_state = ಖಾಲಿ;

	ಆದರೆ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ಹಿಂತಿರುಗಿ SBI_SUCCESS;

	ಆದರೆ (coldboot) {
		hart_state_ptr_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *);
		ಆದರೆ (!hart_state_ptr_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	ಆದರೆ (!hart_state) {
		hart_state = sbi_zalloc(ಗಾತ್ರ(*hart_state));
		ಆದರೆ (!hart_state)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_ಗಣ_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	ಆದರೆ (hart_state->probed)
		ಹೋಗು _probed;

	ಗಾಗಿ (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		ಆದರೆ (trap.cause)
			ನಿಲ್ಲಿಸು;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		ಆದರೆ (trap.cause)
			ನಿಲ್ಲಿಸು;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		ಆದರೆ (val != i)
			ನಿಲ್ಲಿಸು;

		val = csr_read_allowed(CSR_TINFO, &trap);
		ಆದರೆ (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			ಆದರೆ (trap.cause)
				ನಿಲ್ಲಿಸು;

			ಆದರೆ (TDATA1_GET_TYPE(tdata1) == 0)
				ನಿಲ್ಲಿಸು;

			sbi_trigger_init(ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(i),
					 ಬಿಟ್(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->ಒಟ್ಟು_trigs++;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (val == 1)
				ನಿಲ್ಲಿಸು;

			sbi_trigger_init(ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(i), val, i);
			hart_state->ಒಟ್ಟು_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->ಲಭ್ಯ_trigs = hart_state->ಒಟ್ಟು_trigs;

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_get_ಒಟ್ಟು_triggers(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	ಆದರೆ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ಹಿಂತಿರುಗಿ 0;

	hs = dbtr_thishart_state_ptr();
	ಆದರೆ (!hs)
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ hs->ಒಟ್ಟು_trigs;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_ಸಿದ್ಧತೆ_shmem(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_domain *dom, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ smode,
			 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_phys_lo,
			 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ shmem_phys_hi)
{
	ರಚನೆ sbi_dbtr_hart_triggers_state *hart_state;

	ಆದರೆ (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		ಹಿಂತಿರುಗಿ SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	ಆದರೆ (!hart_state)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	/* call is to disable shared memory */
	ಆದರೆ (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		ಹಿಂತಿರುಗಿ SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	ಆದರೆ (!sbi_dbtr_shmem_disabled(hart_state))
		ಹಿಂತಿರುಗಿ SBI_ERR_ALREADY_ಲಭ್ಯ;

	/* lower physical address must be XLEN/8 bytes aligned */
	ಆದರೆ (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

	/*
	* On RV32, the M-mode can only access the first 4GB of
	* the physical address space because M-mode does not have
	* MMU to access full 34-bit physical address space.
	* So fail if the upper 32 bits of the physical address
	* is non-zero on RV32.
	*
	* On RV64, kernel sets upper 64bit address part to zero.
	* So fail if the upper 64bit of the physical address
	* is non-zero on RV64.
	*/
	ಆದರೆ (shmem_phys_hi)
		ಹಿಂತಿರುಗಿ SBI_EINVALID_ADDR;

	ಆದರೆ (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_ವಿಳಾಸ;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಸ್ಥಿರ ಶೂನ್ಯ dbtr_trigger_ಸಿದ್ಧತೆ(ರಚನೆ sbi_dbtr_trigger *trig,
			       ರಚನೆ sbi_dbtr_ದತ್ತಾಂಶ_ಸಂದೇಶ *recv)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata1;

	ಆದರೆ (!trig)
		ಹಿಂತಿರುಗಿ;

	trig->tdata1 = lle_ಗಮ್ಯ_cpu(recv->tdata1);
	trig->tdata2 = lle_ಗಮ್ಯ_cpu(recv->tdata2);
	trig->tdata3 = lle_ಗಮ್ಯ_cpu(recv->tdata3);

	tdata1 = lle_ಗಮ್ಯ_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_ಬಿಟ್(TS, MAPPED), &trig->state);

	ಗಣ_TRIG_HW_ಸೂಚ್ಯಂಕ(trig->state, trig->ಸೂಚ್ಯಂಕ_2);

	ಆಯ್ಕೆ (TDATA1_GET_TYPE(tdata1)) {
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL:
		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(MC, U), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, U), &trig->state);

		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(MC, S), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, S), &trig->state);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL6:
		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(MC6, U), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, U), &trig->state);

		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(MC6, S), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, S), &trig->state);

		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, VU), &trig->state);

		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, VS), &trig->state);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_ICOUNT:
		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, U), &trig->state);

		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, S), &trig->state);

		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, VU), &trig->state);

		ಆದರೆ (__test_bit(RV_DBTR_ಬಿಟ್(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_ಬಿಟ್(TS, VS), &trig->state);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		ನಿಲ್ಲಿಸು;
	}
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ update_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ new, ಪೂರ್ಣಾಂಕ nr, ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr)
{
	ಆದರೆ (new)
		__set_bit(nr, addr);
	ಇಲ್ಲದಿದ್ದರೆ
		__clear_bit(nr, addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ dbtr_trigger_enable(ರಚನೆ sbi_dbtr_trigger *trig)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ state;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata1;

	ಆದರೆ (!trig || !(trig->state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, MAPPED)))
		ಹಿಂತಿರುಗಿ;

	state = trig->state;
	tdata1 = trig->tdata1;

	ಆಯ್ಕೆ (TDATA1_GET_TYPE(tdata1)) {
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL:
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, U),
			   RV_DBTR_ಬಿಟ್(MC, U), &trig->tdata1);
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, S),
			   RV_DBTR_ಬಿಟ್(MC, S), &trig->tdata1);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL6:
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, VU),
			   RV_DBTR_ಬಿಟ್(MC6, VU), &trig->tdata1);
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, VS),
			   RV_DBTR_ಬಿಟ್(MC6, VS), &trig->tdata1);
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, U),
			   RV_DBTR_ಬಿಟ್(MC6, U), &trig->tdata1);
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, S),
			   RV_DBTR_ಬಿಟ್(MC6, S), &trig->tdata1);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_ICOUNT:
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, VU),
			   RV_DBTR_ಬಿಟ್(ICOUNT, VU), &trig->tdata1);
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, VS),
			   RV_DBTR_ಬಿಟ್(ICOUNT, VS), &trig->tdata1);
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, U),
			   RV_DBTR_ಬಿಟ್(ICOUNT, U), &trig->tdata1);
		update_ಬಿಟ್(state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, S),
			   RV_DBTR_ಬಿಟ್(ICOUNT, S), &trig->tdata1);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	/*
	 * RISC-V Debug Support v1.0.0 section 5.5:
	 * Debugger cannot simply set a trigger by writing tdata1, then tdata2,
	 * etc. The current value of tdata2 might not be legal with the new
	 * value of tdata1. To help with this situation, it is guaranteed that
	 * writing 0 to tdata1 disables the trigger, and leaves it in a state
	 * where tdata2 and tdata3 can be written with any value that makes
	 * sense for any trigger type supported by this trigger.
	 */
	csr_write(CSR_TSELECT, trig->ಸೂಚ್ಯಂಕ_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_ಸುರಕ್ಷಿತ(CSR_TDATA2, trig->tdata2);
	tdata_write_ಸುರಕ್ಷಿತ(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ಸ್ಥಿರ ಶೂನ್ಯ dbtr_trigger_disable(ರಚನೆ sbi_dbtr_trigger *trig)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata1;

	ಆದರೆ (!trig || !(trig->state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, MAPPED)))
		ಹಿಂತಿರುಗಿ;

	tdata1 = trig->tdata1;

	ಆಯ್ಕೆ (TDATA1_GET_TYPE(tdata1)) {
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_ಬಿಟ್(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ಬಿಟ್(MC, S), &trig->tdata1);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_ಬಿಟ್(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_ಬಿಟ್(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_ಬಿಟ್(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ಬಿಟ್(MC6, S), &trig->tdata1);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_ಬಿಟ್(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_ಬಿಟ್(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_ಬಿಟ್(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ಬಿಟ್(ICOUNT, S), &trig->tdata1);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	csr_write(CSR_TSELECT, trig->ಸೂಚ್ಯಂಕ_2);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ಸ್ಥಿರ ಶೂನ್ಯ dbtr_trigger_clear(ರಚನೆ sbi_dbtr_trigger *trig)
{
	ಆದರೆ (!trig || !(trig->state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, MAPPED)))
		ಹಿಂತಿರುಗಿ;

	csr_write(CSR_TSELECT, trig->ಸೂಚ್ಯಂಕ_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_ಸುರಕ್ಷಿತ(CSR_TDATA2, 0x0);
	tdata_write_ಸುರಕ್ಷಿತ(CSR_TDATA3, 0x0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dbtr_trigger_supported(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ type)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL:
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL6:
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_ICOUNT:
		ಹಿಂತಿರುಗಿ 1;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dbtr_trigger_valid(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ type, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tdata)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL:
		ಆದರೆ (!(tdata & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(MC, DMODE)) &&
		    !(tdata & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(MC, M)))
			ಹಿಂತಿರುಗಿ 1;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_MCONTROL6:
		ಆದರೆ (!(tdata & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(MC6, M)))
			ಹಿಂತಿರುಗಿ 1;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RISCV_DBTR_TRIG_ICOUNT:
		ಆದರೆ (!(tdata & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(ICOUNT, M)))
			ಹಿಂತಿರುಗಿ 1;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_ಅಂಕೆ_trig(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ type = TDATA1_GET_TYPE(ದತ್ತಾಂಶ);
	u32 hartid = current_hartid();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಒಟ್ಟು = 0;
	ರಚನೆ sbi_dbtr_trigger *trig;
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ಆದರೆ (!hs)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	ಆದರೆ (ದತ್ತಾಂಶ == 0) {
		*ಹೊರಹೋಗು = hs->ಒಟ್ಟು_trigs;
		ಹಿಂತಿರುಗಿ SBI_SUCCESS;
	}

	ಗಾಗಿ (i = 0; i < hs->ಒಟ್ಟು_trigs; i++) {
		trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(i);

		ಆದರೆ (__test_bit(type, &trig->type_ಮುಖವಾಡ) &&
		    dbtr_trigger_hw_supported(trig->ಸೂಚ್ಯಂಕ_2, ದತ್ತಾಂಶ, 0, 0))
			ಒಟ್ಟು++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, ಒಟ್ಟು);

	*ಹೊರಹೋಗು = ಒಟ್ಟು;
	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_read_trig(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ smode,
		       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_idx_ತಳ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_ಎಣಿಕೆ)
{
	ರಚನೆ sbi_dbtr_ದತ್ತಾಂಶ_ಸಂದೇಶ *xmit;
	ರಚನೆ sbi_dbtr_trigger *trig;
	ಒಕ್ಕೂಟ sbi_dbtr_shmem_ನಮೂದು *ನಮೂದು_2;
	ಶೂನ್ಯ *shmem_ತಳ = ಖಾಲಿ;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs = ಖಾಲಿ;

	hs = dbtr_thishart_state_ptr();
	ಆದರೆ (!hs)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	ಆದರೆ (trig_idx_ತಳ >= hs->ಒಟ್ಟು_trigs ||
	    trig_idx_ತಳ + trig_ಎಣಿಕೆ >= hs->ಒಟ್ಟು_trigs)
		ಹಿಂತಿರುಗಿ SBI_ERR_BAD_RANGE;

	ಆದರೆ (sbi_dbtr_shmem_disabled(hs))
		ಹಿಂತಿರುಗಿ SBI_ERR_NO_SHMEM;

	shmem_ತಳ = hart_shmem_ತಳ(hs);

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
					   trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));
	for_each_trig_ನಮೂದು(shmem_ತಳ, trig_ಎಣಿಕೆ, typeof(*ನಮೂದು_2), ನಮೂದು_2) {
		xmit = &ನಮೂದು_2->ದತ್ತಾಂಶ;
		trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER((_idx + trig_idx_ತಳ));
		csr_write(CSR_TSELECT, trig->ಸೂಚ್ಯಂಕ_2);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_ಸುರಕ್ಷಿತ(CSR_TDATA2);
		trig->tdata3 = tdata_read_ಸುರಕ್ಷಿತ(CSR_TDATA3);
		xmit->tstate = cpu_ಗಮ್ಯ_lle(trig->state);
		xmit->tdata1 = cpu_ಗಮ್ಯ_lle(trig->tdata1);
		xmit->tdata2 = cpu_ಗಮ್ಯ_lle(trig->tdata2);
		xmit->tdata3 = cpu_ಗಮ್ಯ_lle(trig->tdata3);
	}
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
					     trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_install_trig(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ smode,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_ಎಣಿಕೆ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು)
{
	ಶೂನ್ಯ *shmem_ತಳ = ಖಾಲಿ;
	ಒಕ್ಕೂಟ sbi_dbtr_shmem_ನಮೂದು *ನಮೂದು_2;
	ರಚನೆ sbi_dbtr_ದತ್ತಾಂಶ_ಸಂದೇಶ *recv;
	ರಚನೆ sbi_dbtr_id_ಸಂದೇಶ *xmit;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ctrl;
	u32 claimed = 0;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನ;
	ರಚನೆ sbi_dbtr_trigger *trig;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs = ಖಾಲಿ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ಆದರೆ (!hs)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	ಆದರೆ (sbi_dbtr_shmem_disabled(hs))
		ಹಿಂತಿರುಗಿ SBI_ERR_NO_SHMEM;

	shmem_ತಳ = hart_shmem_ತಳ(hs);
	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
					   trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));

	/*
	 * SBI v3.0 sec 19.4 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught here; WARL bits tied
	 * off inside an otherwise-implemented CSR are delegated to the
	 * device-specific trigger_supported() callback via
	 * dbtr_trigger_any_hw_supported().
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	/* Check requested triggers configuration */
	for_each_trig_ನಮೂದು(shmem_ತಳ, trig_ಎಣಿಕೆ, typeof(*ನಮೂದು_2), ನಮೂದು_2) {
		recv = (ರಚನೆ sbi_dbtr_ದತ್ತಾಂಶ_ಸಂದೇಶ *)(&ನಮೂದು_2->ದತ್ತಾಂಶ);
		ctrl = recv->tdata1;

		ಆದರೆ (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*ಹೊರಹೋಗು = _idx;
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
							     trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;
		}

		ಆದರೆ (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*ಹೊರಹೋಗು = _idx;
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
							     trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;
		}

		ಆದರೆ ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*ಹೊರಹೋಗು = _idx;
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
							     trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;
		}

		ಆದರೆ (!dbtr_trigger_any_hw_supported(hs,
						   lle_ಗಮ್ಯ_cpu(recv->tdata1),
						   lle_ಗಮ್ಯ_cpu(recv->tdata2),
						   lle_ಗಮ್ಯ_cpu(recv->tdata3))) {
			*ಹೊರಹೋಗು = _idx;
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
							     trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;
		}
	}

	ಆದರೆ (hs->ಲಭ್ಯ_trigs < trig_ಎಣಿಕೆ) {
		*ಹೊರಹೋಗು = hs->ಲಭ್ಯ_trigs;
		sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
						     trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_ನಮೂದು(shmem_ತಳ, trig_ಎಣಿಕೆ, typeof(*ನಮೂದು_2), ನಮೂದು_2) {
		recv = (ರಚನೆ sbi_dbtr_ದತ್ತಾಂಶ_ಸಂದೇಶ *)(&ನಮೂದು_2->ದತ್ತಾಂಶ);
		ಸ್ಥಾನ = dbtr_ಕಂಡುಹಿಡಿ_free_ಸ್ಥಾನ(hs, claimed,
					   lle_ಗಮ್ಯ_cpu(recv->tdata1),
					   lle_ಗಮ್ಯ_cpu(recv->tdata2),
					   lle_ಗಮ್ಯ_cpu(recv->tdata3));
		ಆದರೆ (ಸ್ಥಾನ < 0) {
			*ಹೊರಹೋಗು = _idx;
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
							     trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;
		}
		claimed |= ಬಿಟ್(ಸ್ಥಾನ);
	}

	/* Install triggers */
	for_each_trig_ನಮೂದು(shmem_ತಳ, trig_ಎಣಿಕೆ, typeof(*ನಮೂದು_2), ನಮೂದು_2) {
		recv = (ರಚನೆ sbi_dbtr_ದತ್ತಾಂಶ_ಸಂದೇಶ *)(&ನಮೂದು_2->ದತ್ತಾಂಶ);
		xmit = (ರಚನೆ sbi_dbtr_id_ಸಂದೇಶ *)(&ನಮೂದು_2->id);

		/*
		 * The dry-run above matched every requested configuration
		 * to a free hardware trigger slot, so allocation must
		 * succeed.
		 */
		trig = sbi_alloc_trigger(lle_ಗಮ್ಯ_cpu(recv->tdata1),
					 lle_ಗಮ್ಯ_cpu(recv->tdata2),
					 lle_ಗಮ್ಯ_cpu(recv->tdata3));

		dbtr_trigger_ಸಿದ್ಧತೆ(trig,  recv);
		dbtr_trigger_enable(trig);
		xmit->idx = cpu_ಗಮ್ಯ_lle(trig->ಸೂಚ್ಯಂಕ_2);

	}

	sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)shmem_ತಳ,
					     trig_ಎಣಿಕೆ * ಗಾತ್ರ(*ನಮೂದು_2));

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_uninstall_trig(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_idx_ತಳ,
			    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_idx_ಮುಖವಾಡ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_ಮುಖವಾಡ = trig_idx_ಮುಖವಾಡ << trig_idx_ತಳ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ idx = trig_idx_ತಳ;
	ರಚನೆ sbi_dbtr_trigger *trig;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ಆದರೆ (!hs)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	for_each_ಗಣ_ಬಿಟ್_from(idx, &trig_ಮುಖವಾಡ, hs->ಒಟ್ಟು_trigs) {
		trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(idx);
		ಆದರೆ (!(trig->state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, MAPPED)))
			ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_enable_trig(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_idx_ತಳ,
			 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_idx_ಮುಖವಾಡ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_ಮುಖವಾಡ = trig_idx_ಮುಖವಾಡ << trig_idx_ತಳ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ idx = trig_idx_ತಳ;
	ರಚನೆ sbi_dbtr_trigger *trig;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ಆದರೆ (!hs)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	for_each_ಗಣ_ಬಿಟ್_from(idx, &trig_ಮುಖವಾಡ, hs->ಒಟ್ಟು_trigs) {
		trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_update_trig(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ smode,
			 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_ಎಣಿಕೆ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_idx;
	ರಚನೆ sbi_dbtr_trigger *trig;
	ಒಕ್ಕೂಟ sbi_dbtr_shmem_ನಮೂದು *ನಮೂದು_2;
	ಶೂನ್ಯ *shmem_ತಳ = ಖಾಲಿ;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs = ಖಾಲಿ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ಆದರೆ (!hs)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	ಆದರೆ (sbi_dbtr_shmem_disabled(hs))
		ಹಿಂತಿರುಗಿ SBI_ERR_NO_SHMEM;

	shmem_ತಳ = hart_shmem_ತಳ(hs);

	ಆದರೆ (trig_ಎಣಿಕೆ >= hs->ಒಟ್ಟು_trigs)
		ಹಿಂತಿರುಗಿ SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_ನಮೂದು(shmem_ತಳ, trig_ಎಣಿಕೆ, typeof(*ನಮೂದು_2), ನಮೂದು_2) {
		sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ನಮೂದು_2, ಗಾತ್ರ(*ನಮೂದು_2));
		trig_idx = ನಮೂದು_2->id.idx;

		ಆದರೆ (trig_idx >= hs->ಒಟ್ಟು_trigs) {
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ನಮೂದು_2, ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;
		}

		trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(trig_idx);

		ಆದರೆ (!(trig->state & RV_DBTR_ಬಿಟ್_ಮುಖವಾಡ(TS, MAPPED))) {
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ನಮೂದು_2, ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;
		}

		ಆದರೆ ((ನಮೂದು_2->ದತ್ತಾಂಶ.tdata2 && !tdata2_impl) ||
		    (ನಮೂದು_2->ದತ್ತಾಂಶ.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ನಮೂದು_2, ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;
		}

		ಆದರೆ (!dbtr_trigger_hw_supported(trig->ಸೂಚ್ಯಂಕ_2,
					       lle_ಗಮ್ಯ_cpu(ನಮೂದು_2->ದತ್ತಾಂಶ.tdata1),
					       lle_ಗಮ್ಯ_cpu(ನಮೂದು_2->ದತ್ತಾಂಶ.tdata2),
					       lle_ಗಮ್ಯ_cpu(ನಮೂದು_2->ದತ್ತಾಂಶ.tdata3))) {
			sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ನಮೂದು_2, ಗಾತ್ರ(*ನಮೂದು_2));
			ಹಿಂತಿರುಗಿ SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_ಸಿದ್ಧತೆ(trig, &ನಮೂದು_2->ದತ್ತಾಂಶ);
		sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ನಮೂದು_2, ಗಾತ್ರ(*ನಮೂದು_2));
		dbtr_trigger_enable(trig);
	}

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಪೂರ್ಣಾಂಕ sbi_dbtr_disable_trig(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_idx_ತಳ,
			  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_idx_ಮುಖವಾಡ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ trig_ಮುಖವಾಡ = trig_idx_ಮುಖವಾಡ << trig_idx_ತಳ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ idx = trig_idx_ತಳ;
	ರಚನೆ sbi_dbtr_trigger *trig;
	ರಚನೆ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ಆದರೆ (!hs)
		ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

	for_each_ಗಣ_ಬಿಟ್_from(idx, &trig_ಮುಖವಾಡ, hs->ಒಟ್ಟು_trigs) {
		trig = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}
