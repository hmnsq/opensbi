/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cmo_helper.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#अन्तर्भाव <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

संरचना sifive_tmc0 {
	unsigned long reg;
	संरचना sbi_dlist ग्रन्थिः;
	u32 id;
};

स्थिर SBI_LIST_HEAD(tmc0_list);
स्थिर unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), संरचना sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), संरचना sifive_tmc0 *, tmc0_offset, (__tmc0))

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

पूर्णाङ्क sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	संरचना sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	संरचना sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	जइ (!tmc0)
		प्रत्यागम SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((शून्य *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (शून्य *)addr);

	जाव (!(readl((शून्य *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	प्रत्यागम SBI_OK;
}

शून्य sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	संरचना sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	संरचना sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	जइ (!tmc0)
		प्रत्यागम;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((शून्य *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (शून्य *)addr);

	जाव (readl((शून्य *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	संरचना sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	संरचना sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	जइ (!tmc0)
		प्रत्यागम false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((शून्य *)addr);
	जइ (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		प्रत्यागम false;

	प्रत्यागम true;
}

स्थिर शून्य sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	संरचना sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (शून्य *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#जइ __riscv_xlen > 32
	writel((u32)(addr >> 32), (शून्य *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

स्थिर u32 sifive_tmc0_set_pgprep_enareq(शून्य)
{
	संरचना sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((शून्य *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (शून्य *)reg);
	जाव (!(readl((शून्य *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((शून्य *)reg);
	प्रत्यागम v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

स्थिर शून्य sifive_tmc0_set_pgprep_disreq(शून्य)
{
	संरचना sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((शून्य *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (शून्य *)reg);
	जाव (!(readl((शून्य *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

स्थिर u32 sifive_tmc0_get_pgprep_enarsp(शून्य)
{
	संरचना sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((शून्य *)reg);

	प्रत्यागम v & SIFIVE_TMC_PGPREP_ENARSP;
}

स्थिर शून्य sifive_tmc0_set_pg_enareq(शून्य)
{
	संरचना sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((शून्य *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (शून्य *)reg);
}

स्थिर पूर्णाङ्क sifive_tmc0_prep(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	जइ (!tmc0_ptr_get(scratch))
		प्रत्यागम SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	जइ (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		गच्छ fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	जइ (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		गच्छ fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	प्रत्यागम SBI_OK;

fail:
	प्रत्यागम SBI_EFAIL;
}

स्थिर पूर्णाङ्क sifive_tmc0_enter(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	जइ (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	जइ (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		गच्छ fail;
	}

	जइ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		जाव (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	प्रत्यागम rc;
}

स्थिर पूर्णाङ्क sifive_tmc0_tile_pg(शून्य)
{
	पूर्णाङ्क rc;

	rc = sifive_tmc0_prep();
	जइ (rc)
		प्रत्यागम rc;

	प्रत्यागम sifive_tmc0_enter();
}

स्थिर पूर्णाङ्क sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क sifive_tmc0_stop(शून्य)
{
	unsigned long mie = csr_read(CSR_MIE);
	पूर्णाङ्क rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	जइ (rc) {
		csr_write(CSR_MIE, mie);
		प्रत्यागम rc;
	}

	प्रत्यागम SBI_OK;
}

स्थिर संरचना sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

स्थिर पूर्णाङ्क sifive_tmc0_bind_cpu(संरचना sifive_tmc0 *tmc0)
{
	ध्रुव शून्य *fdt = fdt_get_address();
	संरचना sbi_scratch *scratch;
	पूर्णाङ्क cpus_off, cpu_off, rc;
	ध्रुव fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	जइ (cpus_off < 0)
		प्रत्यागम SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		जइ (rc)
			अनुवर्त;

		scratch = sbi_hartid_to_scratch(hartid);
		जइ (!scratch)
			अनुवर्त;

		val = fdt_getprop(fdt, cpu_off, "power-domains", रिक्त);
		जइ (!val)
			प्रत्यागम SBI_ENOENT;

		जइ (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			प्रत्यागम SBI_OK;
		}
	}

	प्रत्यागम SBI_ENODEV;
}

स्थिर पूर्णाङ्क sifive_tmc0_probe(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_match *match)
{
	संरचना sifive_tmc0 *tmc0;
	u64 addr;
	पूर्णाङ्क rc;

	जइ (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(संरचना sifive_tmc0 *);
		जइ (!tmc0_offset)
			प्रत्यागम SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(परिमाण(*tmc0));
	जइ (!tmc0)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	जइ (rc)
		गच्छ free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	जइ (rc)
		गच्छ free_tmc0;

	प्रत्यागम SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	प्रत्यागम rc;
}

स्थिर ध्रुव संरचना fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
