/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cmo_helper.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#অন্তর্ভাব <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

কাঠামো sifive_tmc0 {
	unsigned long reg;
	কাঠামো sbi_dlist গ্রন্থি;
	u32 id;
};

স্থির SBI_LIST_শীর্ষ(tmc0_list);
স্থির unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), কাঠামো sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), কাঠামো sifive_tmc0 *, tmc0_offset, (__tmc0))

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

পূর্ণসংখ্যা sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	কাঠামো sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	কাঠামো sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	যদি (!tmc0)
		ফেরত SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((শূন্য *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (শূন্য *)addr);

	যখন (!(readl((শূন্য *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	ফেরত SBI_OK;
}

শূন্য sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	কাঠামো sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	কাঠামো sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	যদি (!tmc0)
		ফেরত;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((শূন্য *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (শূন্য *)addr);

	যখন (readl((শূন্য *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	কাঠামো sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	কাঠামো sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	যদি (!tmc0)
		ফেরত false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((শূন্য *)addr);
	যদি (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		ফেরত false;

	ফেরত true;
}

স্থির শূন্য sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	কাঠামো sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (শূন্য *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#যদি __riscv_xlen > 32
	writel((u32)(addr >> 32), (শূন্য *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

স্থির u32 sifive_tmc0_set_pgprep_enareq(শূন্য)
{
	কাঠামো sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((শূন্য *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (শূন্য *)reg);
	যখন (!(readl((শূন্য *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((শূন্য *)reg);
	ফেরত v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

স্থির শূন্য sifive_tmc0_set_pgprep_disreq(শূন্য)
{
	কাঠামো sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((শূন্য *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (শূন্য *)reg);
	যখন (!(readl((শূন্য *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

স্থির u32 sifive_tmc0_get_pgprep_enarsp(শূন্য)
{
	কাঠামো sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((শূন্য *)reg);

	ফেরত v & SIFIVE_TMC_PGPREP_ENARSP;
}

স্থির শূন্য sifive_tmc0_set_pg_enareq(শূন্য)
{
	কাঠামো sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((শূন্য *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (শূন্য *)reg);
}

স্থির পূর্ণসংখ্যা sifive_tmc0_prep(শূন্য)
{
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	যদি (!tmc0_ptr_get(scratch))
		ফেরত SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	যদি (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		যাও fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	যদি (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		যাও fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	ফেরত SBI_OK;

fail:
	ফেরত SBI_EFAIL;
}

স্থির পূর্ণসংখ্যা sifive_tmc0_enter(শূন্য)
{
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	যদি (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	যদি (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		যাও fail;
	}

	যদি (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		যখন (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	ফেরত rc;
}

স্থির পূর্ণসংখ্যা sifive_tmc0_tile_pg(শূন্য)
{
	পূর্ণসংখ্যা rc;

	rc = sifive_tmc0_prep();
	যদি (rc)
		ফেরত rc;

	ফেরত sifive_tmc0_enter();
}

স্থির পূর্ণসংখ্যা sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	ফেরত SBI_OK;
}

স্থির পূর্ণসংখ্যা sifive_tmc0_stop(শূন্য)
{
	unsigned long mie = csr_read(CSR_MIE);
	পূর্ণসংখ্যা rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	যদি (rc) {
		csr_write(CSR_MIE, mie);
		ফেরত rc;
	}

	ফেরত SBI_OK;
}

স্থির কাঠামো sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

স্থির পূর্ণসংখ্যা sifive_tmc0_bind_cpu(কাঠামো sifive_tmc0 *tmc0)
{
	ধ্রুবক শূন্য *fdt = fdt_get_address();
	কাঠামো sbi_scratch *scratch;
	পূর্ণসংখ্যা cpus_off, cpu_off, rc;
	ধ্রুবক fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_off < 0)
		ফেরত SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		যদি (rc)
			চালু;

		scratch = sbi_hartid_to_scratch(hartid);
		যদি (!scratch)
			চালু;

		val = fdt_getprop(fdt, cpu_off, "power-domains", ফাঁকা);
		যদি (!val)
			ফেরত SBI_ENOENT;

		যদি (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			ফেরত SBI_OK;
		}
	}

	ফেরত SBI_ENODEV;
}

স্থির পূর্ণসংখ্যা sifive_tmc0_probe(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, ধ্রুবক কাঠামো fdt_match *match)
{
	কাঠামো sifive_tmc0 *tmc0;
	u64 addr;
	পূর্ণসংখ্যা rc;

	যদি (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(কাঠামো sifive_tmc0 *);
		যদি (!tmc0_offset)
			ফেরত SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(আকার(*tmc0));
	যদি (!tmc0)
		ফেরত SBI_ENOMEM;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &addr, ফাঁকা);
	যদি (rc)
		যাও free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	যদি (rc)
		যাও free_tmc0;

	ফেরত SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	ফেরত rc;
}

স্থির ধ্রুবক কাঠামো fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
