/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_csr_detect.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_byteorder.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_dbtr.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hart_protection.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/riscv_asm.h>


/** Offset of pointer to HART's debug triggers info in scratch space */
ساكن بینښه اوږد hart_state_ptr_دننه_واټن;

/** Device specific debug trigger operations */
ساكن ثابت بنية sbi_dbtr_device *dbtr_dev = عدم;

ثابت بنية sbi_dbtr_device *sbi_dbtr_get_device(فراغ)
{
	أرجع dbtr_dev;
}

فراغ sbi_dbtr_سېټ_device(ثابت بنية sbi_dbtr_device *dev)
{
	إذا (!dev || dbtr_dev)
		أرجع;

	dbtr_dev = dev;
}

#تعریف_کړه dbtr_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, hart_state_ptr_دننه_واټن)

#تعریف_کړه dbtr_thishart_state_ptr()				\
	dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#تعریف_کړه dbtr_سېټ_hart_state_ptr(__scratch, __hart_state)		\
	sbi_scratch_write_type((__scratch), فراغ *, hart_state_ptr_دننه_واټن, \
			       (__hart_state))

#تعریف_کړه tdata_read_خوندي(__csr)						\
	({								\
		بنية sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
	})

#تعریف_کړه tdata_write_خوندي(__csr, __value)				\
	({								\
		بنية sbi_trap_info __trap = {0};			\
		csr_write_allowed((__csr), &__trap, (__value));		\
	})

#تعریف_کړه tdata_implemented(__csr)					\
	({								\
		بنية sbi_trap_info __trap = {0};			\
		csr_read_allowed((__csr), &__trap);			\
		!__trap.cause;						\
	})

#تعریف_کړه شاخص_منزل_TRIGGER(شاخص)					\
	({								\
		بنية sbi_dbtr_trigger *__trg = عدم;			\
		بنية sbi_dbtr_hart_triggers_state *__hs = عدم;	\
		__hs = dbtr_get_hart_state_ptr(sbi_scratch_thishart_ptr()); \
		__trg = &__hs->triggers[شاخص];			\
		(__trg);						\
	})

#تعریف_کړه for_each_trig_ثبت(بنسټ, _max, _etype, ثبت)		\
	لكل (صحيح _idx = 0; ثبت = ((_etype *)بنسټ + _idx),		\
	     _idx < _max;						\
	     _idx++, ثبت = ((_etype *)بنسټ + _idx))

#تعریف_کړه DBTR_SHMEM_MAKE_PHYS(_p_hi, _p_lo) (_p_lo)

/* must call with hs != NULL */
ساكن مضمن bool sbi_dbtr_shmem_disabled(
	بنية sbi_dbtr_hart_triggers_state *hs)
{
	أرجع (hs->shmem.phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR &&
		hs->shmem.phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR
		? true : false);
}

/* must call with hs != NULL */
ساكن مضمن فراغ sbi_dbtr_disable_shmem(
	بنية sbi_dbtr_hart_triggers_state *hs)
{
	hs->shmem.phys_lo = SBI_DBTR_SHMEM_INVALID_ADDR;
	hs->shmem.phys_hi = SBI_DBTR_SHMEM_INVALID_ADDR;
}

/* must call with hs which is not disabled */
ساكن مضمن فراغ *hart_shmem_بنسټ(
	بنية sbi_dbtr_hart_triggers_state *hs)
{
	أرجع ((فراغ *)(بینښه اوږد)DBTR_SHMEM_MAKE_PHYS(
			hs->shmem.phys_hi, hs->shmem.phys_lo));
}

ساكن فراغ sbi_trigger_init(بنية sbi_dbtr_trigger *trig,
			     بینښه اوږد type_ماسک, بینښه اوږد idx)
{
	trig->type_ماسک = type_ماسک;
	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;
	trig->شاخص_2 = idx;
}

ساكن bool dbtr_trigger_hw_supported(بینښه اوږد idx, بینښه اوږد tdata1,
				      بینښه اوږد tdata2,
				      بینښه اوږد tdata3)
{
	إذا (dbtr_dev && dbtr_dev->trigger_supported)
		أرجع dbtr_dev->trigger_supported(idx, tdata1, tdata2,
						   tdata3);

	أرجع true;
}

