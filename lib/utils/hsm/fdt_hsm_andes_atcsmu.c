/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ଅନ୍ତର୍ଭାଵ <andes/andes.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_init.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

ସ୍ଥିର unsigned long atcsmu_base;

ଶୂନ୍ୟ atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (ଅକ୍ଷର *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	ଅଚଳ ଅକ୍ଷର *sleep_mode;

	pcs_cfg = readl_relaxed((ଅକ୍ଷର *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	ବାଛ (sleep_type) {
	ସ୍ଥିତି SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ false;
	}

	ଯଦି (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		ଫେରନ୍ତୁ false;
	}

	ଫେରନ୍ତୁ true;
}

ଶୂନ୍ୟ atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (ଅକ୍ଷର *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (ଅକ୍ଷର *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (ଅକ୍ଷର *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((ଅକ୍ଷର *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((ଅକ୍ଷର *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	ଯଦି (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		ଫେରନ୍ତୁ SBI_EFAIL;
	}

	ଫେରନ୍ତୁ SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	ଫେରନ୍ତୁ readl_relaxed((ଅକ୍ଷର *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

ଶୂନ୍ୟ atcsmu_write_scratch(u32 ମୂଲ୍ୟ)
{
	writel_relaxed(ମୂଲ୍ୟ, (ଅକ୍ଷର *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ readl_relaxed((ଅକ୍ଷର *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(ଶୂନ୍ୟ *opaque)
{
	ସଂରଚନା atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((ଅକ୍ଷର *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	ଫେରନ୍ତୁ EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	ଫେରନ୍ତୁ sbi_ipi_raw_send(hartindex, false);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ae350_hart_stop(ଶୂନ୍ୟ)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	ଯଦି (is_andes(25) && hartid == 0)
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	ଯଦି (!atcsmu_support_sleep_mode(sleep_type, hartid))
		ଫେରନ୍ତୁ SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_set_wakeup_events(PCS_WAKEUP_MSIP_MASK | PCS_WAKEUP_MEIP_MASK, hartid);
	ଯଦି (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} ଅନ୍ୟଥା ଯଦି (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		ଯଦି (rc)
			ଫେରନ୍ତୁ SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ଫେରନ୍ତୁ SBI_ENOTSUPP;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hsm_andes_atcsmu_probe(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା poff, rc;
	u64 addr, size;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	ଯଦି (poff < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, poff, 0, &addr, &size);
	ଯଦି (rc < 0 || !addr || !size)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ଯଦି (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ମୂଳ_add_memrange(
			(unsigned long)addr, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
