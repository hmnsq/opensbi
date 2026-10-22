/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_csr_detect.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_byteorder.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_dbtr.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hart_protection.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
ساکن غیرعلامتی طویل hart_state_ptr_انحراف;

/** Device specific debug trigger operations */
ساکن ثابت ساخت sbi_dbtr_device *dbtr_dev = تہی;

ثابت ساخت sbi_dbtr_device *sbi_dbtr_get_device(خالی)
{
	واپس dbtr_dev;
}

خالی sbi_dbtr_سیٹ_device(ثابت ساخت sbi_dbtr_device *dev)
{
	اگر (!dev || dbtr_dev)
		واپس;

	dbtr_dev = dev;
}

#تعریف_کرو dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), خالی *, hart_state_ptr_انحراف)

#تعریف_کرو dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کرو dbtr_سیٹ_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), خالی *, hart_state_ptr_انحراف, \
			       (__hart_state))

#تعریف_کرو tdata_read_محفوظ(__csr)						\
	({								\
		ساخت sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#تعریف_کرو tdata_write_محفوظ(__csr, __value)				\
	({								\
		ساخت sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#تعریف_کرو tdata_implemented(__csr)					\
	({								\
		ساخت sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#تعریف_کرو اشاریہ_مقصد_TRIGGER(اشاریہ)					\
	({								\
		ساخت sbi_dbtr_trigger *__trg = تہی;			\
		ساخت sbi_dbtr_hart_triggers_state *__hs = تہی;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[اشاریہ];			\
		(__trg);						\
	})

#تعریف_کرو for_each_trig_اندراج(بنیاد, _max, _etype, اندراج_2)		\
	برائے (صحیح _idx = 0; اندراج_2 = ((_etype *)بنیاد + _idx),		\
	     _idx < _max;						\
	     _idx++, اندراج_2 = ((_etype *)بنیاد + _idx))

#تعریف_کرو DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
ساکن اندرونی bool sbi_dbtr_shmem_disabled(
	ساخت sbi_dbtr_hart_triggers_state *hs)
{
	واپس (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
ساکن اندرونی خالی sbi_dbtr_disable_shmem(
	ساخت sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
ساکن اندرونی خالی *hart_shmem_بنیاد(
	ساخت sbi_dbtr_hart_triggers_state *hs)
{
	واپس ((خالی *)(غیرعلامتی طویل)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

ساکن خالی sbi_trigger_init(ساخت sbi_dbtr_trigger *trig,
			     غیرعلامتی طویل type_نقاب, غیرعلامتی طویل idx)
{
	trig->type_نقاب = type_نقاب;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->اشاریہ_2 = idx;
}

ساکن bool dbtr_trigger_hw_supported(غیرعلامتی طویل idx, غیرعلامتی طویل tdata1,
				      غیرعلامتی طویل tdata2,
				      غیرعلامتی طویل tdata3)
{
	اگر (dbtr_dev && dbtr_dev->trigger_supported)
		واپس dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	واپس true;
}

ساکن bool dbtr_trigger_any_hw_supported(
			ساخت sbi_dbtr_hart_triggers_state *hs,
			غیرعلامتی طویل tdata1, غیرعلامتی طویل tdata2,
			غیرعلامتی طویل tdata3)
{
	غیرعلامتی طویل type = TDATA1_GET_TYPE(tdata1);
	ساخت sbi_dbtr_trigger *trig;
	صحیح i;

	برائے (i = 0; i < hs->کل_trigs; i++) {
		trig = اشاریہ_مقصد_TRIGGER(i);
		اگر (__test_bit(type, &trig->type_نقاب) &&
		    dbtr_trigger_hw_supported(trig->اشاریہ_2, tdata1, tdata2,
					      tdata3))
			واپس true;
	}

	واپس false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
ساکن صحیح dbtr_ڈھونڈو_free_خانہ(ساخت sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_نقاب,
			       غیرعلامتی طویل tdata1, غیرعلامتی طویل tdata2,
			       غیرعلامتی طویل tdata3)
{
	غیرعلامتی طویل type = TDATA1_GET_TYPE(tdata1);
	ساخت sbi_dbtr_trigger *trig;
	صحیح i;

	برائے (i = 0; i < hs->کل_trigs; i++) {
		trig = اشاریہ_مقصد_TRIGGER(i);
		اگر (trig->state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, MAPPED))
			جاری;
		اگر (claimed_نقاب & ثنائی_ہندسہ(i))
			جاری;
		اگر (!__test_bit(type, &trig->type_نقاب))
			جاری;
		اگر (!dbtr_trigger_hw_supported(trig->اشاریہ_2, tdata1,
					       tdata2, tdata3))
			جاری;
		واپس i;
	}

	واپس SBI_ENOENT;
}

ساکن اندرونی ساخت sbi_dbtr_trigger *sbi_alloc_trigger(غیرعلامتی طویل tdata1,
							 غیرعلامتی طویل tdata2,
							 غیرعلامتی طویل tdata3)
{
	صحیح i;
	ساخت sbi_dbtr_trigger *f_trig;
	ساخت sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	اگر (!hart_state)
		واپس تہی;

	اگر (hart_state->دستیاب_trigs <= 0)
		واپس تہی;

	i = dbtr_ڈھونڈو_free_خانہ(hart_state, 0, tdata1, tdata2, tdata3);
	اگر (i < 0)
		واپس تہی;

	f_trig = اشاریہ_مقصد_TRIGGER(i);
	hart_state->دستیاب_trigs--;
	__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, MAPPED), &f_trig->state);

	واپس f_trig;
}

ساکن اندرونی خالی sbi_free_trigger(ساخت sbi_dbtr_trigger *trig)
{
	ساخت sbi_dbtr_hart_triggers_state *hart_state;

	اگر (trig == تہی)
		واپس;

	hart_state = dbtr_thishart_state_ptr();
	اگر (!hart_state)
		واپس;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->دستیاب_trigs++;
}

صحیح sbi_dbtr_init(ساخت sbi_scratch *scratch, bool coldboot)
{
	ساخت sbi_trap_info trap = {0};
	غیرعلامتی طویل tdata1;
	غیرعلامتی طویل val;
	صحیح i;
	ساخت sbi_dbtr_hart_triggers_state *hart_state = تہی;

	اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		واپس SBI_SUCCESS;

	اگر (coldboot) {
		hart_state_ptr_انحراف = sbi_scratch_alloc_type_انحراف(خالی *);
		اگر (!hart_state_ptr_انحراف)
			واپس SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	اگر (!hart_state) {
		hart_state = sbi_zalloc(حجم(*hart_state));
		اگر (!hart_state)
			واپس SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_سیٹ_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	اگر (hart_state->probed)
		جاؤ _probed;

	برائے (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		اگر (trap.cause)
			توڑو;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		اگر (trap.cause)
			توڑو;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		اگر (val != i)
			توڑو;

		val = csr_read_allowed(CSR_TINFO, &trap);
		اگر (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			اگر (trap.cause)
				توڑو;

			اگر (TDATA1_GET_TYPE(tdata1) == 0)
				توڑو;

			sbi_trigger_init(اشاریہ_مقصد_TRIGGER(i),
					 ثنائی_ہندسہ(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->کل_trigs++;
		} ورنہ {
			اگر (val == 1)
				توڑو;

			sbi_trigger_init(اشاریہ_مقصد_TRIGGER(i), val, i);
			hart_state->کل_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->دستیاب_trigs = hart_state->کل_trigs;

	واپس SBI_SUCCESS;
}

صحیح sbi_dbtr_get_کل_triggers(خالی)
{
	ساخت sbi_dbtr_hart_triggers_state *hs;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	اگر (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		واپس 0;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		واپس 0;

	واپس hs->کل_trigs;
}

صحیح sbi_dbtr_تیاری_shmem(ثابت ساخت sbi_domain *dom, غیرعلامتی طویل smode,
			 غیرعلامتی طویل shmem_phys_lo,
			 غیرعلامتی طویل shmem_phys_hi)
{
	ساخت sbi_dbtr_hart_triggers_state *hart_state;

	اگر (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		واپس SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	اگر (!hart_state)
		واپس SBI_ERR_FAILED;

	/* call is to disable shared memory */
	اگر (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		واپس SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	اگر (!sbi_dbtr_shmem_disabled(hart_state))
		واپس SBI_ERR_ALREADY_دستیاب;

	/* lower physical address must be XLEN/8 bytes aligned */
	اگر (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_نقاب)
		واپس SBI_ERR_INVALID_PARAM;

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
	اگر (shmem_phys_hi)
		واپس SBI_EINVALID_ADDR;

	اگر (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		واپس SBI_ERR_INVALID_پتہ;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	واپس SBI_SUCCESS;
}

ساکن خالی dbtr_trigger_تیاری(ساخت sbi_dbtr_trigger *trig,
			       ساخت sbi_dbtr_ڈیٹا_پیغام *recv)
{
	غیرعلامتی طویل tdata1;

	اگر (!trig)
		واپس;

	trig->tdata1 = lle_مقصد_cpu(recv->tdata1);
	trig->tdata2 = lle_مقصد_cpu(recv->tdata2);
	trig->tdata3 = lle_مقصد_cpu(recv->tdata3);

	tdata1 = lle_مقصد_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, MAPPED), &trig->state);

	سیٹ_TRIG_HW_اشاریہ(trig->state, trig->اشاریہ_2);

	انتخاب (TDATA1_GET_TYPE(tdata1)) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(MC, U), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, U), &trig->state);

		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(MC, S), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, S), &trig->state);
		توڑو;
	حالت RISCV_DBTR_TRIG_MCONTROL6:
		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(MC6, U), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, U), &trig->state);

		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(MC6, S), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, S), &trig->state);

		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, VU), &trig->state);

		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, VS), &trig->state);
		توڑو;
	حالت RISCV_DBTR_TRIG_ICOUNT:
		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, U), &trig->state);

		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, S), &trig->state);

		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, VU), &trig->state);

		اگر (__test_bit(RV_DBTR_ثنائی_ہندسہ(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_ثنائی_ہندسہ(TS, VS), &trig->state);
		توڑو;
	طےشدہ:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		توڑو;
	}
}

