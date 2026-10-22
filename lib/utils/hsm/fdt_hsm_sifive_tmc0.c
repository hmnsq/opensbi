/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cmo_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

ರಚನೆ sifive_tmc0 {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg;
	ರಚನೆ sbi_dlist ಗಂಟು;
	u32 id;
};

ಸ್ಥಿರ SBI_LIST_ತಲೆ(tmc0_list);
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmc0_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), ರಚನೆ sifive_tmc0 *, tmc0_ಸ್ಥಾನಾಂತರ)

#ವ್ಯಾಖ್ಯಾನಿಸು tmc0_ptr_ಗಣ(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), ರಚನೆ sifive_tmc0 *, tmc0_ಸ್ಥಾನಾಂತರ, (__tmc0))

/* TMC.PGPREP */
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_OFF			0x0
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_ENA_REQ		ಬಿಟ್(31)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_ENA_ACK		ಬಿಟ್(30)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_DIS_REQ		ಬಿಟ್(29)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_DIS_ACK		ಬಿಟ್(28)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_CLFPNOTQ		ಬಿಟ್(18)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_PMCENAERR		ಬಿಟ್(17)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_PMCDENY		ಬಿಟ್(16)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_BUSERR		ಬಿಟ್(15)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_WAKE_DETECT		ಬಿಟ್(12)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_ಆಂತರಿಕ_ABORT	ಬಿಟ್(2)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PGPREP_ENARSP		(SIFIVE_TMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_TMC_PGPREP_PMCENAERR | \
						 SIFIVE_TMC_PGPREP_PMCDENY | \
						 SIFIVE_TMC_PGPREP_BUSERR | \
						 SIFIVE_TMC_PGPREP_WAKE_DETECT)

/* TMC.PG */
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_OFF			0x4
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_ENA_REQ			ಬಿಟ್(31)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_ENA_ACK			ಬಿಟ್(30)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_DIS_REQ			ಬಿಟ್(29)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_DIS_ACK			ಬಿಟ್(28)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_PMC_ENA_ERR		ಬಿಟ್(17)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_PMC_DENY			ಬಿಟ್(16)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_BUS_ERR			ಬಿಟ್(15)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_MASTNOTQ			ಬಿಟ್(14)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_WARM_RESET		ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_PG_ENARSP			(SIFIVE_TMC_PG_PMC_ENA_ERR | \
						 SIFIVE_TMC_PG_PMC_DENY | \
						 SIFIVE_TMC_PG_BUS_ERR | \
						 SIFIVE_TMC_PG_MASTNOTQ)

/* TMC.RESUMEPC */
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_RESUMEPC_LO			0x10
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_RESUMEPC_HI			0x14

/* TMC.WAKEMASK */
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_WAKE_ಮುಖವಾಡ_OFF		0x20
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_WAKE_ಮುಖವಾಡ_WREQ		ಬಿಟ್(31)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_TMC_WAKE_ಮುಖವಾಡ_ACK		ಬಿಟ್(30)

