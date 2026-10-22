/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi_utils/cache/fdt_cmo_helper.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#समावेश <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

काष्ठा sifive_tmc0 {
	अचिन्हित दीर्घ reg;
	काष्ठा sbi_dlist गाँठ;
	u32 id;
};

अटल SBI_LIST_शीर्ष(tmc0_list);
अटल अचिन्हित दीर्घ tmc0_offset;

#घोषणा tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), काष्ठा sifive_tmc0 *, tmc0_offset)

#घोषणा tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), काष्ठा sifive_tmc0 *, tmc0_offset, (__tmc0))

/* TMC.PGPREP */
#घोषणा SIFIVE_TMC_PGPREP_OFF			0x0
#घोषणा SIFIVE_TMC_PGPREP_ENA_REQ		BIT(31)
#घोषणा SIFIVE_TMC_PGPREP_ENA_ACK		BIT(30)
#घोषणा SIFIVE_TMC_PGPREP_DIS_REQ		BIT(29)
#घोषणा SIFIVE_TMC_PGPREP_DIS_ACK		BIT(28)
#घोषणा SIFIVE_TMC_PGPREP_CLFPNOTQ		BIT(18)
#घोषणा SIFIVE_TMC_PGPREP_PMCENAERR		BIT(17)
#घोषणा SIFIVE_TMC_PGPREP_PMCDENY		BIT(16)
#घोषणा SIFIVE_TMC_PGPREP_BUSERR		BIT(15)
#घोषणा SIFIVE_TMC_PGPREP_WAKE_DETECT		BIT(12)
#घोषणा SIFIVE_TMC_PGPREP_INTERNAL_ABORT	BIT(2)
#घोषणा SIFIVE_TMC_PGPREP_ENARSP		(SIFIVE_TMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_TMC_PGPREP_PMCENAERR | \
						 SIFIVE_TMC_PGPREP_PMCDENY | \
						 SIFIVE_TMC_PGPREP_BUSERR | \
						 SIFIVE_TMC_PGPREP_WAKE_DETECT)

/* TMC.PG */
#घोषणा SIFIVE_TMC_PG_OFF			0x4
#घोषणा SIFIVE_TMC_PG_ENA_REQ			BIT(31)
#घोषणा SIFIVE_TMC_PG_ENA_ACK			BIT(30)
#घोषणा SIFIVE_TMC_PG_DIS_REQ			BIT(29)
#घोषणा SIFIVE_TMC_PG_DIS_ACK			BIT(28)
#घोषणा SIFIVE_TMC_PG_PMC_ENA_ERR		BIT(17)
#घोषणा SIFIVE_TMC_PG_PMC_DENY			BIT(16)
#घोषणा SIFIVE_TMC_PG_BUS_ERR			BIT(15)
#घोषणा SIFIVE_TMC_PG_MASTNOTQ			BIT(14)
#घोषणा SIFIVE_TMC_PG_WARM_RESET		BIT(1)
#घोषणा SIFIVE_TMC_PG_ENARSP			(SIFIVE_TMC_PG_PMC_ENA_ERR | \
						 SIFIVE_TMC_PG_PMC_DENY | \
						 SIFIVE_TMC_PG_BUS_ERR | \
						 SIFIVE_TMC_PG_MASTNOTQ)

/* TMC.RESUMEPC */
#घोषणा SIFIVE_TMC_RESUMEPC_LO			0x10
#घोषणा SIFIVE_TMC_RESUMEPC_HI			0x14

/* TMC.WAKEMASK */
#घोषणा SIFIVE_TMC_WAKE_MASK_OFF		0x20
#घोषणा SIFIVE_TMC_WAKE_MASK_WREQ		BIT(31)
#घोषणा SIFIVE_TMC_WAKE_MASK_ACK		BIT(30)

