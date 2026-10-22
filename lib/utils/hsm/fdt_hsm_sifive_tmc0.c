/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cmo_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

ସଂରଚନା sifive_tmc0 {
	unsigned long reg;
	ସଂରଚନା sbi_dlist ଗଣ୍ଠି;
	u32 id;
};

ସ୍ଥିର SBI_LIST_HEAD(tmc0_list);
ସ୍ଥିର unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), ସଂରଚନା sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), ସଂରଚନା sifive_tmc0 *, tmc0_offset, (__tmc0))

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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ସଂରଚନା sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ଯଦି (!tmc0)
		ଫେରନ୍ତୁ SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((ଶୂନ୍ୟ *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (ଶୂନ୍ୟ *)addr);

	ଯେତେବେଳେ (!(readl((ଶୂନ୍ୟ *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	ଫେରନ୍ତୁ SBI_OK;
}

ଶୂନ୍ୟ sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ସଂରଚନା sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ଯଦି (!tmc0)
		ଫେରନ୍ତୁ;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((ଶୂନ୍ୟ *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (ଶୂନ୍ୟ *)addr);

	ଯେତେବେଳେ (readl((ଶୂନ୍ୟ *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ସଂରଚନା sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ଯଦି (!tmc0)
		ଫେରନ୍ତୁ false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((ଶୂନ୍ୟ *)addr);
	ଯଦି (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		ଫେରନ୍ତୁ false;

	ଫେରନ୍ତୁ true;
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	ସଂରଚନା sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (ଶୂନ୍ୟ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#ଯଦି __riscv_xlen > 32
	writel((u32)(addr >> 32), (ଶୂନ୍ୟ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

ସ୍ଥିର u32 sifive_tmc0_set_pgprep_enareq(ଶୂନ୍ୟ)
{
	ସଂରଚନା sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ଶୂନ୍ୟ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (ଶୂନ୍ୟ *)reg);
	ଯେତେବେଳେ (!(readl((ଶୂନ୍ୟ *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((ଶୂନ୍ୟ *)reg);
	ଫେରନ୍ତୁ v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_tmc0_set_pgprep_disreq(ଶୂନ୍ୟ)
{
	ସଂରଚନା sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ଶୂନ୍ୟ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (ଶୂନ୍ୟ *)reg);
	ଯେତେବେଳେ (!(readl((ଶୂନ୍ୟ *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

ସ୍ଥିର u32 sifive_tmc0_get_pgprep_enarsp(ଶୂନ୍ୟ)
{
	ସଂରଚନା sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ଶୂନ୍ୟ *)reg);

	ଫେରନ୍ତୁ v & SIFIVE_TMC_PGPREP_ENARSP;
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_tmc0_set_pg_enareq(ଶୂନ୍ୟ)
{
	ସଂରଚନା sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((ଶୂନ୍ୟ *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (ଶୂନ୍ୟ *)reg);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_tmc0_prep(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	ଯଦି (!tmc0_ptr_get(scratch))
		ଫେରନ୍ତୁ SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	ଯଦି (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		ଯାଅ fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	ଯଦି (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		ଯାଅ fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	ଫେରନ୍ତୁ SBI_OK;

fail:
	ଫେରନ୍ତୁ SBI_EFAIL;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_tmc0_enter(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	ଯଦି (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	ଯଦି (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		ଯାଅ fail;
	}

	ଯଦି (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		ଯେତେବେଳେ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_tmc0_tile_pg(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = sifive_tmc0_prep();
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଫେରନ୍ତୁ sifive_tmc0_enter();
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_tmc0_stop(ଶୂନ୍ୟ)
{
	unsigned long mie = csr_read(CSR_MIE);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	ଯଦି (rc) {
		csr_write(CSR_MIE, mie);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ସଂରଚନା sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_tmc0_bind_cpu(ସଂରଚନା sifive_tmc0 *tmc0)
{
	ଅଚଳ ଶୂନ୍ୟ *fdt = fdt_get_address();
	ସଂରଚନା sbi_scratch *scratch;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpus_off, cpu_off, rc;
	ଅଚଳ fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	ଯଦି (cpus_off < 0)
		ଫେରନ୍ତୁ SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		ଯଦି (rc)
			ଜାରି;

		scratch = sbi_hartid_to_scratch(hartid);
		ଯଦି (!scratch)
			ଜାରି;

		val = fdt_getprop(fdt, cpu_off, "power-domains", ଖାଲି);
		ଯଦି (!val)
			ଫେରନ୍ତୁ SBI_ENOENT;

		ଯଦି (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			ଫେରନ୍ତୁ SBI_OK;
		}
	}

	ଫେରନ୍ତୁ SBI_ENODEV;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_tmc0_probe(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ସଂରଚନା sifive_tmc0 *tmc0;
	u64 addr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(ସଂରଚନା sifive_tmc0 *);
		ଯଦି (!tmc0_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(ଆକାର(*tmc0));
	ଯଦି (!tmc0)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, ଖାଲି);
	ଯଦି (rc)
		ଯାଅ free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	ଯଦି (rc)
		ଯାଅ free_tmc0;

	ଫେରନ୍ତୁ SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
