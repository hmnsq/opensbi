/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/aplic.h>

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
#define APLIC_SOURCECFG_SM_LEVEL_ଉପର	0x6
#define APLIC_SOURCECFG_SM_LEVEL_ତଳ	0x7

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

ସଂରଚନା aplic_msi_data {
	ସଂରଚନା aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

ସ୍ଥିର SBI_LIST_HEAD(aplic_list);
ସ୍ଥିର ଶୂନ୍ୟ aplic_writel_msicfg(ସଂରଚନା aplic_msicfg_data *msicfg,
				ଶୂନ୍ୟ *msicfgaddr, ଶୂନ୍ୟ *msicfgaddrH);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool aplic_is_direct_mode(ସଂରଚନା aplic_data *aplic)
{
	ଫେରନ୍ତୁ aplic && aplic->ଅଙ୍କ_idc;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ aplic_sourcecfg_write(ସଂରଚନା aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * ଆକାର(u32)));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32 aplic_sourcecfg_read(ସଂରଚନା aplic_data *aplic, u32 hwirq)
{
	ଫେରନ୍ତୁ readl((ଶୂନ୍ୟ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * ଆକାର(u32)));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ aplic_target_write(ସଂରଚନା aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * ଆକାର(u32)));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ aplic_irq_setie(ସଂରଚନା aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_SETIENUM));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ aplic_irq_clrie(ସଂରଚନା aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_CLRIENUM));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ aplic_irq_clrip(ସଂରଚନା aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_CLRIPNUM));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ aplic_msi_irq_retrigger_level(ସଂରଚନା aplic_data *aplic, u32 hwirq)
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
	writel(hwirq, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_SETIPNUM_LE));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *aplic_idc_base(ସଂରଚନା aplic_data *aplic, u32 idc_index)
{
	ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32 aplic_idc_read(ସଂରଚନା aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	ଫେରନ୍ତୁ readl(aplic_idc_base(aplic, idc_index) + reg);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ aplic_idc_write(ସଂରଚନା aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

ସ୍ଥିର u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	ବାଛ (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	ସ୍ଥିତି SBI_HWIRQ_FLAGS_EDGE_RISING:
		ଫେରନ୍ତୁ APLIC_SOURCECFG_SM_EDGE_RISE;
	ସ୍ଥିତି SBI_HWIRQ_FLAGS_EDGE_FALLING:
		ଫେରନ୍ତୁ APLIC_SOURCECFG_SM_EDGE_FALL;
	ସ୍ଥିତି SBI_HWIRQ_FLAGS_LEVEL_ଉପର:
		ଫେରନ୍ତୁ APLIC_SOURCECFG_SM_LEVEL_ଉପର;
	ସ୍ଥିତି SBI_HWIRQ_FLAGS_LEVEL_ତଳ:
		ଫେରନ୍ତୁ APLIC_SOURCECFG_SM_LEVEL_ତଳ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ସ୍ଥିର bool aplic_hwirq_is_delegated(ସଂରଚନା aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	ଫେରନ୍ତୁ !!(sourcecfg & APLIC_SOURCECFG_D);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_find_idc_index(ସଂରଚନା aplic_data *aplic, u32 hart_index)
{
	u32 i;

	ପାଇଁ (i = 0; i < aplic->ଅଙ୍କ_idc; i++) {
		ଯଦି (aplic->idc_map[i] == hart_index)
			ଫେରନ୍ତୁ i;
	}

	ଫେରନ୍ତୁ SBI_ENOENT;
}

ସ୍ଥିର ଶୂନ୍ୟ aplic_init(ସଂରଚନା aplic_data *aplic)
{
	ସଂରଚନା aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା locked;

	/* Set domain configuration to 0 */
	writel(0, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	ପାଇଁ (i = 0; i <= aplic->ଅଙ୍କ_source; i += 32)
		writel(-1U, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * ଆକାର(u32)));

	/* Set interrupt type and priority for all interrupts */
	ପାଇଁ (i = 1; i <= aplic->ଅଙ୍କ_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * ଆକାର(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (ଶୂନ୍ୟ *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * ଆକାର(u32)));
	}

	/* Configure IRQ delegation */
	ପାଇଁ (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ଯଦି (!deleg->first_irq || !deleg->last_irq)
			ଜାରି;
		ଯଦି (aplic->ଅଙ୍କ_source < deleg->first_irq ||
		    aplic->ଅଙ୍କ_source < deleg->last_irq)
			ଜାରି;
		ଯଦି (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			ଜାରି;
		ଯଦି (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		ପାଇଁ (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	ଯଦି (aplic_is_direct_mode(aplic)) {
		ପାଇଁ (i = 0; i < aplic->ଅଙ୍କ_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} ଅନ୍ୟଥା {
		/* MSI configuration */
		locked = readl((ଶୂନ୍ୟ *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		ଯଦି (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(ଶୂନ୍ୟ *)(aplic->addr + APLIC_MMSICFGADDR),
						(ଶୂନ୍ୟ *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		ଯଦି (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(ଶୂନ୍ୟ *)(aplic->addr + APLIC_SMSICFGADDR),
						(ଶୂନ୍ୟ *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	ଯଦି (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (ଶୂନ୍ୟ *)(aplic->addr + APLIC_DOMAINCFG));
}

ଶୂନ୍ୟ aplic_reinit_all(ଶୂନ୍ୟ)
{
	ସଂରଚନା aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, ଗଣ୍ଠି)
		aplic_init(aplic);
}

ସ୍ଥିର ଶୂନ୍ୟ aplic_writel_msicfg(ସଂରଚନା aplic_msicfg_data *msicfg,
				ଶୂନ୍ୟ *msicfgaddr, ଶୂନ୍ୟ *msicfgaddrH)
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_check_msicfg(ସଂରଚନା aplic_msicfg_data *msicfg)
{
	ଯଦି (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_warm_init(ସଂରଚନା sbi_irqchip_device *chip)
{
	ସଂରଚନା aplic_data *aplic;
	u32 hart_index;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idc_index;

	aplic = container_of(chip, ସଂରଚନା aplic_data, irqchip);
	ଯଦି (!aplic_is_direct_mode(aplic))
		ଫେରନ୍ତୁ 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	ଯଦି (idc_index < 0)
		ଫେରନ୍ତୁ 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_process_hwirqs(ସଂରଚନା sbi_irqchip_device *chip)
{
	ସଂରଚନା aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idc_index, rc = 0, tmp;

	aplic = container_of(chip, ସଂରଚନା aplic_data, irqchip);

	ଯଦି (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	ଯଦି (idc_index < 0)
		ଫେରନ୍ତୁ 0;

	ଯେତେବେଳେ ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		ଯଦି (!hwirq)
			ଭାଙ୍ଗ;

		ଯଦି (aplic_hwirq_is_delegated(aplic, hwirq))
			ଜାରି;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		ଯଦି (tmp)
			rc = tmp;
	}

	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଶୂନ୍ୟ aplic_hwirq_eoi(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, ସଂରଚନା aplic_data, irqchip);
	ଯଦି (aplic_is_direct_mode(aplic))
		ଫେରନ୍ତୁ;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	ଯଦି (sm == APLIC_SOURCECFG_SM_LEVEL_ଉପର ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_ତଳ)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ସ୍ଥିର ଶୂନ୍ୟ aplic_program_msi_target(ସଂରଚନା aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

ସ୍ଥିର u32 derive_hart_index(ସଂରଚନା aplic_msicfg_data *msicfg,
			      ଅଚଳ ସଂରଚନା sbi_irqchip_msi_ସନ୍ଦେଶ *ସନ୍ଦେଶ)
{
	u64 addr = ((u64)ସନ୍ଦେଶ->address_hi << 32) | ସନ୍ଦେଶ->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	ଫେରନ୍ତୁ hart_index;
}

ସ୍ଥିର ଶୂନ୍ୟ aplic_write_msi(u32 parent_hwirq,
			    ଅଚଳ ସଂରଚନା sbi_irqchip_msi_ସନ୍ଦେଶ *ସନ୍ଦେଶ,
			    ଶୂନ୍ୟ *priv)
{
	ସଂରଚନା aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = ସନ୍ଦେଶ->data;
	ଯଦି (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		ଫେରନ୍ତୁ;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, ସନ୍ଦେଶ),
				guest_index, eiid);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_msi_callback(u32 parent_hwirq, ଶୂନ୍ୟ *priv)
{
	ସଂରଚନା aplic_msi_data *msi_data = priv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	ଯଦି (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_setup_msi(ସଂରଚନା aplic_data *aplic, u32 hwirq)
{
	ସଂରଚନା sbi_irqchip_device *parent;
	u32 first_hwirq;
	ସଂରଚନା aplic_msi_data *msi_data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	ଯଦି (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	msi_data = sbi_zalloc(ଆକାର(ସଂରଚନା aplic_msi_data));
	ଯଦି (!msi_data)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	ଯଦି (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		ଫେରନ୍ତୁ rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_hwirq_setup(ସଂରଚନା sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	ସଂରଚନା aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, ସଂରଚନା aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	ଯଦି (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	ଯଦି (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		ଫେରନ୍ତୁ SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	ଯଦି (!aplic_is_direct_mode(aplic))
		ଫେରନ୍ତୁ aplic_setup_msi(aplic, hwirq);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ aplic_hwirq_cleanup(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା aplic_data *aplic;
	ସଂରଚନା aplic_msi_data *msi_data;

	aplic = container_of(chip, ସଂରଚନା aplic_data, irqchip);
	ଯଦି (aplic_hwirq_is_delegated(aplic, hwirq))
		ଫେରନ୍ତୁ;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (ସଂରଚନା aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	ଯଦି (msi_data)
		sbi_free(msi_data);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_hwirq_set_affinity(ସଂରଚନା sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	ସଂରଚନା aplic_data *aplic;
	ସଂରଚନା aplic_msi_data *msi_data;
	ସଂରଚନା sbi_irqchip_device *parent;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idc_index;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	aplic = container_of(chip, ସଂରଚନା aplic_data, irqchip);
	ଯଦି (aplic_hwirq_is_delegated(aplic, hwirq))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଯଦି (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		ଯଦି (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			ଫେରନ୍ତୁ SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		ଯଦି (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			ଫେରନ୍ତୁ rc;
		}
		ଫେରନ୍ତୁ 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	ଯଦି (idc_index < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ aplic_hwirq_mask(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା aplic_data *aplic;

	aplic = container_of(chip, ସଂରଚନା aplic_data, irqchip);
	ଯଦି (aplic_hwirq_is_delegated(aplic, hwirq))
		ଫେରନ୍ତୁ;

	aplic_irq_clrie(aplic, hwirq);
}

ସ୍ଥିର ଶୂନ୍ୟ aplic_hwirq_unmask(ସଂରଚନା sbi_irqchip_device *chip, u32 hwirq)
{
	ସଂରଚନା aplic_data *aplic;

	aplic = container_of(chip, ସଂରଚନା aplic_data, irqchip);
	ଯଦି (aplic_hwirq_is_delegated(aplic, hwirq))
		ଫେରନ୍ତୁ;

	aplic_irq_setie(aplic, hwirq);
}

ସ୍ଥିର ସଂରଚନା sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା aplic_cold_irqchip_init(ସଂରଚନା aplic_data *aplic)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	ଯଦି (!aplic->ଅଙ୍କ_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	ଯଦି (APLIC_MAX_SOURCE <= aplic->ଅଙ୍କ_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->ଅଙ୍କ_source,
			(unsigned long)APLIC_MAX_SOURCE);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	ଯଦି (!msi_mode && APLIC_MAX_IDC <= aplic->ଅଙ୍କ_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->ଅଙ୍କ_idc,
			(unsigned long)APLIC_MAX_IDC);
		ଫେରନ୍ତୁ SBI_EINVAL;
	}

	ଯଦି (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		ଯଦି (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			ଫେରନ୍ତୁ rc;
		}
	}

	ଯଦି (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		ଯଦି (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			ଫେରନ୍ତୁ rc;
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
	ପାଇଁ (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ଯଦି (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		ଯଦି (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	ଯଦି (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->ଅଙ୍କ_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_ମୂଳ_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	ଯଦି ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.ଅଙ୍କ_hwirq = aplic->ଅଙ୍କ_source + 1;

		ଯଦି (msi_mode) {
			aplic->irqchip.warm_init      = ଖାଲି;
			aplic->irqchip.process_hwirqs = ଖାଲି;
			aplic->irqchip.hwirq_eoi      = ଖାଲି;
		}

		ଯଦି (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		ଅନ୍ୟଥା
			ପାଇଁ (i = 0; i < aplic->ଅଙ୍କ_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		ଯଦି (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			ଫେରନ୍ତୁ rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->ଗଣ୍ଠି, &aplic_list);

	ଫେରନ୍ତୁ 0;
}
