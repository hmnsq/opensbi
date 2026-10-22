/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/aplic.h>

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
#define APLIC_SOURCECFG_SM_LEVEL_മുകളിലെ	0x6
#define APLIC_SOURCECFG_SM_LEVEL_താഴെയുള്ള	0x7

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

ഘടന aplic_msi_data {
	ഘടന aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

സ്ഥിതം SBI_LIST_HEAD(aplic_list);
സ്ഥിതം ശൂന്യം aplic_writel_msicfg(ഘടന aplic_msicfg_data *msicfg,
				ശൂന്യം *msicfgaddr, ശൂന്യം *msicfgaddrH);

സ്ഥിതം വരിയിൽ bool aplic_is_direct_mode(ഘടന aplic_data *aplic)
{
	തിരികെ aplic && aplic->അക്കം_idc;
}

സ്ഥിതം വരിയിൽ ശൂന്യം aplic_sourcecfg_write(ഘടന aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (ശൂന്യം *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * വലുപ്പം(u32)));
}

സ്ഥിതം വരിയിൽ u32 aplic_sourcecfg_read(ഘടന aplic_data *aplic, u32 hwirq)
{
	തിരികെ readl((ശൂന്യം *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * വലുപ്പം(u32)));
}

സ്ഥിതം വരിയിൽ ശൂന്യം aplic_target_write(ഘടന aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (ശൂന്യം *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * വലുപ്പം(u32)));
}

സ്ഥിതം വരിയിൽ ശൂന്യം aplic_irq_setie(ഘടന aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ശൂന്യം *)(aplic->addr + APLIC_SETIENUM));
}

സ്ഥിതം വരിയിൽ ശൂന്യം aplic_irq_clrie(ഘടന aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ശൂന്യം *)(aplic->addr + APLIC_CLRIENUM));
}

സ്ഥിതം വരിയിൽ ശൂന്യം aplic_irq_clrip(ഘടന aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ശൂന്യം *)(aplic->addr + APLIC_CLRIPNUM));
}

സ്ഥിതം വരിയിൽ ശൂന്യം aplic_msi_irq_retrigger_level(ഘടന aplic_data *aplic, u32 hwirq)
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
	writel(hwirq, (ശൂന്യം *)(aplic->addr + APLIC_SETIPNUM_LE));
}

