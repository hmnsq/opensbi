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

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_MMIO_ಪುಟ_LE		0x00
#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_MMIO_ಪುಟ_BE		0x04

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_MIN_ID			63
#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_MAX_ID			2047

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_EIDELIVERY		0x70

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_EITHRESHOLD		0x72

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_TOPEI			0x76
#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_TOPEI_ID_ಸರಿಸು		16
#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_TOPEI_ID_ಮುಖವಾಡ		0x7ff
#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_TOPEI_PRIO_ಮುಖವಾಡ		0x7ff

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_EIP0			0x80

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_EIP63			0xbf

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_EIPx_ಬಿಟ್ಟುಗಳು			32

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_EIE0			0xc0

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_EIE63			0xff

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_EIEx_ಬಿಟ್ಟುಗಳು			32

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_DISABLE_EIDELIVERY	0
#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_ENABLE_EIDELIVERY		1
#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_DISABLE_EITHRESHOLD	1
#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_ENABLE_EITHRESHOLD	0

#ವ್ಯಾಖ್ಯಾನಿಸು IMSIC_IPI_ID			1

#ವ್ಯಾಖ್ಯಾನಿಸು imsic_csr_write(__c, __v)	\
ಮಾಡು { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} ಆಗ (0)

#ವ್ಯಾಖ್ಯಾನಿಸು imsic_csr_read(__c)	\
({ \
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#ವ್ಯಾಖ್ಯಾನಿಸು imsic_csr_ಗಣ(__c, __v)		\
ಮಾಡು { \
	csr_write(CSR_MISELECT, __c); \
	csr_ಗಣ(CSR_MIREG, __v); \
} ಆಗ (0)

#ವ್ಯಾಖ್ಯಾನಿಸು imsic_csr_clear(__c, __v)	\
ಮಾಡು { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} ಆಗ (0)

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ imsic_ptr_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು imsic_get_hart_ದತ್ತಾಂಶ_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, imsic_ptr_ಸ್ಥಾನಾಂತರ)

#ವ್ಯಾಖ್ಯಾನಿಸು imsic_ಗಣ_hart_ದತ್ತಾಂಶ_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, imsic_ptr_ಸ್ಥಾನಾಂತರ, (__imsic))

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ imsic_file_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), ದೀರ್ಘ, imsic_file_ಸ್ಥಾನಾಂತರ)

#ವ್ಯಾಖ್ಯಾನಿಸು imsic_ಗಣ_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), ದೀರ್ಘ, imsic_file_ಸ್ಥಾನಾಂತರ, (__file))

