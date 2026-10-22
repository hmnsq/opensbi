/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <andes/andes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_init.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ atcsmu_ਅਧਾਰ;

ਖਾਲੀ atcsmu_ਸੈੱਟ_wakeup_events(u32 events, u32 hartid)
{
	writel_relaxed(events, (ਅੱਖਰ *)atcsmu_ਅਧਾਰ + PCSm_WE_ਵਿੱਥ(hartid));
}

bool atcsmu_support_sleep_ਬਹੁਲਕ(u32 sleep_type, u32 hartid)
{
	u32 pcs_cfg;
	u32 ਨਕਾਬ;
	ਅਚਲ ਅੱਖਰ *sleep_ਬਹੁਲਕ;

	pcs_cfg = readl_relaxed((ਅੱਖਰ *)atcsmu_ਅਧਾਰ + PCSm_CFG_ਵਿੱਥ(hartid));
	ਚੋਣ (sleep_type) {
	ਹਾਲਤ SBI_SUSP_AE350_LIGHT_SLEEP:
		ਨਕਾਬ = PCS_CFG_LIGHT_SLEEP;
		sleep_ਬਹੁਲਕ = "light sleep";
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SUSP_SLEEP_TYPE_SUSPEND:
		ਨਕਾਬ = PCS_CFG_DEEP_SLEEP;
		sleep_ਬਹੁਲਕ = "deep sleep";
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ false;
	}

	ਜੇ (!ਕੱਢੋ_ਖੇਤਰ(pcs_cfg, ਨਕਾਬ)) {
		sbi_printf("ATCSMU: hart%d (PCS%d) does not support %s mode\n",
			   hartid, hartid + 3, sleep_ਬਹੁਲਕ);
		ਵਾਪਸ false;
	}

	ਵਾਪਸ true;
}

ਖਾਲੀ atcsmu_ਸੈੱਟ_command(u32 pcs_ctl, u32 hartid)
{
	writel(pcs_ctl, (ਅੱਖਰ *)atcsmu_ਅਧਾਰ + PCSm_CTL_ਵਿੱਥ(hartid));
}

ਪੂਰਨ atcsmu_ਸੈੱਟ_reset_ਸਦਿਸ਼(u64 wakeup_addr, u32 hartid)
{
	u32 vec_lo;
	u32 vec_hi;
	u64 reset_ਸਦਿਸ਼;

	writel((u32)wakeup_addr, (ਅੱਖਰ *)atcsmu_ਅਧਾਰ + HARTn_RESET_VEC_LO(hartid));
	writel((u32)(wakeup_addr >> 32), (ਅੱਖਰ *)atcsmu_ਅਧਾਰ + HARTn_RESET_VEC_HI(hartid));
	vec_lo = readl((ਅੱਖਰ *)atcsmu_ਅਧਾਰ + HARTn_RESET_VEC_LO(hartid));
	vec_hi = readl((ਅੱਖਰ *)atcsmu_ਅਧਾਰ + HARTn_RESET_VEC_HI(hartid));
	reset_ਸਦਿਸ਼ = (u64)vec_hi << 32 | vec_lo;
	ਜੇ (reset_ਸਦਿਸ਼ != wakeup_addr) {
		sbi_printf("ATCSMU: hart%d (PCS%d): failed to program the reset vector\n",
			   hartid, hartid + 3);
		ਵਾਪਸ SBI_EFAIL;
	}

	ਵਾਪਸ SBI_OK;
}

u32 atcsmu_get_sleep_type(u32 hartid)
{
	ਵਾਪਸ readl_relaxed((ਅੱਖਰ *)atcsmu_ਅਧਾਰ + PCSm_SCRATCH_ਵਿੱਥ(hartid));
}

ਖਾਲੀ atcsmu_write_scratch(u32 ਮੁੱਲ)
{
	writel_relaxed(ਮੁੱਲ, (ਅੱਖਰ *)atcsmu_ਅਧਾਰ + SCRATCH_PAD_ਵਿੱਥ);
}

u32 atcsmu_read_scratch(ਖਾਲੀ)
{
	ਵਾਪਸ readl_relaxed((ਅੱਖਰ *)atcsmu_ਅਧਾਰ + SCRATCH_PAD_ਵਿੱਥ);
}

