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

#define IMSIC_MMIO_PAGE_LE		0x00
#define IMSIC_MMIO_PAGE_BE		0x04

#define IMSIC_MIN_ID			63
#define IMSIC_MAX_ID			2047

#define IMSIC_EIDELIVERY		0x70

#define IMSIC_EITHRESHOLD		0x72

#define IMSIC_TOPEI			0x76
#define IMSIC_TOPEI_ID_SHIFT		16
#define IMSIC_TOPEI_ID_MASK		0x7ff
#define IMSIC_TOPEI_PRIO_MASK		0x7ff

#define IMSIC_EIP0			0x80

#define IMSIC_EIP63			0xbf

#define IMSIC_EIPx_BITS			32

#define IMSIC_EIE0			0xc0

#define IMSIC_EIE63			0xff

#define IMSIC_EIEx_BITS			32

#define IMSIC_DISABLE_EIDELIVERY	0
#define IMSIC_ENABLE_EIDELIVERY		1
#define IMSIC_DISABLE_EITHRESHOLD	1
#define IMSIC_ENABLE_EITHRESHOLD	0

#define IMSIC_IPI_ID			1

#define imsic_csr_write(__c, __v)	\
करेहि { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} जाव (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
करेहि { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} जाव (0)

#define imsic_csr_clear(__c, __v)	\
करेहि { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} जाव (0)

स्थिर unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), शून्य *, imsic_ptr_offset, (__imsic))

स्थिर unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

