/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_csr_detect.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_irqchip.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi_utils/irqchip/imsic.h>

#घोषणा IMSIC_MMIO_PAGE_LE		0x00
#घोषणा IMSIC_MMIO_PAGE_BE		0x04

#घोषणा IMSIC_MIN_ID			63
#घोषणा IMSIC_MAX_ID			2047

#घोषणा IMSIC_EIDELIVERY		0x70

#घोषणा IMSIC_EITHRESHOLD		0x72

#घोषणा IMSIC_TOPEI			0x76
#घोषणा IMSIC_TOPEI_ID_SHIFT		16
#घोषणा IMSIC_TOPEI_ID_MASK		0x7ff
#घोषणा IMSIC_TOPEI_PRIO_MASK		0x7ff

#घोषणा IMSIC_EIP0			0x80

#घोषणा IMSIC_EIP63			0xbf

#घोषणा IMSIC_EIPx_BITS			32

#घोषणा IMSIC_EIE0			0xc0

#घोषणा IMSIC_EIE63			0xff

#घोषणा IMSIC_EIEx_BITS			32

#घोषणा IMSIC_DISABLE_EIDELIVERY	0
#घोषणा IMSIC_ENABLE_EIDELIVERY		1
#घोषणा IMSIC_DISABLE_EITHRESHOLD	1
#घोषणा IMSIC_ENABLE_EITHRESHOLD	0

#घोषणा IMSIC_IPI_ID			1

#घोषणा imsic_csr_write(__c, __v)	\
करो { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} जबतक (0)

#घोषणा imsic_csr_read(__c)	\
({ \
	अचिन्हित दीर्घ __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#घोषणा imsic_csr_set(__c, __v)		\
करो { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} जबतक (0)

#घोषणा imsic_csr_clear(__c, __v)	\
करो { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} जबतक (0)

अटल अचिन्हित दीर्घ imsic_ptr_offset;

#घोषणा imsic_get_hart_तथ्य_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), व्योम *, imsic_ptr_offset)

#घोषणा imsic_set_hart_तथ्य_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), व्योम *, imsic_ptr_offset, (__imsic))

अटल अचिन्हित दीर्घ imsic_file_offset;

#घोषणा imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), दीर्घ, imsic_file_offset)

#घोषणा imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), दीर्घ, imsic_file_offset, (__file))

पूर्णांक imsic_map_hartid_to_तथ्य(u32 hartid, काष्ठा imsic_तथ्य *imsic, पूर्णांक file)
{
	काष्ठा sbi_scratch *scratch;

	अगर (!imsic || !imsic->targets_mmode)
		वापस SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	अगर (!scratch)
		वापस 0;

	imsic_set_hart_तथ्य_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	वापस 0;
}

काष्ठा imsic_तथ्य *imsic_get_तथ्य(u32 hartindex)
{
	काष्ठा sbi_scratch *scratch;

	अगर (!imsic_ptr_offset)
		वापस शून्य;

	scratch = sbi_hartindex_to_scratch(hartindex);
	अगर (!scratch)
		वापस शून्य;

	वापस imsic_get_hart_तथ्य_ptr(scratch);
}

पूर्णांक imsic_get_target_file(u32 hartindex)
{
	काष्ठा sbi_scratch *scratch;

	अगर (!imsic_file_offset)
		वापस SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	अगर (!scratch)
		वापस SBI_ENOENT;

	वापस imsic_get_hart_file(scratch);
}

अटल पूर्णांक imsic_process_hwirqs(काष्ठा sbi_irqchip_device *chip)
{
	काष्ठा imsic_तथ्य *imsic;
	ulong mirq;
	u32 hwirq;
	पूर्णांक rc, ret = 0;

	imsic = container_of(chip, काष्ठा imsic_तथ्य, irqchip);
	अगर (!imsic || !imsic->targets_mmode)
		वापस SBI_EINVAL;

	अगर (imsic_get_तथ्य(current_hartindex()) != imsic)
		वापस 0;

	जबतक ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		अगर (hwirq > imsic->अंक_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(अचिन्हित दीर्घ)hwirq,
					(अचिन्हित दीर्घ)imsic->अंक_ids);
			जारी;
		}

		अगर (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			जारी;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		अगर (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(अचिन्हित दीर्घ)hwirq, rc);
			ret = rc;
		}
	}

	वापस ret;
}

