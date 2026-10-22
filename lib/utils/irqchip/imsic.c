/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_csr_detect.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_irqchip.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/imsic.h>

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
ಕುರು { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} ಯಾವತ್ (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
ಕುರು { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} ಯಾವತ್ (0)

#define imsic_csr_clear(__c, __v)	\
ಕುರು { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} ಯಾವತ್ (0)

ಸ್ಥಿರ unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, imsic_ptr_offset, (__imsic))

ಸ್ಥಿರ unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

ಪೂರ್ಣಾಙ್ಕ imsic_map_hartid_to_data(u32 hartid, ಸಂರಚನಾ imsic_data *imsic, ಪೂರ್ಣಾಙ್ಕ file)
{
	ಸಂರಚನಾ sbi_scratch *scratch;

	ಯದಿ (!imsic || !imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	ಯದಿ (!scratch)
		ಪ್ರತ್ಯಾಗಮ 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸಂರಚನಾ imsic_data *imsic_get_data(u32 hartindex)
{
	ಸಂರಚನಾ sbi_scratch *scratch;

	ಯದಿ (!imsic_ptr_offset)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ಯದಿ (!scratch)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ imsic_get_hart_data_ptr(scratch);
}

ಪೂರ್ಣಾಙ್ಕ imsic_get_target_file(u32 hartindex)
{
	ಸಂರಚನಾ sbi_scratch *scratch;

	ಯದಿ (!imsic_file_offset)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	scratch = sbi_hartindex_to_scratch(hartindex);
	ಯದಿ (!scratch)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	ಪ್ರತ್ಯಾಗಮ imsic_get_hart_file(scratch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ imsic_process_hwirqs(ಸಂರಚನಾ sbi_irqchip_device *chip)
{
	ಸಂರಚನಾ imsic_data *imsic;
	ulong mirq;
	u32 hwirq;
	ಪೂರ್ಣಾಙ್ಕ rc, ret = 0;

	imsic = container_of(chip, ಸಂರಚನಾ imsic_data, irqchip);
	ಯದಿ (!imsic || !imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (imsic_get_data(current_hartindex()) != imsic)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯಾವತ್ ((mirq = csr_swap(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_SHIFT) &
			IMSIC_TOPEI_ID_MASK;

		ಯದಿ (hwirq > imsic->ಅಂಕೆ_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(unsigned long)hwirq,
					(unsigned long)imsic->ಅಂಕೆ_ids);
			ಅನುವರ್ತ;
		}

		ಯದಿ (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			ಅನುವರ್ತ;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		ಯದಿ (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(unsigned long)hwirq, rc);
			ret = rc;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_ipi_send(u32 hart_index)
{
	unsigned long reloff;
	ಸಂರಚನಾ imsic_regs *regs;
	ಸಂರಚನಾ imsic_data *data;
	ಸಂರಚನಾ sbi_scratch *scratch;
	ಪೂರ್ಣಾಙ್ಕ file;

	scratch = sbi_hartindex_to_scratch(hart_index);
	ಯದಿ (!scratch)
		ಪ್ರತ್ಯಾಗಮ;

	data = imsic_get_hart_data_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	ಯದಿ (!data || !data->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ;

	regs = &data->regs[0];
	reloff = file * (1UL << data->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
	ಯಾವತ್ (regs->size && (regs->size <= reloff)) {
		reloff -= regs->size;
		regs++;
	}

	ಯದಿ (regs->size && (reloff < regs->size))
		writel_relaxed(IMSIC_IPI_ID,
			(ಶೂನ್ಯ *)(regs->addr + reloff + IMSIC_MMIO_PAGE_LE));
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ಸ್ಥಿರ ಶೂನ್ಯ imsic_local_eix_update(unsigned long id,
				   bool pend, bool val)
{
	unsigned long isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_BITS;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= BIT(id);

	ಯದಿ (val)
		imsic_csr_set(isel, ireg);
	ಅನ್ಯಥಾ
		imsic_csr_clear(isel, ireg);
}

ಶೂನ್ಯ imsic_local_irqchip_init(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_trap_info trap = { 0 };

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
	ಯದಿ (trap.cause)
		ಪ್ರತ್ಯಾಗಮ;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ imsic_warm_irqchip_init(ಸಂರಚನಾ sbi_irqchip_device *dev)
{
	ಸಂರಚನಾ imsic_data *imsic;
	ಸಂರಚನಾ imsic_data *hart_imsic;
	ಪೂರ್ಣಾಙ್ಕ i;

	imsic = container_of(dev, ಸಂರಚನಾ imsic_data, irqchip);
	hart_imsic = imsic_get_data(current_hartindex());

	/* Sanity checks */
	ಯದಿ (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* enable interrutps based on the irq state */
	ಪ್ರತಿ (i = 1; i < imsic->ಅಂಕೆ_ids; i++) {
		ಯದಿ (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		ಅನ್ಯಥಾ
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ imsic_data_check(ಸಂರಚನಾ imsic_data *imsic)
{
	u32 i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	ಯದಿ (!imsic ||
	    (imsic->ಅಂಕೆ_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->ಅಂಕೆ_ids))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	ಯದಿ (tmp < imsic->guest_index_bits)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	ಯದಿ (tmp < imsic->hart_index_bits)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	ಯದಿ (tmp < imsic->group_index_bits)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	ಯದಿ (imsic->group_index_shift < tmp)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	ಯದಿ (tmp >= BITS_PER_LONG)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	ಯದಿ ((imsic->ಅಂಕೆ_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* We should have at least one regset */
	ಯದಿ (!imsic->regs[0].size)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	ಪ್ರತಿ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		ಯದಿ (imsic->regs[i].size & mask)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		ಯದಿ (base_addr != addr)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ imsic_hwirq_setup(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	ಯದಿ (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	sbi_irqchip_set_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_hwirq_cleanup(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ imsic_data *imsic;

	ಯದಿ (!chip)
		ಪ್ರತ್ಯಾಗಮ;

	imsic = container_of(chip, ಸಂರಚನಾ imsic_data, irqchip);
	ಯದಿ (!imsic || !imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_hwirq_eoi(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ imsic_compose_msi_ಸಂದೇಶ(ಸಂರಚನಾ imsic_data *imsic,
				 u32 hart_index, u32 eiid,
				 ಸಂರಚನಾ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ)
{
	ಸಂರಚನಾ imsic_regs *regs;
	unsigned long reloff;
	u64 msi_addr;
	ಪೂರ್ಣಾಙ್ಕ file;

	ಯದಿ (!imsic || !ಸಂದೇಶ)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!eiid || eiid == IMSIC_IPI_ID)
		ಪ್ರತ್ಯಾಗಮ 0;

	file = imsic_get_target_file(hart_index);
	ಯದಿ (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hart_index, file);
		ಪ್ರತ್ಯಾಗಮ file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;

	ಯಾವತ್ (regs->size && regs->size <= reloff) {
		reloff -= regs->size;
		regs++;
	}

	ಯದಿ (!regs->size || regs->size <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (unsigned long)hart_index, file, reloff);
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_PAGE_LE;
	ಸಂದೇಶ->address_lo = (u32)(msi_addr);
	ಸಂದೇಶ->address_hi = (u32)(msi_addr >> 32);
	ಸಂದೇಶ->data       = eiid;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ imsic_program_msi(ಸಂರಚನಾ sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_index)
{
	ಸಂರಚನಾ imsic_data *imsic;
	ಸಂರಚನಾ sbi_irqchip_msi_ಸಂದೇಶ ಸಂದೇಶ;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (!chip)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	imsic = imsic_get_data(hart_index);
	ಯದಿ (!imsic || !imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	rc = imsic_compose_msi_ಸಂದೇಶ(imsic, hart_index, eiid, &ಸಂದೇಶ);
	ಯದಿ (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &ಸಂದೇಶ);
	ಯದಿ (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)eiid,
				(unsigned long)hart_index, rc);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ imsic_hwirq_set_affinity(ಸಂರಚನಾ sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	ಸಂರಚನಾ imsic_data *imsic;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (!chip)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	imsic = container_of(chip, ಸಂರಚನಾ imsic_data, irqchip);
	ಯದಿ (!imsic || !imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!hwirq || hwirq == IMSIC_IPI_ID)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ (!sbi_hartmask_test_hartindex(hart_index, &chip->target_harts))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	rc = imsic_program_msi(chip, hwirq, hart_index);
	ಯದಿ (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(unsigned long)hwirq,
				(unsigned long)hart_index, rc);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_hwirq_mask(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ imsic_data *imsic;

	ಯದಿ (!chip)
		ಪ್ರತ್ಯಾಗಮ;

	imsic = container_of(chip, ಸಂರಚನಾ imsic_data, irqchip);
	ಯದಿ (!imsic || !imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ;

	imsic_local_eix_update(hwirq, false, false);
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_hwirq_unmask(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ imsic_data *imsic;

	ಯದಿ (!chip)
		ಪ್ರತ್ಯಾಗಮ;

	imsic = container_of(chip, ಸಂರಚನಾ imsic_data, irqchip);
	ಯದಿ (!imsic || !imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (!hwirq || hwirq == IMSIC_IPI_ID)
		ಪ್ರತ್ಯಾಗಮ;

	imsic_local_eix_update(hwirq, false, true);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_setup		= imsic_hwirq_setup,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_set_affinity	= imsic_hwirq_set_affinity,
	.hwirq_mask		= imsic_hwirq_mask,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

ಪೂರ್ಣಾಙ್ಕ imsic_cold_irqchip_init(ಸಂರಚನಾ imsic_data *imsic)
{
	ಪೂರ್ಣಾಙ್ಕ i, rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* We only initialize M-mode IMSIC */
	ಯದಿ (!imsic->targets_mmode)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ಯದಿ (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(ಶೂನ್ಯ *);
		ಯದಿ (!imsic_ptr_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	ಯದಿ (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		ಯದಿ (!imsic_file_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	ಪ್ರತಿ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		rc = sbi_domain_ಬೇರು_add_memrange(imsic->regs[i].addr,
						  imsic->regs[i].size,
						  IMSIC_MMIO_PAGE_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.ಅಂಕೆ_hwirq = imsic->ಅಂಕೆ_ids + 1;

	sbi_hartmask_set_all(&imsic->irqchip.target_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_add_device(&imsic->irqchip);
	ಯದಿ (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	rc = sbi_irqchip_register_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	ಯದಿ (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	/* Register IPI device */
	sbi_ipi_add_device(&imsic_ipi_device);

	ಪ್ರತ್ಯಾಗಮ 0;
}