पूर्णाङ्क imsic_map_hartid_to_data(u32 hartid, संरचना imsic_data *imsic, पूर्णाङ्क file)
{
	संरचना sbi_scratch *scratch;

	जइ (!imsic || !imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	जइ (!scratch)
		प्रत्यागम 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	प्रत्यागम 0;
}

संरचना imsic_data *imsic_get_data(u32 hartindex)
{
	संरचना sbi_scratch *scratch;

	जइ (!imsic_ptr_offset)
		प्रत्यागम रिक्त;

	scratch = sbi_hartindex_to_scratch(hartindex);
	जइ (!scratch)
		प्रत्यागम रिक्त;

	प्रत्यागम imsic_get_hart_data_ptr(scratch);
}

पूर्णाङ्क imsic_get_target_file(u32 hartindex)
{
	संरचना sbi_scratch *scratch;

	जइ (!imsic_file_offset)
		प्रत्यागम SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	जइ (!scratch)
		प्रत्यागम SBI_ENOENT;

	प्रत्यागम imsic_get_hart_file(scratch);
}

स्थिर पूर्णाङ्क imsic_process_hwirqs(संरचना sbi_irqchip_device *chip)
{
	संरचना imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	पूर्णाङ्क rc, ret = 0;

	imsic = container_of(chip, संरचना imsic_data, irqchip);
	जइ (!imsic || !imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	जइ (imsic_get_data(current_hartindex()) != imsic)
		प्रत्यागम 0;

	जाव ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		जइ (hwirq > imsic->अङ्कः_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->अङ्कः_ids);
			अनुवर्त;
		}

		जइ (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			अनुवर्त;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		जइ (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	प्रत्यागम ret;
}

स्थिर शून्य imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	संरचना imsic_regs *regs;
	संरचना imsic_data *data;
	संरचना sbi_scratch *scratch;
	पूर्णाङ्क file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	जइ (!scratch)
		प्रत्यागम;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	जइ (!data || !data->targets_mmode)
		प्रत्यागम;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	जाव (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	जइ (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(शून्य *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

स्थिर संरचना sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

स्थिर शून्य imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	जइ (val)
		imsic_csr_set(isel, ireg);
	अण्णहा
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
	जइ (trap.cause)
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
	संरचना imsic_data *imsic;
	संरचना imsic_data *hart_imsic;
	पूर्णाङ्क i;

	imsic = container_of(dev, संरचना imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	जइ (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	/* enable interrutps based on the irq state */
	प्रति (i = 1; i < imsic->अङ्कः_ids; i++) {
		जइ (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		अण्णहा
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	प्रत्यागम 0;
}

पूर्णाङ्क imsic_data_check(संरचना imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	जइ (!imsic ||
	    (imsic->अङ्कः_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->अङ्कः_ids))
		प्रत्यागम SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	जइ (tmp < imsic->guest_index_bits)
		प्रत्यागम SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	जइ (tmp < imsic->hart_index_bits)
		प्रत्यागम SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	जइ (tmp < imsic->group_index_bits)
		प्रत्यागम SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	जइ (imsic->group_index_shift < tmp)
		प्रत्यागम SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	जइ (tmp >= BITS_PER_LONG)
		प्रत्यागम SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	जइ ((imsic->अङ्कः_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		प्रत्यागम SBI_EINVAL;

	/* We should have at least one regset */
	जइ (!imsic->regs[0].size)
		प्रत्यागम SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	प्रति (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		जइ (imsic->regs[i].size & mask)
			प्रत्यागम SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		जइ (base_addr != addr)
			प्रत्यागम SBI_EINVAL;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क imsic_hwirq_setup(संरचना sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	जइ (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		प्रत्यागम SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	प्रत्यागम 0;
}

स्थिर शून्य imsic_hwirq_cleanup(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना imsic_data *imsic;

	जइ (!chip)
		प्रत्यागम;

	imsic = container_of(chip, संरचना imsic_data, irqchip);
	जइ (!imsic || !imsic->targets_mmode)
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

स्थिर पूर्णाङ्क imsic_compose_msi_सन्देशः(संरचना imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 संरचना sbi_irqchip_msi_सन्देशः *सन्देशः)
{
	संरचना imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	पूर्णाङ्क file;

	जइ (!imsic || !सन्देशः)
		प्रत्यागम SBI_EINVAL;

	जइ (!eiid || eiid == IMSIC_IPI_ID)
		प्रत्यागम 0;

	file = imsic_get_target_file(hart_index);
	जइ (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		प्रत्यागम file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	जाव (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	जइ (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		प्रत्यागम SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	सन्देशः->address_lo = (u32)(msi_addr);
	सन्देशः->address_hi = (u32)(msi_addr >> 32);
	सन्देशः->data       = eiid;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क imsic_program_msi(संरचना sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	संरचना imsic_data *imsic;
	संरचना sbi_irqchip_msi_सन्देशः सन्देशः;
	पूर्णाङ्क rc;

	जइ (!chip)
		प्रत्यागम SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	जइ (!imsic || !imsic->targets_mmode)
		प्रत्यागम SBI_ENODEV;

	rc = imsic_compose_msi_सन्देशः(imsic, hart_index, eiid, &सन्देशः);
	जइ (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &सन्देशः);
	जइ (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क imsic_hwirq_set_affinity(संरचना sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	संरचना imsic_data *imsic;
	पूर्णाङ्क rc;

	जइ (!chip)
		प्रत्यागम SBI_EINVAL;

	imsic = container_of(chip, संरचना imsic_data, irqchip);
	जइ (!imsic || !imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	जइ (!hwirq || hwirq == IMSIC_IPI_ID)
		प्रत्यागम 0;

	जइ (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		प्रत्यागम SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	जइ (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर शून्य imsic_hwirq_mask(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना imsic_data *imsic;

	जइ (!chip)
		प्रत्यागम;

	imsic = container_of(chip, संरचना imsic_data, irqchip);
	जइ (!imsic || !imsic->targets_mmode)
		प्रत्यागम;

	imsic_local_eix_update(hwirq, false, false);
}

स्थिर शून्य imsic_hwirq_unmask(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना imsic_data *imsic;

	जइ (!chip)
		प्रत्यागम;

	imsic = container_of(chip, संरचना imsic_data, irqchip);
	जइ (!imsic || !imsic->targets_mmode)
		प्रत्यागम;

	जइ (!hwirq || hwirq == IMSIC_IPI_ID)
		प्रत्यागम;

	imsic_local_eix_update(hwirq, false, true);
}

स्थिर संरचना sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

पूर्णाङ्क imsic_cold_irqchip_init(संरचना imsic_data *imsic)
{
	पूर्णाङ्क i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	जइ (rc)
		प्रत्यागम rc;

	/* We only initialize M-mode IMSIC */
	जइ (!imsic->targets_mmode)
		प्रत्यागम SBI_EINVAL;

	/* Allocate scratch space pointer */
	जइ (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		जइ (!imsic_ptr_offset)
			प्रत्यागम SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	जइ (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		जइ (!imsic_file_offset)
			प्रत्यागम SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	प्रति (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_मूलम्_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		जइ (rc)
			प्रत्यागम rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.अङ्कः_hwirq = imsic->अङ्कः_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	जइ (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		प्रत्यागम rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	जइ (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		प्रत्यागम rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	प्रत्यागम 0;
}
