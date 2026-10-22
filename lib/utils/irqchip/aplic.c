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
#define APLIC_IDC_SIZE			32

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

সংরচনা aplic_msi_data {
	সংরচনা aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

স্থির SBI_LIST_HEAD(aplic_list);
স্থির শূন্য aplic_writel_msicfg(সংরচনা aplic_msicfg_data *msicfg,
				শূন্য *msicfgaddr, শূন্য *msicfgaddrH);

স্থির অন্তঃস্থ bool aplic_is_direct_mode(সংরচনা aplic_data *aplic)
{
	প্রত্যাগম aplic && aplic->অঙ্ক_idc;
}

স্থির অন্তঃস্থ শূন্য aplic_sourcecfg_write(সংরচনা aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (শূন্য *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * পরিমাণ(u32)));
}

স্থির অন্তঃস্থ u32 aplic_sourcecfg_read(সংরচনা aplic_data *aplic, u32 hwirq)
{
	প্রত্যাগম readl((শূন্য *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * পরিমাণ(u32)));
}

স্থির অন্তঃস্থ শূন্য aplic_target_write(সংরচনা aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (শূন্য *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * পরিমাণ(u32)));
}

স্থির অন্তঃস্থ শূন্য aplic_irq_setie(সংরচনা aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (শূন্য *)(aplic->addr + APLIC_SETIENUM));
}

স্থির অন্তঃস্থ শূন্য aplic_irq_clrie(সংরচনা aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (শূন্য *)(aplic->addr + APLIC_CLRIENUM));
}

স্থির অন্তঃস্থ শূন্য aplic_irq_clrip(সংরচনা aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (শূন্য *)(aplic->addr + APLIC_CLRIPNUM));
}

স্থির অন্তঃস্থ শূন্য aplic_msi_irq_retrigger_level(সংরচনা aplic_data *aplic, u32 hwirq)
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

