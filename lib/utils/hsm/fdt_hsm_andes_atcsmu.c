/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#অন্তর্ভাব <andes/andes.h>
#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_init.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

স্থির unsigned long atcsmu_base;

শূন্য atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (অক্ষর *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	ধ্রুবক অক্ষর *sleep_mode;

	pcs_cfg = readl_relaxed((অক্ষর *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	নির্বাচন (sleep_type) {
	ক্ষেত্র SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		বিরতি;
	ক্ষেত্র SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		বিরতি;
	পূর্বনির্ধারিত:
		ফেরত false;
	}

	যদি (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		ফেরত false;
	}

	ফেরত true;
}

শূন্য atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (অক্ষর *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

পূর্ণসংখ্যা atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (অক্ষর *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (অক্ষর *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((অক্ষর *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((অক্ষর *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	যদি (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		ফেরত SBI_EFAIL;
	}

	ফেরত SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	ফেরত readl_relaxed((অক্ষর *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

শূন্য atcsmu_write_scratch(u32 মান)
{
	writel_relaxed(মান, (অক্ষর *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(শূন্য)
{
	ফেরত readl_relaxed((অক্ষর *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(শূন্য *opaque)
{
	কাঠামো atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((অক্ষর *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	ফেরত EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

স্থির পূর্ণসংখ্যা ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	ফেরত sbi_ipi_raw_send(hartindex, false);
}

স্থির পূর্ণসংখ্যা ae350_hart_stop(শূন্য)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	পূর্ণসংখ্যা rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	যদি (is_andes(25) && hartid == 0)
		ফেরত SBI_ENOTSUPP;

	যদি (!atcsmu_support_sleep_mode(sleep_type, hartid))
		ফেরত SBI_ENOTSUPP;

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
			ফেরত SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ফেরত SBI_ENOTSUPP;
}

স্থির ধ্রুবক কাঠামো sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

স্থির পূর্ণসংখ্যা hsm_andes_atcsmu_probe(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা poff, rc;
	u64 addr, আকার_2;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	যদি (poff < 0)
		ফেরত SBI_EINVAL;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, poff, 0, &addr, &আকার_2);
	যদি (rc < 0 || !addr || !আকার_2)
		ফেরত SBI_ENODEV;

	যদি (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_মূল_add_memrange(
			(unsigned long)addr, (unsigned long)আকার_2, PAGE_আকার,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		যদি (rc)
			ফেরত rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