സ്ഥിതം വരിയിൽ ശൂന്യം *aplic_idc_base(ഘടന aplic_data *aplic, u32 idc_index)
{
	തിരികെ (ശൂന്യം *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

സ്ഥിതം വരിയിൽ u32 aplic_idc_read(ഘടന aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	തിരികെ readl(aplic_idc_base(aplic, idc_index) + reg);
}

സ്ഥിതം വരിയിൽ ശൂന്യം aplic_idc_write(ഘടന aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

സ്ഥിതം u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	തിരഞ്ഞെടുപ്പ് (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	അവസ്ഥ SBI_HWIRQ_FLAGS_EDGE_RISING:
		തിരികെ APLIC_SOURCECFG_SM_EDGE_RISE;
	അവസ്ഥ SBI_HWIRQ_FLAGS_EDGE_FALLING:
		തിരികെ APLIC_SOURCECFG_SM_EDGE_FALL;
	അവസ്ഥ SBI_HWIRQ_FLAGS_LEVEL_മുകളിലെ:
		തിരികെ APLIC_SOURCECFG_SM_LEVEL_മുകളിലെ;
	അവസ്ഥ SBI_HWIRQ_FLAGS_LEVEL_താഴെയുള്ള:
		തിരികെ APLIC_SOURCECFG_SM_LEVEL_താഴെയുള്ള;
	സ്ഥിരസ്ഥിതി:
		തിരികെ APLIC_SOURCECFG_SM_INACTIVE;
	}
}

സ്ഥിതം bool aplic_hwirq_is_delegated(ഘടന aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	തിരികെ !!(sourcecfg & APLIC_SOURCECFG_D);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ aplic_find_idc_index(ഘടന aplic_data *aplic, u32 hart_index)
{
	u32 i;

	വേണ്ടി (i = 0; i < aplic->അക്കം_idc; i++) {
		എങ്കിൽ (aplic->idc_map[i] == hart_index)
			തിരികെ i;
	}

	തിരികെ SBI_ENOENT;
}

സ്ഥിതം ശൂന്യം aplic_init(ഘടന aplic_data *aplic)
{
	ഘടന aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	പൂർണ്ണസംഖ്യ locked;

	/* Set domain configuration to 0 */
	writel(0, (ശൂന്യം *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	വേണ്ടി (i = 0; i <= aplic->അക്കം_source; i += 32)
		writel(-1U, (ശൂന്യം *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * വലുപ്പം(u32)));

	/* Set interrupt type and priority for all interrupts */
	വേണ്ടി (i = 1; i <= aplic->അക്കം_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (ശൂന്യം *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * വലുപ്പം(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (ശൂന്യം *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * വലുപ്പം(u32)));
	}

	/* Configure IRQ delegation */
	വേണ്ടി (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		എങ്കിൽ (!deleg->first_irq || !deleg->last_irq)
			തുടരുക;
		എങ്കിൽ (aplic->അക്കം_source < deleg->first_irq ||
		    aplic->അക്കം_source < deleg->last_irq)
			തുടരുക;
		എങ്കിൽ (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			തുടരുക;
		എങ്കിൽ (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		വേണ്ടി (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	എങ്കിൽ (aplic_is_direct_mode(aplic)) {
		വേണ്ടി (i = 0; i < aplic->അക്കം_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} അല്ലെങ്കിൽ {
		/* MSI configuration */
		locked = readl((ശൂന്യം *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		എങ്കിൽ (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(ശൂന്യം *)(aplic->addr + APLIC_MMSICFGADDR),
						(ശൂന്യം *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		എങ്കിൽ (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(ശൂന്യം *)(aplic->addr + APLIC_SMSICFGADDR),
						(ശൂന്യം *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	എങ്കിൽ (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (ശൂന്യം *)(aplic->addr + APLIC_DOMAINCFG));
}

ശൂന്യം aplic_reinit_all(ശൂന്യം)
{
	ഘടന aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, കണ്ണി)
		aplic_init(aplic);
}

സ്ഥിതം ശൂന്യം aplic_writel_msicfg(ഘടന aplic_msicfg_data *msicfg,
				ശൂന്യം *msicfgaddr, ശൂന്യം *msicfgaddrH)
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

സ്ഥിതം പൂർണ്ണസംഖ്യ aplic_check_msicfg(ഘടന aplic_msicfg_data *msicfg)
{
	എങ്കിൽ (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		തിരികെ SBI_EINVAL;

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ aplic_warm_init(ഘടന sbi_irqchip_device *chip)
{
	ഘടന aplic_data *aplic;
	u32 hart_index;
	പൂർണ്ണസംഖ്യ idc_index;

	aplic = container_of(chip, ഘടന aplic_data, irqchip);
	എങ്കിൽ (!aplic_is_direct_mode(aplic))
		തിരികെ 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	എങ്കിൽ (idc_index < 0)
		തിരികെ 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ aplic_process_hwirqs(ഘടന sbi_irqchip_device *chip)
{
	ഘടന aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	പൂർണ്ണസംഖ്യ idc_index, rc = 0, tmp;

	aplic = container_of(chip, ഘടന aplic_data, irqchip);

	എങ്കിൽ (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	എങ്കിൽ (idc_index < 0)
		തിരികെ 0;

	ആകുമ്പോൾ ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		എങ്കിൽ (!hwirq)
			നിർത്തുക;

		എങ്കിൽ (aplic_hwirq_is_delegated(aplic, hwirq))
			തുടരുക;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		എങ്കിൽ (tmp)
			rc = tmp;
	}

	തിരികെ rc;
}

സ്ഥിതം ശൂന്യം aplic_hwirq_eoi(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, ഘടന aplic_data, irqchip);
	എങ്കിൽ (aplic_is_direct_mode(aplic))
		തിരികെ;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	എങ്കിൽ (sm == APLIC_SOURCECFG_SM_LEVEL_മുകളിലെ ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_താഴെയുള്ള)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

സ്ഥിതം ശൂന്യം aplic_program_msi_target(ഘടന aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

സ്ഥിതം u32 derive_hart_index(ഘടന aplic_msicfg_data *msicfg,
			      സ്ഥിരം ഘടന sbi_irqchip_msi_സന്ദേശം *സന്ദേശം)
{
	u64 addr = ((u64)സന്ദേശം->address_hi << 32) | സന്ദേശം->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	തിരികെ hart_index;
}

സ്ഥിതം ശൂന്യം aplic_write_msi(u32 parent_hwirq,
			    സ്ഥിരം ഘടന sbi_irqchip_msi_സന്ദേശം *സന്ദേശം,
			    ശൂന്യം *priv)
{
	ഘടന aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = സന്ദേശം->data;
	എങ്കിൽ (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		തിരികെ;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, സന്ദേശം),
				guest_index, eiid);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ aplic_msi_callback(u32 parent_hwirq, ശൂന്യം *priv)
{
	ഘടന aplic_msi_data *msi_data = priv;
	പൂർണ്ണസംഖ്യ rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	എങ്കിൽ (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ aplic_setup_msi(ഘടന aplic_data *aplic, u32 hwirq)
{
	ഘടന sbi_irqchip_device *parent;
	u32 first_hwirq;
	ഘടന aplic_msi_data *msi_data;
	പൂർണ്ണസംഖ്യ rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	എങ്കിൽ (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		തിരികെ SBI_EINVAL;
	}

	msi_data = sbi_zalloc(വലുപ്പം(ഘടന aplic_msi_data));
	എങ്കിൽ (!msi_data)
		തിരികെ SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	എങ്കിൽ (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		തിരികെ rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ aplic_hwirq_setup(ഘടന sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	ഘടന aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, ഘടന aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	എങ്കിൽ (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		തിരികെ SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	എങ്കിൽ (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		തിരികെ SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	എങ്കിൽ (!aplic_is_direct_mode(aplic))
		തിരികെ aplic_setup_msi(aplic, hwirq);

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം aplic_hwirq_cleanup(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന aplic_data *aplic;
	ഘടന aplic_msi_data *msi_data;

	aplic = container_of(chip, ഘടന aplic_data, irqchip);
	എങ്കിൽ (aplic_hwirq_is_delegated(aplic, hwirq))
		തിരികെ;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (ഘടന aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	എങ്കിൽ (msi_data)
		sbi_free(msi_data);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ aplic_hwirq_set_affinity(ഘടന sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	ഘടന aplic_data *aplic;
	ഘടന aplic_msi_data *msi_data;
	ഘടന sbi_irqchip_device *parent;
	പൂർണ്ണസംഖ്യ idc_index;
	പൂർണ്ണസംഖ്യ rc;

	aplic = container_of(chip, ഘടന aplic_data, irqchip);
	എങ്കിൽ (aplic_hwirq_is_delegated(aplic, hwirq))
		തിരികെ SBI_ENOTSUPP;

	എങ്കിൽ (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		എങ്കിൽ (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			തിരികെ SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		എങ്കിൽ (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			തിരികെ rc;
		}
		തിരികെ 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	എങ്കിൽ (idc_index < 0)
		തിരികെ SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം aplic_hwirq_mask(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന aplic_data *aplic;

	aplic = container_of(chip, ഘടന aplic_data, irqchip);
	എങ്കിൽ (aplic_hwirq_is_delegated(aplic, hwirq))
		തിരികെ;

	aplic_irq_clrie(aplic, hwirq);
}

സ്ഥിതം ശൂന്യം aplic_hwirq_unmask(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന aplic_data *aplic;

	aplic = container_of(chip, ഘടന aplic_data, irqchip);
	എങ്കിൽ (aplic_hwirq_is_delegated(aplic, hwirq))
		തിരികെ;

	aplic_irq_setie(aplic, hwirq);
}

സ്ഥിതം ഘടന sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

പൂർണ്ണസംഖ്യ aplic_cold_irqchip_init(ഘടന aplic_data *aplic)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	എങ്കിൽ (!aplic->അക്കം_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		തിരികെ SBI_EINVAL;
	}

	എങ്കിൽ (APLIC_MAX_SOURCE <= aplic->അക്കം_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->അക്കം_source,
			(unsigned long)APLIC_MAX_SOURCE);
		തിരികെ SBI_EINVAL;
	}

	എങ്കിൽ (!msi_mode && APLIC_MAX_IDC <= aplic->അക്കം_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->അക്കം_idc,
			(unsigned long)APLIC_MAX_IDC);
		തിരികെ SBI_EINVAL;
	}

	എങ്കിൽ (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		എങ്കിൽ (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			തിരികെ rc;
		}
	}

	എങ്കിൽ (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		എങ്കിൽ (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			തിരികെ rc;
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
	വേണ്ടി (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		എങ്കിൽ (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		എങ്കിൽ (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	എങ്കിൽ (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->അക്കം_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_വേര്_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	എങ്കിൽ ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.അക്കം_hwirq = aplic->അക്കം_source + 1;

		എങ്കിൽ (msi_mode) {
			aplic->irqchip.warm_init      = ശൂന്യ;
			aplic->irqchip.process_hwirqs = ശൂന്യ;
			aplic->irqchip.hwirq_eoi      = ശൂന്യ;
		}

		എങ്കിൽ (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		അല്ലെങ്കിൽ
			വേണ്ടി (i = 0; i < aplic->അക്കം_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		എങ്കിൽ (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			തിരികെ rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->കണ്ണി, &aplic_list);

	തിരികെ 0;
}