अटल व्योम imsic_ipi_send(u32 hart_सूचकांक)
{
	अचिन्हित दीर्घ reloff;
	काष्ठा imsic_regs *regs;
	काष्ठा imsic_तथ्य *तथ्य;
	काष्ठा sbi_scratch *scratch;
	पूर्णांक file;

	scratch = sbi_hartindex_to_scratch(hart_सूचकांक);
	अगर (!scratch)
		वापस;

	तथ्य = imsic_get_hart_तथ्य_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	अगर (!तथ्य || !तथ्य->targets_mmode)
		वापस;

	regs = &तथ्य->regs[0];
	reloff = file * (1UL << तथ्य->guest_सूचकांक_bits) * IMSIC_MMIO_PAGE_SZ;
	जबतक (regs->आकार && (regs->आकार <= reloff)) {
		reloff -= regs->आकार;
		regs++;
	}

	अगर (regs->आकार && (reloff < regs->आकार))
		writel_relaxed(IMSIC_IPI_ID,
			(व्योम *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

अटल काष्ठा sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

अटल व्योम imsic_local_eix_update(अचिन्हित दीर्घ id,
				   bool pend, bool val)
{
	अचिन्हित दीर्घ isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	अगर (val)
		imsic_csr_set(isel, ireg);
	अन्यथा
		imsic_csr_clear(isel, ireg);
}

व्योम imsic_local_irqchip_init(व्योम)
{
	काष्ठा sbi_trap_info trap = { 0 };

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
	अगर (trap.cause)
		वापस;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

अटल पूर्णांक imsic_warm_irqchip_init(काष्ठा sbi_irqchip_device *dev)
{
	काष्ठा imsic_तथ्य *imsic;
	काष्ठा imsic_तथ्य *hart_imsic;
	पूर्णांक i;

	imsic = container_of(dev, काष्ठा imsic_तथ्य, irqchip);
	hart_imsic = imsic_get_तथ्य(current_hartindex());

	/* Sanity checks */
	अगर (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		वापस SBI_EINVAL;

	/* enable interrutps based on the irq state */
	क्रम (i = 1; i < imsic->अंक_ids; i++) {
		अगर (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		अन्यथा
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	वापस 0;
}

पूर्णांक imsic_तथ्य_check(काष्ठा imsic_तथ्य *imsic)
{
	u32 i, tmp;
	अचिन्हित दीर्घ base_addr, addr, mask;

	/* Sanity checks */
	अगर (!imsic ||
	    (imsic->अंक_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->अंक_ids))
		वापस SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	अगर (tmp < imsic->guest_सूचकांक_bits)
		वापस SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_सूचकांक_bits;
	अगर (tmp < imsic->hart_सूचकांक_bits)
		वापस SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_सूचकांक_bits - imsic->hart_सूचकांक_bits;
	अगर (tmp < imsic->group_सूचकांक_bits)
		वापस SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_सूचकांक_bits +
	      imsic->hart_सूचकांक_bits;
	अगर (imsic->group_सूचकांक_shift < tmp)
		वापस SBI_EINVAL;
	tmp = imsic->group_सूचकांक_bits + imsic->group_सूचकांक_shift - 1;
	अगर (tmp >= BITS_PER_LONG)
		वापस SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	अगर ((imsic->अंक_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		वापस SBI_EINVAL;

	/* We should have at least one regset */
	अगर (!imsic->regs[0].आकार)
		वापस SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_सूचकांक_bits +
				 imsic->hart_सूचकांक_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_सूचकांक_bits) - 1) <<
			imsic->group_सूचकांक_shift);
	क्रम (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].आकार; i++) {
		mask = (1UL << imsic->guest_सूचकांक_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		अगर (imsic->regs[i].आकार & mask)
			वापस SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_सूचकांक_bits +
					 imsic->hart_सूचकांक_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_सूचकांक_bits) - 1) <<
				imsic->group_सूचकांक_shift);
		अगर (base_addr != addr)
			वापस SBI_EINVAL;
	}

	वापस 0;
}

अटल पूर्णांक imsic_hwirq_setup(काष्ठा sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	अगर (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		वापस SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	वापस 0;
}

अटल व्योम imsic_hwirq_cleanup(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा imsic_तथ्य *imsic;

	अगर (!chip)
		वापस;

	imsic = container_of(chip, काष्ठा imsic_तथ्य, irqchip);
	अगर (!imsic || !imsic->targets_mmode)
		वापस;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

अटल व्योम imsic_hwirq_eoi(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

अटल पूर्णांक imsic_compose_msi_संदेश(काष्ठा imsic_तथ्य *imsic,
				 u32 hart_सूचकांक, u32 eiid,
				 काष्ठा sbi_irqchip_msi_संदेश *संदेश)
{
	काष्ठा imsic_regs *regs;
	अचिन्हित दीर्घ reloff;
	u64 msi_addr;
	पूर्णांक file;

	अगर (!imsic || !संदेश)
		वापस SBI_EINVAL;

	अगर (!eiid || eiid == IMSIC_IPI_ID)
		वापस 0;

	file = imsic_get_target_file(hart_सूचकांक);
	अगर (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(अचिन्हित दीर्घ)hart_सूचकांक, file);
		वापस file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_सूचकांक_bits) * IMSIC_MMIO_PAGE_SZ;

	जबतक (regs->आकार && regs->आकार <= reloff) {
		reloff -= regs->आकार;
		regs++;
	}

	अगर (!regs->आकार || regs->आकार <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (अचिन्हित दीर्घ)hart_सूचकांक, file, reloff);
		वापस SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	संदेश->address_lo = (u32)(msi_addr);
	संदेश->address_hi = (u32)(msi_addr >> 32);
	संदेश->तथ्य       = eiid;

	वापस 0;
}

अटल पूर्णांक imsic_program_msi(काष्ठा sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_सूचकांक)
{
	काष्ठा imsic_तथ्य *imsic;
	काष्ठा sbi_irqchip_msi_संदेश संदेश;
	पूर्णांक rc;

	अगर (!chip)
		वापस SBI_EINVAL;

	imsic = imsic_get_तथ्य(hart_सूचकांक);
	अगर (!imsic || !imsic->targets_mmode)
		वापस SBI_ENODEV;

	rc = imsic_compose_msi_संदेश(imsic, hart_सूचकांक, eiid, &संदेश);
	अगर (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(अचिन्हित दीर्घ)eiid,
				(अचिन्हित दीर्घ)hart_सूचकांक, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &संदेश);
	अगर (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(अचिन्हित दीर्घ)eiid,
				(अचिन्हित दीर्घ)hart_सूचकांक, rc);
		वापस rc;
	}

	वापस 0;
}

अटल पूर्णांक imsic_hwirq_set_affinity(काष्ठा sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_सूचकांक)
{
	काष्ठा imsic_तथ्य *imsic;
	पूर्णांक rc;

	अगर (!chip)
		वापस SBI_EINVAL;

	imsic = container_of(chip, काष्ठा imsic_तथ्य, irqchip);
	अगर (!imsic || !imsic->targets_mmode)
		वापस SBI_EINVAL;

	अगर (!hwirq || hwirq == IMSIC_IPI_ID)
		वापस 0;

	अगर (!sbi_hartmask_test_hartindex(hart_सूचकांक, &chip->target_harts))
		वापस SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_सूचकांक);
	अगर (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(अचिन्हित दीर्घ)hwirq,
				(अचिन्हित दीर्घ)hart_सूचकांक, rc);
		वापस rc;
	}

	वापस 0;
}

अटल व्योम imsic_hwirq_mask(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा imsic_तथ्य *imsic;

	अगर (!chip)
		वापस;

	imsic = container_of(chip, काष्ठा imsic_तथ्य, irqchip);
	अगर (!imsic || !imsic->targets_mmode)
		वापस;

	imsic_local_eix_update(hwirq, false, false);
}

अटल व्योम imsic_hwirq_unmask(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा imsic_तथ्य *imsic;

	अगर (!chip)
		वापस;

	imsic = container_of(chip, काष्ठा imsic_तथ्य, irqchip);
	अगर (!imsic || !imsic->targets_mmode)
		वापस;

	अगर (!hwirq || hwirq == IMSIC_IPI_ID)
		वापस;

	imsic_local_eix_update(hwirq, false, true);
}

अटल काष्ठा sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

पूर्णांक imsic_cold_irqchip_init(काष्ठा imsic_तथ्य *imsic)
{
	पूर्णांक i, rc;

	/* Sanity checks */
	rc = imsic_तथ्य_check(imsic);
	अगर (rc)
		वापस rc;

	/* We only initialize M-mode IMSIC */
	अगर (!imsic->targets_mmode)
		वापस SBI_EINVAL;

	/* Allocate scratch space pointer */
	अगर (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(व्योम *);
		अगर (!imsic_ptr_offset)
			वापस SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	अगर (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(दीर्घ);
		अगर (!imsic_file_offset)
			वापस SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	क्रम (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].आकार; i++) {
		rc = sbi_domain_मूल_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].आकार,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		अगर (rc)
			वापस rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.अंक_hwirq = imsic->अंक_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	अगर (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		वापस rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	अगर (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		वापस rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	वापस 0;
}
