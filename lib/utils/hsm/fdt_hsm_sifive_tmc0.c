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

ಸಂರಚನಾ sifive_tmc0 {
	unsigned long reg;
	ಸಂರಚನಾ sbi_dlist ಗಂಟು;
	u32 id;
};

ಸ್ಥಿರ SBI_LIST_HEAD(tmc0_list);
ಸ್ಥಿರ unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), ಸಂರಚನಾ sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), ಸಂರಚನಾ sifive_tmc0 *, tmc0_offset, (__tmc0))

/* TMC.PGPREP */
#define SIFIVE_TMC_PGPREP_OFF			0x0
#define SIFIVE_TMC_PGPREP_ENA_REQ		BIT(31)
#define SIFIVE_TMC_PGPREP_ENA_ACK		BIT(30)
#define SIFIVE_TMC_PGPREP_DIS_REQ		BIT(29)
#define SIFIVE_TMC_PGPREP_DIS_ACK		BIT(28)
#define SIFIVE_TMC_PGPREP_CLFPNOTQ		BIT(18)
#define SIFIVE_TMC_PGPREP_PMCENAERR		BIT(17)
#define SIFIVE_TMC_PGPREP_PMCDENY		BIT(16)
#define SIFIVE_TMC_PGPREP_BUSERR		BIT(15)
#define SIFIVE_TMC_PGPREP_WAKE_DETECT		BIT(12)
#define SIFIVE_TMC_PGPREP_INTERNAL_ABORT	BIT(2)
#define SIFIVE_TMC_PGPREP_ENARSP		(SIFIVE_TMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_TMC_PGPREP_PMCENAERR | \
						 SIFIVE_TMC_PGPREP_PMCDENY | \
						 SIFIVE_TMC_PGPREP_BUSERR | \
						 SIFIVE_TMC_PGPREP_WAKE_DETECT)

/* TMC.PG */
#define SIFIVE_TMC_PG_OFF			0x4
#define SIFIVE_TMC_PG_ENA_REQ			BIT(31)
#define SIFIVE_TMC_PG_ENA_ACK			BIT(30)
#define SIFIVE_TMC_PG_DIS_REQ			BIT(29)
#define SIFIVE_TMC_PG_DIS_ACK			BIT(28)
#define SIFIVE_TMC_PG_PMC_ENA_ERR		BIT(17)
#define SIFIVE_TMC_PG_PMC_DENY			BIT(16)
#define SIFIVE_TMC_PG_BUS_ERR			BIT(15)
#define SIFIVE_TMC_PG_MASTNOTQ			BIT(14)
#define SIFIVE_TMC_PG_WARM_RESET		BIT(1)
#define SIFIVE_TMC_PG_ENARSP			(SIFIVE_TMC_PG_PMC_ENA_ERR | \
						 SIFIVE_TMC_PG_PMC_DENY | \
						 SIFIVE_TMC_PG_BUS_ERR | \
						 SIFIVE_TMC_PG_MASTNOTQ)

/* TMC.RESUMEPC */
#define SIFIVE_TMC_RESUMEPC_LO			0x10
#define SIFIVE_TMC_RESUMEPC_HI			0x14

/* TMC.WAKEMASK */
#define SIFIVE_TMC_WAKE_MASK_OFF		0x20
#define SIFIVE_TMC_WAKE_MASK_WREQ		BIT(31)
#define SIFIVE_TMC_WAKE_MASK_ACK		BIT(30)

ಪೂರ್ಣಾಙ್ಕ sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ಸಂರಚನಾ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ಯದಿ (!tmc0)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((ಶೂನ್ಯ *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (ಶೂನ್ಯ *)addr);

	ಯಾವತ್ (!(readl((ಶೂನ್ಯ *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಶೂನ್ಯ sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ಸಂರಚನಾ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ಯದಿ (!tmc0)
		ಪ್ರತ್ಯಾಗಮ;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((ಶೂನ್ಯ *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (ಶೂನ್ಯ *)addr);

	ಯಾವತ್ (readl((ಶೂನ್ಯ *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ಸಂರಚನಾ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ಯದಿ (!tmc0)
		ಪ್ರತ್ಯಾಗಮ false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((ಶೂನ್ಯ *)addr);
	ಯದಿ (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		ಪ್ರತ್ಯಾಗಮ false;

	ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	ಸಂರಚನಾ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (ಶೂನ್ಯ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#ಯದಿ __riscv_xlen > 32
	writel((u32)(addr >> 32), (ಶೂನ್ಯ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

ಸ್ಥಿರ u32 sifive_tmc0_set_pgprep_enareq(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (ಶೂನ್ಯ *)reg);
	ಯಾವತ್ (!(readl((ಶೂನ್ಯ *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((ಶೂನ್ಯ *)reg);
	ಪ್ರತ್ಯಾಗಮ v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_tmc0_set_pgprep_disreq(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (ಶೂನ್ಯ *)reg);
	ಯಾವತ್ (!(readl((ಶೂನ್ಯ *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

ಸ್ಥಿರ u32 sifive_tmc0_get_pgprep_enarsp(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ಶೂನ್ಯ *)reg);

	ಪ್ರತ್ಯಾಗಮ v & SIFIVE_TMC_PGPREP_ENARSP;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_tmc0_set_pg_enareq(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((ಶೂನ್ಯ *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (ಶೂನ್ಯ *)reg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_tmc0_prep(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	ಯದಿ (!tmc0_ptr_get(scratch))
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	ಯದಿ (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		ಗಚ್ಛ fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	ಯದಿ (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		ಗಚ್ಛ fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;

fail:
	ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_tmc0_enter(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	ಯದಿ (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	ಯದಿ (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		ಗಚ್ಛ fail;
	}

	ಯದಿ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		ಯಾವತ್ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_tmc0_tile_pg(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = sifive_tmc0_prep();
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಪ್ರತ್ಯಾಗಮ sifive_tmc0_enter();
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_tmc0_stop(ಶೂನ್ಯ)
{
	unsigned long mie = csr_read(CSR_MIE);
	ಪೂರ್ಣಾಙ್ಕ rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	ಯದಿ (rc) {
		csr_write(CSR_MIE, mie);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_tmc0_bind_cpu(ಸಂರಚನಾ sifive_tmc0 *tmc0)
{
	ಧ್ರುವ ಶೂನ್ಯ *fdt = fdt_get_address();
	ಸಂರಚನಾ sbi_scratch *scratch;
	ಪೂರ್ಣಾಙ್ಕ cpus_off, cpu_off, rc;
	ಧ್ರುವ fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	ಯದಿ (cpus_off < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		ಯದಿ (rc)
			ಅನುವರ್ತ;

		scratch = sbi_hartid_to_scratch(hartid);
		ಯದಿ (!scratch)
			ಅನುವರ್ತ;

		val = fdt_getprop(fdt, cpu_off, "power-domains", ರಿಕ್ತ);
		ಯದಿ (!val)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

		ಯದಿ (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			ಪ್ರತ್ಯಾಗಮ SBI_OK;
		}
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_tmc0_probe(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಸಂರಚನಾ sifive_tmc0 *tmc0;
	u64 addr;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(ಸಂರಚನಾ sifive_tmc0 *);
		ಯದಿ (!tmc0_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(ಪರಿಮಾಣ(*tmc0));
	ಯದಿ (!tmc0)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (rc)
		ಗಚ್ಛ free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	ಯದಿ (rc)
		ಗಚ್ಛ free_tmc0;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
