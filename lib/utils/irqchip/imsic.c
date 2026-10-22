/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_csr_detect.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_irqchip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/imsic.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_MMIO_ਪੰਨਾ_LE		0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_MMIO_ਪੰਨਾ_BE		0x04

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_MIN_ID			63
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_MAX_ID			2047

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_EIDELIVERY		0x70

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_EITHRESHOLD		0x72

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_TOPEI			0x76
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_TOPEI_ID_ਖਿਸਕਾਓ		16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_TOPEI_ID_ਨਕਾਬ		0x7ff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_TOPEI_PRIO_ਨਕਾਬ		0x7ff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_EIP0			0x80

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_EIP63			0xbf

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_EIPx_ਬਿੱਟ			32

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_EIE0			0xc0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_EIE63			0xff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_EIEx_ਬਿੱਟ			32

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_DISABLE_EIDELIVERY	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_ENABLE_EIDELIVERY		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_DISABLE_EITHRESHOLD	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_ENABLE_EITHRESHOLD	0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMSIC_IPI_ID			1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ imsic_csr_write(__c, __v)	\
ਕਰੋ { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} ਜਦੋਂ (0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ imsic_csr_read(__c)	\
({ \
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ imsic_csr_ਸੈੱਟ(__c, __v)		\
ਕਰੋ { \
	csr_write(CSR_MISELECT, __c); \
	csr_ਸੈੱਟ(CSR_MIREG, __v); \
} ਜਦੋਂ (0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ imsic_csr_clear(__c, __v)	\
ਕਰੋ { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} ਜਦੋਂ (0)

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ imsic_ptr_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ imsic_get_hart_ਅੰਕੜੇ_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ਖਾਲੀ *, imsic_ptr_ਵਿੱਥ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ imsic_ਸੈੱਟ_hart_ਅੰਕੜੇ_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), ਖਾਲੀ *, imsic_ptr_ਵਿੱਥ, (__imsic))

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ imsic_file_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), ਲੰਮਾ, imsic_file_ਵਿੱਥ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ imsic_ਸੈੱਟ_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), ਲੰਮਾ, imsic_file_ਵਿੱਥ, (__file))

ਪੂਰਨ imsic_map_hartid_ਮੰਜ਼ਿਲ_ਅੰਕੜੇ(u32 hartid, ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic, ਪੂਰਨ file)
{
	ਬਣਤਰ sbi_scratch *scratch;

	ਜੇ (!imsic || !imsic->targets_mmode)
		ਵਾਪਸ SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
	ਜੇ (!scratch)
		ਵਾਪਸ 0;

	imsic_ਸੈੱਟ_hart_ਅੰਕੜੇ_ptr(scratch, imsic);
	imsic_ਸੈੱਟ_hart_file(scratch, file);
	ਵਾਪਸ 0;
}

ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic_get_ਅੰਕੜੇ(u32 hartindex)
{
	ਬਣਤਰ sbi_scratch *scratch;

	ਜੇ (!imsic_ptr_ਵਿੱਥ)
		ਵਾਪਸ ਨਿਰਾ;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hartindex);
	ਜੇ (!scratch)
		ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ imsic_get_hart_ਅੰਕੜੇ_ptr(scratch);
}

ਪੂਰਨ imsic_get_ਨਿਸ਼ਾਨਾ_file(u32 hartindex)
{
	ਬਣਤਰ sbi_scratch *scratch;

	ਜੇ (!imsic_file_ਵਿੱਥ)
		ਵਾਪਸ SBI_ENOENT;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hartindex);
	ਜੇ (!scratch)
		ਵਾਪਸ SBI_ENOENT;

	ਵਾਪਸ imsic_get_hart_file(scratch);
}

