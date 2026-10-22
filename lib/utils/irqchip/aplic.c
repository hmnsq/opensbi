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

struct aplic_msi_data {
	struct aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

static SBI_LIST_HEAD(aplic_list);
static void aplic_writel_msicfg(struct aplic_msicfg_data *msicfg,
				void *msicfgaddr, void *msicfgaddrH);

static inline bool aplic_is_direct_mode(struct aplic_data *aplic)
{
	върни aplic && aplic->num_idc;
}

static inline void aplic_sourcecfg_write(struct aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (void *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * sizeof(u32)));
}

static inline u32 aplic_sourcecfg_read(struct aplic_data *aplic, u32 hwirq)
{
	върни readl((void *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * sizeof(u32)));
}

static inline void aplic_target_write(struct aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (void *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * sizeof(u32)));
}

static inline void aplic_irq_setie(struct aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (void *)(aplic->addr + APLIC_SETIENUM));
}

static inline void aplic_irq_clrie(struct aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (void *)(aplic->addr + APLIC_CLRIENUM));
}

static inline void aplic_irq_clrip(struct aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (void *)(aplic->addr + APLIC_CLRIPNUM));
}

static inline void aplic_msi_irq_retrigger_level(struct aplic_data *aplic, u32 hwirq)
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

static inline void *aplic_idc_base(struct aplic_data *aplic, u32 idc_index)
{
	върни (void *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

static inline u32 aplic_idc_read(struct aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	върни readl(aplic_idc_base(aplic, idc_index) + reg);
}

static inline void aplic_idc_write(struct aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

static u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	switch (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	case SBI_HWIRQ_FLAGS_EDGE_RISING:
		върни APLIC_SOURCECFG_SM_EDGE_RISE;
	case SBI_HWIRQ_FLAGS_EDGE_FALLING:
		върни APLIC_SOURCECFG_SM_EDGE_FALL;
	case SBI_HWIRQ_FLAGS_LEVEL_HIGH:
		върни APLIC_SOURCECFG_SM_LEVEL_HIGH;
	case SBI_HWIRQ_FLAGS_LEVEL_LOW:
		върни APLIC_SOURCECFG_SM_LEVEL_LOW;
	default:
		върни APLIC_SOURCECFG_SM_INACTIVE;
	}
}

static bool aplic_hwirq_is_delegated(struct aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	върни !!(sourcecfg & APLIC_SOURCECFG_D);
}

static цяло aplic_find_idc_index(struct aplic_data *aplic, u32 hart_index)
{
	u32 i;

	за (i = 0; i < aplic->num_idc; i++) {
		ако (aplic->idc_map[i] == hart_index)
			върни i;
	}

	върни SBI_ENOENT;
}

static void aplic_init(struct aplic_data *aplic)
{
	struct aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	цяло locked;

	/* Set domain configuration to 0 */
	writel(0, (void *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	за (i = 0; i <= aplic->num_source; i += 32)
		writel(-1U, (void *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * sizeof(u32)));

	/* Set interrupt type and priority for all interrupts */
	за (i = 1; i <= aplic->num_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (void *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * sizeof(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (void *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * sizeof(u32)));
	}

	/* Configure IRQ delegation */
	за (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ако (!deleg->first_irq || !deleg->last_irq)
			continue;
		ако (aplic->num_source < deleg->first_irq ||
		    aplic->num_source < deleg->last_irq)
			continue;
		ако (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			continue;
		ако (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		за (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	ако (aplic_is_direct_mode(aplic)) {
		за (i = 0; i < aplic->num_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} иначе {
		/* MSI configuration */
		locked = readl((void *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		ако (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(void *)(aplic->addr + APLIC_MMSICFGADDR),
						(void *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		ако (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(void *)(aplic->addr + APLIC_SMSICFGADDR),
						(void *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	ако (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (void *)(aplic->addr + APLIC_DOMAINCFG));
}

void aplic_reinit_all(void)
{
	struct aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, node)
		aplic_init(aplic);
}

static void aplic_writel_msicfg(struct aplic_msicfg_data *msicfg,
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

static цяло aplic_check_msicfg(struct aplic_msicfg_data *msicfg)
{
	ако (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		върни SBI_EINVAL;

	ако (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		върни SBI_EINVAL;

	ако (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		върни SBI_EINVAL;

	ако (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		върни SBI_EINVAL;

	върни 0;
}

static цяло aplic_warm_init(struct sbi_irqchip_device *chip)
{
	struct aplic_data *aplic;
	u32 hart_index;
	цяло idc_index;

	aplic = container_of(chip, struct aplic_data, irqchip);
	ако (!aplic_is_direct_mode(aplic))
		върни 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	ако (idc_index < 0)
		върни 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	върни 0;
}

static цяло aplic_process_hwirqs(struct sbi_irqchip_device *chip)
{
	struct aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	цяло idc_index, rc = 0, tmp;

	aplic = container_of(chip, struct aplic_data, irqchip);

	ако (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	ако (idc_index < 0)
		върни 0;

	докато ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		ако (!hwirq)
			break;

		ако (aplic_hwirq_is_delegated(aplic, hwirq))
			continue;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		ако (tmp)
			rc = tmp;
	}

	върни rc;
}

static void aplic_hwirq_eoi(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, struct aplic_data, irqchip);
	ако (aplic_is_direct_mode(aplic))
		върни;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	ако (sm == APLIC_SOURCECFG_SM_LEVEL_HIGH ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_LOW)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

static void aplic_program_msi_target(struct aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

static u32 derive_hart_index(struct aplic_msicfg_data *msicfg,
			      const struct sbi_irqchip_msi_msg *msg)
{
	u64 addr = ((u64)msg->address_hi << 32) | msg->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	върни hart_index;
}

static void aplic_write_msi(u32 parent_hwirq,
			    const struct sbi_irqchip_msi_msg *msg,
			    void *priv)
{
	struct aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = msg->data;
	ако (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		върни;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, msg),
				guest_index, eiid);
}

static цяло aplic_msi_callback(u32 parent_hwirq, void *priv)
{
	struct aplic_msi_data *msi_data = priv;
	цяло rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	ако (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	върни rc;
}

static цяло aplic_setup_msi(struct aplic_data *aplic, u32 hwirq)
{
	struct sbi_irqchip_device *parent;
	u32 first_hwirq;
	struct aplic_msi_data *msi_data;
	цяло rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	ако (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		върни SBI_EINVAL;
	}

	msi_data = sbi_zalloc(sizeof(struct aplic_msi_data));
	ако (!msi_data)
		върни SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	ако (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		върни rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	върни 0;
}

static цяло aplic_hwirq_setup(struct sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	struct aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, struct aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	ако (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		върни SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	ако (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		върни SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	ако (!aplic_is_direct_mode(aplic))
		върни aplic_setup_msi(aplic, hwirq);

	върни 0;
}

static void aplic_hwirq_cleanup(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct aplic_data *aplic;
	struct aplic_msi_data *msi_data;

	aplic = container_of(chip, struct aplic_data, irqchip);
	ако (aplic_hwirq_is_delegated(aplic, hwirq))
		върни;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (struct aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	ако (msi_data)
		sbi_free(msi_data);
}

static цяло aplic_hwirq_set_affinity(struct sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	struct aplic_data *aplic;
	struct aplic_msi_data *msi_data;
	struct sbi_irqchip_device *parent;
	цяло idc_index;
	цяло rc;

	aplic = container_of(chip, struct aplic_data, irqchip);
	ако (aplic_hwirq_is_delegated(aplic, hwirq))
		върни SBI_ENOTSUPP;

	ако (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		ако (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			върни SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		ако (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			върни rc;
		}
		върни 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	ако (idc_index < 0)
		върни SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	върни 0;
}

static void aplic_hwirq_mask(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct aplic_data *aplic;

	aplic = container_of(chip, struct aplic_data, irqchip);
	ако (aplic_hwirq_is_delegated(aplic, hwirq))
		върни;

	aplic_irq_clrie(aplic, hwirq);
}

static void aplic_hwirq_unmask(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct aplic_data *aplic;

	aplic = container_of(chip, struct aplic_data, irqchip);
	ако (aplic_hwirq_is_delegated(aplic, hwirq))
		върни;

	aplic_irq_setie(aplic, hwirq);
}

static struct sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

цяло aplic_cold_irqchip_init(struct aplic_data *aplic)
{
	цяло rc;
	struct aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	ако (!aplic->num_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		върни SBI_EINVAL;
	}

	ако (APLIC_MAX_SOURCE <= aplic->num_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->num_source,
			(unsigned long)APLIC_MAX_SOURCE);
		върни SBI_EINVAL;
	}

	ако (!msi_mode && APLIC_MAX_IDC <= aplic->num_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->num_idc,
			(unsigned long)APLIC_MAX_IDC);
		върни SBI_EINVAL;
	}

	ако (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		ако (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			върни rc;
		}
	}

	ако (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		ако (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			върни rc;
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
	за (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		ако (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		ако (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	ако (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->num_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_root_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		ако (rc)
			върни rc;
	}

	ако ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.num_hwirq = aplic->num_source + 1;

		ако (msi_mode) {
			aplic->irqchip.warm_init      = NULL;
			aplic->irqchip.process_hwirqs = NULL;
			aplic->irqchip.hwirq_eoi      = NULL;
		}

		ако (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		иначе
			за (i = 0; i < aplic->num_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		ако (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			върни rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->node, &aplic_list);

	върни 0;
}
