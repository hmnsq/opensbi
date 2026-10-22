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

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_MAX_IDC			(1UL << 14)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_MAX_ಮೂಲ		1024

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_DOMAINCFG		0x0000
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_DOMAINCFG_IE		(1 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_DOMAINCFG_DM		(1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_DOMAINCFG_BE		(1 << 0)

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_ತಳ		0x0004
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_D		(1 << 10)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_CHILDIDX_ಮುಖವಾಡ	0x000003ff
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_SM_ಮುಖವಾಡ	0x00000007
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_SM_INACTIVE	0x0
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_SM_DETACH	0x1
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_SM_ಅಂಚು_RISE	0x4
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_SM_ಅಂಚು_FALL	0x5
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_SM_LEVEL_ಮೇಲಿನ	0x6
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SOURCECFG_SM_LEVEL_ಕೆಳಗಿನ	0x7

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_MMSICFGADDR		0x1bc0
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_MMSICFGADDRH		0x1bc4
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SMSICFGADDR		0x1bc8
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SMSICFGADDRH		0x1bcc

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_xMSICFGADDRH_L		(1UL << 31)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_HHXS_ಮುಖವಾಡ	0x1f
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_HHXS_ಸರಿಸು	24
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_LHXS_ಮುಖವಾಡ	0x7
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_LHXS_ಸರಿಸು	20
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_HHXW_ಮುಖವಾಡ	0x7
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_HHXW_ಸರಿಸು	16
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_LHXW_ಮುಖವಾಡ	0xf
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_LHXW_ಸರಿಸು	12
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDRH_BAPPN_ಮುಖವಾಡ	0xfff

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDR_PPN_ಸರಿಸು	12

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDR_PPN_LHX_ಮುಖವಾಡ(__lhxw) \
	((1UL << (__lhxw)) - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDR_PPN_LHX_ಸರಿಸು(__lhxs) \
	((__lhxs))
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_ಮುಖವಾಡ(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_ಸರಿಸು(__lhxs))

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDR_PPN_HHX_ಮುಖವಾಡ(__hhxw) \
	((1UL << (__hhxw)) - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_x_MSICFGADDR_PPN_HHX_ಸರಿಸು(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_ಸರಿಸು)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_ಮುಖವಾಡ(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_ಸರಿಸು(__hhxs))

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_ಗುರಿ_GUEST_IDX_ಮುಖವಾಡ) << \
	 APLIC_ಗುರಿ_GUEST_IDX_ಸರಿಸು)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_ಗುರಿ_EIID_ಮುಖವಾಡ)

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SETIP_ತಳ		0x1c00
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SETIPNUM			0x1cdc

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_CLRIP_ತಳ		0x1d00
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_CLRIPNUM			0x1ddc

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SETIE_ತಳ		0x1e00
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SETIENUM			0x1edc

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_CLRIE_ತಳ		0x1f00
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_CLRIENUM			0x1fdc

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SETIPNUM_LE		0x2000
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_SETIPNUM_BE		0x2004

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_ತಳ		0x3004
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_HART_IDX_ಸರಿಸು	18
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_HART_IDX_ಮುಖವಾಡ	0x3fff
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_GUEST_IDX_ಸರಿಸು	12
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_GUEST_IDX_ಮುಖವಾಡ	0x3f
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_IPRIO_ಮುಖವಾಡ	0xff
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_EIID_ಮುಖವಾಡ	0x7ff

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_ಗುರಿ_HART_IDX_ಮುಖವಾಡ) << \
	 APLIC_ಗುರಿ_HART_IDX_ಸರಿಸು)
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ಗುರಿ_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_ಗುರಿ_IPRIO_ಮುಖವಾಡ)

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_ತಳ			0x4000
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_ಗಾತ್ರ			32

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_IDELIVERY		0x00

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_IFORCE		0x04

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_ITHRESHOLD		0x08

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_TOPI			0x18
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_TOPI_ID_ಸರಿಸು	16
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_TOPI_ID_ಮುಖವಾಡ	0x3ff
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_TOPI_PRIO_ಮುಖವಾಡ	0xff

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_IDC_CLAIMI		0x1c

#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_DEFAULT_ಆದ್ಯತೆ		1
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_DISABLE_IDELIVERY		0
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ENABLE_IDELIVERY		1
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_DISABLE_ITHRESHOLD	1
#ವ್ಯಾಖ್ಯಾನಿಸು APLIC_ENABLE_ITHRESHOLD		0

