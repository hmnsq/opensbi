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

स्थिर unsigned long atcsmu_base;

शून्य atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (वर्ण *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	ध्रुव वर्ण *sleep_mode;

	pcs_cfg = readl_relaxed((वर्ण *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	विकल्प (sleep_type) {
	पक्ष SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		भङ्ग;
	पक्ष SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		भङ्ग;
	स्वभाव:
		प्रत्यागम false;
	}

	यदि (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		प्रत्यागम false;
	}

	प्रत्यागम true;
}

शून्य atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (वर्ण *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

पूर्णाङ्क atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (वर्ण *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (वर्ण *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((वर्ण *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((वर्ण *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	यदि (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		प्रत्यागम SBI_EFAIL;
	}

	प्रत्यागम SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	प्रत्यागम readl_relaxed((वर्ण *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

शून्य atcsmu_write_scratch(u32 मूल्य)
{
	writel_relaxed(मूल्य, (वर्ण *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(शून्य)
{
	प्रत्यागम readl_relaxed((वर्ण *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(शून्य *opaque)
{
	संरचना atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((वर्ण *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	प्रत्यागम EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

स्थिर पूर्णाङ्क ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

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

	यदि (!atcsmu_support_sleep_mode(sleep_type, hartid))
		प्रत्यागम SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_set_wakeup_events(PCS_WAKEUP_MSIP_MASK | PCS_WAKEUP_MEIP_MASK, hartid);
	यदि (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} अन्यथा यदि (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		यदि (rc)
			प्रत्यागम SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
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

स्थिर पूर्णाङ्क hsm_andes_atcsmu_probe(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क poff, rc;
	u64 addr, size;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	यदि (poff < 0)
		प्रत्यागम SBI_EINVAL;

	rc = fdt_get_गाँठ_addr_size(fdt, poff, 0, &addr, &size);
	यदि (rc < 0 || !addr || !size)
		प्रत्यागम SBI_ENODEV;

	यदि (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_मूल_add_memrange(
			(unsigned long)addr, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		यदि (rc)
			प्रत्यागम rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
