/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/irqchip/aplic.h>

#define APLIC_MAX_IDC			(1UL << 14)
#define APLIC_MAX_SOURCE		1024

#define APLIC_DOMAINCFG		0x0000
#define APLIC_DOMAINCFG_IE		(1 << 8)
#define APLIC_DOMAINCFG_DM		(1 << 2)
#define APLIC_DOMAINCFG_BE		(1 << 0)

#define APLIC_SOURCECFG_BASE		0x0004
#define APLIC_SOURCECFG_D		(1 << 10)
#define APLIC_SOURCECFG_CHILDIDX_MASK	0x000003ff
#define APLIC_SOURCECFG_SM_MASK	0x00000007
#define APLIC_SOURCECFG_SM_INACTIVE	0x0
#define APLIC_SOURCECFG_SM_DETACH	0x1
#define APLIC_SOURCECFG_SM_EDGE_RISE	0x4
#define APLIC_SOURCECFG_SM_EDGE_FALL	0x5
#define APLIC_SOURCECFG_SM_LEVEL_উপরের	0x6
#define APLIC_SOURCECFG_SM_LEVEL_নিচের	0x7

#define APLIC_MMSICFGADDR		0x1bc0
#define APLIC_MMSICFGADDRH		0x1bc4
#define APLIC_SMSICFGADDR		0x1bc8
#define APLIC_SMSICFGADDRH		0x1bcc

#define APLIC_xMSICFGADDRH_L		(1UL << 31)
#define APLIC_xMSICFGADDRH_HHXS_MASK	0x1f
#define APLIC_xMSICFGADDRH_HHXS_SHIFT	24
#define APLIC_xMSICFGADDRH_LHXS_MASK	0x7
#define APLIC_xMSICFGADDRH_LHXS_SHIFT	20
#define APLIC_xMSICFGADDRH_HHXW_MASK	0x7
#define APLIC_xMSICFGADDRH_HHXW_SHIFT	16
#define APLIC_xMSICFGADDRH_LHXW_MASK	0xf
#define APLIC_xMSICFGADDRH_LHXW_SHIFT	12
#define APLIC_xMSICFGADDRH_BAPPN_MASK	0xfff

#define APLIC_xMSICFGADDR_PPN_SHIFT	12

#define APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#define APLIC_xMSICFGADDR_PPN_LHX_MASK(__lhxw) \
	((1UL << (__lhxw)) - 1)
#define APLIC_xMSICFGADDR_PPN_LHX_SHIFT(__lhxs) \
	((__lhxs))
#define APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_xMSICFGADDR_PPN_LHX_MASK(__lhxw) << \
	 APLIC_xMSICFGADDR_PPN_LHX_SHIFT(__lhxs))

#define APLIC_xMSICFGADDR_PPN_HHX_MASK(__hhxw) \
	((1UL << (__hhxw)) - 1)
#define APLIC_xMSICFGADDR_PPN_HHX_SHIFT(__hhxs) \
	((__hhxs) + APLIC_xMSICFGADDR_PPN_SHIFT)
#define APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_xMSICFGADDR_PPN_HHX_MASK(__hhxw) << \
	 APLIC_xMSICFGADDR_PPN_HHX_SHIFT(__hhxs))

#define APLIC_TARGET_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_TARGET_GUEST_IDX_MASK) << \
	 APLIC_TARGET_GUEST_IDX_SHIFT)
#define APLIC_TARGET_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_TARGET_EIID_MASK)

#define APLIC_SETIP_BASE		0x1c00
#define APLIC_SETIPNUM			0x1cdc

#define APLIC_CLRIP_BASE		0x1d00
#define APLIC_CLRIPNUM			0x1ddc

#define APLIC_SETIE_BASE		0x1e00
#define APLIC_SETIENUM			0x1edc

#define APLIC_CLRIE_BASE		0x1f00
#define APLIC_CLRIENUM			0x1fdc

#define APLIC_SETIPNUM_LE		0x2000
#define APLIC_SETIPNUM_BE		0x2004

#define APLIC_TARGET_BASE		0x3004
#define APLIC_TARGET_HART_IDX_SHIFT	18
#define APLIC_TARGET_HART_IDX_MASK	0x3fff
#define APLIC_TARGET_GUEST_IDX_SHIFT	12
#define APLIC_TARGET_GUEST_IDX_MASK	0x3f
#define APLIC_TARGET_IPRIO_MASK	0xff
#define APLIC_TARGET_EIID_MASK	0x7ff

#define APLIC_TARGET_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_TARGET_HART_IDX_MASK) << \
	 APLIC_TARGET_HART_IDX_SHIFT)