ساکن اندرونی خالی update_ثنائی_ہندسہ(غیرعلامتی طویل new, صحیح nr, متلون غیرعلامتی طویل *addr)
{
	اگر (new)
		__set_bit(nr, addr);
	ورنہ
		__clear_bit(nr, addr);
}

ساکن خالی dbtr_trigger_enable(ساخت sbi_dbtr_trigger *trig)
{
	غیرعلامتی طویل state;
	غیرعلامتی طویل tdata1;

	اگر (!trig || !(trig->state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, MAPPED)))
		واپس;

	state = trig->state;
	tdata1 = trig->tdata1;

	انتخاب (TDATA1_GET_TYPE(tdata1)) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, U),
			   RV_DBTR_ثنائی_ہندسہ(MC, U), &trig->tdata1);
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, S),
			   RV_DBTR_ثنائی_ہندسہ(MC, S), &trig->tdata1);
		توڑو;
	حالت RISCV_DBTR_TRIG_MCONTROL6:
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, VU),
			   RV_DBTR_ثنائی_ہندسہ(MC6, VU), &trig->tdata1);
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, VS),
			   RV_DBTR_ثنائی_ہندسہ(MC6, VS), &trig->tdata1);
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, U),
			   RV_DBTR_ثنائی_ہندسہ(MC6, U), &trig->tdata1);
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, S),
			   RV_DBTR_ثنائی_ہندسہ(MC6, S), &trig->tdata1);
		توڑو;
	حالت RISCV_DBTR_TRIG_ICOUNT:
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, VU),
			   RV_DBTR_ثنائی_ہندسہ(ICOUNT, VU), &trig->tdata1);
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, VS),
			   RV_DBTR_ثنائی_ہندسہ(ICOUNT, VS), &trig->tdata1);
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, U),
			   RV_DBTR_ثنائی_ہندسہ(ICOUNT, U), &trig->tdata1);
		update_ثنائی_ہندسہ(state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, S),
			   RV_DBTR_ثنائی_ہندسہ(ICOUNT, S), &trig->tdata1);
		توڑو;
	طےشدہ:
		توڑو;
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
	csr_write(CSR_TSELECT, trig->اشاریہ_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_محفوظ(CSR_TDATA2, trig->tdata2);
	tdata_write_محفوظ(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ساکن خالی dbtr_trigger_disable(ساخت sbi_dbtr_trigger *trig)
{
	غیرعلامتی طویل tdata1;

	اگر (!trig || !(trig->state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, MAPPED)))
		واپس;

	tdata1 = trig->tdata1;

	انتخاب (TDATA1_GET_TYPE(tdata1)) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(MC, S), &trig->tdata1);
		توڑو;
	حالت RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(MC6, S), &trig->tdata1);
		توڑو;
	حالت RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_ثنائی_ہندسہ(ICOUNT, S), &trig->tdata1);
		توڑو;
	طےشدہ:
		توڑو;
	}

	csr_write(CSR_TSELECT, trig->اشاریہ_2);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ساکن خالی dbtr_trigger_clear(ساخت sbi_dbtr_trigger *trig)
{
	اگر (!trig || !(trig->state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, MAPPED)))
		واپس;

	csr_write(CSR_TSELECT, trig->اشاریہ_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_محفوظ(CSR_TDATA2, 0x0);
	tdata_write_محفوظ(CSR_TDATA3, 0x0);
}

