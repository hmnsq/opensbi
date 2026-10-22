/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_csr_detect.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_byteorder.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_dbtr.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hart_protection.h>
#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
স্থির unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
স্থির ধ্রুবক কাঠামো sbi_dbtr_device *dbtr_dev = ফাঁকা;

ধ্রুবক কাঠামো sbi_dbtr_device *sbi_dbtr_get_device(শূন্য)
{
	ফেরত dbtr_dev;
}

শূন্য sbi_dbtr_set_device(ধ্রুবক কাঠামো sbi_dbtr_device *dev)
{
	যদি (!dev || dbtr_dev)
		ফেরত;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), শূন্য *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		কাঠামো sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		কাঠামো sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		কাঠামো sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define সূচক_TO_TRIGGER(সূচক)					\
	({								\
		কাঠামো sbi_dbtr_trigger *__trg = ফাঁকা;			\
		কাঠামো sbi_dbtr_hart_triggers_state *__hs = ফাঁকা;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[সূচক];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	জন্য (পূর্ণসংখ্যা _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
স্থির অন্তঃরেখ bool sbi_dbtr_shmem_disabled(
	কাঠামো sbi_dbtr_hart_triggers_state *hs)
{
	ফেরত (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
স্থির অন্তঃরেখ শূন্য sbi_dbtr_disable_shmem(
	কাঠামো sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
স্থির অন্তঃরেখ শূন্য *hart_shmem_base(
	কাঠামো sbi_dbtr_hart_triggers_state *hs)
{
	ফেরত ((শূন্য *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

স্থির শূন্য sbi_trigger_init(কাঠামো sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->সূচক_2 = idx;
}

স্থির bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	যদি (dbtr_dev && dbtr_dev->trigger_supported)
		ফেরত dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	ফেরত true;
}

স্থির bool dbtr_trigger_any_hw_supported(
			কাঠামো sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	কাঠামো sbi_dbtr_trigger *trig;
	পূর্ণসংখ্যা i;

	জন্য (i = 0; i < hs->যোগ_trigs; i++) {
		trig = সূচক_TO_TRIGGER(i);
		যদি (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->সূচক_2, tdata1, tdata2,
					      tdata3))
			ফেরত true;
	}

	ফেরত false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
স্থির পূর্ণসংখ্যা dbtr_find_free_slot(কাঠামো sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	কাঠামো sbi_dbtr_trigger *trig;
	পূর্ণসংখ্যা i;

	জন্য (i = 0; i < hs->যোগ_trigs; i++) {
		trig = সূচক_TO_TRIGGER(i);
		যদি (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			চালু;
		যদি (claimed_mask & BIT(i))
			চালু;
		যদি (!__test_bit(type, &trig->type_mask))
			চালু;
		যদি (!dbtr_trigger_hw_supported(trig->সূচক_2, tdata1,
					       tdata2, tdata3))
			চালু;
		ফেরত i;
	}

	ফেরত SBI_ENOENT;
}

স্থির অন্তঃরেখ কাঠামো sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	পূর্ণসংখ্যা i;
	কাঠামো sbi_dbtr_trigger *f_trig;
	কাঠামো sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	যদি (!hart_state)
		ফেরত ফাঁকা;

	যদি (hart_state->available_trigs <= 0)
		ফেরত ফাঁকা;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	যদি (i < 0)
		ফেরত ফাঁকা;

	f_trig = সূচক_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	ফেরত f_trig;
}

স্থির অন্তঃরেখ শূন্য sbi_free_trigger(কাঠামো sbi_dbtr_trigger *trig)
{
	কাঠামো sbi_dbtr_hart_triggers_state *hart_state;

	যদি (trig == ফাঁকা)
		ফেরত;

	hart_state = dbtr_thishart_state_ptr();
	যদি (!hart_state)
		ফেরত;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

পূর্ণসংখ্যা sbi_dbtr_init(কাঠামো sbi_scratch *scratch, bool coldboot)
{
	কাঠামো sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	পূর্ণসংখ্যা i;
	কাঠামো sbi_dbtr_hart_triggers_state *hart_state = ফাঁকা;

	যদি (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ফেরত SBI_SUCCESS;

	যদি (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!hart_state_ptr_offset)
			ফেরত SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	যদি (!hart_state) {
		hart_state = sbi_zalloc(আকার(*hart_state));
		যদি (!hart_state)
			ফেরত SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	যদি (hart_state->probed)
		যাও _probed;

	জন্য (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		যদি (trap.cause)
			বিরতি;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		যদি (trap.cause)
			বিরতি;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		যদি (val != i)
			বিরতি;

		val = csr_read_allowed(CSR_TINFO, &trap);
		যদি (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			যদি (trap.cause)
				বিরতি;

			যদি (TDATA1_GET_TYPE(tdata1) == 0)
				বিরতি;

			sbi_trigger_init(সূচক_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->যোগ_trigs++;
		} অন্যথা {
			যদি (val == 1)
				বিরতি;

			sbi_trigger_init(সূচক_TO_TRIGGER(i), val, i);
			hart_state->যোগ_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->যোগ_trigs;

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_dbtr_get_যোগ_triggers(শূন্য)
{
	কাঠামো sbi_dbtr_hart_triggers_state *hs;
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	যদি (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ফেরত 0;

	hs = dbtr_thishart_state_ptr();
	যদি (!hs)
		ফেরত 0;

	ফেরত hs->যোগ_trigs;
}

পূর্ণসংখ্যা sbi_dbtr_setup_shmem(ধ্রুবক কাঠামো sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	কাঠামো sbi_dbtr_hart_triggers_state *hart_state;

	যদি (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		ফেরত SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	যদি (!hart_state)
		ফেরত SBI_ERR_FAILED;

	/* call is to disable shared memory */
	যদি (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		ফেরত SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	যদি (!sbi_dbtr_shmem_disabled(hart_state))
		ফেরত SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	যদি (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		ফেরত SBI_ERR_INVALID_PARAM;

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
	যদি (shmem_phys_hi)
		ফেরত SBI_EINVALID_ADDR;

	যদি (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ফেরত SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	ফেরত SBI_SUCCESS;
}

স্থির শূন্য dbtr_trigger_setup(কাঠামো sbi_dbtr_trigger *trig,
			       কাঠামো sbi_dbtr_তথ্য_বার্তা *recv)
{
	unsigned long tdata1;

	যদি (!trig)
		ফেরত;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_সূচক(trig->state, trig->সূচক_2);

	নির্বাচন (TDATA1_GET_TYPE(tdata1)) {
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL:
		যদি (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		যদি (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		বিরতি;
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL6:
		যদি (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		যদি (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		যদি (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		যদি (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		বিরতি;
	ক্ষেত্র RISCV_DBTR_TRIG_ICOUNT:
		যদি (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		যদি (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		যদি (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		যদি (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		বিরতি;
	পূর্বনির্ধারিত:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		বিরতি;
	}
}

স্থির অন্তঃরেখ শূন্য update_bit(unsigned long new, পূর্ণসংখ্যা nr, অস্থির unsigned long *addr)
{
	যদি (new)
		__set_bit(nr, addr);
	অন্যথা
		__clear_bit(nr, addr);
}

স্থির শূন্য dbtr_trigger_enable(কাঠামো sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	যদি (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ফেরত;

	state = trig->state;
	tdata1 = trig->tdata1;

	নির্বাচন (TDATA1_GET_TYPE(tdata1)) {
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		বিরতি;
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		বিরতি;
	ক্ষেত্র RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		বিরতি;
	পূর্বনির্ধারিত:
		বিরতি;
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
	csr_write(CSR_TSELECT, trig->সূচক_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, trig->tdata2);
	tdata_write_safe(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

স্থির শূন্য dbtr_trigger_disable(কাঠামো sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	যদি (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ফেরত;

	tdata1 = trig->tdata1;

	নির্বাচন (TDATA1_GET_TYPE(tdata1)) {
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		বিরতি;
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		বিরতি;
	ক্ষেত্র RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		বিরতি;
	পূর্বনির্ধারিত:
		বিরতি;
	}

	csr_write(CSR_TSELECT, trig->সূচক_2);
	csr_write(CSR_TDATA1, trig->tdata1);
}

স্থির শূন্য dbtr_trigger_clear(কাঠামো sbi_dbtr_trigger *trig)
{
	যদি (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ফেরত;

	csr_write(CSR_TSELECT, trig->সূচক_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

স্থির পূর্ণসংখ্যা dbtr_trigger_supported(unsigned long type)
{
	নির্বাচন (type) {
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL:
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL6:
	ক্ষেত্র RISCV_DBTR_TRIG_ICOUNT:
		ফেরত 1;
	পূর্বনির্ধারিত:
		বিরতি;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	নির্বাচন (type) {
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL:
		যদি (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			ফেরত 1;
		বিরতি;
	ক্ষেত্র RISCV_DBTR_TRIG_MCONTROL6:
		যদি (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			ফেরত 1;
		বিরতি;
	ক্ষেত্র RISCV_DBTR_TRIG_ICOUNT:
		যদি (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			ফেরত 1;
		বিরতি;
	পূর্বনির্ধারিত:
		বিরতি;
	}

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_dbtr_অঙ্ক_trig(unsigned long তথ্য, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(তথ্য);
	u32 hartid = current_hartid();
	unsigned long যোগ = 0;
	কাঠামো sbi_dbtr_trigger *trig;
	পূর্ণসংখ্যা i;
	কাঠামো sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	যদি (!hs)
		ফেরত SBI_ERR_FAILED;

	যদি (তথ্য == 0) {
		*out = hs->যোগ_trigs;
		ফেরত SBI_SUCCESS;
	}

	জন্য (i = 0; i < hs->যোগ_trigs; i++) {
		trig = সূচক_TO_TRIGGER(i);

		যদি (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->সূচক_2, তথ্য, 0, 0))
			যোগ++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, যোগ);

	*out = যোগ;
	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_গুনতি)
{
	কাঠামো sbi_dbtr_তথ্য_বার্তা *xmit;
	কাঠামো sbi_dbtr_trigger *trig;
	সংঘ sbi_dbtr_shmem_entry *entry;
	শূন্য *shmem_base = ফাঁকা;
	কাঠামো sbi_dbtr_hart_triggers_state *hs = ফাঁকা;

	hs = dbtr_thishart_state_ptr();
	যদি (!hs)
		ফেরত SBI_ERR_FAILED;

	যদি (trig_idx_base >= hs->যোগ_trigs ||
	    trig_idx_base + trig_গুনতি >= hs->যোগ_trigs)
		ফেরত SBI_ERR_BAD_RANGE;

	যদি (sbi_dbtr_shmem_disabled(hs))
		ফেরত SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_অস্থায়ী_map_range((unsigned long)shmem_base,
					   trig_গুনতি * আকার(*entry));
	for_each_trig_entry(shmem_base, trig_গুনতি, typeof(*entry), entry) {
		xmit = &entry->তথ্য;
		trig = সূচক_TO_TRIGGER((_idx + trig_idx_base));
		csr_write(CSR_TSELECT, trig->সূচক_2);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_safe(CSR_TDATA2);
		trig->tdata3 = tdata_read_safe(CSR_TDATA3);
		xmit->tstate = cpu_to_lle(trig->state);
		xmit->tdata1 = cpu_to_lle(trig->tdata1);
		xmit->tdata2 = cpu_to_lle(trig->tdata2);
		xmit->tdata3 = cpu_to_lle(trig->tdata3);
	}
	sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base,
					     trig_গুনতি * আকার(*entry));

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_গুনতি, unsigned long *out)
{
	শূন্য *shmem_base = ফাঁকা;
	সংঘ sbi_dbtr_shmem_entry *entry;
	কাঠামো sbi_dbtr_তথ্য_বার্তা *recv;
	কাঠামো sbi_dbtr_id_বার্তা *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	পূর্ণসংখ্যা slot;
	কাঠামো sbi_dbtr_trigger *trig;
	কাঠামো sbi_dbtr_hart_triggers_state *hs = ফাঁকা;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	যদি (!hs)
		ফেরত SBI_ERR_FAILED;

	যদি (sbi_dbtr_shmem_disabled(hs))
		ফেরত SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_অস্থায়ী_map_range((unsigned long)shmem_base,
					   trig_গুনতি * আকার(*entry));

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
	for_each_trig_entry(shmem_base, trig_গুনতি, typeof(*entry), entry) {
		recv = (কাঠামো sbi_dbtr_তথ্য_বার্তা *)(&entry->তথ্য);
		ctrl = recv->tdata1;

		যদি (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base,
							     trig_গুনতি * আকার(*entry));
			ফেরত SBI_ERR_FAILED;
		}

		যদি (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base,
							     trig_গুনতি * আকার(*entry));
			ফেরত SBI_ERR_INVALID_PARAM;
		}

		যদি ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base,
							     trig_গুনতি * আকার(*entry));
			ফেরত SBI_ERR_NOT_SUPPORTED;
		}

		যদি (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base,
							     trig_গুনতি * আকার(*entry));
			ফেরত SBI_ERR_NOT_SUPPORTED;
		}
	}

	যদি (hs->available_trigs < trig_গুনতি) {
		*out = hs->available_trigs;
		sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base,
						     trig_গুনতি * আকার(*entry));
		ফেরত SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_গুনতি, typeof(*entry), entry) {
		recv = (কাঠামো sbi_dbtr_তথ্য_বার্তা *)(&entry->তথ্য);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		যদি (slot < 0) {
			*out = _idx;
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base,
							     trig_গুনতি * আকার(*entry));
			ফেরত SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_গুনতি, typeof(*entry), entry) {
		recv = (কাঠামো sbi_dbtr_তথ্য_বার্তা *)(&entry->তথ্য);
		xmit = (কাঠামো sbi_dbtr_id_বার্তা *)(&entry->id);

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
		xmit->idx = cpu_to_lle(trig->সূচক_2);

	}

	sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)shmem_base,
					     trig_গুনতি * আকার(*entry));

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	কাঠামো sbi_dbtr_trigger *trig;
	কাঠামো sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	যদি (!hs)
		ফেরত SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->যোগ_trigs) {
		trig = সূচক_TO_TRIGGER(idx);
		যদি (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			ফেরত SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	কাঠামো sbi_dbtr_trigger *trig;
	কাঠামো sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	যদি (!hs)
		ফেরত SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->যোগ_trigs) {
		trig = সূচক_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_গুনতি)
{
	unsigned long trig_idx;
	কাঠামো sbi_dbtr_trigger *trig;
	সংঘ sbi_dbtr_shmem_entry *entry;
	শূন্য *shmem_base = ফাঁকা;
	কাঠামো sbi_dbtr_hart_triggers_state *hs = ফাঁকা;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	যদি (!hs)
		ফেরত SBI_ERR_FAILED;

	যদি (sbi_dbtr_shmem_disabled(hs))
		ফেরত SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	যদি (trig_গুনতি >= hs->যোগ_trigs)
		ফেরত SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_গুনতি, typeof(*entry), entry) {
		sbi_hart_protection_অস্থায়ী_map_range((unsigned long)entry, আকার(*entry));
		trig_idx = entry->id.idx;

		যদি (trig_idx >= hs->যোগ_trigs) {
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)entry, আকার(*entry));
			ফেরত SBI_ERR_INVALID_PARAM;
		}

		trig = সূচক_TO_TRIGGER(trig_idx);

		যদি (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)entry, আকার(*entry));
			ফেরত SBI_ERR_FAILED;
		}

		যদি ((entry->তথ্য.tdata2 && !tdata2_impl) ||
		    (entry->তথ্য.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)entry, আকার(*entry));
			ফেরত SBI_ERR_NOT_SUPPORTED;
		}

		যদি (!dbtr_trigger_hw_supported(trig->সূচক_2,
					       lle_to_cpu(entry->তথ্য.tdata1),
					       lle_to_cpu(entry->তথ্য.tdata2),
					       lle_to_cpu(entry->তথ্য.tdata3))) {
			sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)entry, আকার(*entry));
			ফেরত SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->তথ্য);
		sbi_hart_protection_অস্থায়ী_unmap_range((unsigned long)entry, আকার(*entry));
		dbtr_trigger_enable(trig);
	}

	ফেরত SBI_SUCCESS;
}

পূর্ণসংখ্যা sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	কাঠামো sbi_dbtr_trigger *trig;
	কাঠামো sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	যদি (!hs)
		ফেরত SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->যোগ_trigs) {
		trig = সূচক_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	ফেরত SBI_SUCCESS;
}
