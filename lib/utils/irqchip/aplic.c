/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/irqchip/aplic.h>

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
#define APLIC_SOURCECFG_SM_LEVEL_بالا	0x6
#define APLIC_SOURCECFG_SM_LEVEL_کم	0x7

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

ساختار aplic_msi_data {
	ساختار aplic_data *aplic;
	u32 hwirq;
	u32 parent_hwirq;
};

ایستا SBI_LIST_HEAD(aplic_list);
ایستا تهی aplic_writel_msicfg(ساختار aplic_msicfg_data *msicfg,
				تهی *msicfgaddr, تهی *msicfgaddrH);

ایستا درونخطی bool aplic_is_direct_mode(ساختار aplic_data *aplic)
{
	بازگشت aplic && aplic->رقم_idc;
}

ایستا درونخطی تهی aplic_sourcecfg_write(ساختار aplic_data *aplic,
					 u32 hwirq, u32 val)
{
	writel(val, (تهی *)(aplic->addr + APLIC_SOURCECFG_BASE +
			     (hwirq - 1) * اندازه(u32)));
}

ایستا درونخطی u32 aplic_sourcecfg_read(ساختار aplic_data *aplic, u32 hwirq)
{
	بازگشت readl((تهی *)(aplic->addr + APLIC_SOURCECFG_BASE +
			      (hwirq - 1) * اندازه(u32)));
}

ایستا درونخطی تهی aplic_target_write(ساختار aplic_data *aplic,
				      u32 hwirq, u32 val)
{
	writel(val, (تهی *)(aplic->addr + APLIC_TARGET_BASE +
			     (hwirq - 1) * اندازه(u32)));
}

ایستا درونخطی تهی aplic_irq_setie(ساختار aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (تهی *)(aplic->addr + APLIC_SETIENUM));
}

ایستا درونخطی تهی aplic_irq_clrie(ساختار aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (تهی *)(aplic->addr + APLIC_CLRIENUM));
}

ایستا درونخطی تهی aplic_irq_clrip(ساختار aplic_data *aplic, u32 hwirq)
{
	writel(hwirq, (تهی *)(aplic->addr + APLIC_CLRIPNUM));
}

ایستا درونخطی تهی aplic_msi_irq_retrigger_level(ساختار aplic_data *aplic, u32 hwirq)
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
	writel(hwirq, (تهی *)(aplic->addr + APLIC_SETIPNUM_LE));
}

ایستا درونخطی تهی *aplic_idc_base(ساختار aplic_data *aplic, u32 idc_index)
{
	بازگشت (تهی *)(aplic->addr + APLIC_IDC_BASE +
			idc_index * APLIC_IDC_SIZE);
}

ایستا درونخطی u32 aplic_idc_read(ساختار aplic_data *aplic, u32 idc_index,
				 u32 reg)
{
	بازگشت readl(aplic_idc_base(aplic, idc_index) + reg);
}

ایستا درونخطی تهی aplic_idc_write(ساختار aplic_data *aplic, u32 idc_index,
				   u32 reg, u32 val)
{
	writel(val, aplic_idc_base(aplic, idc_index) + reg);
}

ایستا u32 aplic_hwirq_flags_to_sourcecfg(u32 hwirq_flags)
{
	انتخاب (hwirq_flags & SBI_HWIRQ_FLAGS_LEVEL_SENSE_MASK) {
	حالت SBI_HWIRQ_FLAGS_EDGE_RISING:
		بازگشت APLIC_SOURCECFG_SM_EDGE_RISE;
	حالت SBI_HWIRQ_FLAGS_EDGE_FALLING:
		بازگشت APLIC_SOURCECFG_SM_EDGE_FALL;
	حالت SBI_HWIRQ_FLAGS_LEVEL_بالا:
		بازگشت APLIC_SOURCECFG_SM_LEVEL_بالا;
	حالت SBI_HWIRQ_FLAGS_LEVEL_کم:
		بازگشت APLIC_SOURCECFG_SM_LEVEL_کم;
	پیشفرض:
		بازگشت APLIC_SOURCECFG_SM_INACTIVE;
	}
}

