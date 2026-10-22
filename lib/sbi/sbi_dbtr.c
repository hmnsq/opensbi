/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_csr_detect.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_byteorder.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_dbtr.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hart_protection.h>
#അന്തര്ഭാവ <sbi/riscv_encoding.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
സ്ഥിതം unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
സ്ഥിതം സ്ഥിരം ഘടന sbi_dbtr_device *dbtr_dev = ശൂന്യ;

സ്ഥിരം ഘടന sbi_dbtr_device *sbi_dbtr_get_device(ശൂന്യം)
{
	തിരികെ dbtr_dev;
}

ശൂന്യം sbi_dbtr_set_device(സ്ഥിരം ഘടന sbi_dbtr_device *dev)
{
	എങ്കിൽ (!dev || dbtr_dev)
		തിരികെ;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ശൂന്യം *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), ശൂന്യം *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		ഘടന sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		ഘടന sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		ഘടന sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define INDEX_TO_TRIGGER(_index)					\
	({								\
		ഘടന sbi_dbtr_trigger *__trg = ശൂന്യ;			\
		ഘടന sbi_dbtr_hart_triggers_state *__hs = ശൂന്യ;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[_index];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	വേണ്ടി (പൂർണ്ണസംഖ്യ _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
സ്ഥിതം വരിയിൽ bool sbi_dbtr_shmem_disabled(
	ഘടന sbi_dbtr_hart_triggers_state *hs)
{
	തിരികെ (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
സ്ഥിതം വരിയിൽ ശൂന്യം sbi_dbtr_disable_shmem(
	ഘടന sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
സ്ഥിതം വരിയിൽ ശൂന്യം *hart_shmem_base(
	ഘടന sbi_dbtr_hart_triggers_state *hs)
{
	തിരികെ ((ശൂന്യം *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

സ്ഥിതം ശൂന്യം sbi_trigger_init(ഘടന sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->index = idx;
}

സ്ഥിതം bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	എങ്കിൽ (dbtr_dev && dbtr_dev->trigger_supported)
		തിരികെ dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	തിരികെ true;
}

സ്ഥിതം bool dbtr_trigger_any_hw_supported(
			ഘടന sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	ഘടന sbi_dbtr_trigger *trig;
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		എങ്കിൽ (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, tdata1, tdata2,
					      tdata3))
			തിരികെ true;
	}

	തിരികെ false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ dbtr_find_free_slot(ഘടന sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	ഘടന sbi_dbtr_trigger *trig;
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		എങ്കിൽ (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			തുടരുക;
		എങ്കിൽ (claimed_mask & BIT(i))
			തുടരുക;
		എങ്കിൽ (!__test_bit(type, &trig->type_mask))
			തുടരുക;
		എങ്കിൽ (!dbtr_trigger_hw_supported(trig->index, tdata1,
					       tdata2, tdata3))
			തുടരുക;
		തിരികെ i;
	}

	തിരികെ SBI_ENOENT;
}

സ്ഥിതം വരിയിൽ ഘടന sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	പൂർണ്ണസംഖ്യ i;
	ഘടന sbi_dbtr_trigger *f_trig;
	ഘടന sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hart_state)
		തിരികെ ശൂന്യ;

	എങ്കിൽ (hart_state->available_trigs <= 0)
		തിരികെ ശൂന്യ;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	എങ്കിൽ (i < 0)
		തിരികെ ശൂന്യ;

	f_trig = INDEX_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	തിരികെ f_trig;
}

സ്ഥിതം വരിയിൽ ശൂന്യം sbi_free_trigger(ഘടന sbi_dbtr_trigger *trig)
{
	ഘടന sbi_dbtr_hart_triggers_state *hart_state;

	എങ്കിൽ (trig == ശൂന്യ)
		തിരികെ;

	hart_state = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hart_state)
		തിരികെ;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_init(ഘടന sbi_scratch *scratch, bool coldboot)
{
	ഘടന sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	പൂർണ്ണസംഖ്യ i;
	ഘടന sbi_dbtr_hart_triggers_state *hart_state = ശൂന്യ;

	എങ്കിൽ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		തിരികെ SBI_SUCCESS;

	എങ്കിൽ (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(ശൂന്യം *);
		എങ്കിൽ (!hart_state_ptr_offset)
			തിരികെ SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	എങ്കിൽ (!hart_state) {
		hart_state = sbi_zalloc(വലുപ്പം(*hart_state));
		എങ്കിൽ (!hart_state)
			തിരികെ SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	എങ്കിൽ (hart_state->probed)
		പോകുക _probed;

	വേണ്ടി (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		എങ്കിൽ (trap.cause)
			നിർത്തുക;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		എങ്കിൽ (trap.cause)
			നിർത്തുക;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		എങ്കിൽ (val != i)
			നിർത്തുക;

		val = csr_read_allowed(CSR_TINFO, &trap);
		എങ്കിൽ (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			എങ്കിൽ (trap.cause)
				നിർത്തുക;

			എങ്കിൽ (TDATA1_GET_TYPE(tdata1) == 0)
				നിർത്തുക;

			sbi_trigger_init(INDEX_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->total_trigs++;
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (val == 1)
				നിർത്തുക;

			sbi_trigger_init(INDEX_TO_TRIGGER(i), val, i);
			hart_state->total_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->total_trigs;

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_get_total_triggers(ശൂന്യം)
{
	ഘടന sbi_dbtr_hart_triggers_state *hs;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	എങ്കിൽ (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		തിരികെ 0;

	hs = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hs)
		തിരികെ 0;

	തിരികെ hs->total_trigs;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_setup_shmem(സ്ഥിരം ഘടന sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	ഘടന sbi_dbtr_hart_triggers_state *hart_state;

	എങ്കിൽ (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		തിരികെ SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hart_state)
		തിരികെ SBI_ERR_FAILED;

	/* call is to disable shared memory */
	എങ്കിൽ (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		തിരികെ SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	എങ്കിൽ (!sbi_dbtr_shmem_disabled(hart_state))
		തിരികെ SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	എങ്കിൽ (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		തിരികെ SBI_ERR_INVALID_PARAM;

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
	എങ്കിൽ (shmem_phys_hi)
		തിരികെ SBI_EINVALID_ADDR;

	എങ്കിൽ (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		തിരികെ SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	തിരികെ SBI_SUCCESS;
}

സ്ഥിതം ശൂന്യം dbtr_trigger_setup(ഘടന sbi_dbtr_trigger *trig,
			       ഘടന sbi_dbtr_data_സന്ദേശം *recv)
{
	unsigned long tdata1;

	എങ്കിൽ (!trig)
		തിരികെ;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_INDEX(trig->state, trig->index);

	തിരഞ്ഞെടുപ്പ് (TDATA1_GET_TYPE(tdata1)) {
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL:
		എങ്കിൽ (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		എങ്കിൽ (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		നിർത്തുക;
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL6:
		എങ്കിൽ (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		എങ്കിൽ (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		എങ്കിൽ (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		എങ്കിൽ (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		നിർത്തുക;
	അവസ്ഥ RISCV_DBTR_TRIG_ICOUNT:
		എങ്കിൽ (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		എങ്കിൽ (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		എങ്കിൽ (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		എങ്കിൽ (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		നിർത്തുക;
	}
}

സ്ഥിതം വരിയിൽ ശൂന്യം update_bit(unsigned long new, പൂർണ്ണസംഖ്യ nr, അസ്ഥിര unsigned long *addr)
{
	എങ്കിൽ (new)
		__set_bit(nr, addr);
	അല്ലെങ്കിൽ
		__clear_bit(nr, addr);
}

സ്ഥിതം ശൂന്യം dbtr_trigger_enable(ഘടന sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	എങ്കിൽ (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		തിരികെ;

	state = trig->state;
	tdata1 = trig->tdata1;

	തിരഞ്ഞെടുപ്പ് (TDATA1_GET_TYPE(tdata1)) {
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		നിർത്തുക;
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		നിർത്തുക;
	അവസ്ഥ RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
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

സ്ഥിതം ശൂന്യം dbtr_trigger_disable(ഘടന sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	എങ്കിൽ (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		തിരികെ;

	tdata1 = trig->tdata1;

	തിരഞ്ഞെടുപ്പ് (TDATA1_GET_TYPE(tdata1)) {
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		നിർത്തുക;
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		നിർത്തുക;
	അവസ്ഥ RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, trig->tdata1);
}

സ്ഥിതം ശൂന്യം dbtr_trigger_clear(ഘടന sbi_dbtr_trigger *trig)
{
	എങ്കിൽ (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		തിരികെ;

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dbtr_trigger_supported(unsigned long type)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL:
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL6:
	അവസ്ഥ RISCV_DBTR_TRIG_ICOUNT:
		തിരികെ 1;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL:
		എങ്കിൽ (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			തിരികെ 1;
		നിർത്തുക;
	അവസ്ഥ RISCV_DBTR_TRIG_MCONTROL6:
		എങ്കിൽ (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			തിരികെ 1;
		നിർത്തുക;
	അവസ്ഥ RISCV_DBTR_TRIG_ICOUNT:
		എങ്കിൽ (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			തിരികെ 1;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_അക്കം_trig(unsigned long data, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(data);
	u32 hartid = current_hartid();
	unsigned long total = 0;
	ഘടന sbi_dbtr_trigger *trig;
	പൂർണ്ണസംഖ്യ i;
	ഘടന sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hs)
		തിരികെ SBI_ERR_FAILED;

	എങ്കിൽ (data == 0) {
		*out = hs->total_trigs;
		തിരികെ SBI_SUCCESS;
	}

	വേണ്ടി (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);

		എങ്കിൽ (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, data, 0, 0))
			total++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, total);

	*out = total;
	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_എണ്ണം)
{
	ഘടന sbi_dbtr_data_സന്ദേശം *xmit;
	ഘടന sbi_dbtr_trigger *trig;
	സംഘം sbi_dbtr_shmem_entry *entry;
	ശൂന്യം *shmem_base = ശൂന്യ;
	ഘടന sbi_dbtr_hart_triggers_state *hs = ശൂന്യ;

	hs = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hs)
		തിരികെ SBI_ERR_FAILED;

	എങ്കിൽ (trig_idx_base >= hs->total_trigs ||
	    trig_idx_base + trig_എണ്ണം >= hs->total_trigs)
		തിരികെ SBI_ERR_BAD_RANGE;

	എങ്കിൽ (sbi_dbtr_shmem_disabled(hs))
		തിരികെ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_എണ്ണം * വലുപ്പം(*entry));
	for_each_trig_entry(shmem_base, trig_എണ്ണം, typeof(*entry), entry) {
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
					     trig_എണ്ണം * വലുപ്പം(*entry));

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_എണ്ണം, unsigned long *out)
{
	ശൂന്യം *shmem_base = ശൂന്യ;
	സംഘം sbi_dbtr_shmem_entry *entry;
	ഘടന sbi_dbtr_data_സന്ദേശം *recv;
	ഘടന sbi_dbtr_id_സന്ദേശം *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	പൂർണ്ണസംഖ്യ slot;
	ഘടന sbi_dbtr_trigger *trig;
	ഘടന sbi_dbtr_hart_triggers_state *hs = ശൂന്യ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hs)
		തിരികെ SBI_ERR_FAILED;

	എങ്കിൽ (sbi_dbtr_shmem_disabled(hs))
		തിരികെ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_എണ്ണം * വലുപ്പം(*entry));

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
	for_each_trig_entry(shmem_base, trig_എണ്ണം, typeof(*entry), entry) {
		recv = (ഘടന sbi_dbtr_data_സന്ദേശം *)(&entry->data);
		ctrl = recv->tdata1;

		എങ്കിൽ (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_എണ്ണം * വലുപ്പം(*entry));
			തിരികെ SBI_ERR_FAILED;
		}

		എങ്കിൽ (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_എണ്ണം * വലുപ്പം(*entry));
			തിരികെ SBI_ERR_INVALID_PARAM;
		}

		എങ്കിൽ ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_എണ്ണം * വലുപ്പം(*entry));
			തിരികെ SBI_ERR_NOT_SUPPORTED;
		}

		എങ്കിൽ (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_എണ്ണം * വലുപ്പം(*entry));
			തിരികെ SBI_ERR_NOT_SUPPORTED;
		}
	}

	എങ്കിൽ (hs->available_trigs < trig_എണ്ണം) {
		*out = hs->available_trigs;
		sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
						     trig_എണ്ണം * വലുപ്പം(*entry));
		തിരികെ SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_എണ്ണം, typeof(*entry), entry) {
		recv = (ഘടന sbi_dbtr_data_സന്ദേശം *)(&entry->data);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		എങ്കിൽ (slot < 0) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_എണ്ണം * വലുപ്പം(*entry));
			തിരികെ SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_എണ്ണം, typeof(*entry), entry) {
		recv = (ഘടന sbi_dbtr_data_സന്ദേശം *)(&entry->data);
		xmit = (ഘടന sbi_dbtr_id_സന്ദേശം *)(&entry->id);

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
					     trig_എണ്ണം * വലുപ്പം(*entry));

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ഘടന sbi_dbtr_trigger *trig;
	ഘടന sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hs)
		തിരികെ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		എങ്കിൽ (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			തിരികെ SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ഘടന sbi_dbtr_trigger *trig;
	ഘടന sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hs)
		തിരികെ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_എണ്ണം)
{
	unsigned long trig_idx;
	ഘടന sbi_dbtr_trigger *trig;
	സംഘം sbi_dbtr_shmem_entry *entry;
	ശൂന്യം *shmem_base = ശൂന്യ;
	ഘടന sbi_dbtr_hart_triggers_state *hs = ശൂന്യ;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hs)
		തിരികെ SBI_ERR_FAILED;

	എങ്കിൽ (sbi_dbtr_shmem_disabled(hs))
		തിരികെ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	എങ്കിൽ (trig_എണ്ണം >= hs->total_trigs)
		തിരികെ SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_എണ്ണം, typeof(*entry), entry) {
		sbi_hart_protection_temp_map_range((unsigned long)entry, വലുപ്പം(*entry));
		trig_idx = entry->id.idx;

		എങ്കിൽ (trig_idx >= hs->total_trigs) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, വലുപ്പം(*entry));
			തിരികെ SBI_ERR_INVALID_PARAM;
		}

		trig = INDEX_TO_TRIGGER(trig_idx);

		എങ്കിൽ (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, വലുപ്പം(*entry));
			തിരികെ SBI_ERR_FAILED;
		}

		എങ്കിൽ ((entry->data.tdata2 && !tdata2_impl) ||
		    (entry->data.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, വലുപ്പം(*entry));
			തിരികെ SBI_ERR_NOT_SUPPORTED;
		}

		എങ്കിൽ (!dbtr_trigger_hw_supported(trig->index,
					       lle_to_cpu(entry->data.tdata1),
					       lle_to_cpu(entry->data.tdata2),
					       lle_to_cpu(entry->data.tdata3))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, വലുപ്പം(*entry));
			തിരികെ SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->data);
		sbi_hart_protection_temp_unmap_range((unsigned long)entry, വലുപ്പം(*entry));
		dbtr_trigger_enable(trig);
	}

	തിരികെ SBI_SUCCESS;
}

പൂർണ്ണസംഖ്യ sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ഘടന sbi_dbtr_trigger *trig;
	ഘടന sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	എങ്കിൽ (!hs)
		തിരികെ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	തിരികെ SBI_SUCCESS;
}
