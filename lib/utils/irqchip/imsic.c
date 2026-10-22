/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/riscv_encoding.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_csr_detect.h>
#অন্তর্ভাৱ <sbi/sbi_domain.h>
#অন্তর্ভাৱ <sbi/sbi_ipi.h>
#অন্তর্ভাৱ <sbi/sbi_irqchip.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi_utils/irqchip/imsic.h>

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
কৰা { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} যেতিয়ালৈ (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
কৰা { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} যেতিয়ালৈ (0)

#define imsic_csr_clear(__c, __v)	\
কৰা { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} যেতিয়ালৈ (0)

স্থির unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), শূন্য *, imsic_ptr_offset, (__imsic))

স্থির unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

পূর্ণাঙ্ক imsic_map_hartid_to_data(u32 hartid, সংরচনা imsic_data *imsic, পূর্ণাঙ্ক file)
{
	সংরচনা sbi_scratch *scratch;

	যদি (!imsic || !imsic->targets_mmode)
		ঘূৰাই SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	যদি (!scratch)
		ঘূৰাই 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	ঘূৰাই 0;
}

সংরচনা imsic_data *imsic_get_data(u32 hartindex)
{
	সংরচনা sbi_scratch *scratch;

	যদি (!imsic_ptr_offset)
		ঘূৰাই রিক্ত;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ঘূৰাই রিক্ত;

	ঘূৰাই imsic_get_hart_data_ptr(scratch);
}

পূর্ণাঙ্ক imsic_get_target_file(u32 hartindex)
{
	সংরচনা sbi_scratch *scratch;

	যদি (!imsic_file_offset)
		ঘূৰাই SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ঘূৰাই SBI_ENOENT;

	ঘূৰাই imsic_get_hart_file(scratch);
}

