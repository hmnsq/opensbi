/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_csr_detect.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_irqchip.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/imsic.h>

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
କର { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} ଯେତେବେଳେ (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
କର { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} ଯେତେବେଳେ (0)

#define imsic_csr_clear(__c, __v)	\
କର { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} ଯେତେବେଳେ (0)

ସ୍ଥିର unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ଶୂନ୍ୟ *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), ଶୂନ୍ୟ *, imsic_ptr_offset, (__imsic))

ସ୍ଥିର unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_map_hartid_to_data(u32 hartid, ସଂରଚନା imsic_data *imsic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା file)
{
	ସଂରଚନା sbi_scratch *scratch;

	ଯଦି (!imsic || !imsic->targets_mmode)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	ଯଦି (!scratch)
		ଫେରନ୍ତୁ 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	ଫେରନ୍ତୁ 0;
}

ସଂରଚନା imsic_data *imsic_get_data(u32 hartindex)
{
	ସଂରଚନା sbi_scratch *scratch;

	ଯଦି (!imsic_ptr_offset)
		ଫେରନ୍ତୁ ଖାଲି;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ଯଦି (!scratch)
		ଫେରନ୍ତୁ ଖାଲି;

	ଫେରନ୍ତୁ imsic_get_hart_data_ptr(scratch);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_get_target_file(u32 hartindex)
{
	ସଂରଚନା sbi_scratch *scratch;

	ଯଦି (!imsic_file_offset)
		ଫେରନ୍ତୁ SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ଯଦି (!scratch)
		ଫେରନ୍ତୁ SBI_ENOENT;

	ଫେରନ୍ତୁ imsic_get_hart_file(scratch);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_process_hwirqs(ସଂରଚନା sbi_irqchip_device *chip)
{
	ସଂରଚନା imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, ret = 0;

	imsic = container_of(chip, ସଂରଚନା imsic_data, irqchip);
	ଯଦି (!imsic || !imsic->targets_mmode)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (imsic_get_data(current_hartindex()) != imsic)
		ଫେରନ୍ତୁ 0;

	ଯେତେବେଳେ ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		ଯଦି (hwirq > imsic->ଅଙ୍କ_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->ଅଙ୍କ_ids);
			ଜାରି;
		}

		ଯଦି (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			ଜାରି;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		ଯଦି (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଶୂନ୍ୟ imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	ସଂରଚନା imsic_regs *regs;
	ସଂରଚନା imsic_data *data;
	ସଂରଚନା sbi_scratch *scratch;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	ଯଦି (!scratch)
		ଫେରନ୍ତୁ;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	ଯଦି (!data || !data->targets_mmode)
		ଫେରନ୍ତୁ;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	ଯେତେବେଳେ (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	ଯଦି (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(ଶୂନ୍ୟ *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

ସ୍ଥିର ସଂରଚନା sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ସ୍ଥିର ଶୂନ୍ୟ imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	ଯଦି (val)
		imsic_csr_set(isel, ireg);
	ଅନ୍ୟଥା
		imsic_csr_clear(isel, ireg);
}

ଶୂନ୍ୟ imsic_local_irqchip_init(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_trap_info trap = { 0 };

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
	ଯଦି (trap.cause)
		ଫେରନ୍ତୁ;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_warm_irqchip_init(ସଂରଚନା sbi_irqchip_device *dev)
{
	ସଂରଚନା imsic_data *imsic;
	ସଂରଚନା imsic_data *hart_imsic;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	imsic = container_of(dev, ସଂରଚନା imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	ଯଦି (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* enable interrutps based on the irq state */
	ପାଇଁ (i = 1; i < imsic->ଅଙ୍କ_ids; i++) {
		ଯଦି (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		ଅନ୍ୟଥା
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_data_check(ସଂରଚନା imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	ଯଦି (!imsic ||
	    (imsic->ଅଙ୍କ_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->ଅଙ୍କ_ids))
		ଫେରନ୍ତୁ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	ଯଦି (tmp < imsic->guest_index_bits)
		ଫେରନ୍ତୁ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	ଯଦି (tmp < imsic->hart_index_bits)
		ଫେରନ୍ତୁ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	ଯଦି (tmp < imsic->group_index_bits)
		ଫେରନ୍ତୁ SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	ଯଦି (imsic->group_index_shift < tmp)
		ଫେରନ୍ତୁ SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	ଯଦି (tmp >= BITS_PER_LONG)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	ଯଦି ((imsic->ଅଙ୍କ_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* We should have at least one regset */
	ଯଦି (!imsic->regs[0].size)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	ପାଇଁ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		ଯଦି (imsic->regs[i].size & mask)
			ଫେରନ୍ତୁ SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		ଯଦି (base_addr != addr)
			ଫେରନ୍ତୁ SBI_EINVAL;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_hwirq_setup(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	ଯଦି (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ imsic_hwirq_cleanup(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା imsic_data *imsic;

	ଯଦି (!chip)
		ଫେରନ୍ତୁ;

	imsic = container_of(chip, ସଂରଚନା imsic_data, irqchip);
	ଯଦି (!imsic || !imsic->targets_mmode)
		ଫେରନ୍ତୁ;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

ସ୍ଥିର ଶୂନ୍ୟ imsic_hwirq_eoi(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_compose_msi_ସନ୍ଦେଶ(ସଂରଚନା imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 ସଂରଚନା sbi_irqchip_msi_ସନ୍ଦେଶ *ସନ୍ଦେଶ)
{
	ସଂରଚନା imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା file;

	ଯଦି (!imsic || !ସନ୍ଦେଶ)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!eiid || eiid == IMSIC_IPI_ID)
		ଫେରନ୍ତୁ 0;

	file = imsic_get_target_file(hart_index);
	ଯଦି (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		ଫେରନ୍ତୁ file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	ଯେତେବେଳେ (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	ଯଦି (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		ଫେରନ୍ତୁ SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	ସନ୍ଦେଶ->address_lo = (u32)(msi_addr);
	ସନ୍ଦେଶ->address_hi = (u32)(msi_addr >> 32);
	ସନ୍ଦେଶ->data       = eiid;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_program_msi(ସଂରଚନା sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	ସଂରଚନା imsic_data *imsic;
	ସଂରଚନା sbi_irqchip_msi_ସନ୍ଦେଶ ସନ୍ଦେଶ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (!chip)
		ଫେରନ୍ତୁ SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	ଯଦି (!imsic || !imsic->targets_mmode)
		ଫେରନ୍ତୁ SBI_ENODEV;

	rc = imsic_compose_msi_ସନ୍ଦେଶ(imsic, hart_index, eiid, &ସନ୍ଦେଶ);
	ଯଦି (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &ସନ୍ଦେଶ);
	ଯଦି (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_hwirq_set_affinity(ସଂରଚନା sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	ସଂରଚନା imsic_data *imsic;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (!chip)
		ଫେରନ୍ତୁ SBI_EINVAL;

	imsic = container_of(chip, ସଂରଚନା imsic_data, irqchip);
	ଯଦି (!imsic || !imsic->targets_mmode)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!hwirq || hwirq == IMSIC_IPI_ID)
		ଫେରନ୍ତୁ 0;

	ଯଦି (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		ଫେରନ୍ତୁ SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	ଯଦି (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ imsic_hwirq_mask(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା imsic_data *imsic;

	ଯଦି (!chip)
		ଫେରନ୍ତୁ;

	imsic = container_of(chip, ସଂରଚନା imsic_data, irqchip);
	ଯଦି (!imsic || !imsic->targets_mmode)
		ଫେରନ୍ତୁ;

	imsic_local_eix_update(hwirq, false, false);
}

ସ୍ଥିର ଶୂନ୍ୟ imsic_hwirq_unmask(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା imsic_data *imsic;

	ଯଦି (!chip)
		ଫେରନ୍ତୁ;

	imsic = container_of(chip, ସଂରଚନା imsic_data, irqchip);
	ଯଦି (!imsic || !imsic->targets_mmode)
		ଫେରନ୍ତୁ;

	ଯଦି (!hwirq || hwirq == IMSIC_IPI_ID)
		ଫେରନ୍ତୁ;

	imsic_local_eix_update(hwirq, false, true);
}

ସ୍ଥିର ସଂରଚନା sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା imsic_cold_irqchip_init(ସଂରଚନା imsic_data *imsic)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* We only initialize M-mode IMSIC */
	ଯଦି (!imsic->targets_mmode)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ଯଦି (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(ଶୂନ୍ୟ *);
		ଯଦି (!imsic_ptr_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	ଯଦି (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		ଯଦି (!imsic_file_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	ପାଇଁ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_ମୂଳ_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.ଅଙ୍କ_hwirq = imsic->ଅଙ୍କ_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	ଯଦି (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		ଫେରନ୍ତୁ rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	ଯଦି (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		ଫେରନ୍ତୁ rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	ଫେରନ୍ତୁ 0;
}
