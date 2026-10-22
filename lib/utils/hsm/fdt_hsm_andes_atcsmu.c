/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#அந்தர்பாவ <andes/andes.h>
#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_init.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

நிலையான unsigned long atcsmu_base;

வெறுமை atcsmu_set_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (எழுத்து *)atcsmu_base + PCSm_WE_OFFSET(hartid));
}

bool atcsmu_support_sleep_mode(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 mask;
	மாறிலி எழுத்து *sleep_mode;

	pcs_cfg = readl_relaxed((எழுத்து *)atcsmu_base + PCSm_CFG_OFFSET(hartid));
	தேர்வு (sleep_type) {
	நிலை SBI_SUSP_AE350_LIGHT_SLEEP:
		mask = PCS_CFG_LIGHT_SLEEP;
		sleep_mode = "light sleep";
		நிறுத்து;
	நிலை SBI_SUSP_SLEEP_TYPE_SUSPEND:
		mask = PCS_CFG_DEEP_SLEEP;
		sleep_mode = "deep sleep";
		நிறுத்து;
	இயல்பு:
		திரும்பு false;
	}

	எனில் (!EXTRACT_FIELD(pcs_cfg, mask)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_mode);
		திரும்பு false;
	}

	திரும்பு true;
}

வெறுமை atcsmu_set_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (எழுத்து *)atcsmu_base + PCSm_CTL_OFFSET(hartid));
}

முழு atcsmu_set_reset_vector(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_vector;

	writel((u32)wakeup_addr, (எழுத்து *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (எழுத்து *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((எழுத்து *)atcsmu_base + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((எழுத்து *)atcsmu_base + HARTn_RESET_VEC_HI(hartid));
	reset_vector = (u64)vec_hi << 32 | vec_lo;
	எனில் (reset_vector != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		திரும்பு SBI_EFAIL;
	}

	திரும்பு SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	திரும்பு readl_relaxed((எழுத்து *)atcsmu_base + PCSm_SCRATCH_OFFSET(hartid));
}

வெறுமை atcsmu_write_scratch(u32 மதிப்பு)
{
	writel_relaxed(மதிப்பு, (எழுத்து *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

u32 atcsmu_read_scratch(வெறுமை)
{
	திரும்பு readl_relaxed((எழுத்து *)atcsmu_base + SCRATCH_PAD_OFFSET);
}

bool atcsmu_hart_is_sleep(வெறுமை *opaque)
{
	கட்டமைப்பு atcsmu_sleep_arg *arg = opaque;

	u32 pcs_status = readl_relaxed((எழுத்து *)atcsmu_base +
				       PCSm_STATUS_OFFSET(arg->hartid));
	u32 pd_status = arg->deep_sleep ? PD_STATUS_DEEP_SLEEP :
					  PD_STATUS_LIGHT_SLEEP;

	திரும்பு EXTRACT_FIELD(pcs_status, PD_TYPE_MASK) == PD_TYPE_SLEEP &&
	       EXTRACT_FIELD(pcs_status, PD_STATUS_MASK) == pd_status;
}

நிலையான முழு ae350_hart_start(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_to_hartindex(hartid);

	திரும்பு sbi_ipi_raw_send(hartindex, false);
}

நிலையான முழு ae350_hart_stop(வெறுமை)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	முழு rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	எனில் (is_andes(25) && hartid == 0)
		திரும்பு SBI_ENOTSUPP;

	எனில் (!atcsmu_support_sleep_mode(sleep_type, hartid))
		திரும்பு SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_set_wakeup_events(PCS_WAKEUP_MSIP_MASK | PCS_WAKEUP_MEIP_MASK, hartid);
	எனில் (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_set(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_set_command(LIGHT_SLEEP_CMD, hartid);
	} இல்லையெனில் எனில் (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_set_reset_vector((ulong)ae350_enable_coherency_warmboot, hartid);
		எனில் (rc)
			திரும்பு SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_set_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	திரும்பு SBI_ENOTSUPP;
}

நிலையான மாறிலி கட்டமைப்பு sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_start = ae350_hart_start,
	.hart_stop = ae350_hart_stop,
};

நிலையான முழு hsm_andes_atcsmu_probe(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு poff, rc;
	u64 addr, size;

	/* Need to find the parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	எனில் (poff < 0)
		திரும்பு SBI_EINVAL;

	rc = fdt_get_முனை_addr_size(fdt, poff, 0, &addr, &size);
	எனில் (rc < 0 || !addr || !size)
		திரும்பு SBI_ENODEV;

	எனில் (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_வேர்_add_memrange(
			(unsigned long)addr, (unsigned long)size, PAGE_SIZE,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		எனில் (rc)
			திரும்பு rc;
	}

	atcsmu_base = addr;
	sbi_hsm_set_device(&hsm_andes_atcsmu);
	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match hsm_andes_atcsmu_match[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_hsm_andes_atcsmu = {
	.match_table = hsm_andes_atcsmu_match,
	.init = hsm_andes_atcsmu_probe,
};