ایستا bool aplic_hwirq_is_delegated(ساختار aplic_data *aplic, u32 hwirq)
{
	u32 sourcecfg;

	sourcecfg = aplic_sourcecfg_read(aplic, hwirq);
	بازگشت !!(sourcecfg & APLIC_SOURCECFG_D);
}

ایستا صحیح aplic_find_idc_index(ساختار aplic_data *aplic, u32 hart_index)
{
	u32 i;

	برای (i = 0; i < aplic->رقم_idc; i++) {
		اگر (aplic->idc_map[i] == hart_index)
			بازگشت i;
	}

	بازگشت SBI_ENOENT;
}

ایستا تهی aplic_init(ساختار aplic_data *aplic)
{
	ساختار aplic_delegate_data *deleg;
	u32 i, j, tmp, domaincfg;
	صحیح locked;

	/* Set domain configuration to 0 */
	writel(0, (تهی *)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	برای (i = 0; i <= aplic->رقم_source; i += 32)
		writel(-1U, (تهی *)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * اندازه(u32)));

	/* Set interrupt type and priority for all interrupts */
	برای (i = 1; i <= aplic->رقم_source; i++) {
		/* Set IRQ source configuration to 0 */
		writel(0, (تهی *)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * اندازه(u32)));
		/* Set IRQ target hart index and priority to 1 */
		writel(APLIC_DEFAULT_PRIORITY, (تهی *)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * اندازه(u32)));
	}

	/* Configure IRQ delegation */
	برای (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		اگر (!deleg->first_irq || !deleg->last_irq)
			ادامه;
		اگر (aplic->رقم_source < deleg->first_irq ||
		    aplic->رقم_source < deleg->last_irq)
			ادامه;
		اگر (deleg->child_index > APLIC_SOURCECFG_CHILDIDX_MASK)
			ادامه;
		اگر (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		برای (j = deleg->first_irq; j <= deleg->last_irq; j++)
			aplic_sourcecfg_write(aplic, j, APLIC_SOURCECFG_D | deleg->child_index);
	}

	/* Default initialization of IDC structures */
	اگر (aplic_is_direct_mode(aplic)) {
		برای (i = 0; i < aplic->رقم_idc; i++) {
			aplic_idc_write(aplic, i, APLIC_IDC_IDELIVERY,
					APLIC_DISABLE_IDELIVERY);
			aplic_idc_write(aplic, i, APLIC_IDC_IFORCE, 0);
			aplic_idc_write(aplic, i, APLIC_IDC_ITHRESHOLD,
					APLIC_DISABLE_ITHRESHOLD);
		}
	} وگرنه {
		/* MSI configuration */
		locked = readl((تهی *)(aplic->addr + APLIC_MMSICFGADDRH)) & APLIC_xMSICFGADDRH_L;
		اگر (aplic->targets_mmode && aplic->has_msicfg_mmode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_mmode,
						(تهی *)(aplic->addr + APLIC_MMSICFGADDR),
						(تهی *)(aplic->addr + APLIC_MMSICFGADDRH));
		}
		اگر (aplic->targets_mmode && aplic->has_msicfg_smode && !locked) {
			aplic_writel_msicfg(&aplic->msicfg_smode,
						(تهی *)(aplic->addr + APLIC_SMSICFGADDR),
						(تهی *)(aplic->addr + APLIC_SMSICFGADDRH));
		}
	}

	domaincfg = APLIC_DOMAINCFG_IE;
	اگر (!aplic_is_direct_mode(aplic))
		domaincfg |= APLIC_DOMAINCFG_DM;

	writel(domaincfg, (تهی *)(aplic->addr + APLIC_DOMAINCFG));
}