স্থির পূর্ণাঙ্ক imsic_process_hwirqs(সংরচনা sbi_irqchip_device *chip)
{
	সংরচনা imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	পূর্ণাঙ্ক rc, ret = 0;

	imsic = container_of(chip, সংরচনা imsic_data, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ঘূৰাই SBI_EINVAL;

	যদি (imsic_get_data(current_hartindex()) != imsic)
		ঘূৰাই 0;

	যেতিয়ালৈ ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		যদি (hwirq > imsic->অঙ্ক_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->অঙ্ক_ids);
			অনুৱর্ত;
		}

		যদি (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			অনুৱর্ত;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		যদি (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	ঘূৰাই ret;
}

স্থির শূন্য imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	সংরচনা imsic_regs *regs;
	সংরচনা imsic_data *data;
	সংরচনা sbi_scratch *scratch;
	পূর্ণাঙ্ক file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	যদি (!scratch)
		ঘূৰাই;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	যদি (!data || !data->targets_mmode)
		ঘূৰাই;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	যেতিয়ালৈ (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	যদি (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(শূন্য *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

স্থির সংরচনা sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

স্থির শূন্য imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	যদি (val)
		imsic_csr_set(isel, ireg);
	অন্যথা
		imsic_csr_clear(isel, ireg);
}

শূন্য imsic_local_irqchip_init(শূন্য)
{
	সংরচনা sbi_trap_info trap = { 0 };

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
	যদি (trap.cause)
		ঘূৰাই;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

স্থির পূর্ণাঙ্ক imsic_warm_irqchip_init(সংরচনা sbi_irqchip_device *dev)
{
	সংরচনা imsic_data *imsic;
	সংরচনা imsic_data *hart_imsic;
	পূর্ণাঙ্ক i;

	imsic = container_of(dev, সংরচনা imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	যদি (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		ঘূৰাই SBI_EINVAL;

	/* enable interrutps based on the irq state */
	বাবে (i = 1; i < imsic->অঙ্ক_ids; i++) {
		যদি (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		অন্যথা
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	ঘূৰাই 0;
}

পূর্ণাঙ্ক imsic_data_check(সংরচনা imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	যদি (!imsic ||
	    (imsic->অঙ্ক_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->অঙ্ক_ids))
		ঘূৰাই SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	যদি (tmp < imsic->guest_index_bits)
		ঘূৰাই SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	যদি (tmp < imsic->hart_index_bits)
		ঘূৰাই SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	যদি (tmp < imsic->group_index_bits)
		ঘূৰাই SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	যদি (imsic->group_index_shift < tmp)
		ঘূৰাই SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	যদি (tmp >= BITS_PER_LONG)
		ঘূৰাই SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	যদি ((imsic->অঙ্ক_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		ঘূৰাই SBI_EINVAL;

	/* We should have at least one regset */
	যদি (!imsic->regs[0].size)
		ঘূৰাই SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	বাবে (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		যদি (imsic->regs[i].size & mask)
			ঘূৰাই SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		যদি (base_addr != addr)
			ঘূৰাই SBI_EINVAL;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক imsic_hwirq_setup(সংরচনা sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	যদি (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		ঘূৰাই SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	ঘূৰাই 0;
}

স্থির শূন্য imsic_hwirq_cleanup(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা imsic_data *imsic;

	যদি (!chip)
		ঘূৰাই;

	imsic = container_of(chip, সংরচনা imsic_data, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ঘূৰাই;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

স্থির শূন্য imsic_hwirq_eoi(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

স্থির পূর্ণাঙ্ক imsic_compose_msi_বাৰ্তা(সংরচনা imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 সংরচনা sbi_irqchip_msi_বাৰ্তা *বাৰ্তা)
{
	সংরচনা imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	পূর্ণাঙ্ক file;

	যদি (!imsic || !বাৰ্তা)
		ঘূৰাই SBI_EINVAL;

	যদি (!eiid || eiid == IMSIC_IPI_ID)
		ঘূৰাই 0;

	file = imsic_get_target_file(hart_index);
	যদি (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		ঘূৰাই file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	যেতিয়ালৈ (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	যদি (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		ঘূৰাই SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	বাৰ্তা->address_lo = (u32)(msi_addr);
	বাৰ্তা->address_hi = (u32)(msi_addr >> 32);
	বাৰ্তা->data       = eiid;

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক imsic_program_msi(সংরচনা sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	সংরচনা imsic_data *imsic;
	সংরচনা sbi_irqchip_msi_বাৰ্তা বাৰ্তা;
	পূর্ণাঙ্ক rc;

	যদি (!chip)
		ঘূৰাই SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	যদি (!imsic || !imsic->targets_mmode)
		ঘূৰাই SBI_ENODEV;

	rc = imsic_compose_msi_বাৰ্তা(imsic, hart_index, eiid, &বাৰ্তা);
	যদি (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &বাৰ্তা);
	যদি (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		ঘূৰাই rc;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক imsic_hwirq_set_affinity(সংরচনা sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	সংরচনা imsic_data *imsic;
	পূর্ণাঙ্ক rc;

	যদি (!chip)
		ঘূৰাই SBI_EINVAL;

	imsic = container_of(chip, সংরচনা imsic_data, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ঘূৰাই SBI_EINVAL;

	যদি (!hwirq || hwirq == IMSIC_IPI_ID)
		ঘূৰাই 0;

	যদি (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		ঘূৰাই SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	যদি (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		ঘূৰাই rc;
	}

	ঘূৰাই 0;
}

স্থির শূন্য imsic_hwirq_mask(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা imsic_data *imsic;

	যদি (!chip)
		ঘূৰাই;

	imsic = container_of(chip, সংরচনা imsic_data, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ঘূৰাই;

	imsic_local_eix_update(hwirq, false, false);
}

স্থির শূন্য imsic_hwirq_unmask(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা imsic_data *imsic;

	যদি (!chip)
		ঘূৰাই;

	imsic = container_of(chip, সংরচনা imsic_data, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ঘূৰাই;

	যদি (!hwirq || hwirq == IMSIC_IPI_ID)
		ঘূৰাই;

	imsic_local_eix_update(hwirq, false, true);
}

স্থির সংরচনা sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

পূর্ণাঙ্ক imsic_cold_irqchip_init(সংরচনা imsic_data *imsic)
{
	পূর্ণাঙ্ক i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	যদি (rc)
		ঘূৰাই rc;

	/* We only initialize M-mode IMSIC */
	যদি (!imsic->targets_mmode)
		ঘূৰাই SBI_EINVAL;

	/* Allocate scratch space pointer */
	যদি (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!imsic_ptr_offset)
			ঘূৰাই SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	যদি (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		যদি (!imsic_file_offset)
			ঘূৰাই SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	বাবে (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_মূল_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		যদি (rc)
			ঘূৰাই rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.অঙ্ক_hwirq = imsic->অঙ্ক_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	যদি (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		ঘূৰাই rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	যদি (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		ঘূৰাই rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	ঘূৰাই 0;
}
