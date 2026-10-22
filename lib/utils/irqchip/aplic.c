/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/irqchip/aplic.h>

#నిర్వచించు APLIC_MAX_IDC			(1UL << 14)
#నిర్వచించు APLIC_MAX_ఆధారం		1024

#నిర్వచించు APLIC_DOMAINCFG		0x0000
#నిర్వచించు APLIC_DOMAINCFG_IE		(1 << 8)
#నిర్వచించు APLIC_DOMAINCFG_DM		(1 << 2)
#నిర్వచించు APLIC_DOMAINCFG_BE		(1 << 0)

#నిర్వచించు APLIC_SOURCECFG_పునాది		0x0004
#నిర్వచించు APLIC_SOURCECFG_D		(1 << 10)
#నిర్వచించు APLIC_SOURCECFG_CHILDIDX_ముసుగు	0x000003ff
#నిర్వచించు APLIC_SOURCECFG_SM_ముసుగు	0x00000007
#నిర్వచించు APLIC_SOURCECFG_SM_INACTIVE	0x0
#నిర్వచించు APLIC_SOURCECFG_SM_DETACH	0x1
#నిర్వచించు APLIC_SOURCECFG_SM_అంచు_RISE	0x4
#నిర్వచించు APLIC_SOURCECFG_SM_అంచు_FALL	0x5
#నిర్వచించు APLIC_SOURCECFG_SM_LEVEL_ఎగువ	0x6
#నిర్వచించు APLIC_SOURCECFG_SM_LEVEL_దిగువ	0x7

#నిర్వచించు APLIC_MMSICFGADDR		0x1bc0
#నిర్వచించు APLIC_MMSICFGADDRH		0x1bc4
#నిర్వచించు APLIC_SMSICFGADDR		0x1bc8
#నిర్వచించు APLIC_SMSICFGADDRH		0x1bcc

#నిర్వచించు APLIC_xMSICFGADDRH_L		(1UL << 31)
#నిర్వచించు APLIC_x_MSICFGADDRH_HHXS_ముసుగు	0x1f
#నిర్వచించు APLIC_x_MSICFGADDRH_HHXS_జరుపు	24
#నిర్వచించు APLIC_x_MSICFGADDRH_LHXS_ముసుగు	0x7
#నిర్వచించు APLIC_x_MSICFGADDRH_LHXS_జరుపు	20
#నిర్వచించు APLIC_x_MSICFGADDRH_HHXW_ముసుగు	0x7
#నిర్వచించు APLIC_x_MSICFGADDRH_HHXW_జరుపు	16
#నిర్వచించు APLIC_x_MSICFGADDRH_LHXW_ముసుగు	0xf
#నిర్వచించు APLIC_x_MSICFGADDRH_LHXW_జరుపు	12
#నిర్వచించు APLIC_x_MSICFGADDRH_BAPPN_ముసుగు	0xfff

#నిర్వచించు APLIC_x_MSICFGADDR_PPN_జరుపు	12

#నిర్వచించు APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#నిర్వచించు APLIC_x_MSICFGADDR_PPN_LHX_ముసుగు(__lhxw) \
	((1UL << (__lhxw)) - 1)
#నిర్వచించు APLIC_x_MSICFGADDR_PPN_LHX_జరుపు(__lhxs) \
	((__lhxs))
#నిర్వచించు APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_ముసుగు(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_జరుపు(__lhxs))

#నిర్వచించు APLIC_x_MSICFGADDR_PPN_HHX_ముసుగు(__hhxw) \
	((1UL << (__hhxw)) - 1)
#నిర్వచించు APLIC_x_MSICFGADDR_PPN_HHX_జరుపు(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_జరుపు)
#నిర్వచించు APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_ముసుగు(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_జరుపు(__hhxs))

#నిర్వచించు APLIC_లక్ష్యం_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_లక్ష్యం_GUEST_IDX_ముసుగు) << \
	 APLIC_లక్ష్యం_GUEST_IDX_జరుపు)
#నిర్వచించు APLIC_లక్ష్యం_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_లక్ష్యం_EIID_ముసుగు)

