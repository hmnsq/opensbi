/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/irqchip/aplic.h>

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
#define APLIC_SOURCECFG_SM_LEVEL_HIGH	0x6
#define APLIC_SOURCECFG_SM_LEVEL_LOW	0x7

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

sa_nracanaa aplic_msi_data {
	sa_nracanaa aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

static SBI_LIST_HEAD(aplic_list);
static void aplic_writel_msicfg(sa_nracanaa aplic_msicfg_data *msicfg,
				void *msicfgaddr, void *msicfgaddrH);

static anta_hstha bool aplic_is_direct_mode(sa_nracanaa aplic_data *aplic)
{
	ᤀᤡᤴ aplic && aplic->num_idc;
}

static anta_hstha void aplic_sourcecfg_write(sa_nracanaa aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (void *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * parimaa_na(u32)));
}

static anta_hstha u32 aplic_sourcecfg_read(sa_nracanaa aplic_data *aplic, u32 hwirq)
{
	ᤀᤡᤴ readl((void *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * parimaa_na(u32)));
}

static anta_hstha void aplic_target_write(sa_nracanaa aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (void *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * parimaa_na(u32)));
}

static anta_hstha void aplic_irq_setie(sa_nracanaa aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (void *)(aplic->addr + APLIC_SETIENUM));
}

static anta_hstha void aplic_irq_clrie(sa_nracanaa aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (void *)(aplic->addr + APLIC_CLRIENUM));
}

static anta_hstha void aplic_irq_clrip(sa_nracanaa aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (void *)(aplic->addr + APLIC_CLRIPNUM));
}

static anta_hstha void aplic_msi_irq_retrigger_level(sa_nracanaa aplic_data *aplic, u32 hwirq)
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
	writel(hwirq, (void *)(aplic->addr + APLIC_SETIPNUM_LE));
}