ಪೂರ್ಣಾಂಕ imsic_map_hartid_ಗಮ್ಯ_ದತ್ತಾಂಶ(u32 hartid, ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic, ಪೂರ್ಣಾಂಕ file)
{
	ರಚನೆ sbi_scratch *scratch;

	ಆದರೆ (!imsic || !imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ 0;

	imsic_ಗಣ_hart_ದತ್ತಾಂಶ_ptr(scratch, imsic);
	imsic_ಗಣ_hart_file(scratch, file);
	ಹಿಂತಿರುಗಿ 0;
}

ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic_get_ದತ್ತಾಂಶ(u32 hartindex)
{
	ರಚನೆ sbi_scratch *scratch;

	ಆದರೆ (!imsic_ptr_ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hartindex);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ imsic_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
}

ಪೂರ್ಣಾಂಕ imsic_get_ಗುರಿ_file(u32 hartindex)
{
	ರಚನೆ sbi_scratch *scratch;

	ಆದರೆ (!imsic_file_ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hartindex);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	ಹಿಂತಿರುಗಿ imsic_get_hart_file(scratch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ imsic_process_hwirqs(ರಚನೆ sbi_irqchip_device *chip)
{
	ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic;
	ulong mirq;
	u32 hwirq;
	ಪೂರ್ಣಾಂಕ rc, ret = 0;

	imsic = container_ದ(chip, ರಚನೆ imsic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (!imsic || !imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (imsic_get_ದತ್ತಾಂಶ(current_hartindex()) != imsic)
		ಹಿಂತಿರುಗಿ 0;

	ಆಗ ((mirq = csr_ಅದಲುಬದಲು(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_ಸರಿಸು) &
			IMSIC_TOPEI_ID_ಮುಖವಾಡ;

		ಆದರೆ (hwirq > imsic->ಅಂಕೆ_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq,
					(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)imsic->ಅಂಕೆ_ids);
			ಮುಂದುವರಿ;
		}

		ಆದರೆ (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			ಮುಂದುವರಿ;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		ಆದರೆ (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq, rc);
			ret = rc;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_ipi_send(u32 hart_ಸೂಚ್ಯಂಕ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reloff;
	ರಚನೆ imsic_regs *regs;
	ರಚನೆ imsic_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;
	ರಚನೆ sbi_scratch *scratch;
	ಪೂರ್ಣಾಂಕ file;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hart_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ;

	ದತ್ತಾಂಶ = imsic_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	ಆದರೆ (!ದತ್ತಾಂಶ || !ದತ್ತಾಂಶ->targets_mmode)
		ಹಿಂತಿರುಗಿ;

	regs = &ದತ್ತಾಂಶ->regs[0];
	reloff = file * (1UL << ದತ್ತಾಂಶ->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) * IMSIC_MMIO_ಪುಟ_SZ;
	ಆಗ (regs->ಗಾತ್ರ_2 && (regs->ಗಾತ್ರ_2 <= reloff)) {
		reloff -= regs->ಗಾತ್ರ_2;
		regs++;
	}

	ಆದರೆ (regs->ಗಾತ್ರ_2 && (reloff < regs->ಗಾತ್ರ_2))
		writel_relaxed(IMSIC_IPI_ID,
			(ಶೂನ್ಯ *)(regs->addr + reloff + IMSIC_MMIO_ಪುಟ_LE));
}

ಸ್ಥಿರ ರಚನೆ sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ಸ್ಥಿರ ಶೂನ್ಯ imsic_local_eix_update(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ id,
				   bool pend, bool val)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_ಬಿಟ್ಟುಗಳು;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= ಬಿಟ್(id);

	ಆದರೆ (val)
		imsic_csr_ಗಣ(isel, ireg);
	ಇಲ್ಲದಿದ್ದರೆ
		imsic_csr_clear(isel, ireg);
}

ಶೂನ್ಯ imsic_local_irqchip_init(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_trap_info trap = { 0 };

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
	ಆದರೆ (trap.cause)
		ಹಿಂತಿರುಗಿ;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ imsic_warm_irqchip_init(ರಚನೆ sbi_irqchip_device *dev)
{
	ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic;
	ರಚನೆ imsic_ದತ್ತಾಂಶ *hart_imsic;
	ಪೂರ್ಣಾಂಕ i;

	imsic = container_ದ(dev, ರಚನೆ imsic_ದತ್ತಾಂಶ, irqchip);
	hart_imsic = imsic_get_ದತ್ತಾಂಶ(current_hartindex());

	/* Sanity checks */
	ಆದರೆ (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* enable interrutps based on the irq state */
	ಗಾಗಿ (i = 1; i < imsic->ಅಂಕೆ_ids; i++) {
		ಆದರೆ (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		ಇಲ್ಲದಿದ್ದರೆ
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ imsic_ದತ್ತಾಂಶ_check(ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic)
{
	u32 i, tmp;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_addr, addr, ಮುಖವಾಡ;

	/* Sanity checks */
	ಆದರೆ (!imsic ||
	    (imsic->ಅಂಕೆ_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->ಅಂಕೆ_ids))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	tmp = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG - IMSIC_MMIO_ಪುಟ_ಸರಿಸು;
	ಆದರೆ (tmp < imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	tmp = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG - IMSIC_MMIO_ಪುಟ_ಸರಿಸು -
	      imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
	ಆದರೆ (tmp < imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	tmp = ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG - IMSIC_MMIO_ಪುಟ_ಸರಿಸು -
	      imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು - imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
	ಆದರೆ (tmp < imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	tmp = IMSIC_MMIO_ಪುಟ_ಸರಿಸು + imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು +
	      imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
	ಆದರೆ (imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಸರಿಸು < tmp)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	tmp = imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಸರಿಸು - 1;
	ಆದರೆ (tmp >= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	ಆದರೆ ((imsic->ಅಂಕೆ_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* We should have at least one regset */
	ಆದರೆ (!imsic->regs[0].ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Match patter of each regset */
	ತಳ_addr = imsic->regs[0].addr;
	ತಳ_addr &= ~((1UL << (imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು +
				 imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು +
				 IMSIC_MMIO_ಪುಟ_ಸರಿಸು)) - 1);
	ತಳ_addr &= ~(((1UL << imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1) <<
			imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
	ಗಾಗಿ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].ಗಾತ್ರ_2; i++) {
		ಮುಖವಾಡ = (1UL << imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) * IMSIC_MMIO_ಪುಟ_SZ;
		ಮುಖವಾಡ -= 1UL;
		ಆದರೆ (imsic->regs[i].ಗಾತ್ರ_2 & ಮುಖವಾಡ)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು +
					 imsic->hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು +
					 IMSIC_MMIO_ಪುಟ_ಸರಿಸು)) - 1);
		addr &= ~(((1UL << imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1) <<
				imsic->ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
		ಆದರೆ (ತಳ_addr != addr)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ imsic_hwirq_ಸಿದ್ಧತೆ(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	ಆದರೆ (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	sbi_irqchip_ಗಣ_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_hwirq_cleanup(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic;

	ಆದರೆ (!chip)
		ಹಿಂತಿರುಗಿ;

	imsic = container_ದ(chip, ರಚನೆ imsic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (!imsic || !imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_hwirq_eoi(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ imsic_compose_msi_ಸಂದೇಶ(ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic,
				 u32 hart_ಸೂಚ್ಯಂಕ, u32 eiid,
				 ರಚನೆ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ)
{
	ರಚನೆ imsic_regs *regs;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reloff;
	u64 msi_addr;
	ಪೂರ್ಣಾಂಕ file;

	ಆದರೆ (!imsic || !ಸಂದೇಶ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!eiid || eiid == IMSIC_IPI_ID)
		ಹಿಂತಿರುಗಿ 0;

	file = imsic_get_ಗುರಿ_file(hart_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_ಸೂಚ್ಯಂಕ, file);
		ಹಿಂತಿರುಗಿ file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) * IMSIC_MMIO_ಪುಟ_SZ;

	ಆಗ (regs->ಗಾತ್ರ_2 && regs->ಗಾತ್ರ_2 <= reloff) {
		reloff -= regs->ಗಾತ್ರ_2;
		regs++;
	}

	ಆದರೆ (!regs->ಗಾತ್ರ_2 || regs->ಗಾತ್ರ_2 <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_ಸೂಚ್ಯಂಕ, file, reloff);
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_ಪುಟ_LE;
	ಸಂದೇಶ->ವಿಳಾಸ_lo = (u32)(msi_addr);
	ಸಂದೇಶ->ವಿಳಾಸ_hi = (u32)(msi_addr >> 32);
	ಸಂದೇಶ->ದತ್ತಾಂಶ       = eiid;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ imsic_ಕಾರ್ಯಕ್ರಮ_msi(ರಚನೆ sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_ಸೂಚ್ಯಂಕ)
{
	ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic;
	ರಚನೆ sbi_irqchip_msi_ಸಂದೇಶ ಸಂದೇಶ;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (!chip)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	imsic = imsic_get_ದತ್ತಾಂಶ(hart_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (!imsic || !imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	rc = imsic_compose_msi_ಸಂದೇಶ(imsic, hart_ಸೂಚ್ಯಂಕ, eiid, &ಸಂದೇಶ);
	ಆದರೆ (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)eiid,
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_ಸೂಚ್ಯಂಕ, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &ಸಂದೇಶ);
	ಆದರೆ (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)eiid,
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_ಸೂಚ್ಯಂಕ, rc);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ imsic_hwirq_ಗಣ_affinity(ರಚನೆ sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_ಸೂಚ್ಯಂಕ)
{
	ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (!chip)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	imsic = container_ದ(chip, ರಚನೆ imsic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (!imsic || !imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!hwirq || hwirq == IMSIC_IPI_ID)
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (!sbi_hartmask_test_hartindex(hart_ಸೂಚ್ಯಂಕ, &chip->ಗುರಿ_harts))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	rc = imsic_ಕಾರ್ಯಕ್ರಮ_msi(chip, hwirq, hart_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq,
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hart_ಸೂಚ್ಯಂಕ, rc);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_hwirq_ಮುಖವಾಡ(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic;

	ಆದರೆ (!chip)
		ಹಿಂತಿರುಗಿ;

	imsic = container_ದ(chip, ರಚನೆ imsic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (!imsic || !imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ;

	imsic_local_eix_update(hwirq, false, false);
}

ಸ್ಥಿರ ಶೂನ್ಯ imsic_hwirq_unmask(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic;

	ಆದರೆ (!chip)
		ಹಿಂತಿರುಗಿ;

	imsic = container_ದ(chip, ರಚನೆ imsic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (!imsic || !imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (!hwirq || hwirq == IMSIC_IPI_ID)
		ಹಿಂತಿರುಗಿ;

	imsic_local_eix_update(hwirq, false, true);
}

ಸ್ಥಿರ ರಚನೆ sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_ಸಿದ್ಧತೆ		= imsic_hwirq_ಸಿದ್ಧತೆ,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_ಗಣ_affinity	= imsic_hwirq_ಗಣ_affinity,
	.hwirq_ಮುಖವಾಡ		= imsic_hwirq_ಮುಖವಾಡ,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

ಪೂರ್ಣಾಂಕ imsic_cold_irqchip_init(ರಚನೆ imsic_ದತ್ತಾಂಶ *imsic)
{
	ಪೂರ್ಣಾಂಕ i, rc;

	/* Sanity checks */
	rc = imsic_ದತ್ತಾಂಶ_check(imsic);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* We only initialize M-mode IMSIC */
	ಆದರೆ (!imsic->targets_mmode)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ಆದರೆ (!imsic_ptr_ಸ್ಥಾನಾಂತರ) {
		imsic_ptr_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *);
		ಆದರೆ (!imsic_ptr_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	ಆದರೆ (!imsic_file_ಸ್ಥಾನಾಂತರ) {
		imsic_file_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ದೀರ್ಘ);
		ಆದರೆ (!imsic_file_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	ಗಾಗಿ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].ಗಾತ್ರ_2; i++) {
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(imsic->regs[i].addr,
						  imsic->regs[i].ಗಾತ್ರ_2,
						  IMSIC_MMIO_ಪುಟ_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.ಅಂಕೆ_hwirq = imsic->ಅಂಕೆ_ids + 1;

	sbi_hartmask_ಗಣ_all(&imsic->irqchip.ಗುರಿ_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_ಕೂಡಿಸು_device(&imsic->irqchip);
	ಆದರೆ (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		ಹಿಂತಿರುಗಿ rc;
	}

	rc = sbi_irqchip_ರಿಜಿಸ್ಟರ್_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	ಆದರೆ (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		ಹಿಂತಿರುಗಿ rc;
	}

	/* Register IPI device */
	sbi_ipi_ಕೂಡಿಸು_device(&imsic_ipi_device);

	ಹಿಂತಿರುಗಿ 0;
}