ساكن bool dbtr_trigger_any_hw_supported(
			بنية sbi_dbtr_hart_triggers_state *hs,
			بینښه اوږد tdata1, بینښه اوږد tdata2,
			بینښه اوږد tdata3)
{
	بینښه اوږد type = TDATA1_GET_TYPE(tdata1);
	بنية sbi_dbtr_trigger *trig;
	صحيح i;

	لكل (i = 0; i < hs->ټول_trigs; i++) {
		trig = شاخص_منزل_TRIGGER(i);
		إذا (__test_bit(type, &trig->type_ماسک) &&
		    dbtr_trigger_hw_supported(trig->شاخص_2, tdata1, tdata2,
					      tdata3))
			أرجع true;
	}

	أرجع false;
}

/*
 * Find the first free hardware trigger slot supporting the configuration.
 * Slots set in claimed_mask are treated as taken, which allows the caller
 * to track slot availability. A 32-bit mask covers RV_MAX_TRIGGERS (32);
 * for a larger number of triggers, this function needs to be updated.
 */
ساكن صحيح dbtr_پیدا_کړه_free_خونه(بنية sbi_dbtr_hart_triggers_state *hs,
			       u32 claimed_ماسک,
			       بینښه اوږد tdata1, بینښه اوږد tdata2,
			       بینښه اوږد tdata3)
{
	بینښه اوږد type = TDATA1_GET_TYPE(tdata1);
	بنية sbi_dbtr_trigger *trig;
	صحيح i;

	لكل (i = 0; i < hs->ټول_trigs; i++) {
		trig = شاخص_منزل_TRIGGER(i);
		إذا (trig->state & RV_DBTR_بټ_ماسک(TS, MAPPED))
			استمر;
		إذا (claimed_ماسک & بټ(i))
			استمر;
		إذا (!__test_bit(type, &trig->type_ماسک))
			استمر;
		إذا (!dbtr_trigger_hw_supported(trig->شاخص_2, tdata1,
					       tdata2, tdata3))
			استمر;
		أرجع i;
	}

	أرجع SBI_ENOENT;
}

ساكن مضمن بنية sbi_dbtr_trigger *sbi_alloc_trigger(بینښه اوږد tdata1,
							 بینښه اوږد tdata2,
							 بینښه اوږد tdata3)
{
	صحيح i;
	بنية sbi_dbtr_trigger *f_trig;
	بنية sbi_dbtr_hart_triggers_state *hart_state;

	hart_state = dbtr_thishart_state_ptr();
	إذا (!hart_state)
		أرجع عدم;

	إذا (hart_state->شته_trigs <= 0)
		أرجع عدم;

	i = dbtr_پیدا_کړه_free_خونه(hart_state, 0, tdata1, tdata2, tdata3);
	إذا (i < 0)
		أرجع عدم;

	f_trig = شاخص_منزل_TRIGGER(i);
	hart_state->شته_trigs--;
	__set_bit(RV_DBTR_بټ(TS, MAPPED), &f_trig->state);

	أرجع f_trig;
}

ساكن مضمن فراغ sbi_free_trigger(بنية sbi_dbtr_trigger *trig)
{
	بنية sbi_dbtr_hart_triggers_state *hart_state;

	إذا (trig == عدم)
		أرجع;

	hart_state = dbtr_thishart_state_ptr();
	إذا (!hart_state)
		أرجع;

	trig->state = 0;
	trig->tdata1 = 0;
	trig->tdata2 = 0;
	trig->tdata3 = 0;

	hart_state->شته_trigs++;
}

