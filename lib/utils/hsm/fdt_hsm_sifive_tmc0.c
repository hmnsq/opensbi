/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cmo_helper.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#அந்தர்பாவ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

கட்டமைப்பு sifive_tmc0 {
	unsigned long reg;
	கட்டமைப்பு sbi_dlist முனை;
	u32 id;
};

நிலையான SBI_LIST_HEAD(tmc0_list);
நிலையான unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), கட்டமைப்பு sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), கட்டமைப்பு sifive_tmc0 *, tmc0_offset, (__tmc0))

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

முழு sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	கட்டமைப்பு sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	எனில் (!tmc0)
		திரும்பு SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((வெறுமை *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (வெறுமை *)addr);

	போது (!(readl((வெறுமை *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	திரும்பு SBI_OK;
}

வெறுமை sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	கட்டமைப்பு sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	எனில் (!tmc0)
		திரும்பு;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((வெறுமை *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (வெறுமை *)addr);

	போது (readl((வெறுமை *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	கட்டமைப்பு sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	எனில் (!tmc0)
		திரும்பு false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((வெறுமை *)addr);
	எனில் (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		திரும்பு false;

	திரும்பு true;
}

நிலையான வெறுமை sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	கட்டமைப்பு sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (வெறுமை *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#எனில் __riscv_xlen > 32
	writel((u32)(addr >> 32), (வெறுமை *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

நிலையான u32 sifive_tmc0_set_pgprep_enareq(வெறுமை)
{
	கட்டமைப்பு sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((வெறுமை *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (வெறுமை *)reg);
	போது (!(readl((வெறுமை *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((வெறுமை *)reg);
	திரும்பு v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

நிலையான வெறுமை sifive_tmc0_set_pgprep_disreq(வெறுமை)
{
	கட்டமைப்பு sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((வெறுமை *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (வெறுமை *)reg);
	போது (!(readl((வெறுமை *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

நிலையான u32 sifive_tmc0_get_pgprep_enarsp(வெறுமை)
{
	கட்டமைப்பு sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((வெறுமை *)reg);

	திரும்பு v & SIFIVE_TMC_PGPREP_ENARSP;
}

நிலையான வெறுமை sifive_tmc0_set_pg_enareq(வெறுமை)
{
	கட்டமைப்பு sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((வெறுமை *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (வெறுமை *)reg);
}

நிலையான முழு sifive_tmc0_prep(வெறுமை)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	எனில் (!tmc0_ptr_get(scratch))
		திரும்பு SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	எனில் (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		செல் fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	எனில் (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		செல் fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	திரும்பு SBI_OK;

fail:
	திரும்பு SBI_EFAIL;
}

நிலையான முழு sifive_tmc0_enter(வெறுமை)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	எனில் (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	எனில் (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		செல் fail;
	}

	எனில் (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		போது (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	திரும்பு rc;
}

நிலையான முழு sifive_tmc0_tile_pg(வெறுமை)
{
	முழு rc;

	rc = sifive_tmc0_prep();
	எனில் (rc)
		திரும்பு rc;

	திரும்பு sifive_tmc0_enter();
}

நிலையான முழு sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	திரும்பு SBI_OK;
}

நிலையான முழு sifive_tmc0_stop(வெறுமை)
{
	unsigned long mie = csr_read(CSR_MIE);
	முழு rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	எனில் (rc) {
		csr_write(CSR_MIE, mie);
		திரும்பு rc;
	}

	திரும்பு SBI_OK;
}

நிலையான கட்டமைப்பு sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

நிலையான முழு sifive_tmc0_bind_cpu(கட்டமைப்பு sifive_tmc0 *tmc0)
{
	மாறிலி வெறுமை *fdt = fdt_get_address();
	கட்டமைப்பு sbi_scratch *scratch;
	முழு cpus_off, cpu_off, rc;
	மாறிலி fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	எனில் (cpus_off < 0)
		திரும்பு SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		எனில் (rc)
			தொடர்;

		scratch = sbi_hartid_to_scratch(hartid);
		எனில் (!scratch)
			தொடர்;

		val = fdt_getprop(fdt, cpu_off, "power-domains", வெற்று);
		எனில் (!val)
			திரும்பு SBI_ENOENT;

		எனில் (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			திரும்பு SBI_OK;
		}
	}

	திரும்பு SBI_ENODEV;
}

நிலையான முழு sifive_tmc0_probe(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	கட்டமைப்பு sifive_tmc0 *tmc0;
	u64 addr;
	முழு rc;

	எனில் (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(கட்டமைப்பு sifive_tmc0 *);
		எனில் (!tmc0_offset)
			திரும்பு SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(அளவு(*tmc0));
	எனில் (!tmc0)
		திரும்பு SBI_ENOMEM;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, வெற்று);
	எனில் (rc)
		செல் free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	எனில் (rc)
		செல் free_tmc0;

	திரும்பு SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	திரும்பு rc;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
