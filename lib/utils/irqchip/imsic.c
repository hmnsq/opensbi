/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_csr_detect.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_irqchip.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi_utils/irqchip/imsic.h>

#నిర్వచించు IMSIC_MMIO_పుట_LE		0x00
#నిర్వచించు IMSIC_MMIO_పుట_BE		0x04

#నిర్వచించు IMSIC_MIN_ID			63
#నిర్వచించు IMSIC_MAX_ID			2047

#నిర్వచించు IMSIC_EIDELIVERY		0x70

#నిర్వచించు IMSIC_EITHRESHOLD		0x72

#నిర్వచించు IMSIC_TOPEI			0x76
#నిర్వచించు IMSIC_TOPEI_ID_జరుపు		16
#నిర్వచించు IMSIC_TOPEI_ID_ముసుగు		0x7ff
#నిర్వచించు IMSIC_TOPEI_PRIO_ముసుగు		0x7ff

#నిర్వచించు IMSIC_EIP0			0x80

#నిర్వచించు IMSIC_EIP63			0xbf

#నిర్వచించు IMSIC_EIPx_బిట్లు			32

#నిర్వచించు IMSIC_EIE0			0xc0

#నిర్వచించు IMSIC_EIE63			0xff

#నిర్వచించు IMSIC_EIEx_బిట్లు			32

#నిర్వచించు IMSIC_DISABLE_EIDELIVERY	0
#నిర్వచించు IMSIC_ENABLE_EIDELIVERY		1
#నిర్వచించు IMSIC_DISABLE_EITHRESHOLD	1
#నిర్వచించు IMSIC_ENABLE_EITHRESHOLD	0

#నిర్వచించు IMSIC_IPI_ID			1

#నిర్వచించు imsic_csr_write(__c, __v)	\
చేయి { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} అయితే (0)