ರಚನೆ aplic_msi_ದತ್ತಾಂಶ {
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;
	u32 hwirq;
	u32 ಪೋಷಕ_hwirq;
};

ಸ್ಥಿರ SBI_LIST_ತಲೆ(aplic_list);
ಸ್ಥಿರ ಶೂನ್ಯ aplic_writel_msicfg(ರಚನೆ aplic_msicfg_ದತ್ತಾಂಶ *msicfg,
				ಶೂನ್ಯ *msicfgaddr, ಶೂನ್ಯ *msicfgaddrH);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool aplic_is_ನೇರ_ಬಹುಲಕ(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic)
{
	ಹಿಂತಿರುಗಿ aplic && aplic->ಅಂಕೆ_idc;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ aplic_sourcecfg_write(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (ಶೂನ್ಯ *)(aplic->addr + APLIC_SOURCECFG_ತಳ +
			     (hwirq - 1) * ಗಾತ್ರ(u32)));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32 aplic_sourcecfg_read(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 hwirq)
{
	ಹಿಂತಿರುಗಿ readl((ಶೂನ್ಯ *)(aplic->addr + APLIC_SOURCECFG_ತಳ +
			      (hwirq - 1) * ಗಾತ್ರ(u32)));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ aplic_ಗುರಿ_write(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (ಶೂನ್ಯ *)(aplic->addr + APLIC_ಗುರಿ_ತಳ +
			     (hwirq - 1) * ಗಾತ್ರ(u32)));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ aplic_irq_setie(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 hwirq)
{
	writel(hwirq, (ಶೂನ್ಯ *)(aplic->addr + APLIC_SETIENUM));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ aplic_irq_clrie(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 hwirq)
{
	writel(hwirq, (ಶೂನ್ಯ *)(aplic->addr + APLIC_CLRIENUM));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ aplic_irq_clrip(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 hwirq)
{
	writel(hwirq, (ಶೂನ್ಯ *)(aplic->addr + APLIC_CLRIPNUM));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ aplic_msi_irq_retrigger_level(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 hwirq)
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *aplic_idc_ತಳ(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 idc_ಸೂಚ್ಯಂಕ)
{
	ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)(aplic->addr + APLIC_IDC_ತಳ +
			idc_ಸೂಚ್ಯಂಕ * APLIC_IDC_ಗಾತ್ರ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32 aplic_idc_read(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 idc_ಸೂಚ್ಯಂಕ,
				 u32 reg)
{
	ಹಿಂತಿರುಗಿ readl(aplic_idc_ತಳ(aplic, idc_ಸೂಚ್ಯಂಕ) + reg);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ aplic_idc_write(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 idc_ಸೂಚ್ಯಂಕ,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_ತಳ(aplic, idc_ಸೂಚ್ಯಂಕ) + reg);
}

ಸ್ಥಿರ u32 aplic_hwirq_flags_ಗಮ್ಯ_sourcecfg(u32 hwirq_flags)
{
	ಆಯ್ಕೆ (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_ಮುಖವಾಡ) {
	ಸ್ಥಿತಿ SBI_HWIRQ_FLAGS_ಅಂಚು_RISING:
		ಹಿಂತಿರುಗಿ APLIC_SOURCECFG_SM_ಅಂಚು_RISE;
	ಸ್ಥಿತಿ SBI_HWIRQ_FLAGS_ಅಂಚು_FALLING:
		ಹಿಂತಿರುಗಿ APLIC_SOURCECFG_SM_ಅಂಚು_FALL;
	ಸ್ಥಿತಿ SBI_HWIRQ_FLAGS_LEVEL_ಮೇಲಿನ:
		ಹಿಂತಿರುಗಿ APLIC_SOURCECFG_SM_LEVEL_ಮೇಲಿನ;
	ಸ್ಥಿತಿ SBI_HWIRQ_FLAGS_LEVEL_ಕೆಳಗಿನ:
		ಹಿಂತಿರುಗಿ APLIC_SOURCECFG_SM_LEVEL_ಕೆಳಗಿನ;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ಸ್ಥಿರ bool aplic_hwirq_is_delegated(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	ಹಿಂತಿರುಗಿ !!(sourcecfg & APLIC_SOURCECFG_D);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aplic_ಕಂಡುಹಿಡಿ_idc_ಸೂಚ್ಯಂಕ(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 hart_ಸೂಚ್ಯಂಕ)
{
	u32 i;

	ಗಾಗಿ (i = 0; i < aplic->ಅಂಕೆ_idc; i++) {
		ಆದರೆ (aplic->idc_map[i] == hart_ಸೂಚ್ಯಂಕ)
			ಹಿಂತಿರುಗಿ i;
	}

	ಹಿಂತಿರುಗಿ SBI_ENOENT;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_init(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic)
{
	ರಚನೆ aplic_delegate_ದತ್ತಾಂಶ *deleg;
	u32 i, j, tmp, domaincfg;
	ಪೂರ್ಣಾಂಕ locked;

	/* Set domain configuration to 0 */
	writel(0, (ಶೂನ್ಯ *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	ಗಾಗಿ (i = 0; i <= aplic->ಅಂಕೆ_ಮೂಲ; i += 32)
		writel(-1U, (ಶೂನ್ಯ *)(aplic->addr + APLIC_CLRIE_ತಳ +
				     (i / 32) * ಗಾತ್ರ(u32)));

	/* Set interrupt type and priority for all interrupts */
	ಗಾಗಿ (i = 1; i <= aplic->ಅಂಕೆ_ಮೂಲ; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (ಶೂನ್ಯ *)(aplic->addr + APLIC_SOURCECFG_ತಳ +
			  (i - 1) * ಗಾತ್ರ(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_ಆದ್ಯತೆ, (ಶೂನ್ಯ *)(aplic->addr +
						APLIC_ಗುರಿ_ತಳ +
						(i - 1) * ಗಾತ್ರ(u32)));
	}

	/* Configure IRQ delegation */
	ಗಾಗಿ (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ಆದರೆ (!deleg->ಮೊದಲ_irq || !deleg->ಕೊನೆಯ_irq)
			ಮುಂದುವರಿ;
		ಆದರೆ (aplic->ಅಂಕೆ_ಮೂಲ < deleg->ಮೊದಲ_irq ||
		    aplic->ಅಂಕೆ_ಮೂಲ < deleg->ಕೊನೆಯ_irq)
			ಮುಂದುವರಿ;
		ಆದರೆ (deleg->ಮಗು_ಸೂಚ್ಯಂಕ > APLIC_SOURCECFG_CHILDIDX_ಮುಖವಾಡ)
			ಮುಂದುವರಿ;
		ಆದರೆ (deleg->ಮೊದಲ_irq > deleg->ಕೊನೆಯ_irq) {
			tmp = deleg->ಮೊದಲ_irq;
			deleg->ಮೊದಲ_irq = deleg->ಕೊನೆಯ_irq;
			deleg->ಕೊನೆಯ_irq = tmp;
		}
		ಗಾಗಿ (j = deleg->ಮೊದಲ_irq; j <= deleg->ಕೊನೆಯ_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->ಮಗು_ಸೂಚ್ಯಂಕ);
	}

	/* Default initialization of IDC structures */
	ಆದರೆ (aplic_is_ನೇರ_ಬಹುಲಕ(aplic)) {
		ಗಾಗಿ (i = 0; i < aplic->ಅಂಕೆ_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* MSI configuration */
		locked = readl((ಶೂನ್ಯ *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		ಆದರೆ (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(ಶೂನ್ಯ *)(aplic->addr + APLIC_MMSICFGADDR),
						(ಶೂನ್ಯ *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		ಆದರೆ (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(ಶೂನ್ಯ *)(aplic->addr + APLIC_SMSICFGADDR),
						(ಶೂನ್ಯ *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	ಆದರೆ (!aplic_is_ನೇರ_ಬಹುಲಕ(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (ಶೂನ್ಯ *)(aplic->addr + APLIC_DOMAINCFG));
}

ಶೂನ್ಯ aplic_reinit_all(ಶೂನ್ಯ)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;

	sbi_list_for_each_ನಮೂದು(aplic, &aplic_list, ಗಂಟು)
		aplic_init(aplic);
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_writel_msicfg(ರಚನೆ aplic_msicfg_ದತ್ತಾಂಶ *msicfg,
				ಶೂನ್ಯ *msicfgaddr, ಶೂನ್ಯ *msicfgaddrH)
{
	u32 val;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_ppn;

	/* Compute the MSI base PPN */
	ತಳ_ppn = msicfg->ತಳ_addr >> APLIC_x_MSICFGADDR_PPN_ಸರಿಸು;
	ತಳ_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	ತಳ_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	ತಳ_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)ತಳ_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)ತಳ_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_ಮುಖವಾಡ;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_ಮುಖವಾಡ)
		<< APLIC_x_MSICFGADDRH_LHXW_ಸರಿಸು;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_ಮುಖವಾಡ)
		<< APLIC_x_MSICFGADDRH_HHXW_ಸರಿಸು;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_ಮುಖವಾಡ)
		<< APLIC_x_MSICFGADDRH_LHXS_ಸರಿಸು;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_ಮುಖವಾಡ)
		<< APLIC_x_MSICFGADDRH_HHXS_ಸರಿಸು;
	writel(val, msicfgaddrH);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aplic_check_msicfg(ರಚನೆ aplic_msicfg_ದತ್ತಾಂಶ *msicfg)
{
	ಆದರೆ (APLIC_x_MSICFGADDRH_LHXS_ಮುಖವಾಡ < msicfg->lhxs)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (APLIC_x_MSICFGADDRH_LHXW_ಮುಖವಾಡ < msicfg->lhxw)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (APLIC_x_MSICFGADDRH_HHXS_ಮುಖವಾಡ < msicfg->hhxs)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (APLIC_x_MSICFGADDRH_HHXW_ಮುಖವಾಡ < msicfg->hhxw)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aplic_warm_init(ರಚನೆ sbi_irqchip_device *chip)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;
	u32 hart_ಸೂಚ್ಯಂಕ;
	ಪೂರ್ಣಾಂಕ idc_ಸೂಚ್ಯಂಕ;

	aplic = container_ದ(chip, ರಚನೆ aplic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (!aplic_is_ನೇರ_ಬಹುಲಕ(aplic))
		ಹಿಂತಿರುಗಿ 0;

	hart_ಸೂಚ್ಯಂಕ = current_hartindex();
	idc_ಸೂಚ್ಯಂಕ  = aplic_ಕಂಡುಹಿಡಿ_idc_ಸೂಚ್ಯಂಕ(aplic, hart_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (idc_ಸೂಚ್ಯಂಕ < 0)
		ಹಿಂತಿರುಗಿ 0;

	aplic_idc_write(aplic, idc_ಸೂಚ್ಯಂಕ, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_ಸೂಚ್ಯಂಕ, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aplic_process_hwirqs(ರಚನೆ sbi_irqchip_device *chip)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;
	u32 hart_ಸೂಚ್ಯಂಕ, claimi, hwirq;
	ಪೂರ್ಣಾಂಕ idc_ಸೂಚ್ಯಂಕ, rc = 0, tmp;

	aplic = container_ದ(chip, ರಚನೆ aplic_ದತ್ತಾಂಶ, irqchip);

	ಆದರೆ (!aplic_is_ನೇರ_ಬಹುಲಕ(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_ಸೂಚ್ಯಂಕ = current_hartindex();
	idc_ಸೂಚ್ಯಂಕ  = aplic_ಕಂಡುಹಿಡಿ_idc_ಸೂಚ್ಯಂಕ(aplic, hart_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (idc_ಸೂಚ್ಯಂಕ < 0)
		ಹಿಂತಿರುಗಿ 0;

	ಆಗ ((claimi = aplic_idc_read(aplic, idc_ಸೂಚ್ಯಂಕ, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_ಸರಿಸು) &
			APLIC_IDC_TOPI_ID_ಮುಖವಾಡ;

		ಆದರೆ (!hwirq)
			ನಿಲ್ಲಿಸು;

		ಆದರೆ (aplic_hwirq_is_delegated(aplic, hwirq))
			ಮುಂದುವರಿ;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		ಆದರೆ (tmp)
			rc = tmp;
	}

	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_hwirq_eoi(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;
	u32 sm;

	aplic = container_ದ(chip, ರಚನೆ aplic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (aplic_is_ನೇರ_ಬಹುಲಕ(aplic))
		ಹಿಂತಿರುಗಿ;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_ಮುಖವಾಡ;
	ಆದರೆ (sm == APLIC_SOURCECFG_SM_LEVEL_ಮೇಲಿನ ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_ಕೆಳಗಿನ)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_ಕಾರ್ಯಕ್ರಮ_msi_ಗುರಿ(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic,
				     u32 hwirq, u32 hart_ಸೂಚ್ಯಂಕ,
				     u32 guest_ಸೂಚ್ಯಂಕ, u32 eiid)
{
	u32 ಗುರಿ;

	ಗುರಿ = APLIC_ಗುರಿ_HART_IDX(hart_ಸೂಚ್ಯಂಕ) |
		 APLIC_ಗುರಿ_GUEST_IDX(guest_ಸೂಚ್ಯಂಕ) |
		 APLIC_ಗುರಿ_EIID(eiid);

	aplic_ಗುರಿ_write(aplic, hwirq, ಗುರಿ);
}

ಸ್ಥಿರ u32 derive_hart_ಸೂಚ್ಯಂಕ(ರಚನೆ aplic_msicfg_ದತ್ತಾಂಶ *msicfg,
			      ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ)
{
	u64 addr = ((u64)ಸಂದೇಶ->ವಿಳಾಸ_hi << 32) | ಸಂದೇಶ->ವಿಳಾಸ_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_ಸರಿಸು;
	u32 ಗುಂಪು_ಸೂಚ್ಯಂಕ, hart_ಸೂಚ್ಯಂಕ;

	ಗುಂಪು_ಸೂಚ್ಯಂಕ = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_ಸರಿಸು(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_ಮುಖವಾಡ(msicfg->hhxw);

	hart_ಸೂಚ್ಯಂಕ = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_ಸೂಚ್ಯಂಕ |= (ಗುಂಪು_ಸೂಚ್ಯಂಕ << msicfg->lhxw);

	ಹಿಂತಿರುಗಿ hart_ಸೂಚ್ಯಂಕ;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_write_msi(u32 ಪೋಷಕ_hwirq,
			    ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ,
			    ಶೂನ್ಯ *priv)
{
	ರಚನೆ aplic_msi_ದತ್ತಾಂಶ *msi_ದತ್ತಾಂಶ = priv;
	u32 guest_ಸೂಚ್ಯಂಕ = 0;
	u32 eiid;

	eiid = ಸಂದೇಶ->ದತ್ತಾಂಶ;
	ಆದರೆ (!eiid || eiid > APLIC_ಗುರಿ_EIID_ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ;

	aplic_ಕಾರ್ಯಕ್ರಮ_msi_ಗುರಿ(msi_ದತ್ತಾಂಶ->aplic, msi_ದತ್ತಾಂಶ->hwirq,
				derive_hart_ಸೂಚ್ಯಂಕ(&msi_ದತ್ತಾಂಶ->aplic->msicfg_mmode, ಸಂದೇಶ),
				guest_ಸೂಚ್ಯಂಕ, eiid);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aplic_msi_callback(u32 ಪೋಷಕ_hwirq, ಶೂನ್ಯ *priv)
{
	ರಚನೆ aplic_msi_ದತ್ತಾಂಶ *msi_ದತ್ತಾಂಶ = priv;
	ಪೂರ್ಣಾಂಕ rc;

	rc = sbi_irqchip_process_hwirq(&msi_ದತ್ತಾಂಶ->aplic->irqchip, msi_ದತ್ತಾಂಶ->hwirq);
	ಆದರೆ (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಪೋಷಕ_hwirq, rc);

	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aplic_ಸಿದ್ಧತೆ_msi(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic, u32 hwirq)
{
	ರಚನೆ sbi_irqchip_device *ಪೋಷಕ;
	u32 ಮೊದಲ_hwirq;
	ರಚನೆ aplic_msi_ದತ್ತಾಂಶ *msi_ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ rc;

	ಪೋಷಕ = sbi_irqchip_ಕಂಡುಹಿಡಿ_device(aplic->ಪೋಷಕ_unique_id);
	ಆದರೆ (!ಪೋಷಕ) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	msi_ದತ್ತಾಂಶ = sbi_zalloc(ಗಾತ್ರ(ರಚನೆ aplic_msi_ದತ್ತಾಂಶ));
	ಆದರೆ (!msi_ದತ್ತಾಂಶ)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	msi_ದತ್ತಾಂಶ->aplic = aplic;
	msi_ದತ್ತಾಂಶ->hwirq = hwirq;

	rc = sbi_irqchip_ರಿಜಿಸ್ಟರ್_msi(ಪೋಷಕ, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_ದತ್ತಾಂಶ,
					&ಮೊದಲ_hwirq);
	ಆದರೆ (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq, rc);
		sbi_free(msi_ದತ್ತಾಂಶ);
		ಹಿಂತಿರುಗಿ rc;
	}

	msi_ದತ್ತಾಂಶ->ಪೋಷಕ_hwirq = ಮೊದಲ_hwirq;
	sbi_irqchip_ಗಣ_hwirq_priv(&aplic->irqchip, hwirq, msi_ದತ್ತಾಂಶ);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aplic_hwirq_ಸಿದ್ಧತೆ(ರಚನೆ sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;
	u32 sourcecfg;

	aplic = container_ದ(chip, ರಚನೆ aplic_ದತ್ತಾಂಶ, irqchip);

	sourcecfg = aplic_hwirq_flags_ಗಮ್ಯ_sourcecfg(hwirq_flags);
	ಆದರೆ (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq_flags,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	ಆದರೆ (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq);
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	ಆದರೆ (!aplic_is_ನೇರ_ಬಹುಲಕ(aplic))
		ಹಿಂತಿರುಗಿ aplic_ಸಿದ್ಧತೆ_msi(aplic, hwirq);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_hwirq_cleanup(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;
	ರಚನೆ aplic_msi_ದತ್ತಾಂಶ *msi_ದತ್ತಾಂಶ;

	aplic = container_ದ(chip, ರಚನೆ aplic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (aplic_hwirq_is_delegated(aplic, hwirq))
		ಹಿಂತಿರುಗಿ;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_ಗುರಿ_write(aplic, hwirq, APLIC_DEFAULT_ಆದ್ಯತೆ);
	msi_ದತ್ತಾಂಶ = (ರಚನೆ aplic_msi_ದತ್ತಾಂಶ *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	ಆದರೆ (msi_ದತ್ತಾಂಶ)
		sbi_free(msi_ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aplic_hwirq_ಗಣ_affinity(ರಚನೆ sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_ಸೂಚ್ಯಂಕ)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;
	ರಚನೆ aplic_msi_ದತ್ತಾಂಶ *msi_ದತ್ತಾಂಶ;
	ರಚನೆ sbi_irqchip_device *ಪೋಷಕ;
	ಪೂರ್ಣಾಂಕ idc_ಸೂಚ್ಯಂಕ;
	ಪೂರ್ಣಾಂಕ rc;

	aplic = container_ದ(chip, ರಚನೆ aplic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (aplic_hwirq_is_delegated(aplic, hwirq))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ಆದರೆ (!aplic_is_ನೇರ_ಬಹುಲಕ(aplic)) {
		ಪೋಷಕ = sbi_irqchip_ಕಂಡುಹಿಡಿ_device(aplic->ಪೋಷಕ_unique_id);
		ಆದರೆ (!ಪೋಷಕ) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq);
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		}

		msi_ದತ್ತಾಂಶ = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_ಗಣ_affinity(ಪೋಷಕ, msi_ದತ್ತಾಂಶ->ಪೋಷಕ_hwirq, hart_ಸೂಚ್ಯಂಕ);
		ಆದರೆ (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)hwirq, rc);
			ಹಿಂತಿರುಗಿ rc;
		}
		ಹಿಂತಿರುಗಿ 0;
	}

	idc_ಸೂಚ್ಯಂಕ = aplic_ಕಂಡುಹಿಡಿ_idc_ಸೂಚ್ಯಂಕ(aplic, hart_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (idc_ಸೂಚ್ಯಂಕ < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	aplic_ಗುರಿ_write(aplic, hwirq,
			   APLIC_ಗುರಿ_HART_IDX(hart_ಸೂಚ್ಯಂಕ) |
			   APLIC_ಗುರಿ_IPRIO(APLIC_DEFAULT_ಆದ್ಯತೆ));

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_hwirq_ಮುಖವಾಡ(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;

	aplic = container_ದ(chip, ರಚನೆ aplic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (aplic_hwirq_is_delegated(aplic, hwirq))
		ಹಿಂತಿರುಗಿ;

	aplic_irq_clrie(aplic, hwirq);
}

ಸ್ಥಿರ ಶೂನ್ಯ aplic_hwirq_unmask(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic;

	aplic = container_ದ(chip, ರಚನೆ aplic_ದತ್ತಾಂಶ, irqchip);
	ಆದರೆ (aplic_hwirq_is_delegated(aplic, hwirq))
		ಹಿಂತಿರುಗಿ;

	aplic_irq_setie(aplic, hwirq);
}

ಸ್ಥಿರ ರಚನೆ sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_ಸಿದ್ಧತೆ		= aplic_hwirq_ಸಿದ್ಧತೆ,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_ಗಣ_affinity	= aplic_hwirq_ಗಣ_affinity,
	.hwirq_ಮುಖವಾಡ			= aplic_hwirq_ಮುಖವಾಡ,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

ಪೂರ್ಣಾಂಕ aplic_cold_irqchip_init(ರಚನೆ aplic_ದತ್ತಾಂಶ *aplic)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ aplic_delegate_ದತ್ತಾಂಶ *deleg;
	u32 ಮೊದಲ_deleg_irq, ಕೊನೆಯ_deleg_irq, i;
	bool msi_ಬಹುಲಕ;

	msi_ಬಹುಲಕ = !aplic_is_ನೇರ_ಬಹುಲಕ(aplic);
	ಆದರೆ (!aplic->ಅಂಕೆ_ಮೂಲ) {
		sbi_printf("%s: num_source is zero\n", __func__);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	ಆದರೆ (APLIC_MAX_ಮೂಲ <= aplic->ಅಂಕೆ_ಮೂಲ) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)aplic->ಅಂಕೆ_ಮೂಲ,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)APLIC_MAX_ಮೂಲ);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	ಆದರೆ (!msi_ಬಹುಲಕ && APLIC_MAX_IDC <= aplic->ಅಂಕೆ_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)aplic->ಅಂಕೆ_idc,
			(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)APLIC_MAX_IDC);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}

	ಆದರೆ (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		ಆದರೆ (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಆದರೆ (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		ಆದರೆ (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	/* Init the APLIC registers */
	aplic_init(aplic);

	/*
	 * Add APLIC region to the root domain if:
	 * 1) It targets M-mode of any HART directly or via MSIs
	 * 2) All interrupts are delegated to some child APLIC
	 */
	ಮೊದಲ_deleg_irq = -1U;
	ಕೊನೆಯ_deleg_irq = 0;
	ಗಾಗಿ (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ಆದರೆ (deleg->ಮೊದಲ_irq < ಮೊದಲ_deleg_irq)
			ಮೊದಲ_deleg_irq = deleg->ಮೊದಲ_irq;
		ಆದರೆ (ಕೊನೆಯ_deleg_irq < deleg->ಕೊನೆಯ_irq)
			ಕೊನೆಯ_deleg_irq = deleg->ಕೊನೆಯ_irq;
	}

	ಆದರೆ (aplic->targets_mmode ||
	    ((ಮೊದಲ_deleg_irq < ಕೊನೆಯ_deleg_irq) &&
	    (ಕೊನೆಯ_deleg_irq == aplic->ಅಂಕೆ_ಮೂಲ) &&
	    (ಮೊದಲ_deleg_irq == 1))) {
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(aplic->addr, aplic->ಗಾತ್ರ_2, ಪುಟ_ಗಾತ್ರ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.ಅಂಕೆ_hwirq = aplic->ಅಂಕೆ_ಮೂಲ + 1;

		ಆದರೆ (msi_ಬಹುಲಕ) {
			aplic->irqchip.warm_init      = ಖಾಲಿ;
			aplic->irqchip.process_hwirqs = ಖಾಲಿ;
			aplic->irqchip.hwirq_eoi      = ಖಾಲಿ;
		}

		ಆದರೆ (msi_ಬಹುಲಕ)
			sbi_hartmask_ಗಣ_all(&aplic->irqchip.ಗುರಿ_harts);
		ಇಲ್ಲದಿದ್ದರೆ
			ಗಾಗಿ (i = 0; i < aplic->ಅಂಕೆ_idc; i++)
				sbi_hartmask_ಗಣ_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.ಗುರಿ_harts);

		rc = sbi_irqchip_ಕೂಡಿಸು_device(&aplic->irqchip);
		ಆದರೆ (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)aplic->irqchip.id,
				msi_ಬಹುಲಕ ? "msi" : "direct",
				(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)sbi_hartmask_ತೂಕ(
					&aplic->irqchip.ಗುರಿ_harts));
			ಹಿಂತಿರುಗಿ rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&aplic->ಗಂಟು, &aplic_list);

	ಹಿಂತಿರುಗಿ 0;
}
