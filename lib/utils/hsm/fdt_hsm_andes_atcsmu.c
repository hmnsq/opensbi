/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#समावेश <andes/andes.h>
#समावेश <libfdt.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_init.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

अटल अचिन्हित दीर्घ atcsmu_base;

व्योम atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (अक्षर *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	स्थिर अक्षर *sleep_mode;

	pcs_cfg = readl_relaxed((अक्षर *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	चयन (sleep_type) {
	हाल SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		अवरोध;
	हाल SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		अवरोध;
	शेष:
		वापस false;
	}

	अगर (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		वापस false;
	}

	वापस true;
}

व्योम atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (अक्षर *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

पूर्णांक atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (अक्षर *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (अक्षर *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((अक्षर *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((अक्षर *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	अगर (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		वापस SBI_EFAIL;
	}

	वापस SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	वापस readl_relaxed((अक्षर *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

व्योम atcsmu_write_scratch(u32 मूल्य)
{
	writel_relaxed(मूल्य, (अक्षर *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(व्योम)
{
	वापस readl_relaxed((अक्षर *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(व्योम *opaque)
{
	काष्ठा atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((अक्षर *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	वापस EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

अटल पूर्णांक ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	वापस sbi_ipi_raw_send(hartindex, false);
}

अटल पूर्णांक ae350_hart_stop(व्योम)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	पूर्णांक rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	अगर (is_andes(25) && hartid == 0)
		वापस SBI_ENOTSUPP;

	अगर (!atcsmu_support_sleep_mode(sleep_type, hartid))
		वापस SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_set_wakeup_events(PCS_WAKEUP_MSIP_MASK | PCS_WAKEUP_MEIP_MASK, hartid);
	अगर (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} अन्यथा अगर (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		अगर (rc)
			वापस SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	वापस SBI_ENOTSUPP;
}

अटल स्थिर काष्ठा sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

अटल पूर्णांक hsm_andes_atcsmu_probe(स्थिर व्योम *fdt, पूर्णांक nodeoff, स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक poff, rc;
	u64 addr, आकार;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	अगर (poff < 0)
		वापस SBI_EINVAL;

	rc = fdt_get_गाँठ_addr_आकार(fdt, poff, 0, &addr, &आकार);
	अगर (rc < 0 || !addr || !आकार)
		वापस SBI_ENODEV;

	अगर (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_मूल_add_memrange(
			(अचिन्हित दीर्घ)addr, (अचिन्हित दीर्घ)आकार, PAGE_आकार,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		अगर (rc)
			वापस rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	वापस 0;
}

अटल स्थिर काष्ठा fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
