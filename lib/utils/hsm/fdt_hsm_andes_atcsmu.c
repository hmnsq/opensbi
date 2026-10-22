/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#شامل <andes/andes.h>
#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_init.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

ساکن غیرعلامتی طویل atcsmu_بنیاد;

خالی atcsmu_سیٹ_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (حرف *)atcsmu_بنیاد + PCSm_WE_انحراف(hartid));
}

bool atcsmu_support_sleep_منوال(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 نقاب;
	ثابت حرف *sleep_منوال;

	pcs_cfg = readl_relaxed((حرف *)atcsmu_بنیاد + PCSm_CFG_انحراف(hartid));
	انتخاب (sleep_type) {
	حالت SBI_SUSP_AE350_LIGHT_SLEEP:
		نقاب = PCS_CFG_LIGHT_SLEEP;
		sleep_منوال = "light sleep";
		توڑو;
	حالت SBI_SUSP_SLEEP_TYPE_SUSPEND:
		نقاب = PCS_CFG_DEEP_SLEEP;
		sleep_منوال = "deep sleep";
		توڑو;
	طےشدہ:
		واپس false;
	}

	اگر (!نکالو_میدان(pcs_cfg, نقاب)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_منوال);
		واپس false;
	}

	واپس true;
}

خالی atcsmu_سیٹ_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (حرف *)atcsmu_بنیاد + PCSm_CTL_انحراف(hartid));
}

صحیح atcsmu_سیٹ_reset_سمتیہ(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_سمتیہ;

	writel((u32)wakeup_addr, (حرف *)atcsmu_بنیاد + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (حرف *)atcsmu_بنیاد + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((حرف *)atcsmu_بنیاد + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((حرف *)atcsmu_بنیاد + HARTn_RESET_VEC_HI(hartid));
	reset_سمتیہ = (u64)vec_hi << 32 | vec_lo;
	اگر (reset_سمتیہ != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		واپس SBI_EFAIL;
	}

	واپس SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	واپس readl_relaxed((حرف *)atcsmu_بنیاد + PCSm_SCRATCH_انحراف(hartid));
}

خالی atcsmu_write_scratch(u32 قدر)
{
	writel_relaxed(قدر, (حرف *)atcsmu_بنیاد + SCRATCH_PAD_انحراف);
}

u32 atcsmu_read_scratch(خالی)
{
	واپس readl_relaxed((حرف *)atcsmu_بنیاد + SCRATCH_PAD_انحراف);
}

bool atcsmu_hart_is_sleep(خالی *opaque)
{
	ساخت atcsmu_sleep_arg *arg = opaque;

	u32 pcs_حالت = readl_relaxed((حرف *)atcsmu_بنیاد +
				       PCSm_حالت_انحراف(arg->hartid));
	u32 pd_حالت = arg->deep_sleep ? PD_حالت_DEEP_SLEEP :
					  PD_حالت_LIGHT_SLEEP;

	واپس نکالو_میدان(pcs_حالت, PD_TYPE_نقاب) == PD_TYPE_SLEEP &&
	       نکالو_میدان(pcs_حالت, PD_حالت_نقاب) == pd_حالت;
}

ساکن صحیح ae350_hart_آغاز(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_مقصد_hartindex(hartid);

	واپس sbi_ipi_raw_send(hartindex, false);
}

ساکن صحیح ae350_hart_stop(خالی)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	صحیح rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	اگر (is_andes(25) && hartid == 0)
		واپس SBI_ENOTSUPP;

	اگر (!atcsmu_support_sleep_منوال(sleep_type, hartid))
		واپس SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_سیٹ_wakeup_events(PCS_WAKEUP_MSIP_نقاب | PCS_WAKEUP_MEIP_نقاب, hartid);
	اگر (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_سیٹ(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_سیٹ_command(LIGHT_SLEEP_CMD, hartid);
	} ورنہ اگر (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_سیٹ_reset_سمتیہ((ulong)ae350_enable_coherency_warmboot, hartid);
		اگر (rc)
			واپس SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_سیٹ_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	واپس SBI_ENOTSUPP;
}

ساکن ثابت ساخت sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_آغاز = ae350_hart_آغاز,
	.hart_stop = ae350_hart_stop,
};

ساکن صحیح hsm_andes_atcsmu_probe(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح poff, rc;
	u64 addr, سائز;

	/* Need to find the parent for the address property  */
	poff = fdt_والد_انحراف(fdt, nodeoff);
	اگر (poff < 0)
		واپس SBI_EINVAL;

	rc = fdt_get_گرہ_addr_سائز(fdt, poff, 0, &addr, &سائز);
	اگر (rc < 0 || !addr || !سائز)
		واپس SBI_ENODEV;

	اگر (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_جڑ_جوڑو_memrange(
			(غیرعلامتی طویل)addr, (غیرعلامتی طویل)سائز, صفحہ_سائز,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		اگر (rc)
			واپس rc;
	}

	atcsmu_بنیاد = addr;
	sbi_hsm_سیٹ_device(&hsm_andes_atcsmu);
	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت hsm_andes_atcsmu_مطابقت[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ثابت ساخت fdt_driver fdt_hsm_andes_atcsmu = {
	.مطابقت_جدول = hsm_andes_atcsmu_مطابقت,
	.init = hsm_andes_atcsmu_probe,
};
