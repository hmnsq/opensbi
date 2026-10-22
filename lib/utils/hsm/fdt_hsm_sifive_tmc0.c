/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cmo_helper.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#અન્તર્ભાવ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

સંરચના sifive_tmc0 {
	unsigned long reg;
	સંરચના sbi_dlist ગાંઠ;
	u32 id;
};

સ્થિર SBI_LIST_HEAD(tmc0_list);
સ્થિર unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), સંરચના sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), સંરચના sifive_tmc0 *, tmc0_offset, (__tmc0))

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

પૂર્ણાંક sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	સંરચના sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	સંરચના sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	જો (!tmc0)
		પાછા SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((શૂન્ય *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (શૂન્ય *)addr);

	જ્યારે (!(readl((શૂન્ય *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	પાછા SBI_OK;
}

શૂન્ય sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	સંરચના sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	સંરચના sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	જો (!tmc0)
		પાછા;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((શૂન્ય *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (શૂન્ય *)addr);

	જ્યારે (readl((શૂન્ય *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	સંરચના sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	સંરચના sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	જો (!tmc0)
		પાછા false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((શૂન્ય *)addr);
	જો (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		પાછા false;

	પાછા true;
}

સ્થિર શૂન્ય sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	સંરચના sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (શૂન્ય *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#જો __riscv_xlen > 32
	writel((u32)(addr >> 32), (શૂન્ય *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

સ્થિર u32 sifive_tmc0_set_pgprep_enareq(શૂન્ય)
{
	સંરચના sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((શૂન્ય *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (શૂન્ય *)reg);
	જ્યારે (!(readl((શૂન્ય *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((શૂન્ય *)reg);
	પાછા v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

સ્થિર શૂન્ય sifive_tmc0_set_pgprep_disreq(શૂન્ય)
{
	સંરચના sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((શૂન્ય *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (શૂન્ય *)reg);
	જ્યારે (!(readl((શૂન્ય *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

સ્થિર u32 sifive_tmc0_get_pgprep_enarsp(શૂન્ય)
{
	સંરચના sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((શૂન્ય *)reg);

	પાછા v & SIFIVE_TMC_PGPREP_ENARSP;
}

સ્થિર શૂન્ય sifive_tmc0_set_pg_enareq(શૂન્ય)
{
	સંરચના sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((શૂન્ય *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (શૂન્ય *)reg);
}

સ્થિર પૂર્ણાંક sifive_tmc0_prep(શૂન્ય)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	જો (!tmc0_ptr_get(scratch))
		પાછા SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	જો (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		જાઓ fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	જો (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		જાઓ fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	પાછા SBI_OK;

fail:
	પાછા SBI_EFAIL;
}

સ્થિર પૂર્ણાંક sifive_tmc0_enter(શૂન્ય)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	જો (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	જો (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		જાઓ fail;
	}

	જો (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		જ્યારે (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	પાછા rc;
}

સ્થિર પૂર્ણાંક sifive_tmc0_tile_pg(શૂન્ય)
{
	પૂર્ણાંક rc;

	rc = sifive_tmc0_prep();
	જો (rc)
		પાછા rc;

	પાછા sifive_tmc0_enter();
}

સ્થિર પૂર્ણાંક sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક sifive_tmc0_stop(શૂન્ય)
{
	unsigned long mie = csr_read(CSR_MIE);
	પૂર્ણાંક rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	જો (rc) {
		csr_write(CSR_MIE, mie);
		પાછા rc;
	}

	પાછા SBI_OK;
}

સ્થિર સંરચના sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

સ્થિર પૂર્ણાંક sifive_tmc0_bind_cpu(સંરચના sifive_tmc0 *tmc0)
{
	અચલ શૂન્ય *fdt = fdt_get_address();
	સંરચના sbi_scratch *scratch;
	પૂર્ણાંક cpus_off, cpu_off, rc;
	અચલ fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	જો (cpus_off < 0)
		પાછા SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		જો (rc)
			ચાલુ;

		scratch = sbi_hartid_to_scratch(hartid);
		જો (!scratch)
			ચાલુ;

		val = fdt_getprop(fdt, cpu_off, "power-domains", ખાલી);
		જો (!val)
			પાછા SBI_ENOENT;

		જો (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			પાછા SBI_OK;
		}
	}

	પાછા SBI_ENODEV;
}

સ્થિર પૂર્ણાંક sifive_tmc0_probe(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	સંરચના sifive_tmc0 *tmc0;
	u64 addr;
	પૂર્ણાંક rc;

	જો (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(સંરચના sifive_tmc0 *);
		જો (!tmc0_offset)
			પાછા SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(કદ(*tmc0));
	જો (!tmc0)
		પાછા SBI_ENOMEM;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, ખાલી);
	જો (rc)
		જાઓ free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	જો (rc)
		જાઓ free_tmc0;

	પાછા SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	પાછા rc;
}

સ્થિર અચલ સંરચના fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

અચલ સંરચના fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