#నిర్వచించు imsic_csr_read(__c)	\
({ \
	గుర్తులేని దీర్ఘ __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#నిర్వచించు imsic_csr_సమితి(__c, __v)		\
చేయి { \
	csr_write(CSR_MISELECT, __c); \
	csr_సమితి(CSR_MIREG, __v); \
} అయితే (0)

#నిర్వచించు imsic_csr_clear(__c, __v)	\
చేయి { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} అయితే (0)

స్థిర గుర్తులేని దీర్ఘ imsic_ptr_స్థానభ్రంశం;

#నిర్వచించు imsic_get_hart_దత్తాంశం_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), శూన్య *, imsic_ptr_స్థానభ్రంశం)

#నిర్వచించు imsic_సమితి_hart_దత్తాంశం_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), శూన్య *, imsic_ptr_స్థానభ్రంశం, (__imsic))

స్థిర గుర్తులేని దీర్ఘ imsic_file_స్థానభ్రంశం;

#నిర్వచించు imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), దీర్ఘ, imsic_file_స్థానభ్రంశం)

#నిర్వచించు imsic_సమితి_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), దీర్ఘ, imsic_file_స్థానభ్రంశం, (__file))

పూర్ణాంకం imsic_map_hartid_గమ్యం_దత్తాంశం(u32 hartid, నిర్మాణం imsic_దత్తాంశం *imsic, పూర్ణాంకం file)
{
	నిర్మాణం sbi_scratch *scratch;

	ఒకవేళ (!imsic || !imsic->targets_mmode)
		తిరిగి SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_గమ్యం_scratch(hartid);
	ఒకవేళ (!scratch)
		తిరిగి 0;

	imsic_సమితి_hart_దత్తాంశం_ptr(scratch, imsic);
	imsic_సమితి_hart_file(scratch, file);
	తిరిగి 0;
}

నిర్మాణం imsic_దత్తాంశం *imsic_get_దత్తాంశం(u32 hartindex)
{
	నిర్మాణం sbi_scratch *scratch;

	ఒకవేళ (!imsic_ptr_స్థానభ్రంశం)
		తిరిగి ఖాళీ;

	scratch = sbi_hartindex_గమ్యం_scratch(hartindex);
	ఒకవేళ (!scratch)
		తిరిగి ఖాళీ;

	తిరిగి imsic_get_hart_దత్తాంశం_ptr(scratch);
}

పూర్ణాంకం imsic_get_లక్ష్యం_file(u32 hartindex)
{
	నిర్మాణం sbi_scratch *scratch;

	ఒకవేళ (!imsic_file_స్థానభ్రంశం)
		తిరిగి SBI_ENOENT;

	scratch = sbi_hartindex_గమ్యం_scratch(hartindex);
	ఒకవేళ (!scratch)
		తిరిగి SBI_ENOENT;

	తిరిగి imsic_get_hart_file(scratch);
}

స్థిర పూర్ణాంకం imsic_process_hwirqs(నిర్మాణం sbi_irqchip_device *chip)
{
	నిర్మాణం imsic_దత్తాంశం *imsic;
	ulong mirq;
	u32 hwirq;
	పూర్ణాంకం rc, ret = 0;

	imsic = container_యొక్క(chip, నిర్మాణం imsic_దత్తాంశం, irqchip);
	ఒకవేళ (!imsic || !imsic->targets_mmode)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (imsic_get_దత్తాంశం(current_hartindex()) != imsic)
		తిరిగి 0;

	అయితే ((mirq = csr_మార్పిడి(CSR_MTOPEI, 0))) {
		hwirq = (mirq >> IMSIC_TOPEI_ID_జరుపు) &
			IMSIC_TOPEI_ID_ముసుగు;

		ఒకవేళ (hwirq > imsic->అంకె_ids) {
			sbi_printf("%s: invalid hwirq=%lu num_ids=%lu\n",
					__func__,
					(గుర్తులేని దీర్ఘ)hwirq,
					(గుర్తులేని దీర్ఘ)imsic->అంకె_ids);
			కొనసాగు;
		}

		ఒకవేళ (hwirq == IMSIC_IPI_ID) {
			sbi_ipi_process();
			కొనసాగు;
		}

		rc = sbi_irqchip_process_hwirq(chip, hwirq);
		ఒకవేళ (rc && rc != SBI_ENOENT) {
			sbi_printf("%s: hwirq=%lu failed rc=%d\n",
					__func__,
					(గుర్తులేని దీర్ఘ)hwirq, rc);
			ret = rc;
		}
	}

	తిరిగి ret;
}

స్థిర శూన్య imsic_ipi_send(u32 hart_సూచీ)
{
	గుర్తులేని దీర్ఘ reloff;
	నిర్మాణం imsic_regs *regs;
	నిర్మాణం imsic_దత్తాంశం *దత్తాంశం;
	నిర్మాణం sbi_scratch *scratch;
	పూర్ణాంకం file;

	scratch = sbi_hartindex_గమ్యం_scratch(hart_సూచీ);
	ఒకవేళ (!scratch)
		తిరిగి;

	దత్తాంశం = imsic_get_hart_దత్తాంశం_ptr(scratch);
	file = imsic_get_hart_file(scratch);
	ఒకవేళ (!దత్తాంశం || !దత్తాంశం->targets_mmode)
		తిరిగి;

	regs = &దత్తాంశం->regs[0];
	reloff = file * (1UL << దత్తాంశం->guest_సూచీ_బిట్లు) * IMSIC_MMIO_పుట_SZ;
	అయితే (regs->కొలత && (regs->కొలత <= reloff)) {
		reloff -= regs->కొలత;
		regs++;
	}

	ఒకవేళ (regs->కొలత && (reloff < regs->కొలత))
		writel_relaxed(IMSIC_IPI_ID,
			(శూన్య *)(regs->addr + reloff + IMSIC_MMIO_పుట_LE));
}

స్థిర నిర్మాణం sbi_ipi_device imsic_ipi_device = {
	.name		= "aia-imsic",
	.rating		= 300,
	.ipi_send	= imsic_ipi_send
};

స్థిర శూన్య imsic_local_eix_update(గుర్తులేని దీర్ఘ id,
				   bool pend, bool val)
{
	గుర్తులేని దీర్ఘ isel, ireg = 0;

	isel = id / __riscv_xlen;
	isel *= __riscv_xlen / IMSIC_EIPx_బిట్లు;
	isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;
	ireg |= బిట్టు(id);

	ఒకవేళ (val)
		imsic_csr_సమితి(isel, ireg);
	లేకపోతే
		imsic_csr_clear(isel, ireg);
}

శూన్య imsic_local_irqchip_init(శూన్య)
{
	నిర్మాణం sbi_trap_info trap = { 0 };

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
	ఒకవేళ (trap.cause)
		తిరిగి;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, false, true);
}

స్థిర పూర్ణాంకం imsic_warm_irqchip_init(నిర్మాణం sbi_irqchip_device *dev)
{
	నిర్మాణం imsic_దత్తాంశం *imsic;
	నిర్మాణం imsic_దత్తాంశం *hart_imsic;
	పూర్ణాంకం i;

	imsic = container_యొక్క(dev, నిర్మాణం imsic_దత్తాంశం, irqchip);
	hart_imsic = imsic_get_దత్తాంశం(current_hartindex());

	/* Sanity checks */
	ఒకవేళ (!hart_imsic || hart_imsic != imsic ||
		!hart_imsic->targets_mmode)
		తిరిగి SBI_EINVAL;

	/* enable interrutps based on the irq state */
	కొరకు (i = 1; i < imsic->అంకె_ids; i++) {
		ఒకవేళ (sbi_irqchip_is_hwirq_enabled(&imsic->irqchip, i) == true)
			imsic_local_eix_update(i, true, true);
		లేకపోతే
			imsic_local_eix_update(i, false, false);
	}

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	తిరిగి 0;
}

పూర్ణాంకం imsic_దత్తాంశం_check(నిర్మాణం imsic_దత్తాంశం *imsic)
{
	u32 i, tmp;
	గుర్తులేని దీర్ఘ పునాది_addr, addr, ముసుగు;

	/* Sanity checks */
	ఒకవేళ (!imsic ||
	    (imsic->అంకె_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->అంకె_ids))
		తిరిగి SBI_EINVAL;

	tmp = బిట్లు_చొప్పున_LONG - IMSIC_MMIO_పుట_జరుపు;
	ఒకవేళ (tmp < imsic->guest_సూచీ_బిట్లు)
		తిరిగి SBI_EINVAL;

	tmp = బిట్లు_చొప్పున_LONG - IMSIC_MMIO_పుట_జరుపు -
	      imsic->guest_సూచీ_బిట్లు;
	ఒకవేళ (tmp < imsic->hart_సూచీ_బిట్లు)
		తిరిగి SBI_EINVAL;

	tmp = బిట్లు_చొప్పున_LONG - IMSIC_MMIO_పుట_జరుపు -
	      imsic->guest_సూచీ_బిట్లు - imsic->hart_సూచీ_బిట్లు;
	ఒకవేళ (tmp < imsic->సమూహం_సూచీ_బిట్లు)
		తిరిగి SBI_EINVAL;

	tmp = IMSIC_MMIO_పుట_జరుపు + imsic->guest_సూచీ_బిట్లు +
	      imsic->hart_సూచీ_బిట్లు;
	ఒకవేళ (imsic->సమూహం_సూచీ_జరుపు < tmp)
		తిరిగి SBI_EINVAL;
	tmp = imsic->సమూహం_సూచీ_బిట్లు + imsic->సమూహం_సూచీ_జరుపు - 1;
	ఒకవేళ (tmp >= బిట్లు_చొప్పున_LONG)
		తిరిగి SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	ఒకవేళ ((imsic->అంకె_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		తిరిగి SBI_EINVAL;

	/* We should have at least one regset */
	ఒకవేళ (!imsic->regs[0].కొలత)
		తిరిగి SBI_EINVAL;

	/* Match patter of each regset */
	పునాది_addr = imsic->regs[0].addr;
	పునాది_addr &= ~((1UL << (imsic->guest_సూచీ_బిట్లు +
				 imsic->hart_సూచీ_బిట్లు +
				 IMSIC_MMIO_పుట_జరుపు)) - 1);
	పునాది_addr &= ~(((1UL << imsic->సమూహం_సూచీ_బిట్లు) - 1) <<
			imsic->సమూహం_సూచీ_జరుపు);
	కొరకు (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].కొలత; i++) {
		ముసుగు = (1UL << imsic->guest_సూచీ_బిట్లు) * IMSIC_MMIO_పుట_SZ;
		ముసుగు -= 1UL;
		ఒకవేళ (imsic->regs[i].కొలత & ముసుగు)
			తిరిగి SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_సూచీ_బిట్లు +
					 imsic->hart_సూచీ_బిట్లు +
					 IMSIC_MMIO_పుట_జరుపు)) - 1);
		addr &= ~(((1UL << imsic->సమూహం_సూచీ_బిట్లు) - 1) <<
				imsic->సమూహం_సూచీ_జరుపు);
		ఒకవేళ (పునాది_addr != addr)
			తిరిగి SBI_EINVAL;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం imsic_hwirq_అమరిక(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq, u32 hwirq_flags)
{
	ఒకవేళ (hwirq_flags != SBI_HWIRQ_FLAGS_NONE)
		తిరిగి SBI_ENOTSUPP;

	sbi_irqchip_సమితి_raw_handler(chip, hwirq,
				    sbi_irqchip_raw_handler_default);

	తిరిగి 0;
}

స్థిర శూన్య imsic_hwirq_cleanup(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం imsic_దత్తాంశం *imsic;

	ఒకవేళ (!chip)
		తిరిగి;

	imsic = container_యొక్క(chip, నిర్మాణం imsic_దత్తాంశం, irqchip);
	ఒకవేళ (!imsic || !imsic->targets_mmode)
		తిరిగి;

	imsic_local_eix_update(hwirq, false, false);
	imsic_local_eix_update(hwirq, true, false);
}

స్థిర శూన్య imsic_hwirq_eoi(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	/*
	 * IMSIC interrupt claim/ack is already done by reading CSR_MTOPEI
	 * in imsic_process_hwirqs(). No extra EOI operation is required.
	 */
}

స్థిర పూర్ణాంకం imsic_compose_msi_సందేశం(నిర్మాణం imsic_దత్తాంశం *imsic,
				 u32 hart_సూచీ, u32 eiid,
				 నిర్మాణం sbi_irqchip_msi_సందేశం *సందేశం)
{
	నిర్మాణం imsic_regs *regs;
	గుర్తులేని దీర్ఘ reloff;
	u64 msi_addr;
	పూర్ణాంకం file;

	ఒకవేళ (!imsic || !సందేశం)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!eiid || eiid == IMSIC_IPI_ID)
		తిరిగి 0;

	file = imsic_get_లక్ష్యం_file(hart_సూచీ);
	ఒకవేళ (file < 0) {
		sbi_printf("%s: no file for hart=%lu rc=%d\n",
				__func__,
				(గుర్తులేని దీర్ఘ)hart_సూచీ, file);
		తిరిగి file;
	}

	regs   = &imsic->regs[0];
	reloff = file * (1UL << imsic->guest_సూచీ_బిట్లు) * IMSIC_MMIO_పుట_SZ;

	అయితే (regs->కొలత && regs->కొలత <= reloff) {
		reloff -= regs->కొలత;
		regs++;
	}

	ఒకవేళ (!regs->కొలత || regs->కొలత <= reloff) {
		sbi_printf("%s: no regset for hart=%lu file=%d reloff=0x%lx\n",
			   __func__,
			   (గుర్తులేని దీర్ఘ)hart_సూచీ, file, reloff);
		తిరిగి SBI_ENODEV;
	}

	msi_addr = (u64)regs->addr + reloff + IMSIC_MMIO_పుట_LE;
	సందేశం->చిరునామా_lo = (u32)(msi_addr);
	సందేశం->చిరునామా_hi = (u32)(msi_addr >> 32);
	సందేశం->దత్తాంశం       = eiid;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం imsic_కార్యక్రమం_msi(నిర్మాణం sbi_irqchip_device *chip,
		      u32 eiid, u32 hart_సూచీ)
{
	నిర్మాణం imsic_దత్తాంశం *imsic;
	నిర్మాణం sbi_irqchip_msi_సందేశం సందేశం;
	పూర్ణాంకం rc;

	ఒకవేళ (!chip)
		తిరిగి SBI_EINVAL;

	imsic = imsic_get_దత్తాంశం(hart_సూచీ);
	ఒకవేళ (!imsic || !imsic->targets_mmode)
		తిరిగి SBI_ENODEV;

	rc = imsic_compose_msi_సందేశం(imsic, hart_సూచీ, eiid, &సందేశం);
	ఒకవేళ (rc) {
		sbi_printf("%s: compose failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(గుర్తులేని దీర్ఘ)eiid,
				(గుర్తులేని దీర్ఘ)hart_సూచీ, rc);
	}

	rc = sbi_irqchip_write_msi(chip, eiid, &సందేశం);
	ఒకవేళ (rc) {
		sbi_printf("%s: write_msi failed eiid=%lu hart=%lu rc=%d\n",
				__func__,
				(గుర్తులేని దీర్ఘ)eiid,
				(గుర్తులేని దీర్ఘ)hart_సూచీ, rc);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం imsic_hwirq_సమితి_affinity(నిర్మాణం sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_సూచీ)
{
	నిర్మాణం imsic_దత్తాంశం *imsic;
	పూర్ణాంకం rc;

	ఒకవేళ (!chip)
		తిరిగి SBI_EINVAL;

	imsic = container_యొక్క(chip, నిర్మాణం imsic_దత్తాంశం, irqchip);
	ఒకవేళ (!imsic || !imsic->targets_mmode)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!hwirq || hwirq == IMSIC_IPI_ID)
		తిరిగి 0;

	ఒకవేళ (!sbi_hartmask_test_hartindex(hart_సూచీ, &chip->లక్ష్యం_harts))
		తిరిగి SBI_EINVAL;

	rc = imsic_కార్యక్రమం_msi(chip, hwirq, hart_సూచీ);
	ఒకవేళ (rc) {
		sbi_printf("%s: failed hwirq=%lu hart=%lu rc=%d\n",
				__func__,
				(గుర్తులేని దీర్ఘ)hwirq,
				(గుర్తులేని దీర్ఘ)hart_సూచీ, rc);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర శూన్య imsic_hwirq_ముసుగు(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం imsic_దత్తాంశం *imsic;

	ఒకవేళ (!chip)
		తిరిగి;

	imsic = container_యొక్క(chip, నిర్మాణం imsic_దత్తాంశం, irqchip);
	ఒకవేళ (!imsic || !imsic->targets_mmode)
		తిరిగి;

	imsic_local_eix_update(hwirq, false, false);
}

స్థిర శూన్య imsic_hwirq_unmask(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం imsic_దత్తాంశం *imsic;

	ఒకవేళ (!chip)
		తిరిగి;

	imsic = container_యొక్క(chip, నిర్మాణం imsic_దత్తాంశం, irqchip);
	ఒకవేళ (!imsic || !imsic->targets_mmode)
		తిరిగి;

	ఒకవేళ (!hwirq || hwirq == IMSIC_IPI_ID)
		తిరిగి;

	imsic_local_eix_update(hwirq, false, true);
}

స్థిర నిర్మాణం sbi_irqchip_device imsic_device = {
	.warm_init		= imsic_warm_irqchip_init,
	.process_hwirqs		= imsic_process_hwirqs,
	.hwirq_అమరిక		= imsic_hwirq_అమరిక,
	.hwirq_cleanup		= imsic_hwirq_cleanup,
	.hwirq_eoi		= imsic_hwirq_eoi,
	.hwirq_సమితి_affinity	= imsic_hwirq_సమితి_affinity,
	.hwirq_ముసుగు		= imsic_hwirq_ముసుగు,
	.hwirq_unmask		= imsic_hwirq_unmask,
};

పూర్ణాంకం imsic_cold_irqchip_init(నిర్మాణం imsic_దత్తాంశం *imsic)
{
	పూర్ణాంకం i, rc;

	/* Sanity checks */
	rc = imsic_దత్తాంశం_check(imsic);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* We only initialize M-mode IMSIC */
	ఒకవేళ (!imsic->targets_mmode)
		తిరిగి SBI_EINVAL;

	/* Allocate scratch space pointer */
	ఒకవేళ (!imsic_ptr_స్థానభ్రంశం) {
		imsic_ptr_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(శూన్య *);
		ఒకవేళ (!imsic_ptr_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	ఒకవేళ (!imsic_file_స్థానభ్రంశం) {
		imsic_file_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(దీర్ఘ);
		ఒకవేళ (!imsic_file_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	/* Add IMSIC regions to the root domain */
	కొరకు (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].కొలత; i++) {
		rc = sbi_domain_మూలం_కూడు_memrange(imsic->regs[i].addr,
						  imsic->regs[i].కొలత,
						  IMSIC_MMIO_పుట_SZ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	imsic->irqchip          = imsic_device;
	imsic->irqchip.id       = imsic->unique_id;
	imsic_device.caps = SBI_IRQCHIP_CAPS_MSI;
	imsic->irqchip.అంకె_hwirq = imsic->అంకె_ids + 1;

	sbi_hartmask_సమితి_all(&imsic->irqchip.లక్ష్యం_harts);

	/* Register irqchip device */
	rc = sbi_irqchip_కూడు_device(&imsic->irqchip);
	ఒకవేళ (rc) {
		sbi_printf("%s: sbi_irqchip_add_device failed rc=%d\n",
				__func__, rc);
		తిరిగి rc;
	}

	rc = sbi_irqchip_రిజిస్టరు_reserved(&imsic->irqchip, 0, IMSIC_IPI_ID + 1);
	ఒకవేళ (rc) {
		sbi_printf("%s: register_reserved failed rc=%d\n",
				__func__, rc);
		తిరిగి rc;
	}

	/* Register IPI device */
	sbi_ipi_కూడు_device(&imsic_ipi_device);

	తిరిగి 0;
}
