/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#अन्तर्भाव <andes/andes.h>
#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_init.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

स्थिर अचिह्नित दीर्घ atcsmu_आधारः;

शून्य atcsmu_समुच्चयः_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (वर्ण *)atcsmu_आधारः + PCSm_WE_विस्थापनम्(hartid));
}

bool atcsmu_support_sleep_बहुलकम्(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 आवरणम्;
	ध्रुव वर्ण *sleep_बहुलकम्;

	pcs_cfg = readl_relaxed((वर्ण *)atcsmu_आधारः + PCSm_CFG_विस्थापनम्(hartid));
	विकल्प (sleep_type) {
	पक्ष SBI_SUSP_AE350_LIGHT_SLEEP:
		आवरणम् = PCS_CFG_LIGHT_SLEEP;
		sleep_बहुलकम् = "light sleep";
		भङ्ग;
	पक्ष SBI_SUSP_SLEEP_TYPE_SUSPEND:
		आवरणम् = PCS_CFG_DEEP_SLEEP;
		sleep_बहुलकम् = "deep sleep";
		भङ्ग;
	स्वभाव:
		प्रत्यागम false;
	}

	यदि (!EXTRACT_क्षेत्रम्(pcs_cfg, आवरणम्)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_बहुलकम्);
		प्रत्यागम false;
	}

	प्रत्यागम true;
}

शून्य atcsmu_समुच्चयः_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (वर्ण *)atcsmu_आधारः + PCSm_CTL_विस्थापनम्(hartid));
}

पूर्णाङ्क atcsmu_समुच्चयः_reset_सदिशम्(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_सदिशम्;

	writel((u32)wakeup_addr, (वर्ण *)atcsmu_आधारः + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (वर्ण *)atcsmu_आधारः + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((वर्ण *)atcsmu_आधारः + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((वर्ण *)atcsmu_आधारः + HARTn_RESET_VEC_HI(hartid));
	reset_सदिशम् = (u64)vec_hi << 32 | vec_lo;
	यदि (reset_सदिशम् != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		प्रत्यागम SBI_EFAIL;
	}

	प्रत्यागम SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	प्रत्यागम readl_relaxed((वर्ण *)atcsmu_आधारः + PCSm_SCRATCH_विस्थापनम्(hartid));
}

शून्य atcsmu_write_scratch(u32 मानम्)
{
	writel_relaxed(मानम्, (वर्ण *)atcsmu_आधारः + SCRATCH_PAD_विस्थापनम्);
}

u32 atcsmu_read_scratch(शून्य)
{
	प्रत्यागम readl_relaxed((वर्ण *)atcsmu_आधारः + SCRATCH_PAD_विस्थापनम्);
}

bool atcsmu_hart_is_sleep(शून्य *opaque)
{
	संरचना atcsmu_sleep_arg *arg = opaque;

	u32 pcs_स्थितिः = readl_relaxed((वर्ण *)atcsmu_आधारः +
				       PCSm_स्थितिः_विस्थापनम्(arg->hartid));
	u32 pd_स्थितिः = arg->deep_sleep ? PD_स्थितिः_DEEP_SLEEP :
					  PD_स्थितिः_LIGHT_SLEEP;

	प्रत्यागम EXTRACT_क्षेत्रम्(pcs_स्थितिः, PD_TYPE_आवरणम्) == PD_TYPE_SLEEP &&
	       EXTRACT_क्षेत्रम्(pcs_स्थितिः, PD_स्थितिः_आवरणम्) == pd_स्थितिः;
}

स्थिर पूर्णाङ्क ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_गन्तव्यम्_hartindex(hartid);

	प्रत्यागम sbi_ipi_raw_send(hartindex, false);
}

स्थिर पूर्णाङ्क ae350_hart_stop(शून्य)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	पूर्णाङ्क rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	यदि (is_andes(25) && hartid == 0)
		प्रत्यागम SBI_ENOTSUPP;

	यदि (!atcsmu_support_sleep_बहुलकम्(sleep_type, hartid))
		प्रत्यागम SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_समुच्चयः_wakeup_events(PCS_WAKEUP_MSIP_आवरणम् | PCS_WAKEUP_MEIP_आवरणम्, hartid);
	यदि (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_समुच्चयः(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_समुच्चयः_command(LIGHT_SLEEP_CMD, hartid);
	} अन्यथा यदि (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_समुच्चयः_reset_सदिशम्((ulong)ae350_enable_coherency_warmboot, hartid);
		यदि (rc)
			प्रत्यागम SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_समुच्चयः_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	प्रत्यागम SBI_ENOTSUPP;
}

स्थिर ध्रुव संरचना sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

स्थिर पूर्णाङ्क hsm_andes_atcsmu_probe(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क poff, rc;
	u64 addr, आकार;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_विस्थापनम्(fdt, nodeoff);
	यदि (poff < 0)
		प्रत्यागम SBI_EINVAL;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, poff, 0, &addr, &आकार);
	यदि (rc < 0 || !addr || !आकार)
		प्रत्यागम SBI_ENODEV;

	यदि (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_मूलम्_सङ्कलय_memrange(
			(अचिह्नित दीर्घ)addr, (अचिह्नित दीर्घ)आकार, PAGE_आकार,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		यदि (rc)
			प्रत्यागम rc;
	}

	atcsmu_आधारः = addr;
	sbi_hsm_समुच्चयः_device(&hsm_andes_atcsmu);
	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् hsm_andes_atcsmu_मेलनम्[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_hsm_andes_atcsmu = {
	.मेलनम्_सारणी = hsm_andes_atcsmu_मेलनम्,
	.init = hsm_andes_atcsmu_probe,
};
