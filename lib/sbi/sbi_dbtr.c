/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_csr_detect.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_byteorder.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_dbtr.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart_protection.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
ସ୍ଥିର unsigned long hart_state_ptr_offset;

/** Device specific debug trigger operations */
ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_dbtr_device *dbtr_dev = ଖାଲି;

ଅଚଳ ସଂରଚନା sbi_dbtr_device *sbi_dbtr_get_device(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ dbtr_dev;
}

ଶୂନ୍ୟ sbi_dbtr_set_device(ଅଚଳ ସଂରଚନା sbi_dbtr_device *dev)
{
	ଯଦି (!dev || dbtr_dev)
		ଫେରନ୍ତୁ;

	dbtr_dev = dev;
}

#define dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ଶୂନ୍ୟ *, hart_state_ptr_offset)

#define dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define dbtr_set_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), ଶୂନ୍ୟ *, hart_state_ptr_offset, \
			       (__hart_state))

#define tdata_read_safe(__csr)						\
	({								\
		ସଂରଚନା sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#define tdata_write_safe(__csr, __value)				\
	({								\
		ସଂରଚନା sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#define tdata_implemented(__csr)					\
	({								\
		ସଂରଚନା sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#define INDEX_TO_TRIGGER(_index)					\
	({								\
		ସଂରଚନା sbi_dbtr_trigger *__trg = ଖାଲି;			\
		ସଂରଚନା sbi_dbtr_hart_triggers_state *__hs = ଖାଲି;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[_index];			\
		(__trg);						\
	})

#define for_each_trig_entry(_base, _max, _etype, _entry)		\
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା _idx = 0; _entry = ((_etype *)_base + _idx),		\
	     _idx < _max;						\
	     _idx++, _entry = ((_etype *)_base + _idx))

#define DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool sbi_dbtr_shmem_disabled(
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs)
{
	ଫେରନ୍ତୁ (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ sbi_dbtr_disable_shmem(
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *hart_shmem_base(
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs)
{
	ଫେରନ୍ତୁ ((ଶୂନ୍ୟ *)(unsigned long)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

ସ୍ଥିର ଶୂନ୍ୟ sbi_trigger_init(ସଂରଚନା sbi_dbtr_trigger *trig,
			     unsigned long type_mask, unsigned long idx)
{
	trig->type_mask = type_mask;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->index = idx;
}

ସ୍ଥିର bool dbtr_trigger_hw_supported(unsigned long idx, unsigned long tdata1,
				      unsigned long tdata2,
				      unsigned long tdata3)
{
	ଯଦି (dbtr_dev && dbtr_dev->trigger_supported)
		ଫେରନ୍ତୁ dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool dbtr_trigger_any_hw_supported(
			ସଂରଚନା sbi_dbtr_hart_triggers_state *hs,
			unsigned long tdata1, unsigned long tdata2,
			unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		ଯଦି (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, tdata1, tdata2,
					      tdata3))
			ଫେରନ୍ତୁ true;
	}

	ଫେରନ୍ତୁ false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dbtr_find_free_slot(ସଂରଚନା sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_mask,
			       unsigned long tdata1, unsigned long tdata2,
			       unsigned long tdata3)
{
	unsigned long type = TDATA1_GET_TYPE(tdata1);
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);
		ଯଦି (trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))
			ଜାରି;
		ଯଦି (claimed_mask & BIT(i))
			ଜାରି;
		ଯଦି (!__test_bit(type, &trig->type_mask))
			ଜାରି;
		ଯଦି (!dbtr_trigger_hw_supported(trig->index, tdata1,
					       tdata2, tdata3))
			ଜାରି;
		ଫେରନ୍ତୁ i;
	}

	ଫେରନ୍ତୁ SBI_ENOENT;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ସଂରଚନା sbi_dbtr_trigger *sbi_alloc_trigger(unsigned long tdata1,
							 unsigned long tdata2,
							 unsigned long tdata3)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା sbi_dbtr_trigger *f_trig;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	ଯଦି (!hart_state)
		ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (hart_state->available_trigs <= 0)
		ଫେରନ୍ତୁ ଖାଲି;

	i = dbtr_find_free_slot(hart_state, 0, tdata1, tdata2, tdata3);
	ଯଦି (i < 0)
		ଫେରନ୍ତୁ ଖାଲି;

	f_trig = INDEX_TO_TRIGGER(i);
	hart_state->available_trigs--;
	__set_bit(RV_DBTR_BIT(TS, MAPPED), &f_trig->state);

	ଫେରନ୍ତୁ f_trig;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ sbi_free_trigger(ସଂରଚନା sbi_dbtr_trigger *trig)
{
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hart_state;

	ଯଦି (trig == ଖାଲି)
		ଫେରନ୍ତୁ;

	hart_state = dbtr_thishart_state_ptr();
	ଯଦି (!hart_state)
		ଫେରନ୍ତୁ;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->available_trigs++;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_init(ସଂରଚନା sbi_scratch *scratch, bool coldboot)
{
	ସଂରଚନା sbi_trap_info trap = {0};
	unsigned long tdata1;
	unsigned long val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hart_state = ଖାଲି;

	ଯଦି (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ଫେରନ୍ତୁ SBI_SUCCESS;

	ଯଦି (coldboot) {
		hart_state_ptr_offset = sbi_scratch_alloc_type_offset(ଶୂନ୍ୟ *);
		ଯଦି (!hart_state_ptr_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	ଯଦି (!hart_state) {
		hart_state = sbi_zalloc(ଆକାର(*hart_state));
		ଯଦି (!hart_state)
			ଫେରନ୍ତୁ SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_set_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	ଯଦି (hart_state->probed)
		ଯାଅ _probed;

	ପାଇଁ (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		ଯଦି (trap.cause)
			ଭାଙ୍ଗ;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		ଯଦି (trap.cause)
			ଭାଙ୍ଗ;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		ଯଦି (val != i)
			ଭାଙ୍ଗ;

		val = csr_read_allowed(CSR_TINFO, &trap);
		ଯଦି (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			ଯଦି (trap.cause)
				ଭାଙ୍ଗ;

			ଯଦି (TDATA1_GET_TYPE(tdata1) == 0)
				ଭାଙ୍ଗ;

			sbi_trigger_init(INDEX_TO_TRIGGER(i),
					 BIT(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->total_trigs++;
		} ଅନ୍ୟଥା {
			ଯଦି (val == 1)
				ଭାଙ୍ଗ;

			sbi_trigger_init(INDEX_TO_TRIGGER(i), val, i);
			hart_state->total_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->available_trigs = hart_state->total_trigs;

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_get_total_triggers(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	ଯଦି (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		ଫେରନ୍ତୁ 0;

	hs = dbtr_thishart_state_ptr();
	ଯଦି (!hs)
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ hs->total_trigs;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_setup_shmem(ଅଚଳ ସଂରଚନା sbi_domain *dom, unsigned long smode,
			 unsigned long shmem_phys_lo,
			 unsigned long shmem_phys_hi)
{
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hart_state;

	ଯଦି (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		ଫେରନ୍ତୁ SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	ଯଦି (!hart_state)
		ଫେରନ୍ତୁ SBI_ERR_FAILED;

	/* call is to disable shared memory */
	ଯଦି (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		ଫେରନ୍ତୁ SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	ଯଦି (!sbi_dbtr_shmem_disabled(hart_state))
		ଫେରନ୍ତୁ SBI_ERR_ALREADY_AVAILABLE;

	/* lower physical address must be XLEN/8 bytes aligned */
	ଯଦି (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_MASK)
		ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

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
	ଯଦି (shmem_phys_hi)
		ଫେରନ୍ତୁ SBI_EINVALID_ADDR;

	ଯଦି (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		ଫେରନ୍ତୁ SBI_ERR_INVALID_ADDRESS;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ସ୍ଥିର ଶୂନ୍ୟ dbtr_trigger_setup(ସଂରଚନା sbi_dbtr_trigger *trig,
			       ସଂରଚନା sbi_dbtr_data_ସନ୍ଦେଶ *recv)
{
	unsigned long tdata1;

	ଯଦି (!trig)
		ଫେରନ୍ତୁ;

	trig->tdata1 = lle_to_cpu(recv->tdata1);
	trig->tdata2 = lle_to_cpu(recv->tdata2);
	trig->tdata3 = lle_to_cpu(recv->tdata3);

	tdata1 = lle_to_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_BIT(TS, MAPPED), &trig->state);

	SET_TRIG_HW_INDEX(trig->state, trig->index);

	ବାଛ (TDATA1_GET_TYPE(tdata1)) {
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL:
		ଯଦି (__test_bit(RV_DBTR_BIT(MC, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		ଯଦି (__test_bit(RV_DBTR_BIT(MC, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL6:
		ଯଦି (__test_bit(RV_DBTR_BIT(MC6, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		ଯଦି (__test_bit(RV_DBTR_BIT(MC6, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		ଯଦି (__test_bit(RV_DBTR_BIT(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		ଯଦି (__test_bit(RV_DBTR_BIT(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RISCV_DBTR_TRIG_ICOUNT:
		ଯଦି (__test_bit(RV_DBTR_BIT(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, U), &trig->state);

		ଯଦି (__test_bit(RV_DBTR_BIT(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, S), &trig->state);

		ଯଦି (__test_bit(RV_DBTR_BIT(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VU), &trig->state);

		ଯଦି (__test_bit(RV_DBTR_BIT(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_BIT(TS, VS), &trig->state);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		ଭାଙ୍ଗ;
	}
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ update_bit(unsigned long new, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, ଅସ୍ଥିର unsigned long *addr)
{
	ଯଦି (new)
		__set_bit(nr, addr);
	ଅନ୍ୟଥା
		__clear_bit(nr, addr);
}

ସ୍ଥିର ଶୂନ୍ୟ dbtr_trigger_enable(ସଂରଚନା sbi_dbtr_trigger *trig)
{
	unsigned long state;
	unsigned long tdata1;

	ଯଦି (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ଫେରନ୍ତୁ;

	state = trig->state;
	tdata1 = trig->tdata1;

	ବାଛ (TDATA1_GET_TYPE(tdata1)) {
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL:
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC, S), &trig->tdata1);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL6:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(MC6, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(MC6, S), &trig->tdata1);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RISCV_DBTR_TRIG_ICOUNT:
		update_bit(state & RV_DBTR_BIT_MASK(TS, VU),
			   RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, VS),
			   RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, U),
			   RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		update_bit(state & RV_DBTR_BIT_MASK(TS, S),
			   RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
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

ସ୍ଥିର ଶୂନ୍ୟ dbtr_trigger_disable(ସଂରଚନା sbi_dbtr_trigger *trig)
{
	unsigned long tdata1;

	ଯଦି (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ଫେରନ୍ତୁ;

	tdata1 = trig->tdata1;

	ବାଛ (TDATA1_GET_TYPE(tdata1)) {
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_BIT(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC, S), &trig->tdata1);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_BIT(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(MC6, S), &trig->tdata1);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_BIT(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_BIT(ICOUNT, S), &trig->tdata1);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ସ୍ଥିର ଶୂନ୍ୟ dbtr_trigger_clear(ସଂରଚନା sbi_dbtr_trigger *trig)
{
	ଯଦି (!trig || !(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
		ଫେରନ୍ତୁ;

	csr_write(CSR_TSELECT, trig->index);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_safe(CSR_TDATA2, 0x0);
	tdata_write_safe(CSR_TDATA3, 0x0);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dbtr_trigger_supported(unsigned long type)
{
	ବାଛ (type) {
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL:
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL6:
	ସ୍ଥିତି RISCV_DBTR_TRIG_ICOUNT:
		ଫେରନ୍ତୁ 1;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dbtr_trigger_valid(unsigned long type, unsigned long tdata)
{
	ବାଛ (type) {
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL:
		ଯଦି (!(tdata & RV_DBTR_BIT_MASK(MC, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC, M)))
			ଫେରନ୍ତୁ 1;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RISCV_DBTR_TRIG_MCONTROL6:
		ଯଦି (!(tdata & RV_DBTR_BIT_MASK(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(MC6, M)))
			ଫେରନ୍ତୁ 1;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RISCV_DBTR_TRIG_ICOUNT:
		ଯଦି (!(tdata & RV_DBTR_BIT_MASK(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_BIT_MASK(ICOUNT, M)))
			ଫେରନ୍ତୁ 1;
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_ଅଙ୍କ_trig(unsigned long data, unsigned long *out)
{
	unsigned long type = TDATA1_GET_TYPE(data);
	u32 hartid = current_hartid();
	unsigned long total = 0;
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ଯଦି (!hs)
		ଫେରନ୍ତୁ SBI_ERR_FAILED;

	ଯଦି (data == 0) {
		*out = hs->total_trigs;
		ଫେରନ୍ତୁ SBI_SUCCESS;
	}

	ପାଇଁ (i = 0; i < hs->total_trigs; i++) {
		trig = INDEX_TO_TRIGGER(i);

		ଯଦି (__test_bit(type, &trig->type_mask) &&
		    dbtr_trigger_hw_supported(trig->index, data, 0, 0))
			total++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, total);

	*out = total;
	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_read_trig(unsigned long smode,
		       unsigned long trig_idx_base, unsigned long trig_ଗଣନା)
{
	ସଂରଚନା sbi_dbtr_data_ସନ୍ଦେଶ *xmit;
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ସଂଘ sbi_dbtr_shmem_entry *entry;
	ଶୂନ୍ୟ *shmem_base = ଖାଲି;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs = ଖାଲି;

	hs = dbtr_thishart_state_ptr();
	ଯଦି (!hs)
		ଫେରନ୍ତୁ SBI_ERR_FAILED;

	ଯଦି (trig_idx_base >= hs->total_trigs ||
	    trig_idx_base + trig_ଗଣନା >= hs->total_trigs)
		ଫେରନ୍ତୁ SBI_ERR_BAD_RANGE;

	ଯଦି (sbi_dbtr_shmem_disabled(hs))
		ଫେରନ୍ତୁ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_ଗଣନା * ଆକାର(*entry));
	for_each_trig_entry(shmem_base, trig_ଗଣନା, typeof(*entry), entry) {
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
					     trig_ଗଣନା * ଆକାର(*entry));

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_install_trig(unsigned long smode,
			  unsigned long trig_ଗଣନା, unsigned long *out)
{
	ଶୂନ୍ୟ *shmem_base = ଖାଲି;
	ସଂଘ sbi_dbtr_shmem_entry *entry;
	ସଂରଚନା sbi_dbtr_data_ସନ୍ଦେଶ *recv;
	ସଂରଚନା sbi_dbtr_id_ସନ୍ଦେଶ *xmit;
	unsigned long ctrl;
	u32 claimed = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା slot;
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs = ଖାଲି;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ଯଦି (!hs)
		ଫେରନ୍ତୁ SBI_ERR_FAILED;

	ଯଦି (sbi_dbtr_shmem_disabled(hs))
		ଫେରନ୍ତୁ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);
	sbi_hart_protection_temp_map_range((unsigned long)shmem_base,
					   trig_ଗଣନା * ଆକାର(*entry));

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
	for_each_trig_entry(shmem_base, trig_ଗଣନା, typeof(*entry), entry) {
		recv = (ସଂରଚନା sbi_dbtr_data_ସନ୍ଦେଶ *)(&entry->data);
		ctrl = recv->tdata1;

		ଯଦି (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ଗଣନା * ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_FAILED;
		}

		ଯଦି (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ଗଣନା * ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;
		}

		ଯଦି ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ଗଣନା * ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_NOT_SUPPORTED;
		}

		ଯଦି (!dbtr_trigger_any_hw_supported(hs,
						   lle_to_cpu(recv->tdata1),
						   lle_to_cpu(recv->tdata2),
						   lle_to_cpu(recv->tdata3))) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ଗଣନା * ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_NOT_SUPPORTED;
		}
	}

	ଯଦି (hs->available_trigs < trig_ଗଣନା) {
		*out = hs->available_trigs;
		sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
						     trig_ଗଣନା * ଆକାର(*entry));
		ଫେରନ୍ତୁ SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_entry(shmem_base, trig_ଗଣନା, typeof(*entry), entry) {
		recv = (ସଂରଚନା sbi_dbtr_data_ସନ୍ଦେଶ *)(&entry->data);
		slot = dbtr_find_free_slot(hs, claimed,
					   lle_to_cpu(recv->tdata1),
					   lle_to_cpu(recv->tdata2),
					   lle_to_cpu(recv->tdata3));
		ଯଦି (slot < 0) {
			*out = _idx;
			sbi_hart_protection_temp_unmap_range((unsigned long)shmem_base,
							     trig_ଗଣନା * ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_FAILED;
		}
		claimed |= BIT(slot);
	}

	/* Install triggers */
	for_each_trig_entry(shmem_base, trig_ଗଣନା, typeof(*entry), entry) {
		recv = (ସଂରଚନା sbi_dbtr_data_ସନ୍ଦେଶ *)(&entry->data);
		xmit = (ସଂରଚନା sbi_dbtr_id_ସନ୍ଦେଶ *)(&entry->id);

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
					     trig_ଗଣନା * ଆକାର(*entry));

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_uninstall_trig(unsigned long trig_idx_base,
			    unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ଯଦି (!hs)
		ଫେରନ୍ତୁ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		ଯଦି (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED)))
			ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_enable_trig(unsigned long trig_idx_base,
			 unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ଯଦି (!hs)
		ଫେରନ୍ତୁ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_update_trig(unsigned long smode,
			 unsigned long trig_ଗଣନା)
{
	unsigned long trig_idx;
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ସଂଘ sbi_dbtr_shmem_entry *entry;
	ଶୂନ୍ୟ *shmem_base = ଖାଲି;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs = ଖାଲି;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	ଯଦି (!hs)
		ଫେରନ୍ତୁ SBI_ERR_FAILED;

	ଯଦି (sbi_dbtr_shmem_disabled(hs))
		ଫେରନ୍ତୁ SBI_ERR_NO_SHMEM;

	shmem_base = hart_shmem_base(hs);

	ଯଦି (trig_ଗଣନା >= hs->total_trigs)
		ଫେରନ୍ତୁ SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_entry(shmem_base, trig_ଗଣନା, typeof(*entry), entry) {
		sbi_hart_protection_temp_map_range((unsigned long)entry, ଆକାର(*entry));
		trig_idx = entry->id.idx;

		ଯଦି (trig_idx >= hs->total_trigs) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;
		}

		trig = INDEX_TO_TRIGGER(trig_idx);

		ଯଦି (!(trig->state & RV_DBTR_BIT_MASK(TS, MAPPED))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_FAILED;
		}

		ଯଦି ((entry->data.tdata2 && !tdata2_impl) ||
		    (entry->data.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_NOT_SUPPORTED;
		}

		ଯଦି (!dbtr_trigger_hw_supported(trig->index,
					       lle_to_cpu(entry->data.tdata1),
					       lle_to_cpu(entry->data.tdata2),
					       lle_to_cpu(entry->data.tdata3))) {
			sbi_hart_protection_temp_unmap_range((unsigned long)entry, ଆକାର(*entry));
			ଫେରନ୍ତୁ SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_setup(trig, &entry->data);
		sbi_hart_protection_temp_unmap_range((unsigned long)entry, ଆକାର(*entry));
		dbtr_trigger_enable(trig);
	}

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dbtr_disable_trig(unsigned long trig_idx_base,
			  unsigned long trig_idx_mask)
{
	unsigned long trig_mask = trig_idx_mask << trig_idx_base;
	unsigned long idx = trig_idx_base;
	ସଂରଚନା sbi_dbtr_trigger *trig;
	ସଂରଚନା sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	ଯଦି (!hs)
		ଫେରନ୍ତୁ SBI_ERR_FAILED;

	for_each_set_bit_from(idx, &trig_mask, hs->total_trigs) {
		trig = INDEX_TO_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	ଫେରନ୍ତୁ SBI_SUCCESS;
}