স্থির অন্তঃস্থ শূন্য *aplic_idc_base(সংরচনা aplic_data *aplic, u32 idc_index)
{
	প্রত্যাগম (শূন্য *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

স্থির অন্তঃস্থ u32 aplic_idc_read(সংরচনা aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	প্রত্যাগম readl(aplic_idc_base(aplic, idc_index) + reg);
}

স্থির অন্তঃস্থ শূন্য aplic_idc_write(সংরচনা aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

স্থির u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	বিকল্প (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	পক্ষ SBI_HWIRQ_FLAGS_EDGE_RISING:
		প্রত্যাগম APLIC_SOURCECFG_SM_EDGE_RISE;
	পক্ষ SBI_HWIRQ_FLAGS_EDGE_FALLING:
		প্রত্যাগম APLIC_SOURCECFG_SM_EDGE_FALL;
	পক্ষ SBI_HWIRQ_FLAGS_LEVEL_উপরের:
		প্রত্যাগম APLIC_SOURCECFG_SM_LEVEL_উপরের;
	পক্ষ SBI_HWIRQ_FLAGS_LEVEL_নিচের:
		প্রত্যাগম APLIC_SOURCECFG_SM_LEVEL_নিচের;
	স্বভাব:
		প্রত্যাগম APLIC_SOURCECFG_SM_INACTIVE;
	}
}

স্থির bool aplic_hwirq_is_delegated(সংরচনা aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	প্রত্যাগম !!(sourcecfg & APLIC_SOURCECFG_D);
}

স্থির পূর্ণাঙ্ক aplic_find_idc_index(সংরচনা aplic_data *aplic, u32 hart_index)
{
	u32 i;

	প্রতি (i = 0; i < aplic->অঙ্ক_idc; i++) {
		যদি (aplic->idc_map[i] == hart_index)
			প্রত্যাগম i;
	}

	প্রত্যাগম SBI_ENOENT;
}

স্থির শূন্য aplic_init(সংরচনা aplic_data *aplic)
{
	সংরচনা aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	পূর্ণাঙ্ক locked;

	/* Set domain configuration to 0 */
	writel(0, (শূন্য *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	প্রতি (i = 0; i <= aplic->অঙ্ক_source; i += 32)
		writel(-1U, (শূন্য *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * পরিমাণ(u32)));

	/* Set interrupt type and priority for all interrupts */
	প্রতি (i = 1; i <= aplic->অঙ্ক_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (শূন্য *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * পরিমাণ(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (শূন্য *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * পরিমাণ(u32)));
	}

	/* Configure IRQ delegation */
	প্রতি (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		যদি (!deleg->first_irq || !deleg->last_irq)
			অনুবর্ত;
		যদি (aplic->অঙ্ক_source < deleg->first_irq ||
		    aplic->অঙ্ক_source < deleg->last_irq)
			অনুবর্ত;
		যদি (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			অনুবর্ত;
		যদি (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		প্রতি (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	যদি (aplic_is_direct_mode(aplic)) {
		প্রতি (i = 0; i < aplic->অঙ্ক_idc; i++) {
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
	সংরচনা aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, গ্রন্থি)
		aplic_init(aplic);
}

স্থির শূন্য aplic_writel_msicfg(সংরচনা aplic_msicfg_data *msicfg,
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

স্থির পূর্ণাঙ্ক aplic_check_msicfg(সংরচনা aplic_msicfg_data *msicfg)
{
	যদি (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		প্রত্যাগম SBI_EINVAL;

	যদি (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		প্রত্যাগম SBI_EINVAL;

	যদি (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		প্রত্যাগম SBI_EINVAL;

	যদি (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		প্রত্যাগম SBI_EINVAL;

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক aplic_warm_init(সংরচনা sbi_irqchip_device *chip)
{
	সংরচনা aplic_data *aplic;
	u32 hart_index;
	পূর্ণাঙ্ক idc_index;

	aplic = container_of(chip, সংরচনা aplic_data, irqchip);
	যদি (!aplic_is_direct_mode(aplic))
		প্রত্যাগম 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	যদি (idc_index < 0)
		প্রত্যাগম 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক aplic_process_hwirqs(সংরচনা sbi_irqchip_device *chip)
{
	সংরচনা aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	পূর্ণাঙ্ক idc_index, rc = 0, tmp;

	aplic = container_of(chip, সংরচনা aplic_data, irqchip);

	যদি (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	যদি (idc_index < 0)
		প্রত্যাগম 0;

	যাবত্ ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		যদি (!hwirq)
			ভঙ্গ;

		যদি (aplic_hwirq_is_delegated(aplic, hwirq))
			অনুবর্ত;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		যদি (tmp)
			rc = tmp;
	}

	প্রত্যাগম rc;
}

স্থির শূন্য aplic_hwirq_eoi(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, সংরচনা aplic_data, irqchip);
	যদি (aplic_is_direct_mode(aplic))
		প্রত্যাগম;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	যদি (sm == APLIC_SOURCECFG_SM_LEVEL_উপরের ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_নিচের)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

স্থির শূন্য aplic_program_msi_target(সংরচনা aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

স্থির u32 derive_hart_index(সংরচনা aplic_msicfg_data *msicfg,
			      ধ্রুব সংরচনা sbi_irqchip_msi_বার্তা *বার্তা)
{
	u64 addr = ((u64)বার্তা->address_hi << 32) | বার্তা->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	প্রত্যাগম hart_index;
}

স্থির শূন্য aplic_write_msi(u32 parent_hwirq,
			    ধ্রুব সংরচনা sbi_irqchip_msi_বার্তা *বার্তা,
			    শূন্য *priv)
{
	সংরচনা aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = বার্তা->data;
	যদি (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		প্রত্যাগম;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, বার্তা),
				guest_index, eiid);
}

স্থির পূর্ণাঙ্ক aplic_msi_callback(u32 parent_hwirq, শূন্য *priv)
{
	সংরচনা aplic_msi_data *msi_data = priv;
	পূর্ণাঙ্ক rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	যদি (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	প্রত্যাগম rc;
}

স্থির পূর্ণাঙ্ক aplic_setup_msi(সংরচনা aplic_data *aplic, u32 hwirq)
{
	সংরচনা sbi_irqchip_device *parent;
	u32 first_hwirq;
	সংরচনা aplic_msi_data *msi_data;
	পূর্ণাঙ্ক rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	যদি (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		প্রত্যাগম SBI_EINVAL;
	}

	msi_data = sbi_zalloc(পরিমাণ(সংরচনা aplic_msi_data));
	যদি (!msi_data)
		প্রত্যাগম SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	যদি (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		প্রত্যাগম rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক aplic_hwirq_setup(সংরচনা sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	সংরচনা aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, সংরচনা aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	যদি (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		প্রত্যাগম SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	যদি (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		প্রত্যাগম SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	যদি (!aplic_is_direct_mode(aplic))
		প্রত্যাগম aplic_setup_msi(aplic, hwirq);

	প্রত্যাগম 0;
}

স্থির শূন্য aplic_hwirq_cleanup(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা aplic_data *aplic;
	সংরচনা aplic_msi_data *msi_data;

	aplic = container_of(chip, সংরচনা aplic_data, irqchip);
	যদি (aplic_hwirq_is_delegated(aplic, hwirq))
		প্রত্যাগম;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (সংরচনা aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	যদি (msi_data)
		sbi_free(msi_data);
}

স্থির পূর্ণাঙ্ক aplic_hwirq_set_affinity(সংরচনা sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	সংরচনা aplic_data *aplic;
	সংরচনা aplic_msi_data *msi_data;
	সংরচনা sbi_irqchip_device *parent;
	পূর্ণাঙ্ক idc_index;
	পূর্ণাঙ্ক rc;

	aplic = container_of(chip, সংরচনা aplic_data, irqchip);
	যদি (aplic_hwirq_is_delegated(aplic, hwirq))
		প্রত্যাগম SBI_ENOTSUPP;

	যদি (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		যদি (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			প্রত্যাগম SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		যদি (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			প্রত্যাগম rc;
		}
		প্রত্যাগম 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	যদি (idc_index < 0)
		প্রত্যাগম SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	প্রত্যাগম 0;
}

স্থির শূন্য aplic_hwirq_mask(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা aplic_data *aplic;

	aplic = container_of(chip, সংরচনা aplic_data, irqchip);
	যদি (aplic_hwirq_is_delegated(aplic, hwirq))
		প্রত্যাগম;

	aplic_irq_clrie(aplic, hwirq);
}

স্থির শূন্য aplic_hwirq_unmask(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা aplic_data *aplic;

	aplic = container_of(chip, সংরচনা aplic_data, irqchip);
	যদি (aplic_hwirq_is_delegated(aplic, hwirq))
		প্রত্যাগম;

	aplic_irq_setie(aplic, hwirq);
}

স্থির সংরচনা sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

পূর্ণাঙ্ক aplic_cold_irqchip_init(সংরচনা aplic_data *aplic)
{
	পূর্ণাঙ্ক rc;
	সংরচনা aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	যদি (!aplic->অঙ্ক_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		প্রত্যাগম SBI_EINVAL;
	}

	যদি (APLIC_MAX_SOURCE <= aplic->অঙ্ক_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->অঙ্ক_source,
			(unsigned long)APLIC_MAX_SOURCE);
		প্রত্যাগম SBI_EINVAL;
	}

	যদি (!msi_mode && APLIC_MAX_IDC <= aplic->অঙ্ক_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->অঙ্ক_idc,
			(unsigned long)APLIC_MAX_IDC);
		প্রত্যাগম SBI_EINVAL;
	}

	যদি (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		যদি (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			প্রত্যাগম rc;
		}
	}

	যদি (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		যদি (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			প্রত্যাগম rc;
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
	প্রতি (i = 0; i < APLIC_MAX_DELEGATE; i++) {
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
		rc = sbi_domain_মূল_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		যদি (rc)
			প্রত্যাগম rc;
	}

	যদি ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.অঙ্ক_hwirq = aplic->অঙ্ক_source + 1;

		যদি (msi_mode) {
			aplic->irqchip.warm_init      = রিক্ত;
			aplic->irqchip.process_hwirqs = রিক্ত;
			aplic->irqchip.hwirq_eoi      = রিক্ত;
		}

		যদি (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		অন্যথা
			প্রতি (i = 0; i < aplic->অঙ্ক_idc; i++)
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
			প্রত্যাগম rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->গ্রন্থি, &aplic_list);

	প্রত্যাগম 0;
}