صحيح sbi_dbtr_init(بنية sbi_scratch *scratch, bool coldboot)
{
	بنية sbi_trap_info trap = {0};
	بینښه اوږد tdata1;
	بینښه اوږد val;
	صحيح i;
	بنية sbi_dbtr_hart_triggers_state *hart_state = عدم;

	إذا (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		أرجع SBI_SUCCESS;

	إذا (coldboot) {
		hart_state_ptr_دننه_واټن = sbi_scratch_alloc_type_دننه_واټن(فراغ *);
		إذا (!hart_state_ptr_دننه_واټن)
			أرجع SBI_ENOMEM;
	}

	hart_state = dbtr_get_hart_state_ptr(scratch);
	إذا (!hart_state) {
		hart_state = sbi_zalloc(حجم(*hart_state));
		إذا (!hart_state)
			أرجع SBI_ENOMEM;
		hart_state->hartid = current_hartid();
		dbtr_سېټ_hart_state_ptr(scratch, hart_state);
	}

	/* disable the shared memory */
	sbi_dbtr_disable_shmem(hart_state);

	/* Skip probing triggers if already probed */
	إذا (hart_state->probed)
		اذهب _probed;

	لكل (i = 0; i < RV_MAX_TRIGGERS; i++) {
		csr_write_allowed(CSR_TSELECT, &trap, i);
		إذا (trap.cause)
			اكسر;

		val = csr_read_allowed(CSR_TSELECT, &trap);
		إذا (trap.cause)
			اكسر;

		/*
		 * Read back tselect and check that it contains the
		 * written value
		 */
		إذا (val != i)
			اكسر;

		val = csr_read_allowed(CSR_TINFO, &trap);
		إذا (trap.cause) {
			/*
			 * If reading tinfo caused an exception, the
			 * debugger must read tdata1 to discover the
			 * type.
			 */
			tdata1 = csr_read_allowed(CSR_TDATA1,
						  &trap);
			إذا (trap.cause)
				اكسر;

			إذا (TDATA1_GET_TYPE(tdata1) == 0)
				اكسر;

			sbi_trigger_init(شاخص_منزل_TRIGGER(i),
					 بټ(TDATA1_GET_TYPE(tdata1)),
					 i);
			hart_state->ټول_trigs++;
		} وإلا {
			إذا (val == 1)
				اكسر;

			sbi_trigger_init(شاخص_منزل_TRIGGER(i), val, i);
			hart_state->ټول_trigs++;
		}
	}

	hart_state->probed = 1;

 _probed:
	hart_state->شته_trigs = hart_state->ټول_trigs;

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_get_ټول_triggers(فراغ)
{
	بنية sbi_dbtr_hart_triggers_state *hs;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/*
	 * This function may be used during ecall registration.
	 * By that time the debug trigger module might not be
	 * initialized. If the extension is not supported, report
	 * number of triggers as 0.
	 */
	إذا (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SDTRIG))
		أرجع 0;

	hs = dbtr_thishart_state_ptr();
	إذا (!hs)
		أرجع 0;

	أرجع hs->ټول_trigs;
}

صحيح sbi_dbtr_تیاری_shmem(ثابت بنية sbi_domain *dom, بینښه اوږد smode,
			 بینښه اوږد shmem_phys_lo,
			 بینښه اوږد shmem_phys_hi)
{
	بنية sbi_dbtr_hart_triggers_state *hart_state;

	إذا (dom && !sbi_domain_is_assigned_hart(dom, current_hartindex())) {
		sbi_dprintf("%s: calling hart not assigned to this domain\n",
			   __func__);
		أرجع SBI_ERR_DENIED;
	}

	hart_state = dbtr_thishart_state_ptr();
	إذا (!hart_state)
		أرجع SBI_ERR_FAILED;

	/* call is to disable shared memory */
	إذا (shmem_phys_lo == SBI_DBTR_SHMEM_INVALID_ADDR
	    && shmem_phys_hi == SBI_DBTR_SHMEM_INVALID_ADDR) {
		sbi_dbtr_disable_shmem(hart_state);
		أرجع SBI_SUCCESS;
	}

	/* the shared memory must be disabled on this hart */
	إذا (!sbi_dbtr_shmem_disabled(hart_state))
		أرجع SBI_ERR_ALREADY_شته;

	/* lower physical address must be XLEN/8 bytes aligned */
	إذا (shmem_phys_lo & SBI_DBTR_SHMEM_ALIGN_ماسک)
		أرجع SBI_ERR_INVALID_PARAM;

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
	إذا (shmem_phys_hi)
		أرجع SBI_EINVALID_ADDR;

	إذا (dom && !sbi_domain_check_addr(dom,
		  DBTR_SHMEM_MAKE_PHYS(shmem_phys_hi, shmem_phys_lo), smode,
		  SBI_DOMAIN_READ | SBI_DOMAIN_WRITE))
		أرجع SBI_ERR_INVALID_پته;

	hart_state->shmem.phys_lo = shmem_phys_lo;
	hart_state->shmem.phys_hi = shmem_phys_hi;

	أرجع SBI_SUCCESS;
}

ساكن فراغ dbtr_trigger_تیاری(بنية sbi_dbtr_trigger *trig,
			       بنية sbi_dbtr_معلومات_پیغام *recv)
{
	بینښه اوږد tdata1;

	إذا (!trig)
		أرجع;

	trig->tdata1 = lle_منزل_cpu(recv->tdata1);
	trig->tdata2 = lle_منزل_cpu(recv->tdata2);
	trig->tdata3 = lle_منزل_cpu(recv->tdata3);

	tdata1 = lle_منزل_cpu(recv->tdata1);

	trig->state = 0;

	__set_bit(RV_DBTR_بټ(TS, MAPPED), &trig->state);

	سېټ_TRIG_HW_شاخص(trig->state, trig->شاخص_2);

	بدل (TDATA1_GET_TYPE(tdata1)) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
		إذا (__test_bit(RV_DBTR_بټ(MC, U), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, U), &trig->state);

		إذا (__test_bit(RV_DBTR_بټ(MC, S), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, S), &trig->state);
		اكسر;
	حالة RISCV_DBTR_TRIG_MCONTROL6:
		إذا (__test_bit(RV_DBTR_بټ(MC6, U), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, U), &trig->state);

		إذا (__test_bit(RV_DBTR_بټ(MC6, S), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, S), &trig->state);

		إذا (__test_bit(RV_DBTR_بټ(MC6, VU), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, VU), &trig->state);

		إذا (__test_bit(RV_DBTR_بټ(MC6, VS), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, VS), &trig->state);
		اكسر;
	حالة RISCV_DBTR_TRIG_ICOUNT:
		إذا (__test_bit(RV_DBTR_بټ(ICOUNT, U), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, U), &trig->state);

		إذا (__test_bit(RV_DBTR_بټ(ICOUNT, S), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, S), &trig->state);

		إذا (__test_bit(RV_DBTR_بټ(ICOUNT, VU), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, VU), &trig->state);

		إذا (__test_bit(RV_DBTR_بټ(ICOUNT, VS), &tdata1))
			__set_bit(RV_DBTR_بټ(TS, VS), &trig->state);
		اكسر;
	افتراضي:
		sbi_dprintf("%s: Unknown type (tdata1: 0x%lx Type: %ld)\n",
			    __func__, tdata1, TDATA1_GET_TYPE(tdata1));
		اكسر;
	}
}

