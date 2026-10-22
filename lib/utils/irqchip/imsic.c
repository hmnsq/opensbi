/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_csr_detect.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_irqchip.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi_utils/irqchip/imsic.h>

#परिभाषय IMSIC_MMIO_PAGE_LE		0x00
#परिभाषय IMSIC_MMIO_PAGE_BE		0x04

#परिभाषय IMSIC_MIN_ID			63
#परिभाषय IMSIC_MAX_ID			2047

#परिभाषय IMSIC_EIDELIVERY		0x70

#परिभाषय IMSIC_EITHRESHOLD		0x72

#परिभाषय IMSIC_TOPEI			0x76
#परिभाषय IMSIC_TOPEI_ID_सरणम्		16
#परिभाषय IMSIC_TOPEI_ID_आवरणम्		0x7ff
#परिभाषय IMSIC_TOPEI_PRIO_आवरणम्		0x7ff

#परिभाषय IMSIC_EIP0			0x80

#परिभाषय IMSIC_EIP63			0xbf

#परिभाषय IMSIC_EIPx_द्वयङ्काः			32

#परिभाषय IMSIC_EIE0			0xc0

#परिभाषय IMSIC_EIE63			0xff

#परिभाषय IMSIC_EIEx_द्वयङ्काः			32

#परिभाषय IMSIC_DISABLE_EIDELIVERY	0
#परिभाषय IMSIC_ENABLE_EIDELIVERY		1
#परिभाषय IMSIC_DISABLE_EITHRESHOLD	1
#परिभाषय IMSIC_ENABLE_EITHRESHOLD	0

#परिभाषय IMSIC_IPI_ID			1

#परिभाषय imsic_csr_write(__c, __v)	\
कुरु { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} यावत् (0)