ಪೂರ್ಣಾಂಕ sifive_tmc0_ಗಣ_wakemask_enareq(u32 hartid)
{
	ರಚನೆ sbi_scratch *scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
	ರಚನೆ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr;
	u32 v;

	ಆದರೆ (!tmc0)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_ಮುಖವಾಡ_OFF;
	v = readl((ಶೂನ್ಯ *)addr);
	writel(v | SIFIVE_TMC_WAKE_ಮುಖವಾಡ_WREQ, (ಶೂನ್ಯ *)addr);

	ಆಗ (!(readl((ಶೂನ್ಯ *)addr) & SIFIVE_TMC_WAKE_ಮುಖವಾಡ_ACK));

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಶೂನ್ಯ sifive_tmc0_ಗಣ_wakemask_disreq(u32 hartid)
{
	ರಚನೆ sbi_scratch *scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
	ರಚನೆ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr;
	u32 v;

	ಆದರೆ (!tmc0)
		ಹಿಂತಿರುಗಿ;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_ಮುಖವಾಡ_OFF;
	v = readl((ಶೂನ್ಯ *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_ಮುಖವಾಡ_WREQ, (ಶೂನ್ಯ *)addr);

	ಆಗ (readl((ಶೂನ್ಯ *)addr) & SIFIVE_TMC_WAKE_ಮುಖವಾಡ_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	ರಚನೆ sbi_scratch *scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
	ರಚನೆ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr;
	u32 v;

	ಆದರೆ (!tmc0)
		ಹಿಂತಿರುಗಿ false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((ಶೂನ್ಯ *)addr);
	ಆದರೆ (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		ಹಿಂತಿರುಗಿ false;

	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_tmc0_ಗಣ_resumepc(physical_addr_t addr)
{
	ರಚನೆ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (ಶೂನ್ಯ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#ಆದರೆ __riscv_xlen > 32
	writel((u32)(addr >> 32), (ಶೂನ್ಯ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ u32 sifive_tmc0_ಗಣ_pgprep_enareq(ಶೂನ್ಯ)
{
	ರಚನೆ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (ಶೂನ್ಯ *)reg);
	ಆಗ (!(readl((ಶೂನ್ಯ *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((ಶೂನ್ಯ *)reg);
	ಹಿಂತಿರುಗಿ v & SIFIVE_TMC_PGPREP_ಆಂತರಿಕ_ABORT;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_tmc0_ಗಣ_pgprep_disreq(ಶೂನ್ಯ)
{
	ರಚನೆ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (ಶೂನ್ಯ *)reg);
	ಆಗ (!(readl((ಶೂನ್ಯ *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

ಸ್ಥಿರ u32 sifive_tmc0_get_pgprep_enarsp(ಶೂನ್ಯ)
{
	ರಚನೆ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)reg);

	ಹಿಂತಿರುಗಿ v & SIFIVE_TMC_PGPREP_ENARSP;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_tmc0_ಗಣ_pg_enareq(ಶೂನ್ಯ)
{
	ರಚನೆ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((ಶೂನ್ಯ *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (ಶೂನ್ಯ *)reg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_tmc0_prep(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	ಆದರೆ (!tmc0_ptr_get(scratch))
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	rc = sifive_tmc0_ಗಣ_pgprep_enareq();
	ಆದರೆ (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		ಹೋಗು fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	ಆದರೆ (rc) {
		sifive_tmc0_ಗಣ_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		ಹೋಗು fail;
	}

	sifive_tmc0_ಗಣ_resumepc(scratch->warmboot_addr);

	ಹಿಂತಿರುಗಿ SBI_OK;

fail:
	ಹಿಂತಿರುಗಿ SBI_EFAIL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_tmc0_enter(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	ಆದರೆ (fdt_cmo_ಖಾಸಗಿ_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	ಆದರೆ (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		ಹೋಗು fail;
	}

	ಆದರೆ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_ಗಣ_pg_enareq();
		ಆಗ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_ಗಣ_pgprep_disreq();
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_tmc0_tile_pg(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = sifive_tmc0_prep();
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ sifive_tmc0_enter();
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_tmc0_ಆರಂಭ(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_ಗಮ್ಯ_hartindex(hartid), true);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_tmc0_stop(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mie = csr_read(CSR_MIE);
	ಪೂರ್ಣಾಂಕ rc;
	/* Set IPI as wake up source */
	csr_ಗಣ(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	ಆದರೆ (rc) {
		csr_write(CSR_MIE, mie);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ರಚನೆ sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_ಆರಂಭ = sifive_tmc0_ಆರಂಭ,
	.hart_stop = sifive_tmc0_stop,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_tmc0_bind_cpu(ರಚನೆ sifive_tmc0 *tmc0)
{
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt = fdt_get_ವಿಳಾಸ();
	ರಚನೆ sbi_scratch *scratch;
	ಪೂರ್ಣಾಂಕ cpus_off, cpu_off, rc;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/cpus");
	ಆದರೆ (cpus_off < 0)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		ಆದರೆ (rc)
			ಮುಂದುವರಿ;

		scratch = sbi_hartid_ಗಮ್ಯ_scratch(hartid);
		ಆದರೆ (!scratch)
			ಮುಂದುವರಿ;

		val = fdt_getprop(fdt, cpu_off, "power-domains", ಖಾಲಿ);
		ಆದರೆ (!val)
			ಹಿಂತಿರುಗಿ SBI_ENOENT;

		ಆದರೆ (tmc0->id == fdt32_ಗಮ್ಯ_cpu(val[0])) {
			tmc0_ptr_ಗಣ(scratch, tmc0);
			ಹಿಂತಿರುಗಿ SBI_OK;
		}
	}

	ಹಿಂತಿರುಗಿ SBI_ENODEV;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_tmc0_probe(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ರಚನೆ sifive_tmc0 *tmc0;
	u64 addr;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (!tmc0_ಸ್ಥಾನಾಂತರ) {
		tmc0_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ರಚನೆ sifive_tmc0 *);
		ಆದರೆ (!tmc0_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;

		sbi_hsm_ಗಣ_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(ಗಾತ್ರ(*tmc0));
	ಆದರೆ (!tmc0)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &addr, ಖಾಲಿ);
	ಆದರೆ (rc)
		ಹೋಗು free_tmc0;

	tmc0->reg = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)addr;
	tmc0->id = fdt_get_phandle(fdt_get_ವಿಳಾಸ(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	ಆದರೆ (rc)
		ಹೋಗು free_tmc0;

	ಹಿಂತಿರುಗಿ SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sifive_tmc0_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_hsm_sifive_tmc0 = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sifive_tmc0_ಹೊಂದಿಕೆ,
	.init = sifive_tmc0_probe,
};