ਸਥਿਰ ਪੂਰਨ imsic_process_hwirqs(ਬਣਤਰ sbi_irqchip_device *chip)
{
	ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic;
	ulong mirq;
	u32 hwirq;
	ਪੂਰਨ rc, ret = 0;

	imsic = container_ਦਾ(chip, ਬਣਤਰ imsic_ਅੰਕੜੇ, irqchip);
	ਜੇ (!imsic || !imsic->targets_mmode)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (imsic_get_ਅੰਕੜੇ(current_hartindex()) != imsic)
		ਵਾਪਸ 0;

	ਜਦੋਂ ((mirq = csr_ਅਦਲਾ_ਬਦਲੀ(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_ਖਿਸਕਾਓ) &
			IMSIC_TOPEI_ID_ਨਕਾਬ;

		ਜੇ (hwirq > imsic->ਅੰਕ_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq,
					(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)imsic->ਅੰਕ_ids);
			ਜਾਰੀ;
		}

		ਜੇ (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			ਜਾਰੀ;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		ਜੇ (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq, rc);
			ret = rc;
		}
	}

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਖਾਲੀ imsic_ipi_send(u32 hart_ਸੂਚਕਾਂਕ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reloff;
	ਬਣਤਰ imsic_regs *regs;
	ਬਣਤਰ imsic_ਅੰਕੜੇ *ਅੰਕੜੇ;
	ਬਣਤਰ sbi_scratch *scratch;
	ਪੂਰਨ file;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hart_ਸੂਚਕਾਂਕ);
	ਜੇ (!scratch)
		ਵਾਪਸ;

	ਅੰਕੜੇ = imsic_get_hart_ਅੰਕੜੇ_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	ਜੇ (!ਅੰਕੜੇ || !ਅੰਕੜੇ->targets_mmode)
		ਵਾਪਸ;

	regs = &ਅੰਕੜੇ->regs[0];
	reloff = file * (1UL << ਅੰਕੜੇ->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ) * IMSIC_MMIO_ਪੰਨਾ_SZ;
	ਜਦੋਂ (regs->ਮਾਪ && (regs->ਮਾਪ <= reloff)) {
		reloff -= regs->ਮਾਪ;
		regs++;
	}

	ਜੇ (regs->ਮਾਪ && (reloff < regs->ਮਾਪ))
		writel_relaxed(IMSIC_IPI_ID,
			(ਖਾਲੀ *)(regs->addr + reloff + IMSIC_MMIO_ਪੰਨਾ_LE));
}

ਸਥਿਰ ਬਣਤਰ sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

ਸਥਿਰ ਖਾਲੀ imsic_local_eix_update(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ id,
				   bool pend, bool val)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_ਬਿੱਟ;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= ਦੋਆਧਾਰੀ_ਅੰਕ(id);

	ਜੇ (val)
		imsic_csr_ਸੈੱਟ(isel, ireg);
	ਨਹੀਂਤਾਂ
		imsic_csr_clear(isel, ireg);
}