#परिभाषय imsic_csr_read(__c)	\
({ \
	अचिह्नित दीर्घ __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#परिभाषय imsic_csr_समुच्चयः(__c, __v)		\
कुरु { \
	csr_write(CSR_MISELECT, __c); \
	csr_समुच्चयः(CSR_MIREG, __v); \
} यावत् (0)

#परिभाषय imsic_csr_clear(__c, __v)	\
कुरु { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} यावत् (0)

स्थिर अचिह्नित दीर्घ imsic_ptr_विस्थापनम्;

#परिभाषय imsic_get_hart_दत्त_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, imsic_ptr_विस्थापनम्)

#परिभाषय imsic_समुच्चयः_hart_दत्त_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), शून्य *, imsic_ptr_विस्थापनम्, (__imsic))

स्थिर अचिह्नित दीर्घ imsic_file_विस्थापनम्;

#परिभाषय imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), दीर्घ, imsic_file_विस्थापनम्)

#परिभाषय imsic_समुच्चयः_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), दीर्घ, imsic_file_विस्थापनम्, (__file))

पूर्णाङ्क imsic_map_hartid_गन्तव्यम्_दत्त(u32 hartid, संरचना imsic_दत्त *imsic, पूर्णाङ्क file)
{
	संरचना sbi_scratch *scratch;

	यदि (!imsic || !imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_गन्तव्यम्_scratch(hartid);
	यदि (!scratch)
		प्रत्यागम 0;

	imsic_समुच्चयः_hart_दत्त_ptr(scratch, imsic);
	imsic_समुच्चयः_hart_file(scratch, file);
	प्रत्यागम 0;
}

संरचना imsic_दत्त *imsic_get_दत्त(u32 hartindex)
{
	संरचना sbi_scratch *scratch;

	यदि (!imsic_ptr_विस्थापनम्)
		प्रत्यागम रिक्त;

	scratch = sbi_hartindex_गन्तव्यम्_scratch(hartindex);
	यदि (!scratch)
		प्रत्यागम रिक्त;

	प्रत्यागम imsic_get_hart_दत्त_ptr(scratch);
}

पूर्णाङ्क imsic_get_target_file(u32 hartindex)
{
	संरचना sbi_scratch *scratch;

	यदि (!imsic_file_विस्थापनम्)
		प्रत्यागम SBI_ENOENT;

	scratch = sbi_hartindex_गन्तव्यम्_scratch(hartindex);
	यदि (!scratch)
		प्रत्यागम SBI_ENOENT;

	प्रत्यागम imsic_get_hart_file(scratch);
}

स्थिर पूर्णाङ्क imsic_process_hwirqs(संरचना sbi_irqchip_device *chip)
{
	संरचना imsic_दत्त *imsic;
	ulong mirq;
	u32 hwirq;
	पूर्णाङ्क rc, ret = 0;

	imsic = container_सम्बन्धि(chip, संरचना imsic_दत्त, irqchip);
	यदि (!imsic || !imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	यदि (imsic_get_दत्त(current_hartindex()) != imsic)
		प्रत्यागम 0;

	यावत् ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_सरणम्) &
			IMSIC_TOPEI_ID_आवरणम्;

		यदि (hwirq > imsic->अङ्कः_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(अचिह्नित दीर्घ)hwirq,
					(अचिह्नित दीर्घ)imsic->अङ्कः_ids);
			अनुवर्त;
		}

		यदि (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			अनुवर्त;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		यदि (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(अचिह्नित दीर्घ)hwirq, rc);
			ret = rc;
		}
	}

	प्रत्यागम ret;
}

स्थिर शून्य imsic_ipi_send(u32 hart_सूचकांक)
{
	अचिह्नित दीर्घ reloff;
	संरचना imsic_regs *regs;
	संरचना imsic_दत्त *दत्त;
	संरचना sbi_scratch *scratch;
	पूर्णाङ्क file;

	scratch = sbi_hartindex_गन्तव्यम्_scratch(hart_सूचकांक);
	यदि (!scratch)
		प्रत्यागम;

	दत्त = imsic_get_hart_दत्त_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	यदि (!दत्त || !दत्त->targets_mmode)
		प्रत्यागम;

	regs = &दत्त->regs[0];
	reloff = file * (1UL << दत्त->guest_सूचकांक_द्वयङ्काः) * IMSIC_MMIO_PAGE_SZ;
	यावत् (regs->आकार && (regs->आकार <= reloff)) {
		reloff -= regs->आकार;
		regs++;
	}

	यदि (regs->आकार && (reloff < regs->आकार))
		writel_relaxed(IMSIC_IPI_ID,
			(शून्य *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

स्थिर संरचना sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

स्थिर शून्य imsic_local_eix_update(अचिह्नित दीर्घ id,
				   bool pend, bool val)
{
	अचिह्नित दीर्घ isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_द्वयङ्काः;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	यदि (val)
		imsic_csr_समुच्चयः(isel, ireg);
	अन्यथा
		imsic_csr_clear(isel, ireg);
}

शून्य imsic_local_irqchip_init(शून्य)
{
	संरचना sbi_trap_info trap = { 0 };

	/*
	 * This function is expected to be called from:
	 * 1) nascent_init() platform callback which is called
	 *    very early on each HART in boot-up path and and
	 *    HSM resume path.
	 * 2) irqchip_init() platform callback which is called
	 *    in boot-up path.
	 */

	/* If Smaia not available then do nothing */
	csr_read_allowed(CSR_MTOPI, &trap);
	यदि (trap.cause)
		प्रत्यागम;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

स्थिर पूर्णाङ्क imsic_warm_irqchip_init(संरचना sbi_irqchip_device *dev)
{
	संरचना imsic_दत्त *imsic;
	संरचना imsic_दत्त *hart_imsic;
	पूर्णाङ्क i;

	imsic = container_सम्बन्धि(dev, संरचना imsic_दत्त, irqchip);
	hart_imsic = imsic_get_दत्त(current_hartindex());

	/* Sanity checks */
	यदि (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	/* enable interrutps based on the irq state */
	प्रति (i = 1; i < imsic->अङ्कः_ids; i++) {
		यदि (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		अन्यथा
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	प्रत्यागम 0;
}

पूर्णाङ्क imsic_दत्त_check(संरचना imsic_दत्त *imsic)
{
	u32 i, tmp;
	अचिह्नित दीर्घ आधारः_addr, addr, आवरणम्;

	/* Sanity checks */
	यदि (!imsic ||
	    (imsic->अङ्कः_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->अङ्कः_ids))
		प्रत्यागम SBI_EINVAL;

	tmp = द्वयङ्काः_प्रत्येकम्_LONG - IMSIC_MMIO_PAGE_सरणम्;
	यदि (tmp < imsic->guest_सूचकांक_द्वयङ्काः)
		प्रत्यागम SBI_EINVAL;

	tmp = द्वयङ्काः_प्रत्येकम्_LONG - IMSIC_MMIO_PAGE_सरणम् -
	      imsic->guest_सूचकांक_द्वयङ्काः;
	यदि (tmp < imsic->hart_सूचकांक_द्वयङ्काः)
		प्रत्यागम SBI_EINVAL;

	tmp = द्वयङ्काः_प्रत्येकम्_LONG - IMSIC_MMIO_PAGE_सरणम् -
	      imsic->guest_सूचकांक_द्वयङ्काः - imsic->hart_सूचकांक_द्वयङ्काः;
	यदि (tmp < imsic->गणः_सूचकांक_द्वयङ्काः)
		प्रत्यागम SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_सरणम् + imsic->guest_सूचकांक_द्वयङ्काः +
	      imsic->hart_सूचकांक_द्वयङ्काः;
	यदि (imsic->गणः_सूचकांक_सरणम् < tmp)
		प्रत्यागम SBI_EINVAL;
	tmp = imsic->गणः_सूचकांक_द्वयङ्काः + imsic->गणः_सूचकांक_सरणम् - 1;
	यदि (tmp >= द्वयङ्काः_प्रत्येकम्_LONG)
		प्रत्यागम SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	यदि ((imsic->अङ्कः_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		प्रत्यागम SBI_EINVAL;

	/* We should have at least one regset */
	यदि (!imsic->regs[0].आकार)
		प्रत्यागम SBI_EINVAL;

	/* Match patter of each regset */
	आधारः_addr = imsic->regs[0].addr;
	आधारः_addr &= ~((1UL << (imsic->guest_सूचकांक_द्वयङ्काः +
				 imsic->hart_सूचकांक_द्वयङ्काः +
				 IMSIC_MMIO_PAGE_सरणम्)) - 1);
	आधारः_addr &= ~(((1UL << imsic->गणः_सूचकांक_द्वयङ्काः) - 1) <<
			imsic->गणः_सूचकांक_सरणम्);
	प्रति (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].आकार; i++) {
		आवरणम् = (1UL << imsic->guest_सूचकांक_द्वयङ्काः) * IMSIC_MMIO_PAGE_SZ;
		आवरणम् -= 1UL;
		यदि (imsic->regs[i].आकार & आवरणम्)
			प्रत्यागम SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_सूचकांक_द्वयङ्काः +
					 imsic->hart_सूचकांक_द्वयङ्काः +
					 IMSIC_MMIO_PAGE_सरणम्)) - 1);
		addr &= ~(((1UL << imsic->गणः_सूचकांक_द्वयङ्काः) - 1) <<
				imsic->गणः_सूचकांक_सरणम्);
		यदि (आधारः_addr != addr)
			प्रत्यागम SBI_EINVAL;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क imsic_hwirq_सज्जता(संरचना sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	यदि (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		प्रत्यागम SBI_ENOTSUPP;

	sbi_irqchip_समुच्चयः_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	प्रत्यागम 0;
}

स्थिर शून्य imsic_hwirq_cleanup(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना imsic_दत्त *imsic;

	यदि (!chip)
		प्रत्यागम;

	imsic = container_सम्बन्धि(chip, संरचना imsic_दत्त, irqchip);
	यदि (!imsic || !imsic->targets_mmode)
		प्रत्यागम;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

स्थिर शून्य imsic_hwirq_eoi(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

स्थिर पूर्णाङ्क imsic_compose_msi_सन्देशः(संरचना imsic_दत्त *imsic,
				 u32 hart_सूचकांक, u32 eiid,
				 संरचना sbi_irqchip_msi_सन्देशः *सन्देशः)
{
	संरचना imsic_regs *regs;
	अचिह्नित दीर्घ reloff;
	u64 msi_addr;
	पूर्णाङ्क file;

	यदि (!imsic || !सन्देशः)
		प्रत्यागम SBI_EINVAL;

	यदि (!eiid || eiid == IMSIC_IPI_ID)
		प्रत्यागम 0;

	file = imsic_get_target_file(hart_सूचकांक);
	यदि (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(अचिह्नित दीर्घ)hart_सूचकांक, file);
		प्रत्यागम file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_सूचकांक_द्वयङ्काः) * IMSIC_MMIO_PAGE_SZ;

	यावत् (regs->आकार && regs->आकार <= reloff) {
		reloff -= regs->आकार;
		regs++;
	}

	यदि (!regs->आकार || regs->आकार <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (अचिह्नित दीर्घ)hart_सूचकांक, file, reloff);
		प्रत्यागम SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	सन्देशः->स्थानाङ्कः_lo = (u32)(msi_addr);
	सन्देशः->स्थानाङ्कः_hi = (u32)(msi_addr >> 32);
	सन्देशः->दत्त       = eiid;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क imsic_क्रमादेशः_msi(संरचना sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_सूचकांक)
{
	संरचना imsic_दत्त *imsic;
	संरचना sbi_irqchip_msi_सन्देशः सन्देशः;
	पूर्णाङ्क rc;

	यदि (!chip)
		प्रत्यागम SBI_EINVAL;

	imsic = imsic_get_दत्त(hart_सूचकांक);
	यदि (!imsic || !imsic->targets_mmode)
		प्रत्यागम SBI_ENODEV;

	rc = imsic_compose_msi_सन्देशः(imsic, hart_सूचकांक, eiid, &सन्देशः);
	यदि (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(अचिह्नित दीर्घ)eiid,
				(अचिह्नित दीर्घ)hart_सूचकांक, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &सन्देशः);
	यदि (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(अचिह्नित दीर्घ)eiid,
				(अचिह्नित दीर्घ)hart_सूचकांक, rc);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क imsic_hwirq_समुच्चयः_affinity(संरचना sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_सूचकांक)
{
	संरचना imsic_दत्त *imsic;
	पूर्णाङ्क rc;

	यदि (!chip)
		प्रत्यागम SBI_EINVAL;

	imsic = container_सम्बन्धि(chip, संरचना imsic_दत्त, irqchip);
	यदि (!imsic || !imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	यदि (!hwirq || hwirq == IMSIC_IPI_ID)
		प्रत्यागम 0;

	यदि (!sbi_hartmask_test_hartindex(hart_सूचकांक, &chip->target_harts))
		प्रत्यागम SBI_EINVAL;

	rc = imsic_क्रमादेशः_msi(chip, hwirq, hart_सूचकांक);
	यदि (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(अचिह्नित दीर्घ)hwirq,
				(अचिह्नित दीर्घ)hart_सूचकांक, rc);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर शून्य imsic_hwirq_आवरणम्(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना imsic_दत्त *imsic;

	यदि (!chip)
		प्रत्यागम;

	imsic = container_सम्बन्धि(chip, संरचना imsic_दत्त, irqchip);
	यदि (!imsic || !imsic->targets_mmode)
		प्रत्यागम;

	imsic_local_eix_update(hwirq, false, false);
}

स्थिर शून्य imsic_hwirq_unmask(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना imsic_दत्त *imsic;

	यदि (!chip)
		प्रत्यागम;

	imsic = container_सम्बन्धि(chip, संरचना imsic_दत्त, irqchip);
	यदि (!imsic || !imsic->targets_mmode)
		प्रत्यागम;

	यदि (!hwirq || hwirq == IMSIC_IPI_ID)
		प्रत्यागम;

	imsic_local_eix_update(hwirq, false, true);
}

स्थिर संरचना sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_सज्जता		= imsic_hwirq_सज्जता,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_समुच्चयः_affinity	= imsic_hwirq_समुच्चयः_affinity,
	.hwirq_आवरणम्		= imsic_hwirq_आवरणम्,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

पूर्णाङ्क imsic_cold_irqchip_init(संरचना imsic_दत्त *imsic)
{
	पूर्णाङ्क i, rc;

	/* Sanity checks */
	rc = imsic_दत्त_check(imsic);
	यदि (rc)
		प्रत्यागम rc;

	/* We only initialize M-mode IMSIC */
	यदि (!imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	/* Allocate scratch space pointer */
	यदि (!imsic_ptr_विस्थापनम्) {
		imsic_ptr_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(शून्य *);
		यदि (!imsic_ptr_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	यदि (!imsic_file_विस्थापनम्) {
		imsic_file_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(दीर्घ);
		यदि (!imsic_file_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	प्रति (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].आकार; i++) {
		rc = sbi_domain_मूलम्_सङ्कलय_memrange(imsic->regs[i].addr,
						  imsic->regs[i].आकार,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		यदि (rc)
			प्रत्यागम rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.अङ्कः_hwirq = imsic->अङ्कः_ids + 1;

	sbi_hartmask_समुच्चयः_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_सङ्कलय_device(&imsic->irqchip);
	यदि (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		प्रत्यागम rc;
	}

	rc = sbi_irqchip_पञ्जिकाकोशः_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	यदि (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		प्रत्यागम rc;
	}

	/* Register IPI device */
	sbi_ipi_सङ्कलय_device(&imsic_ipi_device);

	प्रत्यागम 0;
}
