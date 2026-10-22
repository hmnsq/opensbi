/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/irqchip/aplic.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_MAX_IDC			(1UL << 14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_MAX_ਸਰੋਤ		1024

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_DOMAINCFG		0x0000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_DOMAINCFG_IE		(1 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_DOMAINCFG_DM		(1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_DOMAINCFG_BE		(1 << 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_ਅਧਾਰ		0x0004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_D		(1 << 10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_CHILDIDX_ਨਕਾਬ	0x000003ff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_SM_ਨਕਾਬ	0x00000007
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_SM_INACTIVE	0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_SM_DETACH	0x1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_SM_ਕਿਨਾਰਾ_RISE	0x4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_SM_ਕਿਨਾਰਾ_FALL	0x5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_SM_LEVEL_ਉੱਚਾ	0x6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SOURCECFG_SM_LEVEL_ਨੀਵਾਂ	0x7

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_MMSICFGADDR		0x1bc0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_MMSICFGADDRH		0x1bc4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SMSICFGADDR		0x1bc8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SMSICFGADDRH		0x1bcc

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_xMSICFGADDRH_L		(1UL << 31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_HHXS_ਨਕਾਬ	0x1f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_HHXS_ਖਿਸਕਾਓ	24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_LHXS_ਨਕਾਬ	0x7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_LHXS_ਖਿਸਕਾਓ	20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_HHXW_ਨਕਾਬ	0x7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_HHXW_ਖਿਸਕਾਓ	16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_LHXW_ਨਕਾਬ	0xf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_LHXW_ਖਿਸਕਾਓ	12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDRH_BAPPN_ਨਕਾਬ	0xfff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDR_PPN_ਖਿਸਕਾਓ	12

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDR_PPN_LHX_ਨਕਾਬ(__lhxw) \
	((1UL << (__lhxw)) - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDR_PPN_LHX_ਖਿਸਕਾਓ(__lhxs) \
	((__lhxs))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_x_MSICFGADDR_PPN_LHX_ਨਕਾਬ(__lhxw) << \
	 APLIC_x_MSICFGADDR_PPN_LHX_ਖਿਸਕਾਓ(__lhxs))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDR_PPN_HHX_ਨਕਾਬ(__hhxw) \
	((1UL << (__hhxw)) - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_x_MSICFGADDR_PPN_HHX_ਖਿਸਕਾਓ(__hhxs) \
	((__hhxs) + APLIC_x_MSICFGADDR_PPN_ਖਿਸਕਾਓ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_x_MSICFGADDR_PPN_HHX_ਨਕਾਬ(__hhxw) << \
	 APLIC_x_MSICFGADDR_PPN_HHX_ਖਿਸਕਾਓ(__hhxs))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_GUEST_IDX(__gidx) \
	((((u32)(__gidx)) & APLIC_ਨਿਸ਼ਾਨਾ_GUEST_IDX_ਨਕਾਬ) << \
	 APLIC_ਨਿਸ਼ਾਨਾ_GUEST_IDX_ਖਿਸਕਾਓ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_EIID(__eiid) \
	(((u32)(__eiid)) & APLIC_ਨਿਸ਼ਾਨਾ_EIID_ਨਕਾਬ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SETIP_ਅਧਾਰ		0x1c00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SETIPNUM			0x1cdc

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_CLRIP_ਅਧਾਰ		0x1d00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_CLRIPNUM			0x1ddc

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SETIE_ਅਧਾਰ		0x1e00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SETIENUM			0x1edc

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_CLRIE_ਅਧਾਰ		0x1f00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_CLRIENUM			0x1fdc

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SETIPNUM_LE		0x2000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_SETIPNUM_BE		0x2004

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_ਅਧਾਰ		0x3004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_HART_IDX_ਖਿਸਕਾਓ	18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_HART_IDX_ਨਕਾਬ	0x3fff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_GUEST_IDX_ਖਿਸਕਾਓ	12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_GUEST_IDX_ਨਕਾਬ	0x3f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_IPRIO_ਨਕਾਬ	0xff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_EIID_ਨਕਾਬ	0x7ff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_HART_IDX(__hidx) \
	((((u32)(__hidx)) & APLIC_ਨਿਸ਼ਾਨਾ_HART_IDX_ਨਕਾਬ) << \
	 APLIC_ਨਿਸ਼ਾਨਾ_HART_IDX_ਖਿਸਕਾਓ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ਨਿਸ਼ਾਨਾ_IPRIO(__prio) \
	(((u32)(__prio)) & APLIC_ਨਿਸ਼ਾਨਾ_IPRIO_ਨਕਾਬ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_ਅਧਾਰ			0x4000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_ਮਾਪ			32

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_IDELIVERY		0x00

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_IFORCE		0x04

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_ITHRESHOLD		0x08

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_TOPI			0x18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_TOPI_ID_ਖਿਸਕਾਓ	16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_TOPI_ID_ਨਕਾਬ	0x3ff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_TOPI_PRIO_ਨਕਾਬ	0xff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_IDC_CLAIMI		0x1c

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_DEFAULT_ਤਰਜੀਹ		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_DISABLE_IDELIVERY		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ENABLE_IDELIVERY		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_DISABLE_ITHRESHOLD	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APLIC_ENABLE_ITHRESHOLD		0

ਬਣਤਰ aplic_msi_ਅੰਕੜੇ {
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;
	u32 hwirq;
	u32 ਮਾਪਾ_hwirq;
};

ਸਥਿਰ SBI_LIST_ਸਿਰ(aplic_list);
ਸਥਿਰ ਖਾਲੀ aplic_writel_msicfg(ਬਣਤਰ aplic_msicfg_ਅੰਕੜੇ *msicfg,
				ਖਾਲੀ *msicfgaddr, ਖਾਲੀ *msicfgaddrH);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic)
{
	ਵਾਪਸ aplic && aplic->ਅੰਕ_idc;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ aplic_sourcecfg_write(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (ਖਾਲੀ *)(aplic->addr + APLIC_SOURCECFG_ਅਧਾਰ +
			     (hwirq - 1) * ਆਕਾਰ(u32)));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ u32 aplic_sourcecfg_read(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 hwirq)
{
	ਵਾਪਸ readl((ਖਾਲੀ *)(aplic->addr + APLIC_SOURCECFG_ਅਧਾਰ +
			      (hwirq - 1) * ਆਕਾਰ(u32)));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ aplic_ਨਿਸ਼ਾਨਾ_write(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (ਖਾਲੀ *)(aplic->addr + APLIC_ਨਿਸ਼ਾਨਾ_ਅਧਾਰ +
			     (hwirq - 1) * ਆਕਾਰ(u32)));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ aplic_irq_setie(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 hwirq)
{
	writel(hwirq, (ਖਾਲੀ *)(aplic->addr + APLIC_SETIENUM));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ aplic_irq_clrie(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 hwirq)
{
	writel(hwirq, (ਖਾਲੀ *)(aplic->addr + APLIC_CLRIENUM));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ aplic_irq_clrip(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 hwirq)
{
	writel(hwirq, (ਖਾਲੀ *)(aplic->addr + APLIC_CLRIPNUM));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ aplic_msi_irq_retrigger_level(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 hwirq)
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
	writel(hwirq, (ਖਾਲੀ *)(aplic->addr + APLIC_SETIPNUM_LE));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *aplic_idc_ਅਧਾਰ(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 idc_ਸੂਚਕਾਂਕ)
{
	ਵਾਪਸ (ਖਾਲੀ *)(aplic->addr + APLIC_IDC_ਅਧਾਰ +
			idc_ਸੂਚਕਾਂਕ * APLIC_IDC_ਮਾਪ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ u32 aplic_idc_read(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 idc_ਸੂਚਕਾਂਕ,
				 u32 reg)
{
	ਵਾਪਸ readl(aplic_idc_ਅਧਾਰ(aplic, idc_ਸੂਚਕਾਂਕ) + reg);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ aplic_idc_write(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 idc_ਸੂਚਕਾਂਕ,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_ਅਧਾਰ(aplic, idc_ਸੂਚਕਾਂਕ) + reg);
}

ਸਥਿਰ u32 aplic_hwirq_flags_ਮੰਜ਼ਿਲ_sourcecfg(u32 hwirq_flags)
{
	ਚੋਣ (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_ਨਕਾਬ) {
	ਹਾਲਤ SBI_HWIRQ_FLAGS_ਕਿਨਾਰਾ_RISING:
		ਵਾਪਸ APLIC_SOURCECFG_SM_ਕਿਨਾਰਾ_RISE;
	ਹਾਲਤ SBI_HWIRQ_FLAGS_ਕਿਨਾਰਾ_FALLING:
		ਵਾਪਸ APLIC_SOURCECFG_SM_ਕਿਨਾਰਾ_FALL;
	ਹਾਲਤ SBI_HWIRQ_FLAGS_LEVEL_ਉੱਚਾ:
		ਵਾਪਸ APLIC_SOURCECFG_SM_LEVEL_ਉੱਚਾ;
	ਹਾਲਤ SBI_HWIRQ_FLAGS_LEVEL_ਨੀਵਾਂ:
		ਵਾਪਸ APLIC_SOURCECFG_SM_LEVEL_ਨੀਵਾਂ;
	ਮੂਲ:
		ਵਾਪਸ APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ਸਥਿਰ bool aplic_hwirq_is_delegated(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	ਵਾਪਸ !!(sourcecfg & APLIC_SOURCECFG_D);
}

ਸਥਿਰ ਪੂਰਨ aplic_ਲੱਭੋ_idc_ਸੂਚਕਾਂਕ(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 hart_ਸੂਚਕਾਂਕ)
{
	u32 i;

	ਲਈ (i = 0; i < aplic->ਅੰਕ_idc; i++) {
		ਜੇ (aplic->idc_map[i] == hart_ਸੂਚਕਾਂਕ)
			ਵਾਪਸ i;
	}

	ਵਾਪਸ SBI_ENOENT;
}

ਸਥਿਰ ਖਾਲੀ aplic_init(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic)
{
	ਬਣਤਰ aplic_delegate_ਅੰਕੜੇ *deleg;
	u32 i, j, tmp, domaincfg;
	ਪੂਰਨ locked;

	/* Set domain configuration to 0 */
	writel(0, (ਖਾਲੀ *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	ਲਈ (i = 0; i <= aplic->ਅੰਕ_ਸਰੋਤ; i += 32)
		writel(-1U, (ਖਾਲੀ *)(aplic->addr + APLIC_CLRIE_ਅਧਾਰ +
				     (i / 32) * ਆਕਾਰ(u32)));

	/* Set interrupt type and priority for all interrupts */
	ਲਈ (i = 1; i <= aplic->ਅੰਕ_ਸਰੋਤ; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (ਖਾਲੀ *)(aplic->addr + APLIC_SOURCECFG_ਅਧਾਰ +
			  (i - 1) * ਆਕਾਰ(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_ਤਰਜੀਹ, (ਖਾਲੀ *)(aplic->addr +
						APLIC_ਨਿਸ਼ਾਨਾ_ਅਧਾਰ +
						(i - 1) * ਆਕਾਰ(u32)));
	}

	/* Configure IRQ delegation */
	ਲਈ (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ਜੇ (!deleg->ਪਹਿਲਾ_irq || !deleg->ਆਖਰੀ_irq)
			ਜਾਰੀ;
		ਜੇ (aplic->ਅੰਕ_ਸਰੋਤ < deleg->ਪਹਿਲਾ_irq ||
		    aplic->ਅੰਕ_ਸਰੋਤ < deleg->ਆਖਰੀ_irq)
			ਜਾਰੀ;
		ਜੇ (deleg->ਬੱਚਾ_ਸੂਚਕਾਂਕ > APLIC_SOURCECFG_CHILDIDX_ਨਕਾਬ)
			ਜਾਰੀ;
		ਜੇ (deleg->ਪਹਿਲਾ_irq > deleg->ਆਖਰੀ_irq) {
			tmp = deleg->ਪਹਿਲਾ_irq;
			deleg->ਪਹਿਲਾ_irq = deleg->ਆਖਰੀ_irq;
			deleg->ਆਖਰੀ_irq = tmp;
		}
		ਲਈ (j = deleg->ਪਹਿਲਾ_irq; j <= deleg->ਆਖਰੀ_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->ਬੱਚਾ_ਸੂਚਕਾਂਕ);
	}

	/* Default initialization of IDC structures */
	ਜੇ (aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(aplic)) {
		ਲਈ (i = 0; i < aplic->ਅੰਕ_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} ਨਹੀਂਤਾਂ {
		/* MSI configuration */
		locked = readl((ਖਾਲੀ *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		ਜੇ (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(ਖਾਲੀ *)(aplic->addr + APLIC_MMSICFGADDR),
						(ਖਾਲੀ *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		ਜੇ (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(ਖਾਲੀ *)(aplic->addr + APLIC_SMSICFGADDR),
						(ਖਾਲੀ *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	ਜੇ (!aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (ਖਾਲੀ *)(aplic->addr + APLIC_DOMAINCFG));
}

ਖਾਲੀ aplic_reinit_all(ਖਾਲੀ)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;

	sbi_list_for_each_ਇੰਦਰਾਜ(aplic, &aplic_list, ਗੰਢ)
		aplic_init(aplic);
}

ਸਥਿਰ ਖਾਲੀ aplic_writel_msicfg(ਬਣਤਰ aplic_msicfg_ਅੰਕੜੇ *msicfg,
				ਖਾਲੀ *msicfgaddr, ਖਾਲੀ *msicfgaddrH)
{
	u32 val;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_ppn;

	/* Compute the MSI base PPN */
	ਅਧਾਰ_ppn = msicfg->ਅਧਾਰ_addr >> APLIC_x_MSICFGADDR_PPN_ਖਿਸਕਾਓ;
	ਅਧਾਰ_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	ਅਧਾਰ_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	ਅਧਾਰ_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	writel((u32)ਅਧਾਰ_ppn, msicfgaddr);

	/* Write the upper MSI config register */
	val = (((u64)ਅਧਾਰ_ppn) >> 32) &
		APLIC_x_MSICFGADDRH_BAPPN_ਨਕਾਬ;
	val |= (msicfg->lhxw & APLIC_x_MSICFGADDRH_LHXW_ਨਕਾਬ)
		<< APLIC_x_MSICFGADDRH_LHXW_ਖਿਸਕਾਓ;
	val |= (msicfg->hhxw & APLIC_x_MSICFGADDRH_HHXW_ਨਕਾਬ)
		<< APLIC_x_MSICFGADDRH_HHXW_ਖਿਸਕਾਓ;
	val |= (msicfg->lhxs & APLIC_x_MSICFGADDRH_LHXS_ਨਕਾਬ)
		<< APLIC_x_MSICFGADDRH_LHXS_ਖਿਸਕਾਓ;
	val |= (msicfg->hhxs & APLIC_x_MSICFGADDRH_HHXS_ਨਕਾਬ)
		<< APLIC_x_MSICFGADDRH_HHXS_ਖਿਸਕਾਓ;
	writel(val, msicfgaddrH);
}

ਸਥਿਰ ਪੂਰਨ aplic_check_msicfg(ਬਣਤਰ aplic_msicfg_ਅੰਕੜੇ *msicfg)
{
	ਜੇ (APLIC_x_MSICFGADDRH_LHXS_ਨਕਾਬ < msicfg->lhxs)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (APLIC_x_MSICFGADDRH_LHXW_ਨਕਾਬ < msicfg->lhxw)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (APLIC_x_MSICFGADDRH_HHXS_ਨਕਾਬ < msicfg->hhxs)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (APLIC_x_MSICFGADDRH_HHXW_ਨਕਾਬ < msicfg->hhxw)
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ aplic_warm_init(ਬਣਤਰ sbi_irqchip_device *chip)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;
	u32 hart_ਸੂਚਕਾਂਕ;
	ਪੂਰਨ idc_ਸੂਚਕਾਂਕ;

	aplic = container_ਦਾ(chip, ਬਣਤਰ aplic_ਅੰਕੜੇ, irqchip);
	ਜੇ (!aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(aplic))
		ਵਾਪਸ 0;

	hart_ਸੂਚਕਾਂਕ = current_hartindex();
	idc_ਸੂਚਕਾਂਕ  = aplic_ਲੱਭੋ_idc_ਸੂਚਕਾਂਕ(aplic, hart_ਸੂਚਕਾਂਕ);
	ਜੇ (idc_ਸੂਚਕਾਂਕ < 0)
		ਵਾਪਸ 0;

	aplic_idc_write(aplic, idc_ਸੂਚਕਾਂਕ, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_ਸੂਚਕਾਂਕ, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ aplic_process_hwirqs(ਬਣਤਰ sbi_irqchip_device *chip)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;
	u32 hart_ਸੂਚਕਾਂਕ, claimi, hwirq;
	ਪੂਰਨ idc_ਸੂਚਕਾਂਕ, rc = 0, tmp;

	aplic = container_ਦਾ(chip, ਬਣਤਰ aplic_ਅੰਕੜੇ, irqchip);

	ਜੇ (!aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_ਸੂਚਕਾਂਕ = current_hartindex();
	idc_ਸੂਚਕਾਂਕ  = aplic_ਲੱਭੋ_idc_ਸੂਚਕਾਂਕ(aplic, hart_ਸੂਚਕਾਂਕ);
	ਜੇ (idc_ਸੂਚਕਾਂਕ < 0)
		ਵਾਪਸ 0;

	ਜਦੋਂ ((claimi = aplic_idc_read(aplic, idc_ਸੂਚਕਾਂਕ, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_ਖਿਸਕਾਓ) &
			APLIC_IDC_TOPI_ID_ਨਕਾਬ;

		ਜੇ (!hwirq)
			ਰੋਕੋ;

		ਜੇ (aplic_hwirq_is_delegated(aplic, hwirq))
			ਜਾਰੀ;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		ਜੇ (tmp)
			rc = tmp;
	}

	ਵਾਪਸ rc;
}

ਸਥਿਰ ਖਾਲੀ aplic_hwirq_eoi(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;
	u32 sm;

	aplic = container_ਦਾ(chip, ਬਣਤਰ aplic_ਅੰਕੜੇ, irqchip);
	ਜੇ (aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(aplic))
		ਵਾਪਸ;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_ਨਕਾਬ;
	ਜੇ (sm == APLIC_SOURCECFG_SM_LEVEL_ਉੱਚਾ ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_ਨੀਵਾਂ)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ਸਥਿਰ ਖਾਲੀ aplic_ਪ੍ਰੋਗਰਾਮ_msi_ਨਿਸ਼ਾਨਾ(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic,
				     u32 hwirq, u32 hart_ਸੂਚਕਾਂਕ,
				     u32 guest_ਸੂਚਕਾਂਕ, u32 eiid)
{
	u32 ਨਿਸ਼ਾਨਾ;

	ਨਿਸ਼ਾਨਾ = APLIC_ਨਿਸ਼ਾਨਾ_HART_IDX(hart_ਸੂਚਕਾਂਕ) |
		 APLIC_ਨਿਸ਼ਾਨਾ_GUEST_IDX(guest_ਸੂਚਕਾਂਕ) |
		 APLIC_ਨਿਸ਼ਾਨਾ_EIID(eiid);

	aplic_ਨਿਸ਼ਾਨਾ_write(aplic, hwirq, ਨਿਸ਼ਾਨਾ);
}

ਸਥਿਰ u32 derive_hart_ਸੂਚਕਾਂਕ(ਬਣਤਰ aplic_msicfg_ਅੰਕੜੇ *msicfg,
			      ਅਚਲ ਬਣਤਰ sbi_irqchip_msi_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ)
{
	u64 addr = ((u64)ਸੁਨੇਹਾ->ਪਤਾ_hi << 32) | ਸੁਨੇਹਾ->ਪਤਾ_lo;
	u64 tppn = addr >> APLIC_x_MSICFGADDR_PPN_ਖਿਸਕਾਓ;
	u32 ਸਮੂਹ_ਸੂਚਕਾਂਕ, hart_ਸੂਚਕਾਂਕ;

	ਸਮੂਹ_ਸੂਚਕਾਂਕ = (tppn >> APLIC_x_MSICFGADDR_PPN_HHX_ਖਿਸਕਾਓ(msicfg->hhxs)) &
		      APLIC_x_MSICFGADDR_PPN_HHX_ਨਕਾਬ(msicfg->hhxw);

	hart_ਸੂਚਕਾਂਕ = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_ਸੂਚਕਾਂਕ |= (ਸਮੂਹ_ਸੂਚਕਾਂਕ << msicfg->lhxw);

	ਵਾਪਸ hart_ਸੂਚਕਾਂਕ;
}

ਸਥਿਰ ਖਾਲੀ aplic_write_msi(u32 ਮਾਪਾ_hwirq,
			    ਅਚਲ ਬਣਤਰ sbi_irqchip_msi_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ,
			    ਖਾਲੀ *priv)
{
	ਬਣਤਰ aplic_msi_ਅੰਕੜੇ *msi_ਅੰਕੜੇ = priv;
	u32 guest_ਸੂਚਕਾਂਕ = 0;
	u32 eiid;

	eiid = ਸੁਨੇਹਾ->ਅੰਕੜੇ;
	ਜੇ (!eiid || eiid > APLIC_ਨਿਸ਼ਾਨਾ_EIID_ਨਕਾਬ)
		ਵਾਪਸ;

	aplic_ਪ੍ਰੋਗਰਾਮ_msi_ਨਿਸ਼ਾਨਾ(msi_ਅੰਕੜੇ->aplic, msi_ਅੰਕੜੇ->hwirq,
				derive_hart_ਸੂਚਕਾਂਕ(&msi_ਅੰਕੜੇ->aplic->msicfg_mmode, ਸੁਨੇਹਾ),
				guest_ਸੂਚਕਾਂਕ, eiid);
}

ਸਥਿਰ ਪੂਰਨ aplic_msi_callback(u32 ਮਾਪਾ_hwirq, ਖਾਲੀ *priv)
{
	ਬਣਤਰ aplic_msi_ਅੰਕੜੇ *msi_ਅੰਕੜੇ = priv;
	ਪੂਰਨ rc;

	rc = sbi_irqchip_process_hwirq(&msi_ਅੰਕੜੇ->aplic->irqchip, msi_ਅੰਕੜੇ->hwirq);
	ਜੇ (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਮਾਪਾ_hwirq, rc);

	ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ aplic_ਤਿਆਰੀ_msi(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic, u32 hwirq)
{
	ਬਣਤਰ sbi_irqchip_device *ਮਾਪਾ;
	u32 ਪਹਿਲਾ_hwirq;
	ਬਣਤਰ aplic_msi_ਅੰਕੜੇ *msi_ਅੰਕੜੇ;
	ਪੂਰਨ rc;

	ਮਾਪਾ = sbi_irqchip_ਲੱਭੋ_device(aplic->ਮਾਪਾ_unique_id);
	ਜੇ (!ਮਾਪਾ) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq);
		ਵਾਪਸ SBI_EINVAL;
	}

	msi_ਅੰਕੜੇ = sbi_zalloc(ਆਕਾਰ(ਬਣਤਰ aplic_msi_ਅੰਕੜੇ));
	ਜੇ (!msi_ਅੰਕੜੇ)
		ਵਾਪਸ SBI_ENOMEM;

	msi_ਅੰਕੜੇ->aplic = aplic;
	msi_ਅੰਕੜੇ->hwirq = hwirq;

	rc = sbi_irqchip_ਰਜਿਸਟਰ_msi(ਮਾਪਾ, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_ਅੰਕੜੇ,
					&ਪਹਿਲਾ_hwirq);
	ਜੇ (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq, rc);
		sbi_free(msi_ਅੰਕੜੇ);
		ਵਾਪਸ rc;
	}

	msi_ਅੰਕੜੇ->ਮਾਪਾ_hwirq = ਪਹਿਲਾ_hwirq;
	sbi_irqchip_ਸੈੱਟ_hwirq_priv(&aplic->irqchip, hwirq, msi_ਅੰਕੜੇ);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ aplic_hwirq_ਤਿਆਰੀ(ਬਣਤਰ sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;
	u32 sourcecfg;

	aplic = container_ਦਾ(chip, ਬਣਤਰ aplic_ਅੰਕੜੇ, irqchip);

	sourcecfg = aplic_hwirq_flags_ਮੰਜ਼ਿਲ_sourcecfg(hwirq_flags);
	ਜੇ (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq_flags,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq);
		ਵਾਪਸ SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	ਜੇ (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq);
		ਵਾਪਸ SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	ਜੇ (!aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(aplic))
		ਵਾਪਸ aplic_ਤਿਆਰੀ_msi(aplic, hwirq);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ aplic_hwirq_cleanup(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;
	ਬਣਤਰ aplic_msi_ਅੰਕੜੇ *msi_ਅੰਕੜੇ;

	aplic = container_ਦਾ(chip, ਬਣਤਰ aplic_ਅੰਕੜੇ, irqchip);
	ਜੇ (aplic_hwirq_is_delegated(aplic, hwirq))
		ਵਾਪਸ;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_ਨਿਸ਼ਾਨਾ_write(aplic, hwirq, APLIC_DEFAULT_ਤਰਜੀਹ);
	msi_ਅੰਕੜੇ = (ਬਣਤਰ aplic_msi_ਅੰਕੜੇ *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	ਜੇ (msi_ਅੰਕੜੇ)
		sbi_free(msi_ਅੰਕੜੇ);
}

ਸਥਿਰ ਪੂਰਨ aplic_hwirq_ਸੈੱਟ_affinity(ਬਣਤਰ sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_ਸੂਚਕਾਂਕ)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;
	ਬਣਤਰ aplic_msi_ਅੰਕੜੇ *msi_ਅੰਕੜੇ;
	ਬਣਤਰ sbi_irqchip_device *ਮਾਪਾ;
	ਪੂਰਨ idc_ਸੂਚਕਾਂਕ;
	ਪੂਰਨ rc;

	aplic = container_ਦਾ(chip, ਬਣਤਰ aplic_ਅੰਕੜੇ, irqchip);
	ਜੇ (aplic_hwirq_is_delegated(aplic, hwirq))
		ਵਾਪਸ SBI_ENOTSUPP;

	ਜੇ (!aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(aplic)) {
		ਮਾਪਾ = sbi_irqchip_ਲੱਭੋ_device(aplic->ਮਾਪਾ_unique_id);
		ਜੇ (!ਮਾਪਾ) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq);
			ਵਾਪਸ SBI_EINVAL;
		}

		msi_ਅੰਕੜੇ = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_ਸੈੱਟ_affinity(ਮਾਪਾ, msi_ਅੰਕੜੇ->ਮਾਪਾ_hwirq, hart_ਸੂਚਕਾਂਕ);
		ਜੇ (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)hwirq, rc);
			ਵਾਪਸ rc;
		}
		ਵਾਪਸ 0;
	}

	idc_ਸੂਚਕਾਂਕ = aplic_ਲੱਭੋ_idc_ਸੂਚਕਾਂਕ(aplic, hart_ਸੂਚਕਾਂਕ);
	ਜੇ (idc_ਸੂਚਕਾਂਕ < 0)
		ਵਾਪਸ SBI_EINVAL;

	aplic_ਨਿਸ਼ਾਨਾ_write(aplic, hwirq,
			   APLIC_ਨਿਸ਼ਾਨਾ_HART_IDX(hart_ਸੂਚਕਾਂਕ) |
			   APLIC_ਨਿਸ਼ਾਨਾ_IPRIO(APLIC_DEFAULT_ਤਰਜੀਹ));

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ aplic_hwirq_ਨਕਾਬ(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;

	aplic = container_ਦਾ(chip, ਬਣਤਰ aplic_ਅੰਕੜੇ, irqchip);
	ਜੇ (aplic_hwirq_is_delegated(aplic, hwirq))
		ਵਾਪਸ;

	aplic_irq_clrie(aplic, hwirq);
}

ਸਥਿਰ ਖਾਲੀ aplic_hwirq_unmask(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic;

	aplic = container_ਦਾ(chip, ਬਣਤਰ aplic_ਅੰਕੜੇ, irqchip);
	ਜੇ (aplic_hwirq_is_delegated(aplic, hwirq))
		ਵਾਪਸ;

	aplic_irq_setie(aplic, hwirq);
}

ਸਥਿਰ ਬਣਤਰ sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_ਤਿਆਰੀ		= aplic_hwirq_ਤਿਆਰੀ,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_ਸੈੱਟ_affinity	= aplic_hwirq_ਸੈੱਟ_affinity,
	.hwirq_ਨਕਾਬ			= aplic_hwirq_ਨਕਾਬ,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

ਪੂਰਨ aplic_cold_irqchip_init(ਬਣਤਰ aplic_ਅੰਕੜੇ *aplic)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ aplic_delegate_ਅੰਕੜੇ *deleg;
	u32 ਪਹਿਲਾ_deleg_irq, ਆਖਰੀ_deleg_irq, i;
	bool msi_ਬਹੁਲਕ;

	msi_ਬਹੁਲਕ = !aplic_is_ਸਿੱਧਾ_ਬਹੁਲਕ(aplic);
	ਜੇ (!aplic->ਅੰਕ_ਸਰੋਤ) {
		sbi_printf("%s: num_source is zero\n", __func__);
		ਵਾਪਸ SBI_EINVAL;
	}

	ਜੇ (APLIC_MAX_ਸਰੋਤ <= aplic->ਅੰਕ_ਸਰੋਤ) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)aplic->ਅੰਕ_ਸਰੋਤ,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)APLIC_MAX_ਸਰੋਤ);
		ਵਾਪਸ SBI_EINVAL;
	}

	ਜੇ (!msi_ਬਹੁਲਕ && APLIC_MAX_IDC <= aplic->ਅੰਕ_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)aplic->ਅੰਕ_idc,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)APLIC_MAX_IDC);
		ਵਾਪਸ SBI_EINVAL;
	}

	ਜੇ (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		ਜੇ (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			ਵਾਪਸ rc;
		}
	}

	ਜੇ (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		ਜੇ (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			ਵਾਪਸ rc;
		}
	}

	/* Init the APLIC registers */
	aplic_init(aplic);

	/*
	 * Add APLIC region to the root domain if:
	 * 1) It targets M-mode of any HART directly or via MSIs
	 * 2) All interrupts are delegated to some child APLIC
	 */
	ਪਹਿਲਾ_deleg_irq = -1U;
	ਆਖਰੀ_deleg_irq = 0;
	ਲਈ (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ਜੇ (deleg->ਪਹਿਲਾ_irq < ਪਹਿਲਾ_deleg_irq)
			ਪਹਿਲਾ_deleg_irq = deleg->ਪਹਿਲਾ_irq;
		ਜੇ (ਆਖਰੀ_deleg_irq < deleg->ਆਖਰੀ_irq)
			ਆਖਰੀ_deleg_irq = deleg->ਆਖਰੀ_irq;
	}

	ਜੇ (aplic->targets_mmode ||
	    ((ਪਹਿਲਾ_deleg_irq < ਆਖਰੀ_deleg_irq) &&
	    (ਆਖਰੀ_deleg_irq == aplic->ਅੰਕ_ਸਰੋਤ) &&
	    (ਪਹਿਲਾ_deleg_irq == 1))) {
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(aplic->addr, aplic->ਮਾਪ, ਪੰਨਾ_ਮਾਪ,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	ਜੇ ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.ਅੰਕ_hwirq = aplic->ਅੰਕ_ਸਰੋਤ + 1;

		ਜੇ (msi_ਬਹੁਲਕ) {
			aplic->irqchip.warm_init      = ਨਿਰਾ;
			aplic->irqchip.process_hwirqs = ਨਿਰਾ;
			aplic->irqchip.hwirq_eoi      = ਨਿਰਾ;
		}

		ਜੇ (msi_ਬਹੁਲਕ)
			sbi_hartmask_ਸੈੱਟ_all(&aplic->irqchip.ਨਿਸ਼ਾਨਾ_harts);
		ਨਹੀਂਤਾਂ
			ਲਈ (i = 0; i < aplic->ਅੰਕ_idc; i++)
				sbi_hartmask_ਸੈੱਟ_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.ਨਿਸ਼ਾਨਾ_harts);

		rc = sbi_irqchip_ਜੋੜੋ_device(&aplic->irqchip);
		ਜੇ (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)aplic->irqchip.id,
				msi_ਬਹੁਲਕ ? "msi" : "direct",
				(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)sbi_hartmask_ਭਾਰ(
					&aplic->irqchip.ਨਿਸ਼ਾਨਾ_harts));
			ਵਾਪਸ rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_ਜੋੜੋ_ਪੂਛ(&aplic->ਗੰਢ, &aplic_list);

	ਵਾਪਸ 0;
}
