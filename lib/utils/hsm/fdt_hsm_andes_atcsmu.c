/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#అన్తర్భావ <andes/andes.h>
#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_init.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

స్థిర గుర్తులేని దీర్ఘ atcsmu_పునాది;

శూన్య atcsmu_సమితి_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (అక్షరం *)atcsmu_పునాది + PCSm_WE_స్థానభ్రంశం(hartid));
}

bool atcsmu_support_sleep_బహుళకం(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 ముసుగు;
	స్థిరరాశి అక్షరం *sleep_బహుళకం;

	pcs_cfg = readl_relaxed((అక్షరం *)atcsmu_పునాది + PCSm_CFG_స్థానభ్రంశం(hartid));
	ఎంపిక (sleep_type) {
	స్థితి SBI_SUSP_AE350_LIGHT_SLEEP:
		ముసుగు = PCS_CFG_LIGHT_SLEEP;
		sleep_బహుళకం = "light sleep";
		ఆపు;
	స్థితి SBI_SUSP_SLEEP_TYPE_SUSPEND:
		ముసుగు = PCS_CFG_DEEP_SLEEP;
		sleep_బహుళకం = "deep sleep";
		ఆపు;
	అప్రమేయ:
		తిరిగి false;
	}

	ఒకవేళ (!వెలికితీయి_క్షేత్రం(pcs_cfg, ముసుగు)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_బహుళకం);
		తిరిగి false;
	}

	తిరిగి true;
}

శూన్య atcsmu_సమితి_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (అక్షరం *)atcsmu_పునాది + PCSm_CTL_స్థానభ్రంశం(hartid));
}

పూర్ణాంకం atcsmu_సమితి_reset_సదిశ(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_సదిశ;

	writel((u32)wakeup_addr, (అక్షరం *)atcsmu_పునాది + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (అక్షరం *)atcsmu_పునాది + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((అక్షరం *)atcsmu_పునాది + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((అక్షరం *)atcsmu_పునాది + HARTn_RESET_VEC_HI(hartid));
	reset_సదిశ = (u64)vec_hi << 32 | vec_lo;
	ఒకవేళ (reset_సదిశ != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		తిరిగి SBI_EFAIL;
	}

	తిరిగి SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	తిరిగి readl_relaxed((అక్షరం *)atcsmu_పునాది + PCSm_SCRATCH_స్థానభ్రంశం(hartid));
}

శూన్య atcsmu_write_scratch(u32 విలువ)
{
	writel_relaxed(విలువ, (అక్షరం *)atcsmu_పునాది + SCRATCH_PAD_స్థానభ్రంశం);
}

u32 atcsmu_read_scratch(శూన్య)
{
	తిరిగి readl_relaxed((అక్షరం *)atcsmu_పునాది + SCRATCH_PAD_స్థానభ్రంశం);
}

bool atcsmu_hart_is_sleep(శూన్య *opaque)
{
	నిర్మాణం atcsmu_sleep_arg *arg = opaque;

	u32 pcs_స్థితి = readl_relaxed((అక్షరం *)atcsmu_పునాది +
				       PCSm_స్థితి_స్థానభ్రంశం(arg->hartid));
	u32 pd_స్థితి = arg->deep_sleep ? PD_స్థితి_DEEP_SLEEP :
					  PD_స్థితి_LIGHT_SLEEP;

	తిరిగి వెలికితీయి_క్షేత్రం(pcs_స్థితి, PD_TYPE_ముసుగు) == PD_TYPE_SLEEP &&
	       వెలికితీయి_క్షేత్రం(pcs_స్థితి, PD_స్థితి_ముసుగు) == pd_స్థితి;
}

స్థిర పూర్ణాంకం ae350_hart_ప్రారంభం(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_గమ్యం_hartindex(hartid);

	తిరిగి sbi_ipi_raw_send(hartindex, false);
}

స్థిర పూర్ణాంకం ae350_hart_stop(శూన్య)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	పూర్ణాంకం rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	ఒకవేళ (is_andes(25) && hartid == 0)
		తిరిగి SBI_ENOTSUPP;

	ఒకవేళ (!atcsmu_support_sleep_బహుళకం(sleep_type, hartid))
		తిరిగి SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_సమితి_wakeup_events(PCS_WAKEUP_MSIP_ముసుగు | PCS_WAKEUP_MEIP_ముసుగు, hartid);
	ఒకవేళ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_సమితి(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_సమితి_command(LIGHT_SLEEP_CMD, hartid);
	} లేకపోతే ఒకవేళ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_సమితి_reset_సదిశ((ulong)ae350_enable_coherency_warmboot, hartid);
		ఒకవేళ (rc)
			తిరిగి SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_సమితి_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	తిరిగి SBI_ENOTSUPP;
}

స్థిర స్థిరరాశి నిర్మాణం sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_ప్రారంభం = ae350_hart_ప్రారంభం,
	.hart_stop = ae350_hart_stop,
};

స్థిర పూర్ణాంకం hsm_andes_atcsmu_probe(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం poff, rc;
	u64 addr, కొలత;

	/* Need to find the parent for the address property  */
	poff = fdt_మాతృ_స్థానభ్రంశం(fdt, nodeoff);
	ఒకవేళ (poff < 0)
		తిరిగి SBI_EINVAL;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, poff, 0, &addr, &కొలత);
	ఒకవేళ (rc < 0 || !addr || !కొలత)
		తిరిగి SBI_ENODEV;

	ఒకవేళ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_మూలం_కూడు_memrange(
			(గుర్తులేని దీర్ఘ)addr, (గుర్తులేని దీర్ఘ)కొలత, పుట_కొలత,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	atcsmu_పునాది = addr;
	sbi_hsm_సమితి_device(&hsm_andes_atcsmu);
	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక hsm_andes_atcsmu_సరిపోలిక[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_hsm_andes_atcsmu = {
	.సరిపోలిక_పట్టిక = hsm_andes_atcsmu_సరిపోలిక,
	.init = hsm_andes_atcsmu_probe,
};