#నిర్వచించు APLIC_SETIP_పునాది		0x1c00
#నిర్వచించు APLIC_SETIPNUM			0x1cdc

#నిర్వచించు APLIC_CLRIP_పునాది		0x1d00
#నిర్వచించు APLIC_CLRIPNUM			0x1ddc

#నిర్వచించు APLIC_SETIE_పునాది		0x1e00
#నిర్వచించు APLIC_SETIENUM			0x1edc

#నిర్వచించు APLIC_CLRIE_పునాది		0x1f00
#నిర్వచించు APLIC_CLRIENUM			0x1fdc

#నిర్వచించు APLIC_SETIPNUM_LE		0x2000
#నిర్వచించు APLIC_SETIPNUM_BE		0x2004

#నిర్వచించు APLIC_లక్ష్యం_పునాది		0x3004
#నిర్వచించు APLIC_లక్ష్యం_HART_IDX_జరుపు	18
#నిర్వచించు APLIC_లక్ష్యం_HART_IDX_ముసుగు	0x3fff
#నిర్వచించు APLIC_లక్ష్యం_GUEST_IDX_జరుపు	12
#నిర్వచించు APLIC_లక్ష్యం_GUEST_IDX_ముసుగు	0x3f
#నిర్వచించు APLIC_లక్ష్యం_IPRIO_ముసుగు	0xff
#నిర్వచించు APLIC_లక్ష్యం_EIID_ముసుగు	0x7ff

#నిర్వచించు APLIC_లక్ష్యం_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_లక్ష్యం_HART_IDX_ముసుగు) << \
	 APLIC_లక్ష్యం_HART_IDX_జరుపు)
#నిర్వచించు APLIC_లక్ష్యం_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_లక్ష్యం_IPRIO_ముసుగు)

#నిర్వచించు APLIC_IDC_పునాది			0x4000
#నిర్వచించు APLIC_IDC_కొలత			32

#నిర్వచించు APLIC_IDC_IDELIVERY		0x00

#నిర్వచించు APLIC_IDC_IFORCE		0x04

#నిర్వచించు APLIC_IDC_ITHRESHOLD		0x08

#నిర్వచించు APLIC_IDC_TOPI			0x18
#నిర్వచించు APLIC_IDC_TOPI_ID_జరుపు	16
#నిర్వచించు APLIC_IDC_TOPI_ID_ముసుగు	0x3ff
#నిర్వచించు APLIC_IDC_TOPI_PRIO_ముసుగు	0xff

#నిర్వచించు APLIC_IDC_CLAIMI		0x1c

#నిర్వచించు APLIC_DEFAULT_ప్రాధాన్యత		1
#నిర్వచించు APLIC_DISABLE_IDELIVERY		0
#నిర్వచించు APLIC_ENABLE_IDELIVERY		1
#నిర్వచించు APLIC_DISABLE_ITHRESHOLD	1
#నిర్వచించు APLIC_ENABLE_ITHRESHOLD		0

నిర్మాణం aplic_msi_దత్తాంశం {
	నిర్మాణం aplic_దత్తాంశం *aplic;
	u32 hwirq;
	u32 మాతృ_hwirq;
};

స్థిర SBI_LIST_తల(aplic_list);
స్థిర శూన్య aplic_writel_msicfg(నిర్మాణం aplic_msicfg_దత్తాంశం *msicfg,
				శూన్య *msicfgaddr, శూన్య *msicfgaddrH);

స్థిర అంతఃవరుస bool aplic_is_ప్రత్యక్ష_బహుళకం(నిర్మాణం aplic_దత్తాంశం *aplic)
{
	తిరిగి aplic && aplic->అంకె_idc;
}

స్థిర అంతఃవరుస శూన్య aplic_sourcecfg_write(నిర్మాణం aplic_దత్తాంశం *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (శూన్య *)(aplic->addr + APLIC_SOURCECFG_పునాది +
			     (hwirq - 1) * పరిమాణం(u32)));
}