ساكن مضمن فراغ update_بټ(بینښه اوږد new, صحيح nr, متطاير بینښه اوږد *addr)
{
	إذا (new)
		__set_bit(nr, addr);
	وإلا
		__clear_bit(nr, addr);
}

ساكن فراغ dbtr_trigger_enable(بنية sbi_dbtr_trigger *trig)
{
	بینښه اوږد state;
	بینښه اوږد tdata1;

	إذا (!trig || !(trig->state & RV_DBTR_بټ_ماسک(TS, MAPPED)))
		أرجع;

	state = trig->state;
	tdata1 = trig->tdata1;

	بدل (TDATA1_GET_TYPE(tdata1)) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, U),
			   RV_DBTR_بټ(MC, U), &trig->tdata1);
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, S),
			   RV_DBTR_بټ(MC, S), &trig->tdata1);
		اكسر;
	حالة RISCV_DBTR_TRIG_MCONTROL6:
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, VU),
			   RV_DBTR_بټ(MC6, VU), &trig->tdata1);
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, VS),
			   RV_DBTR_بټ(MC6, VS), &trig->tdata1);
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, U),
			   RV_DBTR_بټ(MC6, U), &trig->tdata1);
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, S),
			   RV_DBTR_بټ(MC6, S), &trig->tdata1);
		اكسر;
	حالة RISCV_DBTR_TRIG_ICOUNT:
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, VU),
			   RV_DBTR_بټ(ICOUNT, VU), &trig->tdata1);
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, VS),
			   RV_DBTR_بټ(ICOUNT, VS), &trig->tdata1);
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, U),
			   RV_DBTR_بټ(ICOUNT, U), &trig->tdata1);
		update_بټ(state & RV_DBTR_بټ_ماسک(TS, S),
			   RV_DBTR_بټ(ICOUNT, S), &trig->tdata1);
		اكسر;
	افتراضي:
		اكسر;
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
	csr_write(CSR_TSELECT, trig->شاخص_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_خوندي(CSR_TDATA2, trig->tdata2);
	tdata_write_خوندي(CSR_TDATA3, trig->tdata3);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ساكن فراغ dbtr_trigger_disable(بنية sbi_dbtr_trigger *trig)
{
	بینښه اوږد tdata1;

	إذا (!trig || !(trig->state & RV_DBTR_بټ_ماسک(TS, MAPPED)))
		أرجع;

	tdata1 = trig->tdata1;

	بدل (TDATA1_GET_TYPE(tdata1)) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
		__clear_bit(RV_DBTR_بټ(MC, U), &trig->tdata1);
		__clear_bit(RV_DBTR_بټ(MC, S), &trig->tdata1);
		اكسر;
	حالة RISCV_DBTR_TRIG_MCONTROL6:
		__clear_bit(RV_DBTR_بټ(MC6, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_بټ(MC6, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_بټ(MC6, U), &trig->tdata1);
		__clear_bit(RV_DBTR_بټ(MC6, S), &trig->tdata1);
		اكسر;
	حالة RISCV_DBTR_TRIG_ICOUNT:
		__clear_bit(RV_DBTR_بټ(ICOUNT, VU), &trig->tdata1);
		__clear_bit(RV_DBTR_بټ(ICOUNT, VS), &trig->tdata1);
		__clear_bit(RV_DBTR_بټ(ICOUNT, U), &trig->tdata1);
		__clear_bit(RV_DBTR_بټ(ICOUNT, S), &trig->tdata1);
		اكسر;
	افتراضي:
		اكسر;
	}

	csr_write(CSR_TSELECT, trig->شاخص_2);
	csr_write(CSR_TDATA1, trig->tdata1);
}

ساكن فراغ dbtr_trigger_clear(بنية sbi_dbtr_trigger *trig)
{
	إذا (!trig || !(trig->state & RV_DBTR_بټ_ماسک(TS, MAPPED)))
		أرجع;

	csr_write(CSR_TSELECT, trig->شاخص_2);
	csr_write(CSR_TDATA1, 0x0);
	tdata_write_خوندي(CSR_TDATA2, 0x0);
	tdata_write_خوندي(CSR_TDATA3, 0x0);
}

ساكن صحيح dbtr_trigger_supported(بینښه اوږد type)
{
	بدل (type) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
	حالة RISCV_DBTR_TRIG_MCONTROL6:
	حالة RISCV_DBTR_TRIG_ICOUNT:
		أرجع 1;
	افتراضي:
		اكسر;
	}

	أرجع 0;
}

ساكن صحيح dbtr_trigger_valid(بینښه اوږد type, بینښه اوږد tdata)
{
	بدل (type) {
	حالة RISCV_DBTR_TRIG_MCONTROL:
		إذا (!(tdata & RV_DBTR_بټ_ماسک(MC, DMODE)) &&
		    !(tdata & RV_DBTR_بټ_ماسک(MC, M)))
			أرجع 1;
		اكسر;
	حالة RISCV_DBTR_TRIG_MCONTROL6:
		إذا (!(tdata & RV_DBTR_بټ_ماسک(MC6, DMODE)) &&
		    !(tdata & RV_DBTR_بټ_ماسک(MC6, M)))
			أرجع 1;
		اكسر;
	حالة RISCV_DBTR_TRIG_ICOUNT:
		إذا (!(tdata & RV_DBTR_بټ_ماسک(ICOUNT, DMODE)) &&
		    !(tdata & RV_DBTR_بټ_ماسک(ICOUNT, M)))
			أرجع 1;
		اكسر;
	افتراضي:
		اكسر;
	}

	أرجع 0;
}

صحيح sbi_dbtr_عدد_trig(بینښه اوږد معلومات, بینښه اوږد *بهر)
{
	بینښه اوږد type = TDATA1_GET_TYPE(معلومات);
	u32 hartid = current_hartid();
	بینښه اوږد ټول = 0;
	بنية sbi_dbtr_trigger *trig;
	صحيح i;
	بنية sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	إذا (!hs)
		أرجع SBI_ERR_FAILED;

	إذا (معلومات == 0) {
		*بهر = hs->ټول_trigs;
		أرجع SBI_SUCCESS;
	}

	لكل (i = 0; i < hs->ټول_trigs; i++) {
		trig = شاخص_منزل_TRIGGER(i);

		إذا (__test_bit(type, &trig->type_ماسک) &&
		    dbtr_trigger_hw_supported(trig->شاخص_2, معلومات, 0, 0))
			ټول++;
	}

	sbi_dprintf("%s: hart%d: total triggers of type %lu: %lu\n",
		    __func__, hartid, type, ټول);

	*بهر = ټول;
	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_read_trig(بینښه اوږد smode,
		       بینښه اوږد trig_idx_بنسټ, بینښه اوږد trig_شمېر)
{
	بنية sbi_dbtr_معلومات_پیغام *xmit;
	بنية sbi_dbtr_trigger *trig;
	اتحاد sbi_dbtr_shmem_ثبت *ثبت_2;
	فراغ *shmem_بنسټ = عدم;
	بنية sbi_dbtr_hart_triggers_state *hs = عدم;

	hs = dbtr_thishart_state_ptr();
	إذا (!hs)
		أرجع SBI_ERR_FAILED;

	إذا (trig_idx_بنسټ >= hs->ټول_trigs ||
	    trig_idx_بنسټ + trig_شمېر >= hs->ټول_trigs)
		أرجع SBI_ERR_BAD_RANGE;

	إذا (sbi_dbtr_shmem_disabled(hs))
		أرجع SBI_ERR_NO_SHMEM;

	shmem_بنسټ = hart_shmem_بنسټ(hs);

	sbi_hart_protection_لنډمهاله_map_range((بینښه اوږد)shmem_بنسټ,
					   trig_شمېر * حجم(*ثبت_2));
	for_each_trig_ثبت(shmem_بنسټ, trig_شمېر, typeof(*ثبت_2), ثبت_2) {
		xmit = &ثبت_2->معلومات;
		trig = شاخص_منزل_TRIGGER((_idx + trig_idx_بنسټ));
		csr_write(CSR_TSELECT, trig->شاخص_2);
		trig->tdata1 = csr_read(CSR_TDATA1);
		trig->tdata2 = tdata_read_خوندي(CSR_TDATA2);
		trig->tdata3 = tdata_read_خوندي(CSR_TDATA3);
		xmit->tstate = cpu_منزل_lle(trig->state);
		xmit->tdata1 = cpu_منزل_lle(trig->tdata1);
		xmit->tdata2 = cpu_منزل_lle(trig->tdata2);
		xmit->tdata3 = cpu_منزل_lle(trig->tdata3);
	}
	sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)shmem_بنسټ,
					     trig_شمېر * حجم(*ثبت_2));

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_install_trig(بینښه اوږد smode,
			  بینښه اوږد trig_شمېر, بینښه اوږد *بهر)
{
	فراغ *shmem_بنسټ = عدم;
	اتحاد sbi_dbtr_shmem_ثبت *ثبت_2;
	بنية sbi_dbtr_معلومات_پیغام *recv;
	بنية sbi_dbtr_id_پیغام *xmit;
	بینښه اوږد ctrl;
	u32 claimed = 0;
	صحيح خونه;
	بنية sbi_dbtr_trigger *trig;
	بنية sbi_dbtr_hart_triggers_state *hs = عدم;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	إذا (!hs)
		أرجع SBI_ERR_FAILED;

	إذا (sbi_dbtr_shmem_disabled(hs))
		أرجع SBI_ERR_NO_SHMEM;

	shmem_بنسټ = hart_shmem_بنسټ(hs);
	sbi_hart_protection_لنډمهاله_map_range((بینښه اوږد)shmem_بنسټ,
					   trig_شمېر * حجم(*ثبت_2));

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
	for_each_trig_ثبت(shmem_بنسټ, trig_شمېر, typeof(*ثبت_2), ثبت_2) {
		recv = (بنية sbi_dbtr_معلومات_پیغام *)(&ثبت_2->معلومات);
		ctrl = recv->tdata1;

		إذا (!dbtr_trigger_supported(TDATA1_GET_TYPE(ctrl))) {
			*بهر = _idx;
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)shmem_بنسټ,
							     trig_شمېر * حجم(*ثبت_2));
			أرجع SBI_ERR_FAILED;
		}

		إذا (!dbtr_trigger_valid(TDATA1_GET_TYPE(ctrl), ctrl)) {
			*بهر = _idx;
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)shmem_بنسټ,
							     trig_شمېر * حجم(*ثبت_2));
			أرجع SBI_ERR_INVALID_PARAM;
		}

		إذا ((recv->tdata2 && !tdata2_impl) ||
		    (recv->tdata3 && !tdata3_impl)) {
			*بهر = _idx;
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)shmem_بنسټ,
							     trig_شمېر * حجم(*ثبت_2));
			أرجع SBI_ERR_NOT_SUPPORTED;
		}

		إذا (!dbtr_trigger_any_hw_supported(hs,
						   lle_منزل_cpu(recv->tdata1),
						   lle_منزل_cpu(recv->tdata2),
						   lle_منزل_cpu(recv->tdata3))) {
			*بهر = _idx;
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)shmem_بنسټ,
							     trig_شمېر * حجم(*ثبت_2));
			أرجع SBI_ERR_NOT_SUPPORTED;
		}
	}

	إذا (hs->شته_trigs < trig_شمېر) {
		*بهر = hs->شته_trigs;
		sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)shmem_بنسټ,
						     trig_شمېر * حجم(*ثبت_2));
		أرجع SBI_ERR_FAILED;
	}

	/*
	 * Dry-run the allocation of the whole batch so that no trigger
	 * is installed if any of the requested configurations cannot be
	 * matched to a free hardware trigger slot.
	 */
	for_each_trig_ثبت(shmem_بنسټ, trig_شمېر, typeof(*ثبت_2), ثبت_2) {
		recv = (بنية sbi_dbtr_معلومات_پیغام *)(&ثبت_2->معلومات);
		خونه = dbtr_پیدا_کړه_free_خونه(hs, claimed,
					   lle_منزل_cpu(recv->tdata1),
					   lle_منزل_cpu(recv->tdata2),
					   lle_منزل_cpu(recv->tdata3));
		إذا (خونه < 0) {
			*بهر = _idx;
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)shmem_بنسټ,
							     trig_شمېر * حجم(*ثبت_2));
			أرجع SBI_ERR_FAILED;
		}
		claimed |= بټ(خونه);
	}

	/* Install triggers */
	for_each_trig_ثبت(shmem_بنسټ, trig_شمېر, typeof(*ثبت_2), ثبت_2) {
		recv = (بنية sbi_dbtr_معلومات_پیغام *)(&ثبت_2->معلومات);
		xmit = (بنية sbi_dbtr_id_پیغام *)(&ثبت_2->id);

		/*
		 * The dry-run above matched every requested configuration
		 * to a free hardware trigger slot, so allocation must
		 * succeed.
		 */
		trig = sbi_alloc_trigger(lle_منزل_cpu(recv->tdata1),
					 lle_منزل_cpu(recv->tdata2),
					 lle_منزل_cpu(recv->tdata3));

		dbtr_trigger_تیاری(trig,  recv);
		dbtr_trigger_enable(trig);
		xmit->idx = cpu_منزل_lle(trig->شاخص_2);

	}

	sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)shmem_بنسټ,
					     trig_شمېر * حجم(*ثبت_2));

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_uninstall_trig(بینښه اوږد trig_idx_بنسټ,
			    بینښه اوږد trig_idx_ماسک)
{
	بینښه اوږد trig_ماسک = trig_idx_ماسک << trig_idx_بنسټ;
	بینښه اوږد idx = trig_idx_بنسټ;
	بنية sbi_dbtr_trigger *trig;
	بنية sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	إذا (!hs)
		أرجع SBI_ERR_FAILED;

	for_each_سېټ_بټ_from(idx, &trig_ماسک, hs->ټول_trigs) {
		trig = شاخص_منزل_TRIGGER(idx);
		إذا (!(trig->state & RV_DBTR_بټ_ماسک(TS, MAPPED)))
			أرجع SBI_ERR_INVALID_PARAM;

		dbtr_trigger_clear(trig);

		sbi_free_trigger(trig);
	}

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_enable_trig(بینښه اوږد trig_idx_بنسټ,
			 بینښه اوږد trig_idx_ماسک)
{
	بینښه اوږد trig_ماسک = trig_idx_ماسک << trig_idx_بنسټ;
	بینښه اوږد idx = trig_idx_بنسټ;
	بنية sbi_dbtr_trigger *trig;
	بنية sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	إذا (!hs)
		أرجع SBI_ERR_FAILED;

	for_each_سېټ_بټ_from(idx, &trig_ماسک, hs->ټول_trigs) {
		trig = شاخص_منزل_TRIGGER(idx);
		sbi_dprintf("%s: enable trigger %lu\n", __func__, idx);
		dbtr_trigger_enable(trig);
	}

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_update_trig(بینښه اوږد smode,
			 بینښه اوږد trig_شمېر)
{
	بینښه اوږد trig_idx;
	بنية sbi_dbtr_trigger *trig;
	اتحاد sbi_dbtr_shmem_ثبت *ثبت_2;
	فراغ *shmem_بنسټ = عدم;
	بنية sbi_dbtr_hart_triggers_state *hs = عدم;
	bool tdata2_impl, tdata3_impl;

	hs = dbtr_thishart_state_ptr();
	إذا (!hs)
		أرجع SBI_ERR_FAILED;

	إذا (sbi_dbtr_shmem_disabled(hs))
		أرجع SBI_ERR_NO_SHMEM;

	shmem_بنسټ = hart_shmem_بنسټ(hs);

	إذا (trig_شمېر >= hs->ټول_trigs)
		أرجع SBI_ERR_BAD_RANGE;

	/*
	 * SBI v3.0 sec 19.5 requires SBI_ERR_NOT_SUPPORTED when a trigger
	 * programs a non-zero value into an unimplemented optional CSR. Only
	 * the "whole CSR unimplemented" case is caught; WARL bits tied off
	 * inside an otherwise-implemented CSR are not.
	 */
	tdata2_impl = tdata_implemented(CSR_TDATA2);
	tdata3_impl = tdata_implemented(CSR_TDATA3);

	for_each_trig_ثبت(shmem_بنسټ, trig_شمېر, typeof(*ثبت_2), ثبت_2) {
		sbi_hart_protection_لنډمهاله_map_range((بینښه اوږد)ثبت_2, حجم(*ثبت_2));
		trig_idx = ثبت_2->id.idx;

		إذا (trig_idx >= hs->ټول_trigs) {
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)ثبت_2, حجم(*ثبت_2));
			أرجع SBI_ERR_INVALID_PARAM;
		}

		trig = شاخص_منزل_TRIGGER(trig_idx);

		إذا (!(trig->state & RV_DBTR_بټ_ماسک(TS, MAPPED))) {
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)ثبت_2, حجم(*ثبت_2));
			أرجع SBI_ERR_FAILED;
		}

		إذا ((ثبت_2->معلومات.tdata2 && !tdata2_impl) ||
		    (ثبت_2->معلومات.tdata3 && !tdata3_impl)) {
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)ثبت_2, حجم(*ثبت_2));
			أرجع SBI_ERR_NOT_SUPPORTED;
		}

		إذا (!dbtr_trigger_hw_supported(trig->شاخص_2,
					       lle_منزل_cpu(ثبت_2->معلومات.tdata1),
					       lle_منزل_cpu(ثبت_2->معلومات.tdata2),
					       lle_منزل_cpu(ثبت_2->معلومات.tdata3))) {
			sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)ثبت_2, حجم(*ثبت_2));
			أرجع SBI_ERR_NOT_SUPPORTED;
		}

		dbtr_trigger_تیاری(trig, &ثبت_2->معلومات);
		sbi_hart_protection_لنډمهاله_unmap_range((بینښه اوږد)ثبت_2, حجم(*ثبت_2));
		dbtr_trigger_enable(trig);
	}

	أرجع SBI_SUCCESS;
}

صحيح sbi_dbtr_disable_trig(بینښه اوږد trig_idx_بنسټ,
			  بینښه اوږد trig_idx_ماسک)
{
	بینښه اوږد trig_ماسک = trig_idx_ماسک << trig_idx_بنسټ;
	بینښه اوږد idx = trig_idx_بنسټ;
	بنية sbi_dbtr_trigger *trig;
	بنية sbi_dbtr_hart_triggers_state *hs;

	hs = dbtr_thishart_state_ptr();
	إذا (!hs)
		أرجع SBI_ERR_FAILED;

	for_each_سېټ_بټ_from(idx, &trig_ماسک, hs->ټول_trigs) {
		trig = شاخص_منزل_TRIGGER(idx);
		dbtr_trigger_disable(trig);
	}

	أرجع SBI_SUCCESS;
}