تهی aplic_reinit_all(تهی)
{
	ساختار aplic_data *aplic;

	sbi_list_for_each_entry(aplic, &aplic_list, گره)
		aplic_init(aplic);
}

ایستا تهی aplic_writel_msicfg(ساختار aplic_msicfg_data *msicfg,
				تهی *msicfgaddr, تهی *msicfgaddrH)
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

ایستا صحیح aplic_check_msicfg(ساختار aplic_msicfg_data *msicfg)
{
	اگر (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		بازگشت SBI_EINVAL;

	اگر (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		بازگشت SBI_EINVAL;

	اگر (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		بازگشت SBI_EINVAL;

	اگر (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		بازگشت SBI_EINVAL;

	بازگشت 0;
}

ایستا صحیح aplic_warm_init(ساختار sbi_irqchip_device *chip)
{
	ساختار aplic_data *aplic;
	u32 hart_index;
	صحیح idc_index;

	aplic = container_of(chip, ساختار aplic_data, irqchip);
	اگر (!aplic_is_direct_mode(aplic))
		بازگشت 0;

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	اگر (idc_index < 0)
		بازگشت 0;

	aplic_idc_write(aplic, idc_index, APLIC_IDC_ITHRESHOLD,
			APLIC_ENABLE_ITHRESHOLD);
	aplic_idc_write(aplic, idc_index, APLIC_IDC_IDELIVERY,
			APLIC_ENABLE_IDELIVERY);

	بازگشت 0;
}

ایستا صحیح aplic_process_hwirqs(ساختار sbi_irqchip_device *chip)
{
	ساختار aplic_data *aplic;
	u32 hart_index, claimi, hwirq;
	صحیح idc_index, rc = 0, tmp;

	aplic = container_of(chip, ساختار aplic_data, irqchip);

	اگر (!aplic_is_direct_mode(aplic))
		sbi_panic("aplic_process_hwirqs called in MSI mode.\n");

	hart_index = current_hartindex();
	idc_index  = aplic_find_idc_index(aplic, hart_index);
	اگر (idc_index < 0)
		بازگشت 0;

	وقتی ((claimi = aplic_idc_read(aplic, idc_index, APLIC_IDC_CLAIMI))) {
		hwirq = (claimi >> APLIC_IDC_TOPI_ID_SHIFT) &
			APLIC_IDC_TOPI_ID_MASK;

		اگر (!hwirq)
			بشکن;

		اگر (aplic_hwirq_is_delegated(aplic, hwirq))
			ادامه;

		tmp = sbi_irqchip_process_hwirq(chip, hwirq);
		اگر (tmp)
			rc = tmp;
	}

	بازگشت rc;
}

ایستا تهی aplic_hwirq_eoi(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار aplic_data *aplic;
	u32 sm;

	aplic = container_of(chip, ساختار aplic_data, irqchip);
	اگر (aplic_is_direct_mode(aplic))
		بازگشت;

	sm = aplic_sourcecfg_read(aplic, hwirq) & APLIC_SOURCECFG_SM_MASK;
	اگر (sm == APLIC_SOURCECFG_SM_LEVEL_بالا ||
	    sm == APLIC_SOURCECFG_SM_LEVEL_کم)
		aplic_msi_irq_retrigger_level(aplic, hwirq);
}

ایستا تهی aplic_program_msi_target(ساختار aplic_data *aplic,
				     u32 hwirq, u32 hart_index,
				     u32 guest_index, u32 eiid)
{
	u32 target;

	target = APLIC_TARGET_HART_IDX(hart_index) |
		 APLIC_TARGET_GUEST_IDX(guest_index) |
		 APLIC_TARGET_EIID(eiid);

	aplic_target_write(aplic, hwirq, target);
}

ایستا u32 derive_hart_index(ساختار aplic_msicfg_data *msicfg,
			      ثابت ساختار sbi_irqchip_msi_پیام *پیام)
{
	u64 addr = ((u64)پیام->address_hi << 32) | پیام->address_lo;
	u64 tppn = addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	u32 group_index, hart_index;

	group_index = (tppn >> APLIC_xMSICFGADDR_PPN_HHX_SHIFT(msicfg->hhxs)) &
		      APLIC_xMSICFGADDR_PPN_HHX_MASK(msicfg->hhxw);

	hart_index = (tppn >> msicfg->lhxs) &
		     ((1UL << msicfg->lhxw) - 1);

	hart_index |= (group_index << msicfg->lhxw);

	بازگشت hart_index;
}

ایستا تهی aplic_write_msi(u32 parent_hwirq,
			    ثابت ساختار sbi_irqchip_msi_پیام *پیام,
			    تهی *priv)
{
	ساختار aplic_msi_data *msi_data = priv;
	u32 guest_index = 0;
	u32 eiid;

	eiid = پیام->data;
	اگر (!eiid || eiid > APLIC_TARGET_EIID_MASK)
		بازگشت;

	aplic_program_msi_target(msi_data->aplic, msi_data->hwirq,
				derive_hart_index(&msi_data->aplic->msicfg_mmode, پیام),
				guest_index, eiid);
}

ایستا صحیح aplic_msi_callback(u32 parent_hwirq, تهی *priv)
{
	ساختار aplic_msi_data *msi_data = priv;
	صحیح rc;

	rc = sbi_irqchip_process_hwirq(&msi_data->aplic->irqchip, msi_data->hwirq);
	اگر (rc && rc != SBI_ENOENT)
		sbi_printf("%s: hwirq=%lu failed rc=%d\n",
			__func__,
			(unsigned long)parent_hwirq, rc);

	بازگشت rc;
}

ایستا صحیح aplic_setup_msi(ساختار aplic_data *aplic, u32 hwirq)
{
	ساختار sbi_irqchip_device *parent;
	u32 first_hwirq;
	ساختار aplic_msi_data *msi_data;
	صحیح rc;

	parent = sbi_irqchip_find_device(aplic->parent_unique_id);
	اگر (!parent) {
		sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
			__func__, (unsigned long)hwirq);
		بازگشت SBI_EINVAL;
	}

	msi_data = sbi_zalloc(اندازه(ساختار aplic_msi_data));
	اگر (!msi_data)
		بازگشت SBI_ENOMEM;

	msi_data->aplic = aplic;
	msi_data->hwirq = hwirq;

	rc = sbi_irqchip_register_msi(parent, 1,
					aplic_write_msi,
					aplic_msi_callback,
					msi_data,
					&first_hwirq);
	اگر (rc) {
		sbi_printf("%s: register_msi failed hwirq=%lu rc=%d\n",
			__func__, (unsigned long)hwirq, rc);
		sbi_free(msi_data);
		بازگشت rc;
	}

	msi_data->parent_hwirq = first_hwirq;
	sbi_irqchip_set_hwirq_priv(&aplic->irqchip, hwirq, msi_data);

	بازگشت 0;
}

ایستا صحیح aplic_hwirq_setup(ساختار sbi_irqchip_device *chip,
			     u32 hwirq, u32 hwirq_flags)
{
	ساختار aplic_data *aplic;
	u32 sourcecfg;

	aplic = container_of(chip, ساختار aplic_data, irqchip);

	sourcecfg = aplic_hwirq_flags_to_sourcecfg(hwirq_flags);
	اگر (sourcecfg == APLIC_SOURCECFG_SM_INACTIVE) {
		sbi_printf("%s: unsupported flags=0x%lx hwirq=%lu\n",
			__func__,
			(unsigned long)hwirq_flags,
			(unsigned long)hwirq);
		بازگشت SBI_EINVAL;
	}

	aplic_sourcecfg_write(aplic, hwirq, sourcecfg);

	اگر (aplic_hwirq_is_delegated(aplic, hwirq)) {
		sbi_printf("%s: hwirq=%lu is delegated\n",
			__func__, (unsigned long)hwirq);
		بازگشت SBI_ENOTSUPP;
	}

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);

	اگر (!aplic_is_direct_mode(aplic))
		بازگشت aplic_setup_msi(aplic, hwirq);

	بازگشت 0;
}

ایستا تهی aplic_hwirq_cleanup(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار aplic_data *aplic;
	ساختار aplic_msi_data *msi_data;

	aplic = container_of(chip, ساختار aplic_data, irqchip);
	اگر (aplic_hwirq_is_delegated(aplic, hwirq))
		بازگشت;

	aplic_irq_clrie(aplic, hwirq);
	aplic_irq_clrip(aplic, hwirq);
	aplic_sourcecfg_write(aplic, hwirq, APLIC_SOURCECFG_SM_INACTIVE);
	aplic_target_write(aplic, hwirq, APLIC_DEFAULT_PRIORITY);
	msi_data = (ساختار aplic_msi_data *) sbi_irqchip_get_hwirq_priv(chip, hwirq);
	اگر (msi_data)
		sbi_free(msi_data);
}

ایستا صحیح aplic_hwirq_set_affinity(ساختار sbi_irqchip_device *chip,
				    u32 hwirq, u32 hart_index)
{
	ساختار aplic_data *aplic;
	ساختار aplic_msi_data *msi_data;
	ساختار sbi_irqchip_device *parent;
	صحیح idc_index;
	صحیح rc;

	aplic = container_of(chip, ساختار aplic_data, irqchip);
	اگر (aplic_hwirq_is_delegated(aplic, hwirq))
		بازگشت SBI_ENOTSUPP;

	اگر (!aplic_is_direct_mode(aplic)) {
		parent = sbi_irqchip_find_device(aplic->parent_unique_id);
		اگر (!parent) {
			sbi_printf("%s: msi_parent is NULL hwirq=%lu\n",
				__func__, (unsigned long)hwirq);
			بازگشت SBI_EINVAL;
		}

		msi_data = sbi_irqchip_get_hwirq_priv(chip, hwirq);
		rc = sbi_irqchip_set_affinity(parent, msi_data->parent_hwirq, hart_index);
		اگر (rc) {
			sbi_printf("%s: sbi_irqchip_set_affinity failed hwirq=%lu rc=%d\n",
				__func__, (unsigned long)hwirq, rc);
			بازگشت rc;
		}
		بازگشت 0;
	}

	idc_index = aplic_find_idc_index(aplic, hart_index);
	اگر (idc_index < 0)
		بازگشت SBI_EINVAL;

	aplic_target_write(aplic, hwirq,
			   APLIC_TARGET_HART_IDX(hart_index) |
			   APLIC_TARGET_IPRIO(APLIC_DEFAULT_PRIORITY));

	بازگشت 0;
}

ایستا تهی aplic_hwirq_mask(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار aplic_data *aplic;

	aplic = container_of(chip, ساختار aplic_data, irqchip);
	اگر (aplic_hwirq_is_delegated(aplic, hwirq))
		بازگشت;

	aplic_irq_clrie(aplic, hwirq);
}

ایستا تهی aplic_hwirq_unmask(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار aplic_data *aplic;

	aplic = container_of(chip, ساختار aplic_data, irqchip);
	اگر (aplic_hwirq_is_delegated(aplic, hwirq))
		بازگشت;

	aplic_irq_setie(aplic, hwirq);
}

ایستا ساختار sbi_irqchip_device aplic_irqchip_template = {
	.warm_init			= aplic_warm_init,
	.process_hwirqs		= aplic_process_hwirqs,
	.hwirq_eoi			= aplic_hwirq_eoi,
	.hwirq_setup		= aplic_hwirq_setup,
	.hwirq_cleanup		= aplic_hwirq_cleanup,
	.hwirq_set_affinity	= aplic_hwirq_set_affinity,
	.hwirq_mask			= aplic_hwirq_mask,
	.hwirq_unmask		= aplic_hwirq_unmask,
};

صحیح aplic_cold_irqchip_init(ساختار aplic_data *aplic)
{
	صحیح rc;
	ساختار aplic_delegate_data *deleg;
	u32 first_deleg_irq, last_deleg_irq, i;
	bool msi_mode;

	msi_mode = !aplic_is_direct_mode(aplic);
	اگر (!aplic->رقم_source) {
		sbi_printf("%s: num_source is zero\n", __func__);
		بازگشت SBI_EINVAL;
	}

	اگر (APLIC_MAX_SOURCE <= aplic->رقم_source) {
		sbi_printf("%s: num_source=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->رقم_source,
			(unsigned long)APLIC_MAX_SOURCE);
		بازگشت SBI_EINVAL;
	}

	اگر (!msi_mode && APLIC_MAX_IDC <= aplic->رقم_idc) {
		sbi_printf("%s: num_idc=%lu exceeds max=%lu\n",
			__func__,
			(unsigned long)aplic->رقم_idc,
			(unsigned long)APLIC_MAX_IDC);
		بازگشت SBI_EINVAL;
	}

	اگر (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		اگر (rc) {
			sbi_printf("%s: invalid M-mode msicfg rc=%d\n",
				__func__, rc);
			بازگشت rc;
		}
	}

	اگر (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		اگر (rc) {
			sbi_printf("%s: invalid S-mode msicfg rc=%d\n",
				__func__, rc);
			بازگشت rc;
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
	برای (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		اگر (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		اگر (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
	}

	اگر (aplic->targets_mmode ||
	    ((first_deleg_irq < last_deleg_irq) &&
	    (last_deleg_irq == aplic->رقم_source) &&
	    (first_deleg_irq == 1))) {
		rc = sbi_domain_ریشه_add_memrange(aplic->addr, aplic->size, PAGE_SIZE,
						  SBI_DOMAIN_MEMREGION_MMIO |
						  SBI_DOMAIN_MEMREGION_M_READABLE |
						  SBI_DOMAIN_MEMREGION_M_WRITABLE);
		اگر (rc)
			بازگشت rc;
	}

	اگر ((aplic->targets_mmode)) {
		aplic->irqchip		 = aplic_irqchip_template;
		aplic->irqchip.id	 = aplic->unique_id;
		aplic->irqchip.caps = SBI_IRQCHIP_CAPS_WIRED;
		aplic->irqchip.رقم_hwirq = aplic->رقم_source + 1;

		اگر (msi_mode) {
			aplic->irqchip.warm_init      = پوچ;
			aplic->irqchip.process_hwirqs = پوچ;
			aplic->irqchip.hwirq_eoi      = پوچ;
		}

		اگر (msi_mode)
			sbi_hartmask_set_all(&aplic->irqchip.target_harts);
		وگرنه
			برای (i = 0; i < aplic->رقم_idc; i++)
				sbi_hartmask_set_hartindex(aplic->idc_map[i],
							   &aplic->irqchip.target_harts);

		rc = sbi_irqchip_add_device(&aplic->irqchip);
		اگر (rc) {
			sbi_printf("%s: sbi_irqchip_add_device failed rc=%d id=%lu mode=%s target_weight=%lu\n",
				__func__, rc,
				(unsigned long)aplic->irqchip.id,
				msi_mode ? "msi" : "direct",
				(unsigned long)sbi_hartmask_weight(
					&aplic->irqchip.target_harts));
			بازگشت rc;
		}
	}
	/* Attach to the aplic list */
	sbi_list_add_tail(&aplic->گره, &aplic_list);

	بازگشت 0;
}