స్థిర అంతఃవరుస u32 aplic_sourcecfg_read(నిర్మాణం aplic_దత్తాంశం *aplic, u32 hwirq)
{
	తిరిగి readl((శూన్య *)(aplic->addr + APLIC_SOURCECFG_పునాది +
			      (hwirq - 1) * పరిమాణం(u32)));
}

స్థిర అంతఃవరుస శూన్య aplic_లక్ష్యం_write(నిర్మాణం aplic_దత్తాంశం *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (శూన్య *)(aplic->addr + APLIC_లక్ష్యం_పునాది +
			     (hwirq - 1) * పరిమాణం(u32)));
}

స్థిర అంతఃవరుస శూన్య aplic_irq_setie(నిర్మాణం aplic_దత్తాంశం *aplic, u32 hwirq)
{
	writel(hwirq, (శూన్య *)(aplic->addr + APLIC_SETIENUM));
}

స్థిర అంతఃవరుస శూన్య aplic_irq_clrie(నిర్మాణం aplic_దత్తాంశం *aplic, u32 hwirq)
{
	writel(hwirq, (శూన్య *)(aplic->addr + APLIC_CLRIENUM));
}

స్థిర అంతఃవరుస శూన్య aplic_irq_clrip(నిర్మాణం aplic_దత్తాంశం *aplic, u32 hwirq)
{
	writel(hwirq, (శూన్య *)(aplic->addr + APLIC_CLRIPNUM));
}

స్థిర అంతఃవరుస శూన్య aplic_msi_irq_retrigger_level(నిర్మాణం aplic_దత్తాంశం *aplic, u32 hwirq)
{
	/*
	 * The section "4.9.2 Special consideration for level-sensitive interrupt
	 * sources" of the RISC-V AIA specification says:
	 *
	 * A second option is for the interrupt service routine to write the
	 * APLIC’s source identity number for the interrupt to the domain’s
	 * setipnum register just before exiting. This will cause the interrupt’s
	 * pending bit to be set to one again if the source is still asserting
	 * an interrupt, but not if the source is not asserting an interrupt.
	 */
	writel(hwirq, (శూన్య *)(aplic->addr + APLIC_SETIPNUM_LE));
}

స్థిర అంతఃవరుస శూన్య *aplic_idc_పునాది(నిర్మాణం aplic_దత్తాంశం *aplic, u32 idc_సూచీ)
{
	తిరిగి (శూన్య *)(aplic->addr + APLIC_IDC_పునాది +
			idc_సూచీ * APLIC_IDC_కొలత);
}

స్థిర అంతఃవరుస u32 aplic_idc_read(నిర్మాణం aplic_దత్తాంశం *aplic, u32 idc_సూచీ,
				 u32 reg)
{
	తిరిగి readl(aplic_idc_పునాది(aplic, idc_సూచీ) + reg);
}

స్థిర అంతఃవరుస శూన్య aplic_idc_write(నిర్మాణం aplic_దత్తాంశం *aplic, u32 idc_సూచీ,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_పునాది(aplic, idc_సూచీ) + reg);
}

స్థిర u32 aplic_hwirq_flags_గమ్యం_sourcecfg(u32 hwirq_flags)
{
	ఎంపిక (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_ముసుగు) {
	స్థితి SBI_HWIRQ_FLAGS_అంచు_RISING:
		తిరిగి APLIC_SOURCECFG_SM_అంచు_RISE;
	స్థితి SBI_HWIRQ_FLAGS_అంచు_FALLING:
		తిరిగి APLIC_SOURCECFG_SM_అంచు_FALL;
	స్థితి SBI_HWIRQ_FLAGS_LEVEL_ఎగువ:
		తిరిగి APLIC_SOURCECFG_SM_LEVEL_ఎగువ;
	స్థితి SBI_HWIRQ_FLAGS_LEVEL_దిగువ:
		తిరిగి APLIC_SOURCECFG_SM_LEVEL_దిగువ;
	అప్రమేయ:
		తిరిగి APLIC_SOURCECFG_SM_INACTIVE;
	}
}

