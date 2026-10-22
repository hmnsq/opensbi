/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#অন্তর্ভাৱ <andes/andes.h>
#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_hsm.h>
#অন্তর্ভাৱ <sbi/sbi_init.h>
#অন্তর্ভাৱ <sbi/sbi_ipi.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

স্থির unsigned long atcsmu_base;

শূন্য atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (ৱর্ণ *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	ধ্রুৱ ৱর্ণ *sleep_mode;

	pcs_cfg = readl_relaxed((ৱর্ণ *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	ৱিকল্প (sleep_type) {
	পক্ষ SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		ভঙ্গ;
	পক্ষ SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		ভঙ্গ;
	স্ৱভাৱ:
		ঘূৰাই false;
	}

	যদি (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		ঘূৰাই false;
	}

	ঘূৰাই true;
}

শূন্য atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (ৱর্ণ *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

পূর্ণাঙ্ক atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (ৱর্ণ *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (ৱর্ণ *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((ৱর্ণ *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((ৱর্ণ *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	যদি (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		ঘূৰাই SBI_EFAIL;
	}

	ঘূৰাই SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	ঘূৰাই readl_relaxed((ৱর্ণ *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

শূন্য atcsmu_write_scratch(u32 মান)
{
	writel_relaxed(মান, (ৱর্ণ *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(শূন্য)
{
	ঘূৰাই readl_relaxed((ৱর্ণ *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(শূন্য *opaque)
{
	সংরচনা atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((ৱর্ণ *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	ঘূৰাই EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

স্থির পূর্ণাঙ্ক ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	ঘূৰাই sbi_ipi_raw_send(hartindex, false);
}

স্থির পূর্ণাঙ্ক ae350_hart_stop(শূন্য)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	পূর্ণাঙ্ক rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	যদি (is_andes(25) && hartid == 0)
		ঘূৰাই SBI_ENOTSUPP;

	যদি (!atcsmu_support_sleep_mode(sleep_type, hartid))
		ঘূৰাই SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_set_wakeup_events(PCS_WAKEUP_MSIP_MASK | PCS_WAKEUP_MEIP_MASK, hartid);
	যদি (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} অন্যথা যদি (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		যদি (rc)
			ঘূৰাই SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ঘূৰাই SBI_ENOTSUPP;
}

স্থির ধ্রুৱ সংরচনা sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

স্থির পূর্ণাঙ্ক hsm_andes_atcsmu_probe(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff, ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক poff, rc;
	u64 addr, size;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	যদি (poff < 0)
		ঘূৰাই SBI_EINVAL;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, poff, 0, &addr, &size);
	যদি (rc < 0 || !addr || !size)
		ঘূৰাই SBI_ENODEV;

	যদি (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_মূল_add_memrange(
			(unsigned long)addr, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		যদি (rc)
			ঘূৰাই rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
