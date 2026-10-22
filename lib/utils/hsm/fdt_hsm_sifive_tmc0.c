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

ساخت sifive_tmc0 {
	غیرعلامتی طویل reg;
	ساخت sbi_dlist گرہ;
	u32 id;
};

ساکن SBI_LIST_سر(tmc0_list);
ساکن غیرعلامتی طویل tmc0_انحراف;

#تعریف_کرو tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), ساخت sifive_tmc0 *, tmc0_انحراف)

#تعریف_کرو tmc0_ptr_سیٹ(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), ساخت sifive_tmc0 *, tmc0_انحراف, (__tmc0))

/* TMC.PGPREP */
#تعریف_کرو SIFIVE_TMC_PGPREP_OFF			0x0
#تعریف_کرو SIFIVE_TMC_PGPREP_ENA_REQ		ثنائی_ہندسہ(31)
#تعریف_کرو SIFIVE_TMC_PGPREP_ENA_ACK		ثنائی_ہندسہ(30)
#تعریف_کرو SIFIVE_TMC_PGPREP_DIS_REQ		ثنائی_ہندسہ(29)
#تعریف_کرو SIFIVE_TMC_PGPREP_DIS_ACK		ثنائی_ہندسہ(28)
#تعریف_کرو SIFIVE_TMC_PGPREP_CLFPNOTQ		ثنائی_ہندسہ(18)
#تعریف_کرو SIFIVE_TMC_PGPREP_PMCENAERR		ثنائی_ہندسہ(17)
#تعریف_کرو SIFIVE_TMC_PGPREP_PMCDENY		ثنائی_ہندسہ(16)
#تعریف_کرو SIFIVE_TMC_PGPREP_BUSERR		ثنائی_ہندسہ(15)
#تعریف_کرو SIFIVE_TMC_PGPREP_WAKE_DETECT		ثنائی_ہندسہ(12)
#تعریف_کرو SIFIVE_TMC_PGPREP_اندرونی_ABORT	ثنائی_ہندسہ(2)
#تعریف_کرو SIFIVE_TMC_PGPREP_ENARSP		(SIFIVE_TMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_TMC_PGPREP_PMCENAERR | \
						 SIFIVE_TMC_PGPREP_PMCDENY | \
						 SIFIVE_TMC_PGPREP_BUSERR | \
						 SIFIVE_TMC_PGPREP_WAKE_DETECT)

/* TMC.PG */
#تعریف_کرو SIFIVE_TMC_PG_OFF			0x4
#تعریف_کرو SIFIVE_TMC_PG_ENA_REQ			ثنائی_ہندسہ(31)
#تعریف_کرو SIFIVE_TMC_PG_ENA_ACK			ثنائی_ہندسہ(30)
#تعریف_کرو SIFIVE_TMC_PG_DIS_REQ			ثنائی_ہندسہ(29)
#تعریف_کرو SIFIVE_TMC_PG_DIS_ACK			ثنائی_ہندسہ(28)
#تعریف_کرو SIFIVE_TMC_PG_PMC_ENA_ERR		ثنائی_ہندسہ(17)
#تعریف_کرو SIFIVE_TMC_PG_PMC_DENY			ثنائی_ہندسہ(16)
#تعریف_کرو SIFIVE_TMC_PG_BUS_ERR			ثنائی_ہندسہ(15)
#تعریف_کرو SIFIVE_TMC_PG_MASTNOTQ			ثنائی_ہندسہ(14)
#تعریف_کرو SIFIVE_TMC_PG_WARM_RESET		ثنائی_ہندسہ(1)
#تعریف_کرو SIFIVE_TMC_PG_ENARSP			(SIFIVE_TMC_PG_PMC_ENA_ERR | \
						 SIFIVE_TMC_PG_PMC_DENY | \
						 SIFIVE_TMC_PG_BUS_ERR | \
						 SIFIVE_TMC_PG_MASTNOTQ)

/* TMC.RESUMEPC */
#تعریف_کرو SIFIVE_TMC_RESUMEPC_LO			0x10
#تعریف_کرو SIFIVE_TMC_RESUMEPC_HI			0x14

/* TMC.WAKEMASK */
#تعریف_کرو SIFIVE_TMC_WAKE_نقاب_OFF		0x20
#تعریف_کرو SIFIVE_TMC_WAKE_نقاب_WREQ		ثنائی_ہندسہ(31)
#تعریف_کرو SIFIVE_TMC_WAKE_نقاب_ACK		ثنائی_ہندسہ(30)

صحیح sifive_tmc0_سیٹ_wakemask_enareq(u32 hartid)
{
	ساخت sbi_scratch *scratch = sbi_hartid_مقصد_scratch(hartid);
	ساخت sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	غیرعلامتی طویل addr;
	u32 v;

	اگر (!tmc0)
		واپس SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_نقاب_OFF;
	v = readl((خالی *)addr);
	writel(v | SIFIVE_TMC_WAKE_نقاب_WREQ, (خالی *)addr);

	جب (!(readl((خالی *)addr) & SIFIVE_TMC_WAKE_نقاب_ACK));

	واپس SBI_OK;
}