bool atcsmu_hart_is_sleep(ਖਾਲੀ *opaque)
{
	ਬਣਤਰ atcsmu_sleep_arg *arg = opaque;

	u32 pcs_ਸਥਿਤੀ = readl_relaxed((ਅੱਖਰ *)atcsmu_ਅਧਾਰ +
				       PCSm_ਸਥਿਤੀ_ਵਿੱਥ(arg->hartid));
	u32 pd_ਸਥਿਤੀ = arg->deep_sleep ? PD_ਸਥਿਤੀ_DEEP_SLEEP :
					  PD_ਸਥਿਤੀ_LIGHT_SLEEP;

	ਵਾਪਸ ਕੱਢੋ_ਖੇਤਰ(pcs_ਸਥਿਤੀ, PD_TYPE_ਨਕਾਬ) == PD_TYPE_SLEEP &&
	       ਕੱਢੋ_ਖੇਤਰ(pcs_ਸਥਿਤੀ, PD_ਸਥਿਤੀ_ਨਕਾਬ) == pd_ਸਥਿਤੀ;
}

ਸਥਿਰ ਪੂਰਨ ae350_hart_ਸ਼ੁਰੂ(u32 hartid, ulong saddr)
{
	u32 hartindex = sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid);

	ਵਾਪਸ sbi_ipi_raw_send(hartindex, false);
}

ਸਥਿਰ ਪੂਰਨ ae350_hart_stop(ਖਾਲੀ)
{
	u32 hartid = current_hartid();
	u32 sleep_type = atcsmu_get_sleep_type(hartid);
	ਪੂਰਨ rc;

	/*
	 * For Andes AX25MP, the hart0 shares power domain with the last level
	 * cache. Instead of turning it off, it should fall through and jump to
	 * warmboot_addr.
	 */
	ਜੇ (is_andes(25) && hartid == 0)
		ਵਾਪਸ SBI_ENOTSUPP;

	ਜੇ (!atcsmu_support_sleep_ਬਹੁਲਕ(sleep_type, hartid))
		ਵਾਪਸ SBI_ENOTSUPP;

	/* Prevent the core leaving the WFI mode unexpectedly */
	csr_write(CSR_MIE, 0);

	atcsmu_ਸੈੱਟ_wakeup_events(PCS_WAKEUP_MSIP_ਨਕਾਬ | PCS_WAKEUP_MEIP_ਨਕਾਬ, hartid);
	ਜੇ (sleep_type == SBI_SUSP_AE350_LIGHT_SLEEP) {
		/* Clock-gated only: needs MSI or MEI set to resume past the WFI */
		csr_ਸੈੱਟ(CSR_MIE, MIP_MSIP | MIP_MEIP);
		atcsmu_ਸੈੱਟ_command(LIGHT_SLEEP_CMD, hartid);
	} ਨਹੀਂਤਾਂ ਜੇ (sleep_type == SBI_SUSP_SLEEP_TYPE_SUSPEND) {
		/* Power-gated: SMU wakes it via cold reset, interrupts not needed */
		rc = atcsmu_ਸੈੱਟ_reset_ਸਦਿਸ਼((ulong)ae350_enable_coherency_warmboot, hartid);
		ਜੇ (rc)
			ਵਾਪਸ SBI_EFAIL;

		ae350_non_ret_save(sbi_scratch_thishart_ptr());
		atcsmu_ਸੈੱਟ_command(DEEP_SLEEP_CMD, hartid);
	}

	ae350_disable_coherency();
	wfi();

	/* Light sleep resumes here */
	ae350_enable_coherency();

	ਵਾਪਸ SBI_ENOTSUPP;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_hsm_device hsm_andes_atcsmu = {
	.name = "andes_atcsmu",
	.hart_ਸ਼ੁਰੂ = ae350_hart_ਸ਼ੁਰੂ,
	.hart_stop = ae350_hart_stop,
};

ਸਥਿਰ ਪੂਰਨ hsm_andes_atcsmu_probe(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ poff, rc;
	u64 addr, ਮਾਪ;

	/* Need to find the parent for the address property  */
	poff = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, nodeoff);
	ਜੇ (poff < 0)
		ਵਾਪਸ SBI_EINVAL;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, poff, 0, &addr, &ਮਾਪ);
	ਜੇ (rc < 0 || !addr || !ਮਾਪ)
		ਵਾਪਸ SBI_ENODEV;

	ਜੇ (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SMEPMP)) {
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(
			(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)addr, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਮਾਪ, ਪੰਨਾ_ਮਾਪ,
			SBI_DOMAIN_MEMREGION_MMIO |
			SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	atcsmu_ਅਧਾਰ = addr;
	sbi_hsm_ਸੈੱਟ_device(&hsm_andes_atcsmu);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ hsm_andes_atcsmu_ਮੇਲ[] = {
	{ .compatible = "andestech,atcsmu-hsm" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_hsm_andes_atcsmu = {
	.ਮੇਲ_ਸਾਰਣੀ = hsm_andes_atcsmu_ਮੇਲ,
	.init = hsm_andes_atcsmu_probe,
};
