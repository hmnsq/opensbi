/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/aplic.h>

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
#define APLIC_SOURCECFG_SM_LEVEL_ಮೇಲಿನ	0x6
#define APLIC_SOURCECFG_SM_LEVEL_ಕೆಳಗಿನ	0x7

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

ಸಂರಚನಾ aplic_msi_data {
	ಸಂರಚನಾ aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

ಸ್ಥಿರ SBI_LIST_HEAD(aplic_list);
ಸ್ಥಿರ ಶೂನ್ಯ aplic_writel_msicfg(ಸಂರಚನಾ aplic_msicfg_data *msicfg,
				ಶೂನ್ಯ *msicfgaddr, ಶೂನ್ಯ *msicfgaddrH);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool aplic_is_direct_mode(ಸಂರಚನಾ aplic_data *aplic)
{
	ಪ್ರತ್ಯಾಗಮ aplic && aplic->ಅಂಕೆ_idc;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ aplic_sourcecfg_write(ಸಂರಚನಾ aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (ಶೂನ್ಯ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * ಪರಿಮಾಣ(u32)));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32 aplic_sourcecfg_read(ಸಂರಚನಾ aplic_data *aplic, u32 hwirq)
{
	ಪ್ರತ್ಯಾಗಮ readl((ಶೂನ್ಯ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * ಪರಿಮಾಣ(u32)));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ aplic_target_write(ಸಂರಚನಾ aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (ಶೂನ್ಯ *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * ಪರಿಮಾಣ(u32)));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ aplic_irq_setie(ಸಂರಚನಾ aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ಶೂನ್ಯ *)(aplic->addr + APLIC_SETIENUM));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ aplic_irq_clrie(ಸಂರಚನಾ aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ಶೂನ್ಯ *)(aplic->addr + APLIC_CLRIENUM));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ aplic_irq_clrip(ಸಂರಚನಾ aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ಶೂನ್ಯ *)(aplic->addr + APLIC_CLRIPNUM));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ aplic_msi_irq_retrigger_level(ಸಂರಚನಾ aplic_data *aplic, u32 hwirq)
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
	writel(hwirq, (ಶೂನ್ಯ *)(aplic->addr + APLIC_SETIPNUM_LE));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *aplic_idc_base(ಸಂರಚನಾ aplic_data *aplic, u32 idc_index)
{
	ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32 aplic_idc_read(ಸಂರಚನಾ aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	ಪ್ರತ್ಯಾಗಮ readl(aplic_idc_base(aplic, idc_index) + reg);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ aplic_idc_write(ಸಂರಚನಾ aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

ಸ್ಥಿರ u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	ವಿಕಲ್ಪ (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	ಪಕ್ಷ SBI_HWIRQ_FLAGS_EDGE_RISING:
		ಪ್ರತ್ಯಾಗಮ APLIC_SOURCECFG_SM_EDGE_RISE;
	ಪಕ್ಷ SBI_HWIRQ_FLAGS_EDGE_FALLING:
		ಪ್ರತ್ಯಾಗಮ APLIC_SOURCECFG_SM_EDGE_FALL;
	ಪಕ್ಷ SBI_HWIRQ_FLAGS_LEVEL_ಮೇಲಿನ:
		ಪ್ರತ್ಯಾಗಮ APLIC_SOURCECFG_SM_LEVEL_ಮೇಲಿನ;
	ಪಕ್ಷ SBI_HWIRQ_FLAGS_LEVEL_ಕೆಳಗಿನ:
		ಪ್ರತ್ಯಾಗಮ APLIC_SOURCECFG_SM_LEVEL_ಕೆಳಗಿನ;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ಸ್ಥಿರ bool aplic_hwirq_is_delegated(ಸಂರಚನಾ aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	ಪ್ರತ್ಯಾಗಮ !!(sourcecfg & APLIC_SOURCECFG_D);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aplic_find_idc_index(ಸಂರಚನಾ aplic_data *aplic, u32 hart_index)
{
	u32 i;

	ಪ್ರತಿ (i = 0; i < aplic->ಅಂಕೆ_idc; i++) {
		ಯದಿ (aplic->idc_map[i] == hart_index)
			ಪ್ರತ್ಯಾಗಮ i;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_init(ಸಂರಚನಾ aplic_data *aplic)
{
	ಸಂರಚನಾ aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	ಪೂರ್ಣಾಙ್ಕ locked;

	/* Set domain configuration to 0 */
	writel(0, (ಶೂನ್ಯ *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	ಪ್ರತಿ (i = 0; i <= aplic->ಅಂಕೆ_source; i += 32)
		writel(-1U, (ಶೂನ್ಯ *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * ಪರಿಮಾಣ(u32)));

	/* Set interrupt type and priority for all interrupts */
	ಪ್ರತಿ (i = 1; i <= aplic->ಅಂಕೆ_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (ಶೂನ್ಯ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * ಪರಿಮಾಣ(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (ಶೂನ್ಯ *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * ಪರಿಮಾಣ(u32)));
	}

	/* Configure IRQ delegation */
	ಪ್ರತಿ (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ಯದಿ (!deleg->first_irq || !deleg->last_irq)
			ಅನುವರ್ತ;
		ಯದಿ (aplic->ಅಂಕೆ_source < deleg->first_irq ||
		    aplic->ಅಂಕೆ_source < deleg->last_irq)
			ಅನುವರ್ತ;
		ಯದಿ (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			ಅನುವರ್ತ;
		ಯದಿ (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		ಪ್ರತಿ (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	ಯದಿ (aplic_is_direct_mode(aplic)) {
		ಪ್ರತಿ (i = 0; i < aplic->ಅಂಕೆ_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} ಅನ್ಯಥಾ {
		/* MSI configuration */
		locked = readl((ಶೂನ್ಯ *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		ಯದಿ (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(ಶೂನ್ಯ *)(aplic->addr + APLIC_MMSICFGADDR),
						(ಶೂನ್ಯ *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		ಯದಿ (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(ಶೂನ್ಯ *)(aplic->addr + APLIC_SMSICFGADDR),
						(ಶೂನ್ಯ *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	ಯದಿ (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (ಶೂನ್ಯ *)(aplic->addr + APLIC_DOMAINCFG));
}

ಶೂನ್ಯ aplic_reinit_all(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, ಗಂಟು)
		aplic_init(aplic);
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_writel_msicfg(ಸಂರಚನಾ aplic_msicfg_data *msicfg,
				ಶೂನ್ಯ *msicfgaddr, ಶೂನ್ಯ *msicfgaddrH)
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aplic_check_msicfg(ಸಂರಚನಾ aplic_msicfg_data *msicfg)
{
	ಯದಿ (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aplic_warm_init(ಸಂರಚನಾ sbi_irqchip_device *chip)
{
	ಸಂರಚನಾ aplic_data *aplic;
	u32 hart_index;
	ಪೂರ್ಣಾಙ್ಕ idc_index;

	aplic = container_of(chip, ಸಂರಚನಾ aplic_data, irqchip);
	ಯದಿ (!aplic_is_direct_mode(aplic))
		ಪ್ರತ್ಯಾಗಮ 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	ಯದಿ (idc_index < 0)
		ಪ್ರತ್ಯಾಗಮ 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aplic_process_hwirqs(ಸಂರಚನಾ sbi_irqchip_device *chip)
{
	ಸಂರಚನಾ aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	ಪೂರ್ಣಾಙ್ಕ idc_index, rc = 0, tmp;

	aplic = container_of(chip, ಸಂರಚನಾ aplic_data, irqchip);

	ಯದಿ (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	ಯದಿ (idc_index < 0)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯಾವತ್ ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		ಯದಿ (!hwirq)
			ಭಙ್ಗ;

		ಯದಿ (aplic_hwirq_is_delegated(aplic, hwirq))
			ಅನುವರ್ತ;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		ಯದಿ (tmp)
			rc = tmp;
	}

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_hwirq_eoi(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, ಸಂರಚನಾ aplic_data, irqchip);
	ಯದಿ (aplic_is_direct_mode(aplic))
		ಪ್ರತ್ಯಾಗಮ;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	ಯದಿ (sm == APLIC_SOURCECFG_SM_LEVEL_ಮೇಲಿನ ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_ಕೆಳಗಿನ)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_program_msi_target(ಸಂರಚನಾ aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

ಸ್ಥಿರ u32 derive_hart_index(ಸಂರಚನಾ aplic_msicfg_data *msicfg,
			      ಧ್ರುವ ಸಂರಚನಾ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ)
{
	u64 addr = ((u64)ಸಂದೇಶ->address_hi << 32) | ಸಂದೇಶ->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	ಪ್ರತ್ಯಾಗಮ hart_index;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_write_msi(u32 parent_hwirq,
			    ಧ್ರುವ ಸಂರಚನಾ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ,
			    ಶೂನ್ಯ *priv)
{
	ಸಂರಚನಾ aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = ಸಂದೇಶ->data;
	ಯದಿ (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		ಪ್ರತ್ಯಾಗಮ;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, ಸಂದೇಶ),
				guest_index, eiid);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aplic_msi_callback(u32 parent_hwirq, ಶೂನ್ಯ *priv)
{
	ಸಂರಚನಾ aplic_msi_data *msi_data = priv;
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	ಯದಿ (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aplic_setup_msi(ಸಂರಚನಾ aplic_data *aplic, u32 hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_device *parent;
	u32 first_hwirq;
	ಸಂರಚನಾ aplic_msi_data *msi_data;
	ಪೂರ್ಣಾಙ್ಕ rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	ಯದಿ (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	msi_data = sbi_zalloc(ಪರಿಮಾಣ(ಸಂರಚನಾ aplic_msi_data));
	ಯದಿ (!msi_data)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	ಯದಿ (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aplic_hwirq_setup(ಸಂರಚನಾ sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	ಸಂರಚನಾ aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, ಸಂರಚನಾ aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	ಯದಿ (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	ಯದಿ (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	ಯದಿ (!aplic_is_direct_mode(aplic))
		ಪ್ರತ್ಯಾಗಮ aplic_setup_msi(aplic, hwirq);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_hwirq_cleanup(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ aplic_data *aplic;
	ಸಂರಚನಾ aplic_msi_data *msi_data;

	aplic = container_of(chip, ಸಂರಚನಾ aplic_data, irqchip);
	ಯದಿ (aplic_hwirq_is_delegated(aplic, hwirq))
		ಪ್ರತ್ಯಾಗಮ;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (ಸಂರಚನಾ aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	ಯದಿ (msi_data)
		sbi_free(msi_data);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aplic_hwirq_set_affinity(ಸಂರಚನಾ sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	ಸಂರಚನಾ aplic_data *aplic;
	ಸಂರಚನಾ aplic_msi_data *msi_data;
	ಸಂರಚನಾ sbi_irqchip_device *parent;
	ಪೂರ್ಣಾಙ್ಕ idc_index;
	ಪೂರ್ಣಾಙ್ಕ rc;

	aplic = container_of(chip, ಸಂರಚನಾ aplic_data, irqchip);
	ಯದಿ (aplic_hwirq_is_delegated(aplic, hwirq))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	ಯದಿ (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		ಯದಿ (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		ಯದಿ (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	ಯದಿ (idc_index < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_hwirq_mask(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ aplic_data *aplic;

	aplic = container_of(chip, ಸಂರಚನಾ aplic_data, irqchip);
	ಯದಿ (aplic_hwirq_is_delegated(aplic, hwirq))
		ಪ್ರತ್ಯಾಗಮ;

	aplic_irq_clrie(aplic, hwirq);
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_hwirq_unmask(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ aplic_data *aplic;

	aplic = container_of(chip, ಸಂರಚನಾ aplic_data, irqchip);
	ಯದಿ (aplic_hwirq_is_delegated(aplic, hwirq))
		ಪ್ರತ್ಯಾಗಮ;

	aplic_irq_setie(aplic, hwirq);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

ಪೂರ್ಣಾಙ್ಕ aplic_cold_irqchip_init(ಸಂರಚನಾ aplic_data *aplic)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	ಯದಿ (!aplic->ಅಂಕೆ_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	ಯದಿ (APLIC_MAX_SOURCE <= aplic->ಅಂಕೆ_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->ಅಂಕೆ_source,
			(unsigned long)APLIC_MAX_SOURCE);
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	ಯದಿ (!msi_mode && APLIC_MAX_IDC <= aplic->ಅಂಕೆ_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->ಅಂಕೆ_idc,
			(unsigned long)APLIC_MAX_IDC);
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	}

	ಯದಿ (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		ಯದಿ (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}

	ಯದಿ (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		ಯದಿ (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			ಪ್ರತ್ಯಾಗಮ rc;
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
	ಪ್ರತಿ (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ಯದಿ (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		ಯದಿ (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	ಯದಿ (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->ಅಂಕೆ_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_ಬೇರು_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.ಅಂಕೆ_hwirq = aplic->ಅಂಕೆ_source + 1;

		ಯದಿ (msi_mode) {
			aplic->irqchip.warm_init      = ರಿಕ್ತ;
			aplic->irqchip.process_hwirqs = ರಿಕ್ತ;
			aplic->irqchip.hwirq_eoi      = ರಿಕ್ತ;
		}

		ಯದಿ (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		ಅನ್ಯಥಾ
			ಪ್ರತಿ (i = 0; i < aplic->ಅಂಕೆ_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		ಯದಿ (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			ಪ್ರತ್ಯಾಗಮ rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->ಗಂಟು, &aplic_list);

	ಪ್ರತ್ಯಾಗಮ 0;
}
