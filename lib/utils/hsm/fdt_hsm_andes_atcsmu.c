/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#അന്തര്ഭാവ <andes/andes.h>
#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_init.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

സ്ഥിതം unsigned long atcsmu_base;

ശൂന്യം atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (അക്ഷരം *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	സ്ഥിരം അക്ഷരം *sleep_mode;

	pcs_cfg = readl_relaxed((അക്ഷരം *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	തിരഞ്ഞെടുപ്പ് (sleep_type) {
	അവസ്ഥ SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		നിർത്തുക;
	അവസ്ഥ SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ false;
	}

	എങ്കിൽ (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		തിരികെ false;
	}

	തിരികെ true;
}

ശൂന്യം atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (അക്ഷരം *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

പൂർണ്ണസംഖ്യ atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (അക്ഷരം *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (അക്ഷരം *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((അക്ഷരം *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((അക്ഷരം *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	എങ്കിൽ (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		തിരികെ SBI_EFAIL;
	}

	തിരികെ SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	തിരികെ readl_relaxed((അക്ഷരം *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

ശൂന്യം atcsmu_write_scratch(u32 മൂല്യം)
{
	writel_relaxed(മൂല്യം, (അക്ഷരം *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(ശൂന്യം)
{
	തിരികെ readl_relaxed((അക്ഷരം *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(ശൂന്യം *opaque)
{
	ഘടന atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((അക്ഷരം *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	തിരികെ EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	തിരികെ sbi_ipi_raw_send(hartindex, false);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ae350_hart_stop(ശൂന്യം)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	പൂർണ്ണസംഖ്യ rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	എങ്കിൽ (is_andes(25) && hartid == 0)
		തിരികെ SBI_ENOTSUPP;

	എങ്കിൽ (!atcsmu_support_sleep_mode(sleep_type, hartid))
		തിരികെ SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_set_wakeup_events(PCS_WAKEUP_MSIP_MASK | PCS_WAKEUP_MEIP_MASK, hartid);
	എങ്കിൽ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} അല്ലെങ്കിൽ എങ്കിൽ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		എങ്കിൽ (rc)
			തിരികെ SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	തിരികെ SBI_ENOTSUPP;
}

സ്ഥിതം സ്ഥിരം ഘടന sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ hsm_andes_atcsmu_probe(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ poff, rc;
	u64 addr, size;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	എങ്കിൽ (poff < 0)
		തിരികെ SBI_EINVAL;

	rc = fdt_get_കണ്ണി_addr_size(fdt, poff, 0, &addr, &size);
	എങ്കിൽ (rc < 0 || !addr || !size)
		തിരികെ SBI_ENODEV;

	എങ്കിൽ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_വേര്_add_memrange(
			(unsigned long)addr, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