స్థిర bool aplic_hwirq_is_delegated(నిర్మాణం aplic_దత్తాంశం *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	తిరిగి !!(sourcecfg & APLIC_SOURCECFG_D);
}

స్థిర పూర్ణాంకం aplic_కనుగొను_idc_సూచీ(నిర్మాణం aplic_దత్తాంశం *aplic, u32 hart_సూచీ)
{
	u32 i;

	కొరకు (i = 0; i < aplic->అంకె_idc; i++) {
		ఒకవేళ (aplic->idc_map[i] == hart_సూచీ)
			తిరిగి i;
	}

	తిరిగి SBI_ENOENT;
}

స్థిర శూన్య aplic_init(నిర్మాణం aplic_దత్తాంశం *aplic)
{
	నిర్మాణం aplic_delegate_దత్తాంశం *deleg;
	u32 i, j, tmp, domaincfg;
	పూర్ణాంకం locked;

	/* Set domain configuration to 0 */
	writel(0, (శూన్య *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	కొరకు (i = 0; i <= aplic->అంకె_ఆధారం; i += 32)
		writel(-1U, (శూన్య *)(aplic->addr + APLIC_CLRIE_పునాది +
				     (i / 32) * పరిమాణం(u32)));

	/* Set interrupt type and priority for all interrupts */
	కొరకు (i = 1; i <= aplic->అంకె_ఆధారం; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (శూన్య *)(aplic->addr + APLIC_SOURCECFG_పునాది +
			  (i - 1) * పరిమాణం(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_ప్రాధాన్యత, (శూన్య *)(aplic->addr +
						APLIC_లక్ష్యం_పునాది +
						(i - 1) * పరిమాణం(u32)));
	}

	/* Configure IRQ delegation */
	కొరకు (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ఒకవేళ (!deleg->మొదటి_irq || !deleg->చివరి_irq)
			కొనసాగు;
		ఒకవేళ (aplic->అంకె_ఆధారం < deleg->మొదటి_irq ||
		    aplic->అంకె_ఆధారం < deleg->చివరి_irq)
			కొనసాగు;
		ఒకవేళ (deleg->బిడ్డ_సూచీ > APLIC_SOURCECFG_CHILDIDX_ముసుగు)
			కొనసాగు;
		ఒకవేళ (deleg->మొదటి_irq > deleg->చివరి_irq) {
			tmp = deleg->మొదటి_irq;
			deleg->మొదటి_irq = deleg->చివరి_irq;
			deleg->చివరి_irq = tmp;
		}
		కొరకు (j = deleg->మొదటి_irq; j <= deleg->చివరి_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->బిడ్డ_సూచీ);
	}

	/* Default initialization of IDC structures */
	ఒకవేళ (aplic_is_ప్రత్యక్ష_బహుళకం(aplic)) {
		కొరకు (i = 0; i < aplic->అంకె_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} లేకపోతే {
		/* MSI configuration */
		locked = readl((శూన్య *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		ఒకవేళ (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(శూన్య *)(aplic->addr + APLIC_MMSICFGADDR),
						(శూన్య *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		ఒకవేళ (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(శూన్య *)(aplic->addr + APLIC_SMSICFGADDR),
						(శూన్య *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	ఒకవేళ (!aplic_is_ప్రత్యక్ష_బహుళకం(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (శూన్య *)(aplic->addr + APLIC_DOMAINCFG));
}

శూన్య aplic_reinit_all(శూన్య)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;

	sbi_list_for_each_నమోదు(aplic, &aplic_list, కణుపు)
		aplic_init(aplic);
}

స్థిర శూన్య aplic_writel_msicfg(నిర్మాణం aplic_msicfg_దత్తాంశం *msicfg,
				శూన్య *msicfgaddr, శూన్య *msicfgaddrH)
{
	u32 val;
	గుర్తులేని దీర్ఘ పునాది_ppn;

	/* Compute the MSI base PPN */
	పునాది_ppn = msicfg->పునాది_addr >> APLIC_x_MSICFGADDR_PPN_జరుపు;
	పునాది_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	పునాది_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	పునాది_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)పునాది_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)పునాది_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_ముసుగు;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_ముసుగు)
		<< APLIC_x_MSICFGADDRH_LHXW_జరుపు;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_ముసుగు)
		<< APLIC_x_MSICFGADDRH_HHXW_జరుపు;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_ముసుగు)
		<< APLIC_x_MSICFGADDRH_LHXS_జరుపు;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_ముసుగు)
		<< APLIC_x_MSICFGADDRH_HHXS_జరుపు;
	writel(val, msicfgaddrH);
}

స్థిర పూర్ణాంకం aplic_check_msicfg(నిర్మాణం aplic_msicfg_దత్తాంశం *msicfg)
{
	ఒకవేళ (APLIC_x_MSICFGADDRH_LHXS_ముసుగు < msicfg->lhxs)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (APLIC_x_MSICFGADDRH_LHXW_ముసుగు < msicfg->lhxw)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (APLIC_x_MSICFGADDRH_HHXS_ముసుగు < msicfg->hhxs)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (APLIC_x_MSICFGADDRH_HHXW_ముసుగు < msicfg->hhxw)
		తిరిగి SBI_EINVAL;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం aplic_warm_init(నిర్మాణం sbi_irqchip_device *chip)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;
	u32 hart_సూచీ;
	పూర్ణాంకం idc_సూచీ;

	aplic = container_యొక్క(chip, నిర్మాణం aplic_దత్తాంశం, irqchip);
	ఒకవేళ (!aplic_is_ప్రత్యక్ష_బహుళకం(aplic))
		తిరిగి 0;

	hart_సూచీ = current_hartindex();
	idc_సూచీ  = aplic_కనుగొను_idc_సూచీ(aplic, hart_సూచీ);
	ఒకవేళ (idc_సూచీ < 0)
		తిరిగి 0;

	aplic_idc_write(aplic, idc_సూచీ, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_సూచీ, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం aplic_process_hwirqs(నిర్మాణం sbi_irqchip_device *chip)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;
	u32 hart_సూచీ, claimi, hwirq;
	పూర్ణాంకం idc_సూచీ, rc = 0, tmp;

	aplic = container_యొక్క(chip, నిర్మాణం aplic_దత్తాంశం, irqchip);

	ఒకవేళ (!aplic_is_ప్రత్యక్ష_బహుళకం(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_సూచీ = current_hartindex();
	idc_సూచీ  = aplic_కనుగొను_idc_సూచీ(aplic, hart_సూచీ);
	ఒకవేళ (idc_సూచీ < 0)
		తిరిగి 0;

	అయితే ((claimi = aplic_idc_read(aplic, idc_సూచీ, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_జరుపు) &
			APLIC_IDC_TOPI_ID_ముసుగు;

		ఒకవేళ (!hwirq)
			ఆపు;

		ఒకవేళ (aplic_hwirq_is_delegated(aplic, hwirq))
			కొనసాగు;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		ఒకవేళ (tmp)
			rc = tmp;
	}

	తిరిగి rc;
}

స్థిర శూన్య aplic_hwirq_eoi(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;
	u32 sm;

	aplic = container_యొక్క(chip, నిర్మాణం aplic_దత్తాంశం, irqchip);
	ఒకవేళ (aplic_is_ప్రత్యక్ష_బహుళకం(aplic))
		తిరిగి;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_ముసుగు;
	ఒకవేళ (sm == APLIC_SOURCECFG_SM_LEVEL_ఎగువ ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_దిగువ)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

స్థిర శూన్య aplic_కార్యక్రమం_msi_లక్ష్యం(నిర్మాణం aplic_దత్తాంశం *aplic,
				     u32 hwirq, u32 hart_సూచీ,
				     u32 guest_సూచీ, u32 eiid)
{
	u32 లక్ష్యం;

	లక్ష్యం = APLIC_లక్ష్యం_HART_IDX(hart_సూచీ) |
		 APLIC_లక్ష్యం_GUEST_IDX(guest_సూచీ) |
		 APLIC_లక్ష్యం_EIID(eiid);

	aplic_లక్ష్యం_write(aplic, hwirq, లక్ష్యం);
}

స్థిర u32 derive_hart_సూచీ(నిర్మాణం aplic_msicfg_దత్తాంశం *msicfg,
			      స్థిరరాశి నిర్మాణం sbi_irqchip_msi_సందేశం *సందేశం)
{
	u64 addr = ((u64)సందేశం->చిరునామా_hi << 32) | సందేశం->చిరునామా_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_జరుపు;
	u32 సమూహం_సూచీ, hart_సూచీ;

	సమూహం_సూచీ = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_జరుపు(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_ముసుగు(msicfg->hhxw);

	hart_సూచీ = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_సూచీ |= (సమూహం_సూచీ << msicfg->lhxw);

	తిరిగి hart_సూచీ;
}

స్థిర శూన్య aplic_write_msi(u32 మాతృ_hwirq,
			    స్థిరరాశి నిర్మాణం sbi_irqchip_msi_సందేశం *సందేశం,
			    శూన్య *priv)
{
	నిర్మాణం aplic_msi_దత్తాంశం *msi_దత్తాంశం = priv;
	u32 guest_సూచీ = 0;
	u32 eiid;

	eiid = సందేశం->దత్తాంశం;
	ఒకవేళ (!eiid || eiid > APLIC_లక్ష్యం_EIID_ముసుగు)
		తిరిగి;

	aplic_కార్యక్రమం_msi_లక్ష్యం(msi_దత్తాంశం->aplic, msi_దత్తాంశం->hwirq,
				derive_hart_సూచీ(&msi_దత్తాంశం->aplic->msicfg_mmode, సందేశం),
				guest_సూచీ, eiid);
}

స్థిర పూర్ణాంకం aplic_msi_callback(u32 మాతృ_hwirq, శూన్య *priv)
{
	నిర్మాణం aplic_msi_దత్తాంశం *msi_దత్తాంశం = priv;
	పూర్ణాంకం rc;

	rc = sbi_irqchip_process_hwirq(&msi_దత్తాంశం->aplic->irqchip, msi_దత్తాంశం->hwirq);
	ఒకవేళ (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(గుర్తులేని దీర్ఘ)మాతృ_hwirq, rc);

	తిరిగి rc;
}

స్థిర పూర్ణాంకం aplic_అమరిక_msi(నిర్మాణం aplic_దత్తాంశం *aplic, u32 hwirq)
{
	నిర్మాణం sbi_irqchip_device *మాతృ;
	u32 మొదటి_hwirq;
	నిర్మాణం aplic_msi_దత్తాంశం *msi_దత్తాంశం;
	పూర్ణాంకం rc;

	మాతృ = sbi_irqchip_కనుగొను_device(aplic->మాతృ_unique_id);
	ఒకవేళ (!మాతృ) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (గుర్తులేని దీర్ఘ)hwirq);
		తిరిగి SBI_EINVAL;
	}

	msi_దత్తాంశం = sbi_zalloc(పరిమాణం(నిర్మాణం aplic_msi_దత్తాంశం));
	ఒకవేళ (!msi_దత్తాంశం)
		తిరిగి SBI_ENOMEM;

	msi_దత్తాంశం->aplic = aplic;
	msi_దత్తాంశం->hwirq = hwirq;

	rc = sbi_irqchip_రిజిస్టరు_msi(మాతృ, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_దత్తాంశం,
					&మొదటి_hwirq);
	ఒకవేళ (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (గుర్తులేని దీర్ఘ)hwirq, rc);
		sbi_free(msi_దత్తాంశం);
		తిరిగి rc;
	}

	msi_దత్తాంశం->మాతృ_hwirq = మొదటి_hwirq;
	sbi_irqchip_సమితి_hwirq_priv(&aplic->irqchip, hwirq, msi_దత్తాంశం);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం aplic_hwirq_అమరిక(నిర్మాణం sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;
	u32 sourcecfg;

	aplic = container_యొక్క(chip, నిర్మాణం aplic_దత్తాంశం, irqchip);

	sourcecfg = aplic_hwirq_flags_గమ్యం_sourcecfg(hwirq_flags);
	ఒకవేళ (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(గుర్తులేని దీర్ఘ)hwirq_flags,
			(గుర్తులేని దీర్ఘ)hwirq);
		తిరిగి SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	ఒకవేళ (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (గుర్తులేని దీర్ఘ)hwirq);
		తిరిగి SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	ఒకవేళ (!aplic_is_ప్రత్యక్ష_బహుళకం(aplic))
		తిరిగి aplic_అమరిక_msi(aplic, hwirq);

	తిరిగి 0;
}

స్థిర శూన్య aplic_hwirq_cleanup(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;
	నిర్మాణం aplic_msi_దత్తాంశం *msi_దత్తాంశం;

	aplic = container_యొక్క(chip, నిర్మాణం aplic_దత్తాంశం, irqchip);
	ఒకవేళ (aplic_hwirq_is_delegated(aplic, hwirq))
		తిరిగి;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_లక్ష్యం_write(aplic, hwirq, APLIC_DEFAULT_ప్రాధాన్యత);
	msi_దత్తాంశం = (నిర్మాణం aplic_msi_దత్తాంశం *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	ఒకవేళ (msi_దత్తాంశం)
		sbi_free(msi_దత్తాంశం);
}

స్థిర పూర్ణాంకం aplic_hwirq_సమితి_affinity(నిర్మాణం sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_సూచీ)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;
	నిర్మాణం aplic_msi_దత్తాంశం *msi_దత్తాంశం;
	నిర్మాణం sbi_irqchip_device *మాతృ;
	పూర్ణాంకం idc_సూచీ;
	పూర్ణాంకం rc;

	aplic = container_యొక్క(chip, నిర్మాణం aplic_దత్తాంశం, irqchip);
	ఒకవేళ (aplic_hwirq_is_delegated(aplic, hwirq))
		తిరిగి SBI_ENOTSUPP;

	ఒకవేళ (!aplic_is_ప్రత్యక్ష_బహుళకం(aplic)) {
		మాతృ = sbi_irqchip_కనుగొను_device(aplic->మాతృ_unique_id);
		ఒకవేళ (!మాతృ) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (గుర్తులేని దీర్ఘ)hwirq);
			తిరిగి SBI_EINVAL;
		}

		msi_దత్తాంశం = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_సమితి_affinity(మాతృ, msi_దత్తాంశం->మాతృ_hwirq, hart_సూచీ);
		ఒకవేళ (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (గుర్తులేని దీర్ఘ)hwirq, rc);
			తిరిగి rc;
		}
		తిరిగి 0;
	}

	idc_సూచీ = aplic_కనుగొను_idc_సూచీ(aplic, hart_సూచీ);
	ఒకవేళ (idc_సూచీ < 0)
		తిరిగి SBI_EINVAL;

	aplic_లక్ష్యం_write(aplic, hwirq,
			   APLIC_లక్ష్యం_HART_IDX(hart_సూచీ) |
			   APLIC_లక్ష్యం_IPRIO(APLIC_DEFAULT_ప్రాధాన్యత));

	తిరిగి 0;
}

స్థిర శూన్య aplic_hwirq_ముసుగు(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;

	aplic = container_యొక్క(chip, నిర్మాణం aplic_దత్తాంశం, irqchip);
	ఒకవేళ (aplic_hwirq_is_delegated(aplic, hwirq))
		తిరిగి;

	aplic_irq_clrie(aplic, hwirq);
}

స్థిర శూన్య aplic_hwirq_unmask(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం aplic_దత్తాంశం *aplic;

	aplic = container_యొక్క(chip, నిర్మాణం aplic_దత్తాంశం, irqchip);
	ఒకవేళ (aplic_hwirq_is_delegated(aplic, hwirq))
		తిరిగి;

	aplic_irq_setie(aplic, hwirq);
}

స్థిర నిర్మాణం sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_అమరిక		= aplic_hwirq_అమరిక,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_సమితి_affinity	= aplic_hwirq_సమితి_affinity,
	.hwirq_ముసుగు			= aplic_hwirq_ముసుగు,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

పూర్ణాంకం aplic_cold_irqchip_init(నిర్మాణం aplic_దత్తాంశం *aplic)
{
	పూర్ణాంకం rc;
	నిర్మాణం aplic_delegate_దత్తాంశం *deleg;
	u32 మొదటి_deleg_irq, చివరి_deleg_irq, i;
	bool msi_బహుళకం;

	msi_బహుళకం = !aplic_is_ప్రత్యక్ష_బహుళకం(aplic);
	ఒకవేళ (!aplic->అంకె_ఆధారం) {
		sbi_printf("%s: num_source is zero\n", __func__);
		తిరిగి SBI_EINVAL;
	}

	ఒకవేళ (APLIC_MAX_ఆధారం <= aplic->అంకె_ఆధారం) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(గుర్తులేని దీర్ఘ)aplic->అంకె_ఆధారం,
			(గుర్తులేని దీర్ఘ)APLIC_MAX_ఆధారం);
		తిరిగి SBI_EINVAL;
	}

	ఒకవేళ (!msi_బహుళకం && APLIC_MAX_IDC <= aplic->అంకె_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(గుర్తులేని దీర్ఘ)aplic->అంకె_idc,
			(గుర్తులేని దీర్ఘ)APLIC_MAX_IDC);
		తిరిగి SBI_EINVAL;
	}

	ఒకవేళ (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		ఒకవేళ (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			తిరిగి rc;
		}
	}

	ఒకవేళ (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		ఒకవేళ (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			తిరిగి rc;
		}
	}

	/* Init the APLIC registers */
	aplic_init(aplic);

	/*
	 * Add APLIC region to the root domain if:
	 * 1) It targets M-mode of any HART directly or via MSIs
	 * 2) All interrupts are delegated to some child APLIC
	 */
	మొదటి_deleg_irq = -1U;
	చివరి_deleg_irq = 0;
	కొరకు (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ఒకవేళ (deleg->మొదటి_irq < మొదటి_deleg_irq)
			మొదటి_deleg_irq = deleg->మొదటి_irq;
		ఒకవేళ (చివరి_deleg_irq < deleg->చివరి_irq)
			చివరి_deleg_irq = deleg->చివరి_irq;
	}

	ఒకవేళ (aplic->targets_mmode ||
	    ((మొదటి_deleg_irq < చివరి_deleg_irq) &&
	    (చివరి_deleg_irq == aplic->అంకె_ఆధారం) &&
	    (మొదటి_deleg_irq == 1))) {
		rc = sbi_domain_మూలం_కూడు_memrange(aplic->addr, aplic->కొలత, పుట_కొలత,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	ఒకవేళ ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.అంకె_hwirq = aplic->అంకె_ఆధారం + 1;

		ఒకవేళ (msi_బహుళకం) {
			aplic->irqchip.warm_init      = ఖాళీ;
			aplic->irqchip.process_hwirqs = ఖాళీ;
			aplic->irqchip.hwirq_eoi      = ఖాళీ;
		}

		ఒకవేళ (msi_బహుళకం)
			sbi_hartmask_సమితి_all(&aplic->irqchip.లక్ష్యం_harts);
		లేకపోతే
			కొరకు (i = 0; i < aplic->అంకె_idc; i++)
				sbi_hartmask_సమితి_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.లక్ష్యం_harts);

		rc = sbi_irqchip_కూడు_device(&aplic->irqchip);
		ఒకవేళ (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(గుర్తులేని దీర్ఘ)aplic->irqchip.id,
				msi_బహుళకం ? "msi" : "direct",
				(గుర్తులేని దీర్ఘ)sbi_hartmask_బరువు(
					&aplic->irqchip.లక్ష్యం_harts));
			తిరిగి rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_కూడు_తోక(&aplic->కణుపు, &aplic_list);

	తిరిగి 0;
}