#define APLIC_TARGET_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_TARGET_IPRIO_MASK)

#define APLIC_IDC_BASE			0x4000
#define APLIC_IDC_আকার			32

#define APLIC_IDC_IDELIVERY		0x00

#define APLIC_IDC_IFORCE		0x04

#define APLIC_IDC_ITHRESHOLD		0x08

#define APLIC_IDC_TOPI			0x18
#define APLIC_IDC_TOPI_ID_SHIFT	16
#define APLIC_IDC_TOPI_ID_MASK	0x3ff
#define APLIC_IDC_TOPI_PRIO_MASK	0xff

#define APLIC_IDC_CLAIMI		0x1c

#define APLIC_DEFAULT_PRIORITY		1
#define APLIC_DISABLE_IDELIVERY		0
#define APLIC_ENABLE_IDELIVERY		1
#define APLIC_DISABLE_ITHRESHOLD	1
#define APLIC_ENABLE_ITHRESHOLD		0

কাঠামো aplic_msi_তথ্য {
	কাঠামো aplic_তথ্য *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

স্থির SBI_LIST_শীর্ষ(aplic_list);
স্থির শূন্য aplic_writel_msicfg(কাঠামো aplic_msicfg_তথ্য *msicfg,
				শূন্য *msicfgaddr, শূন্য *msicfgaddrH);

স্থির অন্তঃরেখ bool aplic_is_direct_mode(কাঠামো aplic_তথ্য *aplic)
{
	ফেরত aplic && aplic->অঙ্ক_idc;
}

স্থির অন্তঃরেখ শূন্য aplic_sourcecfg_write(কাঠামো aplic_তথ্য *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (শূন্য *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * আকার(u32)));
}

স্থির অন্তঃরেখ u32 aplic_sourcecfg_read(কাঠামো aplic_তথ্য *aplic, u32 hwirq)
{
	ফেরত readl((শূন্য *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * আকার(u32)));
}

স্থির অন্তঃরেখ শূন্য aplic_target_write(কাঠামো aplic_তথ্য *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (শূন্য *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * আকার(u32)));
}

স্থির অন্তঃরেখ শূন্য aplic_irq_setie(কাঠামো aplic_তথ্য *aplic, u32 hwirq)
{
	writel(hwirq, (শূন্য *)(aplic->addr + APLIC_SETIENUM));
}

স্থির অন্তঃরেখ শূন্য aplic_irq_clrie(কাঠামো aplic_তথ্য *aplic, u32 hwirq)
{
	writel(hwirq, (শূন্য *)(aplic->addr + APLIC_CLRIENUM));
}

স্থির অন্তঃরেখ শূন্য aplic_irq_clrip(কাঠামো aplic_তথ্য *aplic, u32 hwirq)
{
	writel(hwirq, (শূন্য *)(aplic->addr + APLIC_CLRIPNUM));
}

স্থির অন্তঃরেখ শূন্য aplic_msi_irq_retrigger_level(কাঠামো aplic_তথ্য *aplic, u32 hwirq)
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
	writel(hwirq, (শূন্য *)(aplic->addr + APLIC_SETIPNUM_LE));
}

স্থির অন্তঃরেখ শূন্য *aplic_idc_base(কাঠামো aplic_তথ্য *aplic, u32 idc_সূচক)
{
	ফেরত (শূন্য *)(aplic->addr + APLIC_IDC_BASE +
			idc_সূচক * APLIC_IDC_আকার);
}

স্থির অন্তঃরেখ u32 aplic_idc_read(কাঠামো aplic_তথ্য *aplic, u32 idc_সূচক,
				 u32 reg)
{
	ফেরত readl(aplic_idc_base(aplic, idc_সূচক) + reg);
}

স্থির অন্তঃরেখ শূন্য aplic_idc_write(কাঠামো aplic_তথ্য *aplic, u32 idc_সূচক,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_সূচক) + reg);
}

স্থির u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	নির্বাচন (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	ক্ষেত্র SBI_HWIRQ_FLAGS_EDGE_RISING:
		ফেরত APLIC_SOURCECFG_SM_EDGE_RISE;
	ক্ষেত্র SBI_HWIRQ_FLAGS_EDGE_FALLING:
		ফেরত APLIC_SOURCECFG_SM_EDGE_FALL;
	ক্ষেত্র SBI_HWIRQ_FLAGS_LEVEL_উপরের:
		ফেরত APLIC_SOURCECFG_SM_LEVEL_উপরের;
	ক্ষেত্র SBI_HWIRQ_FLAGS_LEVEL_নিচের:
		ফেরত APLIC_SOURCECFG_SM_LEVEL_নিচের;
	পূর্বনির্ধারিত:
		ফেরত APLIC_SOURCECFG_SM_INACTIVE;
	}
}

