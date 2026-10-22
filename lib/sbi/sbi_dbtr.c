/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_csr_detect.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_byteorder.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_dbtr.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hart_protection.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
स्थिर unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
स्थिर ध्रुव संरचना sbi_dbtr_device *dbtr_dev = रिक्त;

ध्रुव संरचना sbi_dbtr_device *sbi_dbtr_get_device(शून्य)
{
	प्रत्यागम dbtr_dev;
}

शून्य sbi_dbtr_set_device(ध्रुव संरचना sbi_dbtr_device *dev)
{
	यदि (!dev || dbtr_dev)
		प्रत्यागम;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), शून्य *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		संरचना sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		संरचना sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		संरचना sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define INDEX_TO_TRIGGER(_index)					\
	({								\
		संरचना sbi_dbtr_trigger *__trg = रिक्त;			\
		संरचना sbi_dbtr_hart_triggers_state *__hs = रिक्त;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[_index];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	प्रति (पूर्णाङ्क _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
स्थिर अन्तःस्थ bool sbi_dbtr_shmem_disabled(
	संरचना sbi_dbtr_hart_triggers_state *hs)
{
	प्रत्यागम (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
स्थिर अन्तःस्थ शून्य sbi_dbtr_disable_shmem(
	संरचना sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
स्थिर अन्तःस्थ शून्य *hart_shmem_base(
	संरचना sbi_dbtr_hart_triggers_state *hs)
{
	प्रत्यागम ((शून्य *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

स्थिर शून्य sbi_trigger_init(संरचना sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->index = idx;
}

स्थिर bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	यदि (dbtr_dev && dbtr_dev->trigger_supported)
		प्रत्यागम dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	प्रत्यागम true;
}

स्थिर bool dbtr_trigger_any_hw_supported(
			संरचना sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	संरचना sbi_dbtr_trigger *trig;
	पूर्णाङ्क i;

	प्रति (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		यदि (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, tdata1, tdata2,
					      tdata3))
			प्रत्यागम true;
	}

	प्रत्यागम false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
स्थिर पूर्णाङ्क dbtr_find_free_slot(संरचना sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	संरचना sbi_dbtr_trigger *trig;
	पूर्णाङ्क i;

	प्रति (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		यदि (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			अनुवर्त;
		यदि (claimed_mask & BIT(i))
			अनुवर्त;
		यदि (!__test_bit(type, &trig->type_mask))
			अनुवर्त;
		यदि (!dbtr_trigger_hw_supported(trig->index, tdata1,
					       tdata2, tdata3))
			अनुवर्त;
		प्रत्यागम i;
	}

	प्रत्यागम SBI_ENOENT;
}

स्थिर अन्तःस्थ संरचना sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	पूर्णाङ्क i;
	संरचना sbi_dbtr_trigger *f_trig;
	संरचना sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	यदि (!hart_state)
		प्रत्यागम रिक्त;

	यदि (hart_state->available_trigs <= 0)
		प्रत्यागम रिक्त;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	यदि (i < 0)
		प्रत्यागम रिक्त;

	f_trig = INDEX_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	प्रत्यागम f_trig;
}

स्थिर अन्तःस्थ शून्य sbi_free_trigger(संरचना sbi_dbtr_trigger *trig)
{
	संरचना sbi_dbtr_hart_triggers_state *hart_state;

	यदि (trig == रिक्त)
		प्रत्यागम;

	hart_state = dbtr_thishart_state_ptr();
	यदि (!hart_state)
		प्रत्यागम;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

पूर्णाङ्क sbi_dbtr_init(संरचना sbi_scratch *scratch, bool coldboot)
{
	संरचना sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	पूर्णाङ्क i;
	संरचना sbi_dbtr_hart_triggers_state *hart_state = रिक्त;

	यदि (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		प्रत्यागम SBI_SUCCESS;

	यदि (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		यदि (!hart_state_ptr_offset)
			प्रत्यागम SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	यदि (!hart_state) {
		hart_state = sbi_zalloc(परिमाण(*hart_state));
		यदि (!hart_state)
			प्रत्यागम SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	यदि (hart_state->probed)
		गच्छ _probed;

	प्रति (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		यदि (trap.cause)
			भङ्ग;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		यदि (trap.cause)
			भङ्ग;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		यदि (val != i)
			भङ्ग;

		val = csr_read_allowed(CSR_TINFO, &trap);
		यदि (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			यदि (trap.cause)
				भङ्ग;

			यदि (TDATA1_GET_TYPE(tdata1) == 0)
				भङ्ग;

			sbi_trigger_init(INDEX_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->total_trigs++;
		} अन्यथा {
			यदि (val == 1)
				भङ्ग;

			sbi_trigger_init(INDEX_TO_TRIGGER(i), val, i);
			hart_state->total_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->total_trigs;

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_get_total_triggers(शून्य)
{
	संरचना sbi_dbtr_hart_triggers_state *hs;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	यदि (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		प्रत्यागम 0;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम 0;

	प्रत्यागम hs->total_trigs;
}

पूर्णाङ्क sbi_dbtr_setup_shmem(ध्रुव संरचना sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	संरचना sbi_dbtr_hart_triggers_state *hart_state;

	यदि (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		प्रत्यागम SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	यदि (!hart_state)
		प्रत्यागम SBI_ERR_FAILED;

	/* call is to disable shared memory */
	यदि (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		प्रत्यागम SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	यदि (!sbi_dbtr_shmem_disabled(hart_state))
		प्रत्यागम SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	यदि (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		प्रत्यागम SBI_ERR_INVALID_PARAM;

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
	यदि (shmem_phys_hi)
		प्रत्यागम SBI_EINVALID_ADDR;

	यदि (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		प्रत्यागम SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	प्रत्यागम SBI_SUCCESS;
}

स्थिर शून्य dbtr_trigger_setup(संरचना sbi_dbtr_trigger *trig,
			       संरचना sbi_dbtr_data_संदेश *recv)
{
	unsigned long tdata1;

	यदि (!trig)
		प्रत्यागम;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_INDEX(trig->state, trig->index);

	विकल्प (TDATA1_GET_TYPE(tdata1)) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
		यदि (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
		यदि (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		यदि (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		यदि (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		भङ्ग;
	स्वभाव:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		भङ्ग;
	}
}

स्थिर अन्तःस्थ शून्य update_bit(unsigned long new, पूर्णाङ्क nr, चञ्चल unsigned long *addr)
{
	यदि (new)
		__set_bit(nr, addr);
	अन्यथा
		__clear_bit(nr, addr);
}

स्थिर शून्य dbtr_trigger_enable(संरचना sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	यदि (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		प्रत्यागम;

	state = trig->state;
	tdata1 = trig->tdata1;

	विकल्प (TDATA1_GET_TYPE(tdata1)) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		भङ्ग;
	स्वभाव:
		भङ्ग;
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

स्थिर शून्य dbtr_trigger_disable(संरचना sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	यदि (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		प्रत्यागम;

	tdata1 = trig->tdata1;

	विकल्प (TDATA1_GET_TYPE(tdata1)) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, trig->tdata1);
}

स्थिर शून्य dbtr_trigger_clear(संरचना sbi_dbtr_trigger *trig)
{
	यदि (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		प्रत्यागम;

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

स्थिर पूर्णाङ्क dbtr_trigger_supported(unsigned long type)
{
	विकल्प (type) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		प्रत्यागम 1;
	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	विकल्प (type) {
	पक्ष RISCV_DBTR_TRIG_MCONTROL:
		यदि (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			प्रत्यागम 1;
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_MCONTROL6:
		यदि (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			प्रत्यागम 1;
		भङ्ग;
	पक्ष RISCV_DBTR_TRIG_ICOUNT:
		यदि (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			प्रत्यागम 1;
		भङ्ग;
	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_dbtr_अंक_trig(unsigned long data, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(data);
	u32 hartid = current_hartid();
	unsigned long total = 0;
	संरचना sbi_dbtr_trigger *trig;
	पूर्णाङ्क i;
	संरचना sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (data == 0) {
		*out = hs->total_trigs;
		प्रत्यागम SBI_SUCCESS;
	}

	प्रति (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);

		यदि (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, data, 0, 0))
			total++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, total);

	*out = total;
	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_गिनती)
{
	संरचना sbi_dbtr_data_संदेश *xmit;
	संरचना sbi_dbtr_trigger *trig;
	सङ्घ sbi_dbtr_shmem_entry *entry;
	शून्य *shmem_base = रिक्त;
	संरचना sbi_dbtr_hart_triggers_state *hs = रिक्त;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (trig_idx_base >= hs->total_trigs ||
	    trig_idx_base + trig_गिनती >= hs->total_trigs)
		प्रत्यागम SBI_ERR_BAD_RANGE;

	यदि (sbi_dbtr_shmem_disabled(hs))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_गिनती * परिमाण(*entry));
	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
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
					     trig_गिनती * परिमाण(*entry));

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_गिनती, unsigned long *out)
{
	शून्य *shmem_base = रिक्त;
	सङ्घ sbi_dbtr_shmem_entry *entry;
	संरचना sbi_dbtr_data_संदेश *recv;
	संरचना sbi_dbtr_id_संदेश *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	पूर्णाङ्क slot;
	संरचना sbi_dbtr_trigger *trig;
	संरचना sbi_dbtr_hart_triggers_state *hs = रिक्त;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (sbi_dbtr_shmem_disabled(hs))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_गिनती * परिमाण(*entry));

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
	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		recv = (संरचना sbi_dbtr_data_संदेश *)(&entry->data);
		ctrl = recv->tdata1;

		यदि (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_गिनती * परिमाण(*entry));
			प्रत्यागम SBI_ERR_FAILED;
		}

		यदि (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_गिनती * परिमाण(*entry));
			प्रत्यागम SBI_ERR_INVALID_PARAM;
		}

		यदि ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_गिनती * परिमाण(*entry));
			प्रत्यागम SBI_ERR_NOT_SUPPORTED;
		}

		यदि (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_गिनती * परिमाण(*entry));
			प्रत्यागम SBI_ERR_NOT_SUPPORTED;
		}
	}

	यदि (hs->available_trigs < trig_गिनती) {
		*out = hs->available_trigs;
		sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
						     trig_गिनती * परिमाण(*entry));
		प्रत्यागम SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		recv = (संरचना sbi_dbtr_data_संदेश *)(&entry->data);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		यदि (slot < 0) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_गिनती * परिमाण(*entry));
			प्रत्यागम SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		recv = (संरचना sbi_dbtr_data_संदेश *)(&entry->data);
		xmit = (संरचना sbi_dbtr_id_संदेश *)(&entry->id);

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
					     trig_गिनती * परिमाण(*entry));

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	संरचना sbi_dbtr_trigger *trig;
	संरचना sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		यदि (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			प्रत्यागम SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	संरचना sbi_dbtr_trigger *trig;
	संरचना sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_गिनती)
{
	unsigned long trig_idx;
	संरचना sbi_dbtr_trigger *trig;
	सङ्घ sbi_dbtr_shmem_entry *entry;
	शून्य *shmem_base = रिक्त;
	संरचना sbi_dbtr_hart_triggers_state *hs = रिक्त;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	यदि (sbi_dbtr_shmem_disabled(hs))
		प्रत्यागम SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	यदि (trig_गिनती >= hs->total_trigs)
		प्रत्यागम SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_गिनती, typeof(*entry), entry) {
		sbi_hart_protection_temp_map_range((unsigned long)entry, परिमाण(*entry));
		trig_idx = entry->id.idx;

		यदि (trig_idx >= hs->total_trigs) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, परिमाण(*entry));
			प्रत्यागम SBI_ERR_INVALID_PARAM;
		}

		trig = INDEX_TO_TRIGGER(trig_idx);

		यदि (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, परिमाण(*entry));
			प्रत्यागम SBI_ERR_FAILED;
		}

		यदि ((entry->data.tdata2 && !tdata2_impl) ||
		    (entry->data.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, परिमाण(*entry));
			प्रत्यागम SBI_ERR_NOT_SUPPORTED;
		}

		यदि (!dbtr_trigger_hw_supported(trig->index,
					       lle_to_cpu(entry->data.tdata1),
					       lle_to_cpu(entry->data.tdata2),
					       lle_to_cpu(entry->data.tdata3))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, परिमाण(*entry));
			प्रत्यागम SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->data);
		sbi_hart_protection_temp_unmap_range((unsigned long)entry, परिमाण(*entry));
		dbtr_trigger_enable(trig);
	}

	प्रत्यागम SBI_SUCCESS;
}

पूर्णाङ्क sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	संरचना sbi_dbtr_trigger *trig;
	संरचना sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	यदि (!hs)
		प्रत्यागम SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	प्रत्यागम SBI_SUCCESS;
}