ساکن صحیح dbtr_trigger_supported(غیرعلامتی طویل type)
{
	انتخاب (type) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
	حالت RISCV_DBTR_TRIG_MCONTROL6:
	حالت RISCV_DBTR_TRIG_ICOUNT:
		واپس 1;
	طےشدہ:
		توڑو;
	}

	واپس 0;
}

ساکن صحیح dbtr_trigger_valid(غیرعلامتی طویل type, غیرعلامتی طویل tdata)
{
	انتخاب (type) {
	حالت RISCV_DBTR_TRIG_MCONTROL:
		اگر (!(tdata & RV_DBTR_ثنائی_ہندسہ_نقاب(MC, DMODE)) &&
		    !(tdata & RV_DBTR_ثنائی_ہندسہ_نقاب(MC, M)))
			واپس 1;
		توڑو;
	حالت RISCV_DBTR_TRIG_MCONTROL6:
		اگر (!(tdata & RV_DBTR_ثنائی_ہندسہ_نقاب(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_ثنائی_ہندسہ_نقاب(MC6, M)))
			واپس 1;
		توڑو;
	حالت RISCV_DBTR_TRIG_ICOUNT:
		اگر (!(tdata & RV_DBTR_ثنائی_ہندسہ_نقاب(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_ثنائی_ہندسہ_نقاب(ICOUNT, M)))
			واپس 1;
		توڑو;
	طےشدہ:
		توڑو;
	}

	واپس 0;
}

صحیح sbi_dbtr_عدد_trig(غیرعلامتی طویل ڈیٹا, غیرعلامتی طویل *برآمد)
{
	غیرعلامتی طویل type = TDATA1_GET_TYPE(ڈیٹا);
	u32 hartid = current_hartid();
	غیرعلامتی طویل کل = 0;
	ساخت sbi_dbtr_trigger *trig;
	صحیح i;
	ساخت sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		واپس SBI_ERR_FAILED;

	اگر (ڈیٹا == 0) {
		*برآمد = hs->کل_trigs;
		واپس SBI_SUCCESS;
	}

	برائے (i = 0; i < hs->کل_trigs; i++) {
		trig = اشاریہ_مقصد_TRIGGER(i);

		اگر (__test_bit(type, &trig->type_نقاب) &&
		    dbtr_trigger_hw_supported(trig->اشاریہ_2, ڈیٹا, 0, 0))
			کل++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, کل);

	*برآمد = کل;
	واپس SBI_SUCCESS;
}

صحیح sbi_dbtr_read_trig(غیرعلامتی طویل smode,
		       غیرعلامتی طویل trig_idx_بنیاد, غیرعلامتی طویل trig_گنتی)
{
	ساخت sbi_dbtr_ڈیٹا_پیغام *xmit;
	ساخت sbi_dbtr_trigger *trig;
	اتحاد sbi_dbtr_shmem_اندراج *اندراج_3;
	خالی *shmem_بنیاد = تہی;
	ساخت sbi_dbtr_hart_triggers_state *hs = تہی;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		واپس SBI_ERR_FAILED;

	اگر (trig_idx_بنیاد >= hs->کل_trigs ||
	    trig_idx_بنیاد + trig_گنتی >= hs->کل_trigs)
		واپس SBI_ERR_BAD_RANGE;

	اگر (sbi_dbtr_shmem_disabled(hs))
		واپس SBI_ERR_NO_SHMEM;

	shmem_بنیاد = hart_shmem_بنیاد(hs);

	sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)shmem_بنیاد,
					   trig_گنتی * حجم(*اندراج_3));
	for_each_trig_اندراج(shmem_بنیاد, trig_گنتی, typeof(*اندراج_3), اندراج_3) {
		xmit = &اندراج_3->ڈیٹا;
		trig = اشاریہ_مقصد_TRIGGER((_idx + trig_idx_بنیاد));
		csr_write(CSR_TSELECT, trig->اشاریہ_2);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_محفوظ(CSR_TDATA2);
		trig->tdata3 = tdata_read_محفوظ(CSR_TDATA3);
		xmit->tstate = cpu_مقصد_lle(trig->state);
		xmit->tdata1 = cpu_مقصد_lle(trig->tdata1);
		xmit->tdata2 = cpu_مقصد_lle(trig->tdata2);
		xmit->tdata3 = cpu_مقصد_lle(trig->tdata3);
	}
	sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد,
					     trig_گنتی * حجم(*اندراج_3));

	واپس SBI_SUCCESS;
}