স্থির bool aplic_hwirq_is_delegated(কাঠামো aplic_তথ্য *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	ফেরত !!(sourcecfg & APLIC_SOURCECFG_D);
}

স্থির পূর্ণসংখ্যা aplic_find_idc_সূচক(কাঠামো aplic_তথ্য *aplic, u32 hart_সূচক)
{
	u32 i;

	জন্য (i = 0; i < aplic->অঙ্ক_idc; i++) {
		যদি (aplic->idc_map[i] == hart_সূচক)
			ফেরত i;
	}

	ফেরত SBI_ENOENT;
}

স্থির শূন্য aplic_init(কাঠামো aplic_তথ্য *aplic)
{
	কাঠামো aplic_delegate_তথ্য *deleg;
	u32 i, j, tmp, domaincfg;
	পূর্ণসংখ্যা locked;

	/* Set domain configuration to 0 */
	writel(0, (শূন্য *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	জন্য (i = 0; i <= aplic->অঙ্ক_source; i += 32)
		writel(-1U, (শূন্য *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * আকার(u32)));

	/* Set interrupt type and priority for all interrupts */
	জন্য (i = 1; i <= aplic->অঙ্ক_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (শূন্য *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * আকার(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (শূন্য *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * আকার(u32)));
	}

	/* Configure IRQ delegation */
	জন্য (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		যদি (!deleg->first_irq || !deleg->last_irq)
			চালু;
		যদি (aplic->অঙ্ক_source < deleg->first_irq ||
		    aplic->অঙ্ক_source < deleg->last_irq)
			চালু;
		যদি (deleg->child_সূচক > APLIC_SOURCECFG_CHILDIDX_MASK)
			চালু;
		যদি (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		জন্য (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_সূচক);
	}

	/* Default initialization of IDC structures */
	যদি (aplic_is_direct_mode(aplic)) {
		জন্য (i = 0; i < aplic->অঙ্ক_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} অন্যথা {
		/* MSI configuration */
		locked = readl((শূন্য *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		যদি (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(শূন্য *)(aplic->addr + APLIC_MMSICFGADDR),
						(শূন্য *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		যদি (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(শূন্য *)(aplic->addr + APLIC_SMSICFGADDR),
						(শূন্য *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	যদি (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (শূন্য *)(aplic->addr + APLIC_DOMAINCFG));
}

শূন্য aplic_reinit_all(শূন্য)
{
	কাঠামো aplic_তথ্য *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, গ্রন্থি)
		aplic_init(aplic);
}

স্থির শূন্য aplic_writel_msicfg(কাঠামো aplic_msicfg_তথ্য *msicfg,
				শূন্য *msicfgaddr, শূন্য *msicfgaddrH)
{
	u32 val;
	unsigned long base_ppn;

	/* Compute the MSI base PPN */
	base_ppn = msicfg->base_addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)base_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)base_ppn) >> 32) &
		APLIC_xMSICFGADDRH_BAPPN_MASK;
	val |= (msicfg->lhxw & APLIC_xMSICFGADDRH_LHXW_MASK)
		<< APLIC_xMSICFGADDRH_LHXW_SHIFT;
	val |= (msicfg->hhxw & APLIC_xMSICFGADDRH_HHXW_MASK)
		<< APLIC_xMSICFGADDRH_HHXW_SHIFT;
	val |= (msicfg->lhxs & APLIC_xMSICFGADDRH_LHXS_MASK)
		<< APLIC_xMSICFGADDRH_LHXS_SHIFT;
	val |= (msicfg->hhxs & APLIC_xMSICFGADDRH_HHXS_MASK)
		<< APLIC_xMSICFGADDRH_HHXS_SHIFT;
	writel(val, msicfgaddrH);
}

স্থির পূর্ণসংখ্যা aplic_check_msicfg(কাঠামো aplic_msicfg_তথ্য *msicfg)
{
	যদি (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		ফেরত SBI_EINVAL;

	যদি (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		ফেরত SBI_EINVAL;

	যদি (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		ফেরত SBI_EINVAL;

	যদি (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		ফেরত SBI_EINVAL;

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা aplic_warm_init(কাঠামো sbi_irqchip_device *chip)
{
	কাঠামো aplic_তথ্য *aplic;
	u32 hart_সূচক;
	পূর্ণসংখ্যা idc_সূচক;

	aplic = container_of(chip, কাঠামো aplic_তথ্য, irqchip);
	যদি (!aplic_is_direct_mode(aplic))
		ফেরত 0;

	hart_সূচক = current_hartindex();
	idc_সূচক  = aplic_find_idc_সূচক(aplic, hart_সূচক);
	যদি (idc_সূচক < 0)
		ফেরত 0;

	aplic_idc_write(aplic, idc_সূচক, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_সূচক, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা aplic_process_hwirqs(কাঠামো sbi_irqchip_device *chip)
{
	কাঠামো aplic_তথ্য *aplic;
	u32 hart_সূচক, claimi, hwirq;
	পূর্ণসংখ্যা idc_সূচক, rc = 0, tmp;

	aplic = container_of(chip, কাঠামো aplic_তথ্য, irqchip);

	যদি (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_সূচক = current_hartindex();
	idc_সূচক  = aplic_find_idc_সূচক(aplic, hart_সূচক);
	যদি (idc_সূচক < 0)
		ফেরত 0;

	যখন ((claimi = aplic_idc_read(aplic, idc_সূচক, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		যদি (!hwirq)
			বিরতি;

		যদি (aplic_hwirq_is_delegated(aplic, hwirq))
			চালু;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		যদি (tmp)
			rc = tmp;
	}

	ফেরত rc;
}

স্থির শূন্য aplic_hwirq_eoi(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো aplic_তথ্য *aplic;
	u32 sm;

	aplic = container_of(chip, কাঠামো aplic_তথ্য, irqchip);
	যদি (aplic_is_direct_mode(aplic))
		ফেরত;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	যদি (sm == APLIC_SOURCECFG_SM_LEVEL_উপরের ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_নিচের)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

স্থির শূন্য aplic_program_msi_target(কাঠামো aplic_তথ্য *aplic,
				     u32 hwirq, u32 hart_সূচক,
				     u32 guest_সূচক, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_সূচক) |
		 APLIC_TARGET_GUEST_IDX(guest_সূচক) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

স্থির u32 derive_hart_সূচক(কাঠামো aplic_msicfg_তথ্য *msicfg,
			      ধ্রুবক কাঠামো sbi_irqchip_msi_বার্তা *বার্তা)
{
	u64 addr = ((u64)বার্তা->address_hi << 32) | বার্তা->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_সূচক, hart_সূচক;

	group_সূচক = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_সূচক = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_সূচক |= (group_সূচক << msicfg->lhxw);

	ফেরত hart_সূচক;
}

স্থির শূন্য aplic_write_msi(u32 parent_hwirq,
			    ধ্রুবক কাঠামো sbi_irqchip_msi_বার্তা *বার্তা,
			    শূন্য *priv)
{
	কাঠামো aplic_msi_তথ্য *msi_তথ্য = priv;
	u32 guest_সূচক = 0;
	u32 eiid;

	eiid = বার্তা->তথ্য;
	যদি (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		ফেরত;

	aplic_program_msi_target(msi_তথ্য->aplic, msi_তথ্য->hwirq,
				derive_hart_সূচক(&msi_তথ্য->aplic->msicfg_mmode, বার্তা),
				guest_সূচক, eiid);
}

স্থির পূর্ণসংখ্যা aplic_msi_callback(u32 parent_hwirq, শূন্য *priv)
{
	কাঠামো aplic_msi_তথ্য *msi_তথ্য = priv;
	পূর্ণসংখ্যা rc;

	rc = sbi_irqchip_process_hwirq(&msi_তথ্য->aplic->irqchip, msi_তথ্য->hwirq);
	যদি (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	ফেরত rc;
}

স্থির পূর্ণসংখ্যা aplic_setup_msi(কাঠামো aplic_তথ্য *aplic, u32 hwirq)
{
	কাঠামো sbi_irqchip_device *parent;
	u32 first_hwirq;
	কাঠামো aplic_msi_তথ্য *msi_তথ্য;
	পূর্ণসংখ্যা rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	যদি (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		ফেরত SBI_EINVAL;
	}

	msi_তথ্য = sbi_zalloc(আকার(কাঠামো aplic_msi_তথ্য));
	যদি (!msi_তথ্য)
		ফেরত SBI_ENOMEM;

	msi_তথ্য->aplic = aplic;
	msi_তথ্য->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_তথ্য,
					&first_hwirq);
	যদি (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_তথ্য);
		ফেরত rc;
	}

	msi_তথ্য->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_তথ্য);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা aplic_hwirq_setup(কাঠামো sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	কাঠামো aplic_তথ্য *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, কাঠামো aplic_তথ্য, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	যদি (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		ফেরত SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	যদি (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		ফেরত SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	যদি (!aplic_is_direct_mode(aplic))
		ফেরত aplic_setup_msi(aplic, hwirq);

	ফেরত 0;
}

স্থির শূন্য aplic_hwirq_cleanup(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো aplic_তথ্য *aplic;
	কাঠামো aplic_msi_তথ্য *msi_তথ্য;

	aplic = container_of(chip, কাঠামো aplic_তথ্য, irqchip);
	যদি (aplic_hwirq_is_delegated(aplic, hwirq))
		ফেরত;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_তথ্য = (কাঠামো aplic_msi_তথ্য *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	যদি (msi_তথ্য)
		sbi_free(msi_তথ্য);
}

স্থির পূর্ণসংখ্যা aplic_hwirq_set_affinity(কাঠামো sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_সূচক)
{
	কাঠামো aplic_তথ্য *aplic;
	কাঠামো aplic_msi_তথ্য *msi_তথ্য;
	কাঠামো sbi_irqchip_device *parent;
	পূর্ণসংখ্যা idc_সূচক;
	পূর্ণসংখ্যা rc;

	aplic = container_of(chip, কাঠামো aplic_তথ্য, irqchip);
	যদি (aplic_hwirq_is_delegated(aplic, hwirq))
		ফেরত SBI_ENOTSUPP;

	যদি (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		যদি (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			ফেরত SBI_EINVAL;
		}

		msi_তথ্য = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_তথ্য->parent_hwirq, hart_সূচক);
		যদি (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			ফেরত rc;
		}
		ফেরত 0;
	}

	idc_সূচক = aplic_find_idc_সূচক(aplic, hart_সূচক);
	যদি (idc_সূচক < 0)
		ফেরত SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_সূচক) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	ফেরত 0;
}

স্থির শূন্য aplic_hwirq_mask(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো aplic_তথ্য *aplic;

	aplic = container_of(chip, কাঠামো aplic_তথ্য, irqchip);
	যদি (aplic_hwirq_is_delegated(aplic, hwirq))
		ফেরত;

	aplic_irq_clrie(aplic, hwirq);
}

স্থির শূন্য aplic_hwirq_unmask(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো aplic_তথ্য *aplic;

	aplic = container_of(chip, কাঠামো aplic_তথ্য, irqchip);
	যদি (aplic_hwirq_is_delegated(aplic, hwirq))
		ফেরত;

	aplic_irq_setie(aplic, hwirq);
}

স্থির কাঠামো sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

পূর্ণসংখ্যা aplic_cold_irqchip_init(কাঠামো aplic_তথ্য *aplic)
{
	পূর্ণসংখ্যা rc;
	কাঠামো aplic_delegate_তথ্য *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	যদি (!aplic->অঙ্ক_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		ফেরত SBI_EINVAL;
	}

	যদি (APLIC_MAX_SOURCE <= aplic->অঙ্ক_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->অঙ্ক_source,
			(unsigned long)APLIC_MAX_SOURCE);
		ফেরত SBI_EINVAL;
	}

	যদি (!msi_mode && APLIC_MAX_IDC <= aplic->অঙ্ক_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->অঙ্ক_idc,
			(unsigned long)APLIC_MAX_IDC);
		ফেরত SBI_EINVAL;
	}

	যদি (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		যদি (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			ফেরত rc;
		}
	}

	যদি (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		যদি (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			ফেরত rc;
		}
	}

	/* Init the APLIC registers */
	aplic_init(aplic);

	/*
	 * Add APLIC region to the root domain if:
	 * 1) It targets M-mode of any HART directly or via MSIs
	 * 2) All interrupts are delegated to some child APLIC
	 */
	first_deleg_irq = -1U;
	last_deleg_irq = 0;
	জন্য (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		যদি (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		যদি (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	যদি (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->অঙ্ক_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_মূল_add_memrange(aplic->addr, aplic->আকার_2, PAGE_আকার,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		যদি (rc)
			ফেরত rc;
	}

	যদি ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.অঙ্ক_hwirq = aplic->অঙ্ক_source + 1;

		যদি (msi_mode) {
			aplic->irqchip.warm_init      = ফাঁকা;
			aplic->irqchip.process_hwirqs = ফাঁকা;
			aplic->irqchip.hwirq_eoi      = ফাঁকা;
		}

		যদি (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		অন্যথা
			জন্য (i = 0; i < aplic->অঙ্ক_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		যদি (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			ফেরত rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_লেজ(&aplic->গ্রন্থি, &aplic_list);

	ফেরত 0;
}
