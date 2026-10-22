/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ضمن <andes/andes.h>
#ضمن <libfdt.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_init.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

ساكن بنانشانی لما atcsmu_ادھار;

فراغ atcsmu_سیٹ_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (حرف *)atcsmu_ادھار + PCSm_WE_وتھ(hartid));
}

bool atcsmu_support_sleep_منوال(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 نقاب;
	ثابت حرف *sleep_منوال;

	pcs_cfg = readl_relaxed((حرف *)atcsmu_ادھار + PCSm_CFG_وتھ(hartid));
	بدل (sleep_type) {
	حالة SBI_SUSP_AE350_LIGHT_SLEEP:
		نقاب = PCS_CFG_LIGHT_SLEEP;
		sleep_منوال = "light sleep";
		اكسر;
	حالة SBI_SUSP_SLEEP_TYPE_SUSPEND:
		نقاب = PCS_CFG_DEEP_SLEEP;
		sleep_منوال = "deep sleep";
		اكسر;
	افتراضي:
		أرجع false;
	}

	إذا (!کڈھو_میدان(pcs_cfg, نقاب)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_منوال);
		أرجع false;
	}

	أرجع true;
}

فراغ atcsmu_سیٹ_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (حرف *)atcsmu_ادھار + PCSm_CTL_وتھ(hartid));
}

صحيح atcsmu_سیٹ_reset_سمتیہ(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_سمتیہ;

	writel((u32)wakeup_addr, (حرف *)atcsmu_ادھار + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (حرف *)atcsmu_ادھار + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((حرف *)atcsmu_ادھار + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((حرف *)atcsmu_ادھار + HARTn_RESET_VEC_HI(hartid));
	reset_سمتیہ = (u64)vec_hi << 32 | vec_lo;
	إذا (reset_سمتیہ != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		أرجع SBI_EFAIL;
	}

	أرجع SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	أرجع readl_relaxed((حرف *)atcsmu_ادھار + PCSm_SCRATCH_وتھ(hartid));
}

فراغ atcsmu_write_scratch(u32 قدر)
{
	writel_relaxed(قدر, (حرف *)atcsmu_ادھار + SCRATCH_PAD_وتھ);
}

u32 atcsmu_read_scratch(فراغ)
{
	أرجع readl_relaxed((حرف *)atcsmu_ادھار + SCRATCH_PAD_وتھ);
}

bool atcsmu_hart_is_sleep(فراغ *opaque)
{
	بنية atcsmu_sleep_arg *arg = opaque;

	u32 pcs_حالت = readl_relaxed((حرف *)atcsmu_ادھار +
				       PCSm_حالت_وتھ(arg->hartid));
	u32 pd_حالت = arg->deep_sleep ? PD_حالت_DEEP_SLEEP :
					  PD_حالت_LIGHT_SLEEP;

	أرجع کڈھو_میدان(pcs_حالت, PD_TYPE_نقاب) == PD_TYPE_SLEEP &&
	       کڈھو_میدان(pcs_حالت, PD_حالت_نقاب) == pd_حالت;
}

ساكن صحيح ae350_hart_شروع(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_منزل_hartindex(hartid);

	أرجع sbi_ipi_raw_send(hartindex, false);
}

ساكن صحيح ae350_hart_stop(فراغ)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	صحيح rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	إذا (is_andes(25) && hartid == 0)
		أرجع SBI_ENOTSUPP;

	إذا (!atcsmu_support_sleep_منوال(sleep_type, hartid))
		أرجع SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_سیٹ_wakeup_events(PCS_WAKEUP_MSIP_نقاب | PCS_WAKEUP_MEIP_نقاب, hartid);
	إذا (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_سیٹ(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_سیٹ_command(LIGHT_SLEEP_CMD, hartid);
	} وإلا إذا (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_سیٹ_reset_سمتیہ((ulong)ae350_enable_coherency_warmboot, hartid);
		إذا (rc)
			أرجع SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_سیٹ_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	أرجع SBI_ENOTSUPP;
}

ساكن ثابت بنية sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_شروع = ae350_hart_شروع,
	.hart_stop = ae350_hart_stop,
};

ساكن صحيح hsm_andes_atcsmu_probe(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_میل *میل)
{
	صحيح poff, rc;
	u64 addr, ناپ;

	/* Need to find the parent for the address property  */
	poff = fdt_ماپا_وتھ(fdt, nodeoff);
	إذا (poff < 0)
		أرجع SBI_EINVAL;

	rc = fdt_get_گرہ_addr_ناپ(fdt, poff, 0, &addr, &ناپ);
	إذا (rc < 0 || !addr || !ناپ)
		أرجع SBI_ENODEV;

	إذا (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_جڑ_جوڑو_memrange(
			(بنانشانی لما)addr, (بنانشانی لما)ناپ, ورقہ_ناپ,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		إذا (rc)
			أرجع rc;
	}

	atcsmu_ادھار = addr;
	sbi_hsm_سیٹ_device(&hsm_andes_atcsmu);
	أرجع 0;
}

ساكن ثابت بنية fdt_میل hsm_andes_atcsmu_میل[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ثابت بنية fdt_driver fdt_hsm_andes_atcsmu = {
	.میل_جدول = hsm_andes_atcsmu_میل,
	.init = hsm_andes_atcsmu_probe,
};
