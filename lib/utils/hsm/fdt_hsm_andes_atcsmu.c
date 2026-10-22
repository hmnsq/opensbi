/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#અન્તર્ભાવ <andes/andes.h>
#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_init.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

સ્થિર unsigned long atcsmu_base;

શૂન્ય atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (અક્ષર *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	અચલ અક્ષર *sleep_mode;

	pcs_cfg = readl_relaxed((અક્ષર *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	પસંદગી (sleep_type) {
	સ્થિતિ SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		તોડો;
	સ્થિતિ SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		તોડો;
	મૂળભૂત:
		પાછા false;
	}

	જો (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		પાછા false;
	}

	પાછા true;
}

શૂન્ય atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (અક્ષર *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

પૂર્ણાંક atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (અક્ષર *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (અક્ષર *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((અક્ષર *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((અક્ષર *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	જો (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		પાછા SBI_EFAIL;
	}

	પાછા SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	પાછા readl_relaxed((અક્ષર *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

શૂન્ય atcsmu_write_scratch(u32 કિંમત)
{
	writel_relaxed(કિંમત, (અક્ષર *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(શૂન્ય)
{
	પાછા readl_relaxed((અક્ષર *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(શૂન્ય *opaque)
{
	સંરચના atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((અક્ષર *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	પાછા EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

સ્થિર પૂર્ણાંક ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	પાછા sbi_ipi_raw_send(hartindex, false);
}

સ્થિર પૂર્ણાંક ae350_hart_stop(શૂન્ય)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	પૂર્ણાંક rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	જો (is_andes(25) && hartid == 0)
		પાછા SBI_ENOTSUPP;

	જો (!atcsmu_support_sleep_mode(sleep_type, hartid))
		પાછા SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_set_wakeup_events(PCS_WAKEUP_MSIP_MASK | PCS_WAKEUP_MEIP_MASK, hartid);
	જો (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} નહીંતર જો (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		જો (rc)
			પાછા SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	પાછા SBI_ENOTSUPP;
}

સ્થિર અચલ સંરચના sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

સ્થિર પૂર્ણાંક hsm_andes_atcsmu_probe(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક poff, rc;
	u64 addr, size;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	જો (poff < 0)
		પાછા SBI_EINVAL;

	rc = fdt_get_ગાંઠ_addr_size(fdt, poff, 0, &addr, &size);
	જો (rc < 0 || !addr || !size)
		પાછા SBI_ENODEV;

	જો (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_મૂળ_add_memrange(
			(unsigned long)addr, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		જો (rc)
			પાછા rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

અચલ સંરચના fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