خالی sifive_tmc0_سیٹ_wakemask_disreq(u32 hartid)
{
	ساخت sbi_scratch *scratch = sbi_hartid_مقصد_scratch(hartid);
	ساخت sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	غیرعلامتی طویل addr;
	u32 v;

	اگر (!tmc0)
		واپس;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_نقاب_OFF;
	v = readl((خالی *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_نقاب_WREQ, (خالی *)addr);

	جب (readl((خالی *)addr) & SIFIVE_TMC_WAKE_نقاب_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	ساخت sbi_scratch *scratch = sbi_hartid_مقصد_scratch(hartid);
	ساخت sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	غیرعلامتی طویل addr;
	u32 v;

	اگر (!tmc0)
		واپس false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((خالی *)addr);
	اگر (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		واپس false;

	واپس true;
}

ساکن خالی sifive_tmc0_سیٹ_resumepc(physical_addr_t addr)
{
	ساخت sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (خالی *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#اگر __riscv_xlen > 32
	writel((u32)(addr >> 32), (خالی *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#اگر_ختم
}

ساکن u32 sifive_tmc0_سیٹ_pgprep_enareq(خالی)
{
	ساخت sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	غیرعلامتی طویل reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((خالی *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (خالی *)reg);
	جب (!(readl((خالی *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((خالی *)reg);
	واپس v & SIFIVE_TMC_PGPREP_اندرونی_ABORT;
}

ساکن خالی sifive_tmc0_سیٹ_pgprep_disreq(خالی)
{
	ساخت sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	غیرعلامتی طویل reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((خالی *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (خالی *)reg);
	جب (!(readl((خالی *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

ساکن u32 sifive_tmc0_get_pgprep_enarsp(خالی)
{
	ساخت sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	غیرعلامتی طویل reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((خالی *)reg);

	واپس v & SIFIVE_TMC_PGPREP_ENARSP;
}

ساکن خالی sifive_tmc0_سیٹ_pg_enareq(خالی)
{
	ساخت sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	غیرعلامتی طویل reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((خالی *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (خالی *)reg);
}

ساکن صحیح sifive_tmc0_prep(خالی)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	اگر (!tmc0_ptr_get(scratch))
		واپس SBI_ENODEV;

	rc = sifive_tmc0_سیٹ_pgprep_enareq();
	اگر (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		جاؤ fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	اگر (rc) {
		sifive_tmc0_سیٹ_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		جاؤ fail;
	}

	sifive_tmc0_سیٹ_resumepc(scratch->warmboot_addr);

	واپس SBI_OK;

fail:
	واپس SBI_EFAIL;
}

ساکن صحیح sifive_tmc0_enter(خالی)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	اگر (fdt_cmo_نجی_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	اگر (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		جاؤ fail;
	}

	اگر (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_سیٹ_pg_enareq();
		جب (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_سیٹ_pgprep_disreq();
	واپس rc;
}

ساکن صحیح sifive_tmc0_tile_pg(خالی)
{
	صحیح rc;

	rc = sifive_tmc0_prep();
	اگر (rc)
		واپس rc;

	واپس sifive_tmc0_enter();
}

ساکن صحیح sifive_tmc0_آغاز(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_مقصد_hartindex(hartid), true);

	واپس SBI_OK;
}

ساکن صحیح sifive_tmc0_stop(خالی)
{
	غیرعلامتی طویل mie = csr_read(CSR_MIE);
	صحیح rc;
	/* Set IPI as wake up source */
	csr_سیٹ(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	اگر (rc) {
		csr_write(CSR_MIE, mie);
		واپس rc;
	}

	واپس SBI_OK;
}

ساکن ساخت sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_آغاز = sifive_tmc0_آغاز,
	.hart_stop = sifive_tmc0_stop,
};

ساکن صحیح sifive_tmc0_bind_cpu(ساخت sifive_tmc0 *tmc0)
{
	ثابت خالی *fdt = fdt_get_پتہ();
	ساخت sbi_scratch *scratch;
	صحیح cpus_off, cpu_off, rc;
	ثابت fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_راستہ_انحراف(fdt, "/cpus");
	اگر (cpus_off < 0)
		واپس SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		اگر (rc)
			جاری;

		scratch = sbi_hartid_مقصد_scratch(hartid);
		اگر (!scratch)
			جاری;

		val = fdt_getprop(fdt, cpu_off, "power-domains", تہی);
		اگر (!val)
			واپس SBI_ENOENT;

		اگر (tmc0->id == fdt32_مقصد_cpu(val[0])) {
			tmc0_ptr_سیٹ(scratch, tmc0);
			واپس SBI_OK;
		}
	}

	واپس SBI_ENODEV;
}

ساکن صحیح sifive_tmc0_probe(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	ساخت sifive_tmc0 *tmc0;
	u64 addr;
	صحیح rc;

	اگر (!tmc0_انحراف) {
		tmc0_انحراف = sbi_scratch_alloc_type_انحراف(ساخت sifive_tmc0 *);
		اگر (!tmc0_انحراف)
			واپس SBI_ENOMEM;

		sbi_hsm_سیٹ_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(حجم(*tmc0));
	اگر (!tmc0)
		واپس SBI_ENOMEM;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, تہی);
	اگر (rc)
		جاؤ free_tmc0;

	tmc0->reg = (غیرعلامتی طویل)addr;
	tmc0->id = fdt_get_phandle(fdt_get_پتہ(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	اگر (rc)
		جاؤ free_tmc0;

	واپس SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	واپس rc;
}

ساکن ثابت ساخت fdt_مطابقت sifive_tmc0_مطابقت[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ثابت ساخت fdt_driver fdt_hsm_sifive_tmc0 = {
	.مطابقت_جدول = sifive_tmc0_مطابقت,
	.init = sifive_tmc0_probe,
};