static anta_hstha void *aplic_idc_base(sa_nracanaa aplic_data *aplic, u32 idc_index)
{
	ᤀᤡᤴ (void *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

static anta_hstha u32 aplic_idc_read(sa_nracanaa aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	ᤀᤡᤴ readl(aplic_idc_base(aplic, idc_index) + reg);
}

static anta_hstha void aplic_idc_write(sa_nracanaa aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

static u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	switch (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	pak_sa SBI_HWIRQ_FLAGS_EDGE_RISING:
		ᤀᤡᤴ APLIC_SOURCECFG_SM_EDGE_RISE;
	pak_sa SBI_HWIRQ_FLAGS_EDGE_FALLING:
		ᤀᤡᤴ APLIC_SOURCECFG_SM_EDGE_FALL;
	pak_sa SBI_HWIRQ_FLAGS_LEVEL_HIGH:
		ᤀᤡᤴ APLIC_SOURCECFG_SM_LEVEL_HIGH;
	pak_sa SBI_HWIRQ_FLAGS_LEVEL_LOW:
		ᤀᤡᤴ APLIC_SOURCECFG_SM_LEVEL_LOW;
	default:
		ᤀᤡᤴ APLIC_SOURCECFG_SM_INACTIVE;
	}
}

static bool aplic_hwirq_is_delegated(sa_nracanaa aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	ᤀᤡᤴ !!(sourcecfg & APLIC_SOURCECFG_D);
}

static puur_naa_nka aplic_find_idc_index(sa_nracanaa aplic_data *aplic, u32 hart_index)
{
	u32 i;

	for (i = 0; i < aplic->num_idc; i++) {
		ᤀᤡ᤺ (aplic->idc_map[i] == hart_index)
			ᤀᤡᤴ i;
	}

	ᤀᤡᤴ SBI_ENOENT;
}

static void aplic_init(sa_nracanaa aplic_data *aplic)
{
	sa_nracanaa aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	puur_naa_nka locked;

	/* Set domain configuration to 0 */
	writel(0, (void *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	for (i = 0; i <= aplic->num_source; i += 32)
		writel(-1U, (void *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * parimaa_na(u32)));

	/* Set interrupt type and priority for all interrupts */
	for (i = 1; i <= aplic->num_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (void *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * parimaa_na(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (void *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * parimaa_na(u32)));
	}

	/* Configure IRQ delegation */
	for (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ᤀᤡ᤺ (!deleg->first_irq || !deleg->last_irq)
			continue;
		ᤀᤡ᤺ (aplic->num_source < deleg->first_irq ||
		    aplic->num_source < deleg->last_irq)
			continue;
		ᤀᤡ᤺ (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			continue;
		ᤀᤡ᤺ (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		for (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	ᤀᤡ᤺ (aplic_is_direct_mode(aplic)) {
		for (i = 0; i < aplic->num_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} ᤀᤡ᤺ᤆᤡ {
		/* MSI configuration */
		locked = readl((void *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		ᤀᤡ᤺ (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(void *)(aplic->addr + APLIC_MMSICFGADDR),
						(void *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		ᤀᤡ᤺ (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(void *)(aplic->addr + APLIC_SMSICFGADDR),
						(void *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	ᤀᤡ᤺ (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (void *)(aplic->addr + APLIC_DOMAINCFG));
}

void aplic_reinit_all(void)
{
	sa_nracanaa aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, node)
		aplic_init(aplic);
}

static void aplic_writel_msicfg(sa_nracanaa aplic_msicfg_data *msicfg,
				void *msicfgaddr, void *msicfgaddrH)
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

static puur_naa_nka aplic_check_msicfg(sa_nracanaa aplic_msicfg_data *msicfg)
{
	ᤀᤡ᤺ (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		ᤀᤡᤴ SBI_EINVAL;

	ᤀᤡ᤺ (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		ᤀᤡᤴ SBI_EINVAL;

	ᤀᤡ᤺ (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		ᤀᤡᤴ SBI_EINVAL;

	ᤀᤡ᤺ (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		ᤀᤡᤴ SBI_EINVAL;

	ᤀᤡᤴ 0;
}

static puur_naa_nka aplic_warm_init(sa_nracanaa sbi_irqchip_device *chip)
{
	sa_nracanaa aplic_data *aplic;
	u32 hart_index;
	puur_naa_nka idc_index;

	aplic = container_of(chip, sa_nracanaa aplic_data, irqchip);
	ᤀᤡ᤺ (!aplic_is_direct_mode(aplic))
		ᤀᤡᤴ 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	ᤀᤡ᤺ (idc_index < 0)
		ᤀᤡᤴ 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	ᤀᤡᤴ 0;
}

static puur_naa_nka aplic_process_hwirqs(sa_nracanaa sbi_irqchip_device *chip)
{
	sa_nracanaa aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	puur_naa_nka idc_index, rc = 0, tmp;

	aplic = container_of(chip, sa_nracanaa aplic_data, irqchip);

	ᤀᤡ᤺ (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	ᤀᤡ᤺ (idc_index < 0)
		ᤀᤡᤴ 0;

	ᤀᤠ᤺ ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		ᤀᤡ᤺ (!hwirq)
			bha_nga;

		ᤀᤡ᤺ (aplic_hwirq_is_delegated(aplic, hwirq))
			continue;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		ᤀᤡ᤺ (tmp)
			rc = tmp;
	}

	ᤀᤡᤴ rc;
}

static void aplic_hwirq_eoi(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, sa_nracanaa aplic_data, irqchip);
	ᤀᤡ᤺ (aplic_is_direct_mode(aplic))
		ᤀᤡᤴ;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	ᤀᤡ᤺ (sm == APLIC_SOURCECFG_SM_LEVEL_HIGH ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_LOW)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

static void aplic_program_msi_target(sa_nracanaa aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

static u32 derive_hart_index(sa_nracanaa aplic_msicfg_data *msicfg,
			      const sa_nracanaa sbi_irqchip_msi_msg *msg)
{
	u64 addr = ((u64)msg->address_hi << 32) | msg->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	ᤀᤡᤴ hart_index;
}

static void aplic_write_msi(u32 parent_hwirq,
			    const sa_nracanaa sbi_irqchip_msi_msg *msg,
			    void *priv)
{
	sa_nracanaa aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = msg->data;
	ᤀᤡ᤺ (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		ᤀᤡᤴ;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, msg),
				guest_index, eiid);
}

static puur_naa_nka aplic_msi_callback(u32 parent_hwirq, void *priv)
{
	sa_nracanaa aplic_msi_data *msi_data = priv;
	puur_naa_nka rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	ᤀᤡ᤺ (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	ᤀᤡᤴ rc;
}

static puur_naa_nka aplic_setup_msi(sa_nracanaa aplic_data *aplic, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_device *parent;
	u32 first_hwirq;
	sa_nracanaa aplic_msi_data *msi_data;
	puur_naa_nka rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	ᤀᤡ᤺ (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		ᤀᤡᤴ SBI_EINVAL;
	}

	msi_data = sbi_zalloc(parimaa_na(sa_nracanaa aplic_msi_data));
	ᤀᤡ᤺ (!msi_data)
		ᤀᤡᤴ SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	ᤀᤡ᤺ (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		ᤀᤡᤴ rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	ᤀᤡᤴ 0;
}

static puur_naa_nka aplic_hwirq_setup(sa_nracanaa sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	sa_nracanaa aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, sa_nracanaa aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	ᤀᤡ᤺ (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		ᤀᤡᤴ SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	ᤀᤡ᤺ (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		ᤀᤡᤴ SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	ᤀᤡ᤺ (!aplic_is_direct_mode(aplic))
		ᤀᤡᤴ aplic_setup_msi(aplic, hwirq);

	ᤀᤡᤴ 0;
}

static void aplic_hwirq_cleanup(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa aplic_data *aplic;
	sa_nracanaa aplic_msi_data *msi_data;

	aplic = container_of(chip, sa_nracanaa aplic_data, irqchip);
	ᤀᤡ᤺ (aplic_hwirq_is_delegated(aplic, hwirq))
		ᤀᤡᤴ;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (sa_nracanaa aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	ᤀᤡ᤺ (msi_data)
		sbi_free(msi_data);
}

static puur_naa_nka aplic_hwirq_set_affinity(sa_nracanaa sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	sa_nracanaa aplic_data *aplic;
	sa_nracanaa aplic_msi_data *msi_data;
	sa_nracanaa sbi_irqchip_device *parent;
	puur_naa_nka idc_index;
	puur_naa_nka rc;

	aplic = container_of(chip, sa_nracanaa aplic_data, irqchip);
	ᤀᤡ᤺ (aplic_hwirq_is_delegated(aplic, hwirq))
		ᤀᤡᤴ SBI_ENOTSUPP;

	ᤀᤡ᤺ (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		ᤀᤡ᤺ (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			ᤀᤡᤴ SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		ᤀᤡ᤺ (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			ᤀᤡᤴ rc;
		}
		ᤀᤡᤴ 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	ᤀᤡ᤺ (idc_index < 0)
		ᤀᤡᤴ SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	ᤀᤡᤴ 0;
}

static void aplic_hwirq_mask(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa aplic_data *aplic;

	aplic = container_of(chip, sa_nracanaa aplic_data, irqchip);
	ᤀᤡ᤺ (aplic_hwirq_is_delegated(aplic, hwirq))
		ᤀᤡᤴ;

	aplic_irq_clrie(aplic, hwirq);
}

static void aplic_hwirq_unmask(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa aplic_data *aplic;

	aplic = container_of(chip, sa_nracanaa aplic_data, irqchip);
	ᤀᤡ᤺ (aplic_hwirq_is_delegated(aplic, hwirq))
		ᤀᤡᤴ;

	aplic_irq_setie(aplic, hwirq);
}

static sa_nracanaa sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

puur_naa_nka aplic_cold_irqchip_init(sa_nracanaa aplic_data *aplic)
{
	puur_naa_nka rc;
	sa_nracanaa aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	ᤀᤡ᤺ (!aplic->num_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		ᤀᤡᤴ SBI_EINVAL;
	}

	ᤀᤡ᤺ (APLIC_MAX_SOURCE <= aplic->num_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->num_source,
			(unsigned long)APLIC_MAX_SOURCE);
		ᤀᤡᤴ SBI_EINVAL;
	}

	ᤀᤡ᤺ (!msi_mode && APLIC_MAX_IDC <= aplic->num_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->num_idc,
			(unsigned long)APLIC_MAX_IDC);
		ᤀᤡᤴ SBI_EINVAL;
	}

	ᤀᤡ᤺ (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		ᤀᤡ᤺ (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			ᤀᤡᤴ rc;
		}
	}

	ᤀᤡ᤺ (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		ᤀᤡ᤺ (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			ᤀᤡᤴ rc;
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
	for (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ᤀᤡ᤺ (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		ᤀᤡ᤺ (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	ᤀᤡ᤺ (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->num_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_root_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ᤀᤡ᤺ (rc)
			ᤀᤡᤴ rc;
	}

	ᤀᤡ᤺ ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.num_hwirq = aplic->num_source + 1;

		ᤀᤡ᤺ (msi_mode) {
			aplic->irqchip.warm_init      = NULL;
			aplic->irqchip.process_hwirqs = NULL;
			aplic->irqchip.hwirq_eoi      = NULL;
		}

		ᤀᤡ᤺ (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		ᤀᤡ᤺ᤆᤡ
			for (i = 0; i < aplic->num_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		ᤀᤡ᤺ (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			ᤀᤡᤴ rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->node, &aplic_list);

	ᤀᤡᤴ 0;
}