صحیح sbi_dbtr_install_trig(غیرعلامتی طویل smode,
			  غیرعلامتی طویل trig_گنتی, غیرعلامتی طویل *برآمد)
{
	خالی *shmem_بنیاد = تہی;
	اتحاد sbi_dbtr_shmem_اندراج *اندراج_3;
	ساخت sbi_dbtr_ڈیٹا_پیغام *recv;
	ساخت sbi_dbtr_id_پیغام *xmit;
	غیرعلامتی طویل ctrl;
	u32 claimed = 0;
	صحیح خانہ;
	ساخت sbi_dbtr_trigger *trig;
	ساخت sbi_dbtr_hart_triggers_state *hs = تہی;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		واپس SBI_ERR_FAILED;

	اگر (sbi_dbtr_shmem_disabled(hs))
		واپس SBI_ERR_NO_SHMEM;

	shmem_بنیاد = hart_shmem_بنیاد(hs);
	sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)shmem_بنیاد,
					   trig_گنتی * حجم(*اندراج_3));

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
	for_each_trig_اندراج(shmem_بنیاد, trig_گنتی, typeof(*اندراج_3), اندراج_3) {
		recv = (ساخت sbi_dbtr_ڈیٹا_پیغام *)(&اندراج_3->ڈیٹا);
		ctrl = recv->tdata1;

		اگر (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*برآمد = _idx;
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد,
							     trig_گنتی * حجم(*اندراج_3));
			واپس SBI_ERR_FAILED;
		}

		اگر (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*برآمد = _idx;
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد,
							     trig_گنتی * حجم(*اندراج_3));
			واپس SBI_ERR_INVALID_PARAM;
		}

		اگر ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*برآمد = _idx;
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد,
							     trig_گنتی * حجم(*اندراج_3));
			واپس SBI_ERR_NOT_SUPPORTED;
		}

		اگر (!dbtr_trigger_any_hw_supported(hs,
						   lle_مقصد_cpu(recv->tdata1),
						   lle_مقصد_cpu(recv->tdata2),
						   lle_مقصد_cpu(recv->tdata3))) {
			*برآمد = _idx;
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد,
							     trig_گنتی * حجم(*اندراج_3));
			واپس SBI_ERR_NOT_SUPPORTED;
		}
	}

	اگر (hs->دستیاب_trigs < trig_گنتی) {
		*برآمد = hs->دستیاب_trigs;
		sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد,
						     trig_گنتی * حجم(*اندراج_3));
		واپس SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_اندراج(shmem_بنیاد, trig_گنتی, typeof(*اندراج_3), اندراج_3) {
		recv = (ساخت sbi_dbtr_ڈیٹا_پیغام *)(&اندراج_3->ڈیٹا);
		خانہ = dbtr_ڈھونڈو_free_خانہ(hs, claimed,
					   lle_مقصد_cpu(recv->tdata1),
					   lle_مقصد_cpu(recv->tdata2),
					   lle_مقصد_cpu(recv->tdata3));
		اگر (خانہ < 0) {
			*برآمد = _idx;
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد,
							     trig_گنتی * حجم(*اندراج_3));
			واپس SBI_ERR_FAILED;
		}
		claimed |= ثنائی_ہندسہ(خانہ);
	}

	/* Install triggers */
	for_each_trig_اندراج(shmem_بنیاد, trig_گنتی, typeof(*اندراج_3), اندراج_3) {
		recv = (ساخت sbi_dbtr_ڈیٹا_پیغام *)(&اندراج_3->ڈیٹا);
		xmit = (ساخت sbi_dbtr_id_پیغام *)(&اندراج_3->id);

		/*
		 * The dry-run above matched every requested configuration
		 * to a free hardware trigger slot, so allocation must
		 * succeed.
		 */
		trig = sbi_alloc_trigger(lle_مقصد_cpu(recv->tdata1),
					 lle_مقصد_cpu(recv->tdata2),
					 lle_مقصد_cpu(recv->tdata3));

		dbtr_trigger_تیاری(trig,  recv);
		dbtr_trigger_enable(trig);
		xmit->idx = cpu_مقصد_lle(trig->اشاریہ_2);

	}

	sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)shmem_بنیاد,
					     trig_گنتی * حجم(*اندراج_3));

	واپس SBI_SUCCESS;
}

