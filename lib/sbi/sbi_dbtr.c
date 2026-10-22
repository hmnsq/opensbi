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
ಸ್ಥಿರ unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_dbtr_device *dbtr_dev = ರಿಕ್ತ;

ಧ್ರುವ ಸಂರಚನಾ sbi_dbtr_device *sbi_dbtr_get_device(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ dbtr_dev;
}

ಶೂನ್ಯ sbi_dbtr_set_device(ಧ್ರುವ ಸಂರಚನಾ sbi_dbtr_device *dev)
{
	ಯದಿ (!dev || dbtr_dev)
		ಪ್ರತ್ಯಾಗಮ;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		ಸಂರಚನಾ sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		ಸಂರಚನಾ sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		ಸಂರಚನಾ sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define INDEX_TO_TRIGGER(_index)					\
	({								\
		ಸಂರಚನಾ sbi_dbtr_trigger *__trg = ರಿಕ್ತ;			\
		ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *__hs = ರಿಕ್ತ;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[_index];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool sbi_dbtr_shmem_disabled(
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs)
{
	ಪ್ರತ್ಯಾಗಮ (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ sbi_dbtr_disable_shmem(
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *hart_shmem_base(
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs)
{
	ಪ್ರತ್ಯಾಗಮ ((ಶೂನ್ಯ *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

ಸ್ಥಿರ ಶೂನ್ಯ sbi_trigger_init(ಸಂರಚನಾ sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->index = idx;
}

ಸ್ಥಿರ bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	ಯದಿ (dbtr_dev && dbtr_dev->trigger_supported)
		ಪ್ರತ್ಯಾಗಮ dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool dbtr_trigger_any_hw_supported(
			ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		ಯದಿ (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, tdata1, tdata2,
					      tdata3))
			ಪ್ರತ್ಯಾಗಮ true;
	}

	ಪ್ರತ್ಯಾಗಮ false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbtr_find_free_slot(ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		ಯದಿ (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			ಅನುವರ್ತ;
		ಯದಿ (claimed_mask & BIT(i))
			ಅನುವರ್ತ;
		ಯದಿ (!__test_bit(type, &trig->type_mask))
			ಅನುವರ್ತ;
		ಯದಿ (!dbtr_trigger_hw_supported(trig->index, tdata1,
					       tdata2, tdata3))
			ಅನುವರ್ತ;
		ಪ್ರತ್ಯಾಗಮ i;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಸಂರಚನಾ sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ sbi_dbtr_trigger *f_trig;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	ಯದಿ (!hart_state)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (hart_state->available_trigs <= 0)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	ಯದಿ (i < 0)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	f_trig = INDEX_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	ಪ್ರತ್ಯಾಗಮ f_trig;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ sbi_free_trigger(ಸಂರಚನಾ sbi_dbtr_trigger *trig)
{
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hart_state;

	ಯದಿ (trig == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;

	hart_state = dbtr_thishart_state_ptr();
	ಯದಿ (!hart_state)
		ಪ್ರತ್ಯಾಗಮ;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_init(ಸಂರಚನಾ sbi_scratch *scratch, bool coldboot)
{
	ಸಂರಚನಾ sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hart_state = ರಿಕ್ತ;

	ಯದಿ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;

	ಯದಿ (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(ಶೂನ್ಯ *);
		ಯದಿ (!hart_state_ptr_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	ಯದಿ (!hart_state) {
		hart_state = sbi_zalloc(ಪರಿಮಾಣ(*hart_state));
		ಯದಿ (!hart_state)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	ಯದಿ (hart_state->probed)
		ಗಚ್ಛ _probed;

	ಪ್ರತಿ (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		ಯದಿ (trap.cause)
			ಭಙ್ಗ;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		ಯದಿ (trap.cause)
			ಭಙ್ಗ;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		ಯದಿ (val != i)
			ಭಙ್ಗ;

		val = csr_read_allowed(CSR_TINFO, &trap);
		ಯದಿ (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			ಯದಿ (trap.cause)
				ಭಙ್ಗ;

			ಯದಿ (TDATA1_GET_TYPE(tdata1) == 0)
				ಭಙ್ಗ;

			sbi_trigger_init(INDEX_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->total_trigs++;
		} ಅನ್ಯಥಾ {
			ಯದಿ (val == 1)
				ಭಙ್ಗ;

			sbi_trigger_init(INDEX_TO_TRIGGER(i), val, i);
			hart_state->total_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->total_trigs;

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_get_total_triggers(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	ಯದಿ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ಪ್ರತ್ಯಾಗಮ 0;

	hs = dbtr_thishart_state_ptr();
	ಯದಿ (!hs)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ hs->total_trigs;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_setup_shmem(ಧ್ರುವ ಸಂರಚನಾ sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hart_state;

	ಯದಿ (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	ಯದಿ (!hart_state)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	/* call is to disable shared memory */
	ಯದಿ (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	ಯದಿ (!sbi_dbtr_shmem_disabled(hart_state))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	ಯದಿ (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

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
	ಯದಿ (shmem_phys_hi)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVALID_ADDR;

	ಯದಿ (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಸ್ಥಿರ ಶೂನ್ಯ dbtr_trigger_setup(ಸಂರಚನಾ sbi_dbtr_trigger *trig,
			       ಸಂರಚನಾ sbi_dbtr_data_ಸಂದೇಶ *recv)
{
	unsigned long tdata1;

	ಯದಿ (!trig)
		ಪ್ರತ್ಯಾಗಮ;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_INDEX(trig->state, trig->index);

	ವಿಕಲ್ಪ (TDATA1_GET_TYPE(tdata1)) {
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL:
		ಯದಿ (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		ಯದಿ (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		ಭಙ್ಗ;
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL6:
		ಯದಿ (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		ಯದಿ (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		ಯದಿ (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		ಯದಿ (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		ಭಙ್ಗ;
	ಪಕ್ಷ RISCV_DBTR_TRIG_ICOUNT:
		ಯದಿ (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		ಯದಿ (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		ಯದಿ (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		ಯದಿ (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		ಭಙ್ಗ;
	}
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ update_bit(unsigned long new, ಪೂರ್ಣಾಙ್ಕ nr, ಚಞ್ಚಲ unsigned long *addr)
{
	ಯದಿ (new)
		__set_bit(nr, addr);
	ಅನ್ಯಥಾ
		__clear_bit(nr, addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ dbtr_trigger_enable(ಸಂರಚನಾ sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	ಯದಿ (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ಪ್ರತ್ಯಾಗಮ;

	state = trig->state;
	tdata1 = trig->tdata1;

	ವಿಕಲ್ಪ (TDATA1_GET_TYPE(tdata1)) {
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		ಭಙ್ಗ;
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		ಭಙ್ಗ;
	ಪಕ್ಷ RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
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
	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, trig->tdata2);
	tdata_write_safe(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ಸ್ಥಿರ ಶೂನ್ಯ dbtr_trigger_disable(ಸಂರಚನಾ sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	ಯದಿ (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ಪ್ರತ್ಯಾಗಮ;

	tdata1 = trig->tdata1;

	ವಿಕಲ್ಪ (TDATA1_GET_TYPE(tdata1)) {
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		ಭಙ್ಗ;
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		ಭಙ್ಗ;
	ಪಕ್ಷ RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ಸ್ಥಿರ ಶೂನ್ಯ dbtr_trigger_clear(ಸಂರಚನಾ sbi_dbtr_trigger *trig)
{
	ಯದಿ (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ಪ್ರತ್ಯಾಗಮ;

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbtr_trigger_supported(unsigned long type)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL:
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL6:
	ಪಕ್ಷ RISCV_DBTR_TRIG_ICOUNT:
		ಪ್ರತ್ಯಾಗಮ 1;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL:
		ಯದಿ (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			ಪ್ರತ್ಯಾಗಮ 1;
		ಭಙ್ಗ;
	ಪಕ್ಷ RISCV_DBTR_TRIG_MCONTROL6:
		ಯದಿ (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			ಪ್ರತ್ಯಾಗಮ 1;
		ಭಙ್ಗ;
	ಪಕ್ಷ RISCV_DBTR_TRIG_ICOUNT:
		ಯದಿ (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			ಪ್ರತ್ಯಾಗಮ 1;
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_ಅಂಕೆ_trig(unsigned long data, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(data);
	u32 hartid = current_hartid();
	unsigned long total = 0;
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ಯದಿ (!hs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	ಯದಿ (data == 0) {
		*out = hs->total_trigs;
		ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
	}

	ಪ್ರತಿ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);

		ಯದಿ (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, data, 0, 0))
			total++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, total);

	*out = total;
	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_ಎಣಿಕೆ)
{
	ಸಂರಚನಾ sbi_dbtr_data_ಸಂದೇಶ *xmit;
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಸಙ್ಘ sbi_dbtr_shmem_entry *entry;
	ಶೂನ್ಯ *shmem_base = ರಿಕ್ತ;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs = ರಿಕ್ತ;

	hs = dbtr_thishart_state_ptr();
	ಯದಿ (!hs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	ಯದಿ (trig_idx_base >= hs->total_trigs ||
	    trig_idx_base + trig_ಎಣಿಕೆ >= hs->total_trigs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_BAD_RANGE;

	ಯದಿ (sbi_dbtr_shmem_disabled(hs))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));
	for_each_trig_entry(shmem_base, trig_ಎಣಿಕೆ, typeof(*entry), entry) {
		xmit = &entry->data;
		trig = INDEX_TO_TRIGGER((_idx + trig_idx_base));
		csr_write(CSR_TSELECT, trig->index);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_safe(CSR_TDATA2);
		trig->tdata3 = tdata_read_safe(CSR_TDATA3);
		xmit->tstate = cpu_to_lle(trig->state);
		xmit->tdata1 = cpu_to_lle(trig->tdata1);
		xmit->tdata2 = cpu_to_lle(trig->tdata2);
		xmit->tdata3 = cpu_to_lle(trig->tdata3);
	}
	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
					     trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_ಎಣಿಕೆ, unsigned long *out)
{
	ಶೂನ್ಯ *shmem_base = ರಿಕ್ತ;
	ಸಙ್ಘ sbi_dbtr_shmem_entry *entry;
	ಸಂರಚನಾ sbi_dbtr_data_ಸಂದೇಶ *recv;
	ಸಂರಚನಾ sbi_dbtr_id_ಸಂದೇಶ *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	ಪೂರ್ಣಾಙ್ಕ slot;
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs = ರಿಕ್ತ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ಯದಿ (!hs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	ಯದಿ (sbi_dbtr_shmem_disabled(hs))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));

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
	for_each_trig_entry(shmem_base, trig_ಎಣಿಕೆ, typeof(*entry), entry) {
		recv = (ಸಂರಚನಾ sbi_dbtr_data_ಸಂದೇಶ *)(&entry->data);
		ctrl = recv->tdata1;

		ಯದಿ (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;
		}

		ಯದಿ (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;
		}

		ಯದಿ ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;
		}

		ಯದಿ (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;
		}
	}

	ಯದಿ (hs->available_trigs < trig_ಎಣಿಕೆ) {
		*out = hs->available_trigs;
		sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
						     trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_ಎಣಿಕೆ, typeof(*entry), entry) {
		recv = (ಸಂರಚನಾ sbi_dbtr_data_ಸಂದೇಶ *)(&entry->data);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		ಯದಿ (slot < 0) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_ಎಣಿಕೆ, typeof(*entry), entry) {
		recv = (ಸಂರಚನಾ sbi_dbtr_data_ಸಂದೇಶ *)(&entry->data);
		xmit = (ಸಂರಚನಾ sbi_dbtr_id_ಸಂದೇಶ *)(&entry->id);

		/*
		 * The dry-run above matched every requested configuration
		 * to a free hardware trigger slot, so allocation must
		 * succeed.
		 */
		trig = sbi_alloc_trigger(lle_to_cpu(recv->tdata1),
					 lle_to_cpu(recv->tdata2),
					 lle_to_cpu(recv->tdata3));

		dbtr_trigger_setup(trig,  recv);
		dbtr_trigger_enable(trig);
		xmit->idx = cpu_to_lle(trig->index);

	}

	sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
					     trig_ಎಣಿಕೆ * ಪರಿಮಾಣ(*entry));

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ಯದಿ (!hs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		ಯದಿ (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ಯದಿ (!hs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_ಎಣಿಕೆ)
{
	unsigned long trig_idx;
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಸಙ್ಘ sbi_dbtr_shmem_entry *entry;
	ಶೂನ್ಯ *shmem_base = ರಿಕ್ತ;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs = ರಿಕ್ತ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ಯದಿ (!hs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	ಯದಿ (sbi_dbtr_shmem_disabled(hs))
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	ಯದಿ (trig_ಎಣಿಕೆ >= hs->total_trigs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_ಎಣಿಕೆ, typeof(*entry), entry) {
		sbi_hart_protection_temp_map_range((unsigned long)entry, ಪರಿಮಾಣ(*entry));
		trig_idx = entry->id.idx;

		ಯದಿ (trig_idx >= hs->total_trigs) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;
		}

		trig = INDEX_TO_TRIGGER(trig_idx);

		ಯದಿ (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;
		}

		ಯದಿ ((entry->data.tdata2 && !tdata2_impl) ||
		    (entry->data.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;
		}

		ಯದಿ (!dbtr_trigger_hw_supported(trig->index,
					       lle_to_cpu(entry->data.tdata1),
					       lle_to_cpu(entry->data.tdata2),
					       lle_to_cpu(entry->data.tdata3))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, ಪರಿಮಾಣ(*entry));
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->data);
		sbi_hart_protection_temp_unmap_range((unsigned long)entry, ಪರಿಮಾಣ(*entry));
		dbtr_trigger_enable(trig);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ಸಂರಚನಾ sbi_dbtr_trigger *trig;
	ಸಂರಚನಾ sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ಯದಿ (!hs)
		ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}