ਖਾਲੀ imsic_local_irqchip_init(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_trap_info trap = { 0 };

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
	ਜੇ (trap.cause)
		ਵਾਪਸ;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

ਸਥਿਰ ਪੂਰਨ imsic_warm_irqchip_init(ਬਣਤਰ sbi_irqchip_device *dev)
{
	ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic;
	ਬਣਤਰ imsic_ਅੰਕੜੇ *hart_imsic;
	ਪੂਰਨ i;

	imsic = container_ਦਾ(dev, ਬਣਤਰ imsic_ਅੰਕੜੇ, irqchip);
	hart_imsic = imsic_get_ਅੰਕੜੇ(current_hartindex());

	/* Sanity checks */
	ਜੇ (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		ਵਾਪਸ SBI_EINVAL;

	/* enable interrutps based on the irq state */
	ਲਈ (i = 1; i < imsic->ਅੰਕ_ids; i++) {
		ਜੇ (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		ਨਹੀਂਤਾਂ
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	ਵਾਪਸ 0;
}

ਪੂਰਨ imsic_ਅੰਕੜੇ_check(ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic)
{
	u32 i, tmp;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_addr, addr, ਨਕਾਬ;

	/* Sanity checks */
	ਜੇ (!imsic ||
	    (imsic->ਅੰਕ_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->ਅੰਕ_ids))
		ਵਾਪਸ SBI_EINVAL;

	tmp = ਬਿੱਟ_ਪ੍ਰਤੀ_LONG - IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ;
	ਜੇ (tmp < imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ)
		ਵਾਪਸ SBI_EINVAL;

	tmp = ਬਿੱਟ_ਪ੍ਰਤੀ_LONG - IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ -
	      imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
	ਜੇ (tmp < imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ)
		ਵਾਪਸ SBI_EINVAL;

	tmp = ਬਿੱਟ_ਪ੍ਰਤੀ_LONG - IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ -
	      imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ - imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
	ਜੇ (tmp < imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ)
		ਵਾਪਸ SBI_EINVAL;

	tmp = IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ + imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ +
	      imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
	ਜੇ (imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ < tmp)
		ਵਾਪਸ SBI_EINVAL;
	tmp = imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ + imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ - 1;
	ਜੇ (tmp >= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG)
		ਵਾਪਸ SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	ਜੇ ((imsic->ਅੰਕ_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		ਵਾਪਸ SBI_EINVAL;

	/* We should have at least one regset */
	ਜੇ (!imsic->regs[0].ਮਾਪ)
		ਵਾਪਸ SBI_EINVAL;

	/* Match patter of each regset */
	ਅਧਾਰ_addr = imsic->regs[0].addr;
	ਅਧਾਰ_addr &= ~((1UL << (imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ +
				 imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ +
				 IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ)) - 1);
	ਅਧਾਰ_addr &= ~(((1UL << imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1) <<
			imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
	ਲਈ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].ਮਾਪ; i++) {
		ਨਕਾਬ = (1UL << imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ) * IMSIC_MMIO_ਪੰਨਾ_SZ;
		ਨਕਾਬ -= 1UL;
		ਜੇ (imsic->regs[i].ਮਾਪ & ਨਕਾਬ)
			ਵਾਪਸ SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ +
					 imsic->hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ +
					 IMSIC_MMIO_ਪੰਨਾ_ਖਿਸਕਾਓ)) - 1);
		addr &= ~(((1UL << imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1) <<
				imsic->ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
		ਜੇ (ਅਧਾਰ_addr != addr)
			ਵਾਪਸ SBI_EINVAL;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ imsic_hwirq_ਤਿਆਰੀ(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	ਜੇ (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		ਵਾਪਸ SBI_ENOTSUPP;

	sbi_irqchip_ਸੈੱਟ_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ imsic_hwirq_cleanup(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic;

	ਜੇ (!chip)
		ਵਾਪਸ;

	imsic = container_ਦਾ(chip, ਬਣਤਰ imsic_ਅੰਕੜੇ, irqchip);
	ਜੇ (!imsic || !imsic->targets_mmode)
		ਵਾਪਸ;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

ਸਥਿਰ ਖਾਲੀ imsic_hwirq_eoi(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

ਸਥਿਰ ਪੂਰਨ imsic_compose_msi_ਸੁਨੇਹਾ(ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic,
				 u32 hart_ਸੂਚਕਾਂਕ, u32 eiid,
				 ਬਣਤਰ sbi_irqchip_msi_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ)
{
	ਬਣਤਰ imsic_regs *regs;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reloff;
	u64 msi_addr;
	ਪੂਰਨ file;

	ਜੇ (!imsic || !ਸੁਨੇਹਾ)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!eiid || eiid == IMSIC_IPI_ID)
		ਵਾਪਸ 0;

	file = imsic_get_ਨਿਸ਼ਾਨਾ_file(hart_ਸੂਚਕਾਂਕ);
	ਜੇ (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_ਸੂਚਕਾਂਕ, file);
		ਵਾਪਸ file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ) * IMSIC_MMIO_ਪੰਨਾ_SZ;

	ਜਦੋਂ (regs->ਮਾਪ && regs->ਮਾਪ <= reloff) {
		reloff -= regs->ਮਾਪ;
		regs++;
	}

	ਜੇ (!regs->ਮਾਪ || regs->ਮਾਪ <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_ਸੂਚਕਾਂਕ, file, reloff);
		ਵਾਪਸ SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_ਪੰਨਾ_LE;
	ਸੁਨੇਹਾ->ਪਤਾ_lo = (u32)(msi_addr);
	ਸੁਨੇਹਾ->ਪਤਾ_hi = (u32)(msi_addr >> 32);
	ਸੁਨੇਹਾ->ਅੰਕੜੇ       = eiid;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ imsic_ਪ੍ਰੋਗਰਾਮ_msi(ਬਣਤਰ sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_ਸੂਚਕਾਂਕ)
{
	ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic;
	ਬਣਤਰ sbi_irqchip_msi_ਸੁਨੇਹਾ ਸੁਨੇਹਾ;
	ਪੂਰਨ rc;

	ਜੇ (!chip)
		ਵਾਪਸ SBI_EINVAL;

	imsic = imsic_get_ਅੰਕੜੇ(hart_ਸੂਚਕਾਂਕ);
	ਜੇ (!imsic || !imsic->targets_mmode)
		ਵਾਪਸ SBI_ENODEV;

	rc = imsic_compose_msi_ਸੁਨੇਹਾ(imsic, hart_ਸੂਚਕਾਂਕ, eiid, &ਸੁਨੇਹਾ);
	ਜੇ (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)eiid,
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_ਸੂਚਕਾਂਕ, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &ਸੁਨੇਹਾ);
	ਜੇ (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)eiid,
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_ਸੂਚਕਾਂਕ, rc);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ imsic_hwirq_ਸੈੱਟ_affinity(ਬਣਤਰ sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_ਸੂਚਕਾਂਕ)
{
	ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic;
	ਪੂਰਨ rc;

	ਜੇ (!chip)
		ਵਾਪਸ SBI_EINVAL;

	imsic = container_ਦਾ(chip, ਬਣਤਰ imsic_ਅੰਕੜੇ, irqchip);
	ਜੇ (!imsic || !imsic->targets_mmode)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!hwirq || hwirq == IMSIC_IPI_ID)
		ਵਾਪਸ 0;

	ਜੇ (!sbi_hartmask_test_hartindex(hart_ਸੂਚਕਾਂਕ, &chip->ਨਿਸ਼ਾਨਾ_harts))
		ਵਾਪਸ SBI_EINVAL;

	rc = imsic_ਪ੍ਰੋਗਰਾਮ_msi(chip, hwirq, hart_ਸੂਚਕਾਂਕ);
	ਜੇ (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq,
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hart_ਸੂਚਕਾਂਕ, rc);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ imsic_hwirq_ਨਕਾਬ(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic;

	ਜੇ (!chip)
		ਵਾਪਸ;

	imsic = container_ਦਾ(chip, ਬਣਤਰ imsic_ਅੰਕੜੇ, irqchip);
	ਜੇ (!imsic || !imsic->targets_mmode)
		ਵਾਪਸ;

	imsic_local_eix_update(hwirq, false, false);
}

ਸਥਿਰ ਖਾਲੀ imsic_hwirq_unmask(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic;

	ਜੇ (!chip)
		ਵਾਪਸ;

	imsic = container_ਦਾ(chip, ਬਣਤਰ imsic_ਅੰਕੜੇ, irqchip);
	ਜੇ (!imsic || !imsic->targets_mmode)
		ਵਾਪਸ;

	ਜੇ (!hwirq || hwirq == IMSIC_IPI_ID)
		ਵਾਪਸ;

	imsic_local_eix_update(hwirq, false, true);
}

ਸਥਿਰ ਬਣਤਰ sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_ਤਿਆਰੀ		= imsic_hwirq_ਤਿਆਰੀ,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_ਸੈੱਟ_affinity	= imsic_hwirq_ਸੈੱਟ_affinity,
	.hwirq_ਨਕਾਬ		= imsic_hwirq_ਨਕਾਬ,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

ਪੂਰਨ imsic_cold_irqchip_init(ਬਣਤਰ imsic_ਅੰਕੜੇ *imsic)
{
	ਪੂਰਨ i, rc;

	/* Sanity checks */
	rc = imsic_ਅੰਕੜੇ_check(imsic);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* We only initialize M-mode IMSIC */
	ਜੇ (!imsic->targets_mmode)
		ਵਾਪਸ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ਜੇ (!imsic_ptr_ਵਿੱਥ) {
		imsic_ptr_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਖਾਲੀ *);
		ਜੇ (!imsic_ptr_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	ਜੇ (!imsic_file_ਵਿੱਥ) {
		imsic_file_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਲੰਮਾ);
		ਜੇ (!imsic_file_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	ਲਈ (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].ਮਾਪ; i++) {
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(imsic->regs[i].addr,
						  imsic->regs[i].ਮਾਪ,
						  IMSIC_MMIO_ਪੰਨਾ_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.ਅੰਕ_hwirq = imsic->ਅੰਕ_ids + 1;

	sbi_hartmask_ਸੈੱਟ_all(&imsic->irqchip.ਨਿਸ਼ਾਨਾ_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_ਜੋੜੋ_device(&imsic->irqchip);
	ਜੇ (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		ਵਾਪਸ rc;
	}

	rc = sbi_irqchip_ਰਜਿਸਟਰ_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	ਜੇ (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		ਵਾਪਸ rc;
	}

	/* Register IPI device */
	sbi_ipi_ਜੋੜੋ_device(&imsic_ipi_device);

	ਵਾਪਸ 0;
}