पूर्णांक sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	काष्ठा sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	काष्ठा sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	अचिन्हित दीर्घ addr;
	u32 v;

	अगर (!tmc0)
		वापस SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((व्योम *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (व्योम *)addr);

	जबतक (!(readl((व्योम *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	वापस SBI_OK;
}

व्योम sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	काष्ठा sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	काष्ठा sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	अचिन्हित दीर्घ addr;
	u32 v;

	अगर (!tmc0)
		वापस;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((व्योम *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (व्योम *)addr);

	जबतक (readl((व्योम *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	काष्ठा sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	काष्ठा sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	अचिन्हित दीर्घ addr;
	u32 v;

	अगर (!tmc0)
		वापस false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((व्योम *)addr);
	अगर (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		वापस false;

	वापस true;
}

अटल व्योम sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	काष्ठा sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (व्योम *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#अगर __riscv_xlen > 32
	writel((u32)(addr >> 32), (व्योम *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#पूर्ण_अगर
}

अटल u32 sifive_tmc0_set_pgprep_enareq(व्योम)
{
	काष्ठा sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	अचिन्हित दीर्घ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((व्योम *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (व्योम *)reg);
	जबतक (!(readl((व्योम *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((व्योम *)reg);
	वापस v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

अटल व्योम sifive_tmc0_set_pgprep_disreq(व्योम)
{
	काष्ठा sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	अचिन्हित दीर्घ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((व्योम *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (व्योम *)reg);
	जबतक (!(readl((व्योम *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

अटल u32 sifive_tmc0_get_pgprep_enarsp(व्योम)
{
	काष्ठा sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	अचिन्हित दीर्घ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((व्योम *)reg);

	वापस v & SIFIVE_TMC_PGPREP_ENARSP;
}

अटल व्योम sifive_tmc0_set_pg_enareq(व्योम)
{
	काष्ठा sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	अचिन्हित दीर्घ reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((व्योम *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (व्योम *)reg);
}

अटल पूर्णांक sifive_tmc0_prep(व्योम)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	अगर (!tmc0_ptr_get(scratch))
		वापस SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	अगर (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		जाओ fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	अगर (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		जाओ fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	वापस SBI_OK;

fail:
	वापस SBI_EFAIL;
}

अटल पूर्णांक sifive_tmc0_enter(व्योम)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	अगर (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	अगर (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		जाओ fail;
	}

	अगर (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		जबतक (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	वापस rc;
}

अटल पूर्णांक sifive_tmc0_tile_pg(व्योम)
{
	पूर्णांक rc;

	rc = sifive_tmc0_prep();
	अगर (rc)
		वापस rc;

	वापस sifive_tmc0_enter();
}

अटल पूर्णांक sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	वापस SBI_OK;
}

अटल पूर्णांक sifive_tmc0_stop(व्योम)
{
	अचिन्हित दीर्घ mie = csr_read(CSR_MIE);
	पूर्णांक rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	अगर (rc) {
		csr_write(CSR_MIE, mie);
		वापस rc;
	}

	वापस SBI_OK;
}

अटल काष्ठा sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

अटल पूर्णांक sifive_tmc0_bind_cpu(काष्ठा sifive_tmc0 *tmc0)
{
	स्थिर व्योम *fdt = fdt_get_address();
	काष्ठा sbi_scratch *scratch;
	पूर्णांक cpus_off, cpu_off, rc;
	स्थिर fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	अगर (cpus_off < 0)
		वापस SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		अगर (rc)
			जारी;

		scratch = sbi_hartid_to_scratch(hartid);
		अगर (!scratch)
			जारी;

		val = fdt_getprop(fdt, cpu_off, "power-domains", शून्य);
		अगर (!val)
			वापस SBI_ENOENT;

		अगर (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			वापस SBI_OK;
		}
	}

	वापस SBI_ENODEV;
}

अटल पूर्णांक sifive_tmc0_probe(स्थिर व्योम *fdt, पूर्णांक nodeoff, स्थिर काष्ठा fdt_match *match)
{
	काष्ठा sifive_tmc0 *tmc0;
	u64 addr;
	पूर्णांक rc;

	अगर (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(काष्ठा sifive_tmc0 *);
		अगर (!tmc0_offset)
			वापस SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(माप(*tmc0));
	अगर (!tmc0)
		वापस SBI_ENOMEM;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, शून्य);
	अगर (rc)
		जाओ free_tmc0;

	tmc0->reg = (अचिन्हित दीर्घ)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	अगर (rc)
		जाओ free_tmc0;

	वापस SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	वापस rc;
}

अटल स्थिर काष्ठा fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