صحیح sbi_dbtr_uninstall_trig(غیرعلامتی طویل trig_idx_بنیاد,
			    غیرعلامتی طویل trig_idx_نقاب)
{
	غیرعلامتی طویل trig_نقاب = trig_idx_نقاب << trig_idx_بنیاد;
	غیرعلامتی طویل idx = trig_idx_بنیاد;
	ساخت sbi_dbtr_trigger *trig;
	ساخت sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		واپس SBI_ERR_FAILED;

	for_each_سیٹ_ثنائی_ہندسہ_from(idx, &trig_نقاب, hs->کل_trigs) {
		trig = اشاریہ_مقصد_TRIGGER(idx);
		اگر (!(trig->state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, MAPPED)))
			واپس SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	واپس SBI_SUCCESS;
}

صحیح sbi_dbtr_enable_trig(غیرعلامتی طویل trig_idx_بنیاد,
			 غیرعلامتی طویل trig_idx_نقاب)
{
	غیرعلامتی طویل trig_نقاب = trig_idx_نقاب << trig_idx_بنیاد;
	غیرعلامتی طویل idx = trig_idx_بنیاد;
	ساخت sbi_dbtr_trigger *trig;
	ساخت sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		واپس SBI_ERR_FAILED;

	for_each_سیٹ_ثنائی_ہندسہ_from(idx, &trig_نقاب, hs->کل_trigs) {
		trig = اشاریہ_مقصد_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	واپس SBI_SUCCESS;
}

صحیح sbi_dbtr_update_trig(غیرعلامتی طویل smode,
			 غیرعلامتی طویل trig_گنتی)
{
	غیرعلامتی طویل trig_idx;
	ساخت sbi_dbtr_trigger *trig;
	اتحاد sbi_dbtr_shmem_اندراج *اندراج_3;
	خالی *shmem_بنیاد = تہی;
	ساخت sbi_dbtr_hart_triggers_state *hs = تہی;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		واپس SBI_ERR_FAILED;

	اگر (sbi_dbtr_shmem_disabled(hs))
		واپس SBI_ERR_NO_SHMEM;

	shmem_بنیاد = hart_shmem_بنیاد(hs);

	اگر (trig_گنتی >= hs->کل_trigs)
		واپس SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_اندراج(shmem_بنیاد, trig_گنتی, typeof(*اندراج_3), اندراج_3) {
		sbi_hart_protection_عارضی_map_range((غیرعلامتی طویل)اندراج_3, حجم(*اندراج_3));
		trig_idx = اندراج_3->id.idx;

		اگر (trig_idx >= hs->کل_trigs) {
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)اندراج_3, حجم(*اندراج_3));
			واپس SBI_ERR_INVALID_PARAM;
		}

		trig = اشاریہ_مقصد_TRIGGER(trig_idx);

		اگر (!(trig->state & RV_DBTR_ثنائی_ہندسہ_نقاب(TS, MAPPED))) {
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)اندراج_3, حجم(*اندراج_3));
			واپس SBI_ERR_FAILED;
		}

		اگر ((اندراج_3->ڈیٹا.tdata2 && !tdata2_impl) ||
		    (اندراج_3->ڈیٹا.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)اندراج_3, حجم(*اندراج_3));
			واپس SBI_ERR_NOT_SUPPORTED;
		}

		اگر (!dbtr_trigger_hw_supported(trig->اشاریہ_2,
					       lle_مقصد_cpu(اندراج_3->ڈیٹا.tdata1),
					       lle_مقصد_cpu(اندراج_3->ڈیٹا.tdata2),
					       lle_مقصد_cpu(اندراج_3->ڈیٹا.tdata3))) {
			sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)اندراج_3, حجم(*اندراج_3));
			واپس SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_تیاری(trig, &اندراج_3->ڈیٹا);
		sbi_hart_protection_عارضی_unmap_range((غیرعلامتی طویل)اندراج_3, حجم(*اندراج_3));
		dbtr_trigger_enable(trig);
	}

	واپس SBI_SUCCESS;
}

صحیح sbi_dbtr_disable_trig(غیرعلامتی طویل trig_idx_بنیاد,
			  غیرعلامتی طویل trig_idx_نقاب)
{
	غیرعلامتی طویل trig_نقاب = trig_idx_نقاب << trig_idx_بنیاد;
	غیرعلامتی طویل idx = trig_idx_بنیاد;
	ساخت sbi_dbtr_trigger *trig;
	ساخت sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	اگر (!hs)
		واپس SBI_ERR_FAILED;

	for_each_سیٹ_ثنائی_ہندسہ_from(idx, &trig_نقاب, hs->کل_trigs) {
		trig = اشاریہ_مقصد_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	واپس SBI_SUCCESS;
}
