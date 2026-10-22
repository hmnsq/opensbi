/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi_utils/cache/fdt_cmo_helper.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#شامل <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

ساختار sifive_tmc0 {
	unsigned long reg;
	ساختار sbi_dlist گره;
	u32 id;
};

ایستا SBI_LIST_HEAD(tmc0_list);
ایستا unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), ساختار sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), ساختار sifive_tmc0 *, tmc0_offset, (__tmc0))

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

صحیح sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	ساختار sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ساختار sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	اگر (!tmc0)
		بازگشت SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((تهی *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (تهی *)addr);

	وقتی (!(readl((تهی *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	بازگشت SBI_OK;
}

تهی sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	ساختار sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ساختار sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	اگر (!tmc0)
		بازگشت;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((تهی *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (تهی *)addr);

	وقتی (readl((تهی *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	ساختار sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ساختار sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	اگر (!tmc0)
		بازگشت false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((تهی *)addr);
	اگر (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		بازگشت false;

	بازگشت true;
}

ایستا تهی sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	ساختار sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (تهی *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#اگر __riscv_xlen > 32
	writel((u32)(addr >> 32), (تهی *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

ایستا u32 sifive_tmc0_set_pgprep_enareq(تهی)
{
	ساختار sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((تهی *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (تهی *)reg);
	وقتی (!(readl((تهی *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((تهی *)reg);
	بازگشت v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

ایستا تهی sifive_tmc0_set_pgprep_disreq(تهی)
{
	ساختار sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((تهی *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (تهی *)reg);
	وقتی (!(readl((تهی *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

ایستا u32 sifive_tmc0_get_pgprep_enarsp(تهی)
{
	ساختار sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((تهی *)reg);

	بازگشت v & SIFIVE_TMC_PGPREP_ENARSP;
}

ایستا تهی sifive_tmc0_set_pg_enareq(تهی)
{
	ساختار sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((تهی *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (تهی *)reg);
}

ایستا صحیح sifive_tmc0_prep(تهی)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	اگر (!tmc0_ptr_get(scratch))
		بازگشت SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	اگر (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		برو fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	اگر (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		برو fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	بازگشت SBI_OK;

fail:
	بازگشت SBI_EFAIL;
}

ایستا صحیح sifive_tmc0_enter(تهی)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	اگر (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	اگر (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		برو fail;
	}

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		وقتی (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	بازگشت rc;
}

ایستا صحیح sifive_tmc0_tile_pg(تهی)
{
	صحیح rc;

	rc = sifive_tmc0_prep();
	اگر (rc)
		بازگشت rc;

	بازگشت sifive_tmc0_enter();
}

ایستا صحیح sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	بازگشت SBI_OK;
}

ایستا صحیح sifive_tmc0_stop(تهی)
{
	unsigned long mie = csr_read(CSR_MIE);
	صحیح rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	اگر (rc) {
		csr_write(CSR_MIE, mie);
		بازگشت rc;
	}

	بازگشت SBI_OK;
}

ایستا ساختار sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

ایستا صحیح sifive_tmc0_bind_cpu(ساختار sifive_tmc0 *tmc0)
{
	ثابت تهی *fdt = fdt_get_address();
	ساختار sbi_scratch *scratch;
	صحیح cpus_off, cpu_off, rc;
	ثابت fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	اگر (cpus_off < 0)
		بازگشت SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		اگر (rc)
			ادامه;

		scratch = sbi_hartid_to_scratch(hartid);
		اگر (!scratch)
			ادامه;

		val = fdt_getprop(fdt, cpu_off, "power-domains", پوچ);
		اگر (!val)
			بازگشت SBI_ENOENT;

		اگر (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			بازگشت SBI_OK;
		}
	}

	بازگشت SBI_ENODEV;
}

ایستا صحیح sifive_tmc0_probe(ثابت تهی *fdt, صحیح nodeoff, ثابت ساختار fdt_match *match)
{
	ساختار sifive_tmc0 *tmc0;
	u64 addr;
	صحیح rc;

	اگر (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(ساختار sifive_tmc0 *);
		اگر (!tmc0_offset)
			بازگشت SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(اندازه(*tmc0));
	اگر (!tmc0)
		بازگشت SBI_ENOMEM;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &addr, پوچ);
	اگر (rc)
		برو free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	اگر (rc)
		برو free_tmc0;

	بازگشت SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	بازگشت rc;
}

ایستا ثابت ساختار fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ثابت ساختار fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
