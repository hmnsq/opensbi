/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_csr_detect.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi/sbi_irqchip.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi_utils/irqchip/imsic.h>

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
করো { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} যখন (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
করো { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} যখন (0)

#define imsic_csr_clear(__c, __v)	\
করো { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} যখন (0)

স্থির unsigned long imsic_ptr_offset;

#define imsic_get_hart_তথ্য_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, imsic_ptr_offset)

#define imsic_set_hart_তথ্য_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), শূন্য *, imsic_ptr_offset, (__imsic))

স্থির unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

পূর্ণসংখ্যা imsic_map_hartid_to_তথ্য(u32 hartid, কাঠামো imsic_তথ্য *imsic, পূর্ণসংখ্যা file)
{
	কাঠামো sbi_scratch *scratch;

	যদি (!imsic || !imsic->targets_mmode)
		ফেরত SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	যদি (!scratch)
		ফেরত 0;

	imsic_set_hart_তথ্য_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	ফেরত 0;
}

কাঠামো imsic_তথ্য *imsic_get_তথ্য(u32 hartindex)
{
	কাঠামো sbi_scratch *scratch;

	যদি (!imsic_ptr_offset)
		ফেরত ফাঁকা;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ফেরত ফাঁকা;

	ফেরত imsic_get_hart_তথ্য_ptr(scratch);
}

পূর্ণসংখ্যা imsic_get_target_file(u32 hartindex)
{
	কাঠামো sbi_scratch *scratch;

	যদি (!imsic_file_offset)
		ফেরত SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	যদি (!scratch)
		ফেরত SBI_ENOENT;

	ফেরত imsic_get_hart_file(scratch);
}

স্থির পূর্ণসংখ্যা imsic_process_hwirqs(কাঠামো sbi_irqchip_device *chip)
{
	কাঠামো imsic_তথ্য *imsic;
	ulong mirq;
	u32 hwirq;
	পূর্ণসংখ্যা rc, ret = 0;

	imsic = container_of(chip, কাঠামো imsic_তথ্য, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ফেরত SBI_EINVAL;

	যদি (imsic_get_তথ্য(current_hartindex()) != imsic)
		ফেরত 0;

	যখন ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		যদি (hwirq > imsic->অঙ্ক_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->অঙ্ক_ids);
			চালু;
		}

		যদি (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			চালু;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		যদি (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	ফেরত ret;
}

স্থির শূন্য imsic_ipi_send(u32 hart_সূচক)
{
	unsigned long reloff;
	কাঠামো imsic_regs *regs;
	কাঠামো imsic_তথ্য *তথ্য;
	কাঠামো sbi_scratch *scratch;
	পূর্ণসংখ্যা file;

	scratch = sbi_hartindex_to_scratch(hart_সূচক);
	যদি (!scratch)
		ফেরত;

	তথ্য = imsic_get_hart_তথ্য_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	যদি (!তথ্য || !তথ্য->targets_mmode)
		ফেরত;

	regs = &তথ্য->regs[0];
	reloff = file * (1UL << তথ্য->guest_সূচক_bits) * IMSIC_MMIO_PAGE_SZ;
	যখন (regs->আকার_2 && (regs->আকার_2 <= reloff)) {
		reloff -= regs->আকার_2;
		regs++;
	}

	যদি (regs->আকার_2 && (reloff < regs->আকার_2))
		writel_relaxed(IMSIC_IPI_ID,
			(শূন্য *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

স্থির কাঠামো sbi_ipi_device imsic_ipi_device = {
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
	কাঠামো sbi_trap_info trap = { 0 };

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
		ফেরত;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

স্থির পূর্ণসংখ্যা imsic_warm_irqchip_init(কাঠামো sbi_irqchip_device *dev)
{
	কাঠামো imsic_তথ্য *imsic;
	কাঠামো imsic_তথ্য *hart_imsic;
	পূর্ণসংখ্যা i;

	imsic = container_of(dev, কাঠামো imsic_তথ্য, irqchip);
	hart_imsic = imsic_get_তথ্য(current_hartindex());

	/* Sanity checks */
	যদি (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		ফেরত SBI_EINVAL;

	/* enable interrutps based on the irq state */
	জন্য (i = 1; i < imsic->অঙ্ক_ids; i++) {
		যদি (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		অন্যথা
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	ফেরত 0;
}

পূর্ণসংখ্যা imsic_তথ্য_check(কাঠামো imsic_তথ্য *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	যদি (!imsic ||
	    (imsic->অঙ্ক_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->অঙ্ক_ids))
		ফেরত SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	যদি (tmp < imsic->guest_সূচক_bits)
		ফেরত SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_সূচক_bits;
	যদি (tmp < imsic->hart_সূচক_bits)
		ফেরত SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_সূচক_bits - imsic->hart_সূচক_bits;
	যদি (tmp < imsic->group_সূচক_bits)
		ফেরত SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_সূচক_bits +
	      imsic->hart_সূচক_bits;
	যদি (imsic->group_সূচক_shift < tmp)
		ফেরত SBI_EINVAL;
	tmp = imsic->group_সূচক_bits + imsic->group_সূচক_shift - 1;
	যদি (tmp >= BITS_PER_LONG)
		ফেরত SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	যদি ((imsic->অঙ্ক_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		ফেরত SBI_EINVAL;

	/* We should have at least one regset */
	যদি (!imsic->regs[0].আকার_2)
		ফেরত SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_সূচক_bits +
				 imsic->hart_সূচক_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_সূচক_bits) - 1) <<
			imsic->group_সূচক_shift);
	জন্য (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].আকার_2; i++) {
		mask = (1UL << imsic->guest_সূচক_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		যদি (imsic->regs[i].আকার_2 & mask)
			ফেরত SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_সূচক_bits +
					 imsic->hart_সূচক_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_সূচক_bits) - 1) <<
				imsic->group_সূচক_shift);
		যদি (base_addr != addr)
			ফেরত SBI_EINVAL;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা imsic_hwirq_setup(কাঠামো sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	যদি (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		ফেরত SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	ফেরত 0;
}

স্থির শূন্য imsic_hwirq_cleanup(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো imsic_তথ্য *imsic;

	যদি (!chip)
		ফেরত;

	imsic = container_of(chip, কাঠামো imsic_তথ্য, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ফেরত;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

স্থির শূন্য imsic_hwirq_eoi(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

স্থির পূর্ণসংখ্যা imsic_compose_msi_বার্তা(কাঠামো imsic_তথ্য *imsic,
				 u32 hart_সূচক, u32 eiid,
				 কাঠামো sbi_irqchip_msi_বার্তা *বার্তা)
{
	কাঠামো imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	পূর্ণসংখ্যা file;

	যদি (!imsic || !বার্তা)
		ফেরত SBI_EINVAL;

	যদি (!eiid || eiid == IMSIC_IPI_ID)
		ফেরত 0;

	file = imsic_get_target_file(hart_সূচক);
	যদি (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_সূচক, file);
		ফেরত file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_সূচক_bits) * IMSIC_MMIO_PAGE_SZ;

	যখন (regs->আকার_2 && regs->আকার_2 <= reloff) {
		reloff -= regs->আকার_2;
		regs++;
	}

	যদি (!regs->আকার_2 || regs->আকার_2 <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_সূচক, file, reloff);
		ফেরত SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	বার্তা->address_lo = (u32)(msi_addr);
	বার্তা->address_hi = (u32)(msi_addr >> 32);
	বার্তা->তথ্য       = eiid;

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা imsic_program_msi(কাঠামো sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_সূচক)
{
	কাঠামো imsic_তথ্য *imsic;
	কাঠামো sbi_irqchip_msi_বার্তা বার্তা;
	পূর্ণসংখ্যা rc;

	যদি (!chip)
		ফেরত SBI_EINVAL;

	imsic = imsic_get_তথ্য(hart_সূচক);
	যদি (!imsic || !imsic->targets_mmode)
		ফেরত SBI_ENODEV;

	rc = imsic_compose_msi_বার্তা(imsic, hart_সূচক, eiid, &বার্তা);
	যদি (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_সূচক, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &বার্তা);
	যদি (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_সূচক, rc);
		ফেরত rc;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা imsic_hwirq_set_affinity(কাঠামো sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_সূচক)
{
	কাঠামো imsic_তথ্য *imsic;
	পূর্ণসংখ্যা rc;

	যদি (!chip)
		ফেরত SBI_EINVAL;

	imsic = container_of(chip, কাঠামো imsic_তথ্য, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ফেরত SBI_EINVAL;

	যদি (!hwirq || hwirq == IMSIC_IPI_ID)
		ফেরত 0;

	যদি (!sbi_hartmask_test_hartindex(hart_সূচক, &chip->target_harts))
		ফেরত SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_সূচক);
	যদি (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_সূচক, rc);
		ফেরত rc;
	}

	ফেরত 0;
}

স্থির শূন্য imsic_hwirq_mask(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো imsic_তথ্য *imsic;

	যদি (!chip)
		ফেরত;

	imsic = container_of(chip, কাঠামো imsic_তথ্য, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ফেরত;

	imsic_local_eix_update(hwirq, false, false);
}

স্থির শূন্য imsic_hwirq_unmask(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো imsic_তথ্য *imsic;

	যদি (!chip)
		ফেরত;

	imsic = container_of(chip, কাঠামো imsic_তথ্য, irqchip);
	যদি (!imsic || !imsic->targets_mmode)
		ফেরত;

	যদি (!hwirq || hwirq == IMSIC_IPI_ID)
		ফেরত;

	imsic_local_eix_update(hwirq, false, true);
}

স্থির কাঠামো sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

পূর্ণসংখ্যা imsic_cold_irqchip_init(কাঠামো imsic_তথ্য *imsic)
{
	পূর্ণসংখ্যা i, rc;

	/* Sanity checks */
	rc = imsic_তথ্য_check(imsic);
	যদি (rc)
		ফেরত rc;

	/* We only initialize M-mode IMSIC */
	যদি (!imsic->targets_mmode)
		ফেরত SBI_EINVAL;

	/* Allocate scratch space pointer */
	যদি (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!imsic_ptr_offset)
			ফেরত SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	যদি (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		যদি (!imsic_file_offset)
			ফেরত SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	জন্য (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].আকার_2; i++) {
		rc = sbi_domain_মূল_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].আকার_2,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		যদি (rc)
			ফেরত rc;
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
		ফেরত rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	যদি (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		ফেরত rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	ফেরত 0;
}
