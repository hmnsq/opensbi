/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cmo_helper.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#అన్తర్భావ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

నిర్మాణం sifive_tmc0 {
	గుర్తులేని దీర్ఘ reg;
	నిర్మాణం sbi_dlist కణుపు;
	u32 id;
};

స్థిర SBI_LIST_తల(tmc0_list);
స్థిర గుర్తులేని దీర్ఘ tmc0_స్థానభ్రంశం;

#నిర్వచించు tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), నిర్మాణం sifive_tmc0 *, tmc0_స్థానభ్రంశం)

#నిర్వచించు tmc0_ptr_సమితి(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), నిర్మాణం sifive_tmc0 *, tmc0_స్థానభ్రంశం, (__tmc0))

/* TMC.PGPREP */
#నిర్వచించు SIFIVE_TMC_PGPREP_OFF			0x0
#నిర్వచించు SIFIVE_TMC_PGPREP_ENA_REQ		బిట్టు(31)
#నిర్వచించు SIFIVE_TMC_PGPREP_ENA_ACK		బిట్టు(30)
#నిర్వచించు SIFIVE_TMC_PGPREP_DIS_REQ		బిట్టు(29)
#నిర్వచించు SIFIVE_TMC_PGPREP_DIS_ACK		బిట్టు(28)
#నిర్వచించు SIFIVE_TMC_PGPREP_CLFPNOTQ		బిట్టు(18)
#నిర్వచించు SIFIVE_TMC_PGPREP_PMCENAERR		బిట్టు(17)
#నిర్వచించు SIFIVE_TMC_PGPREP_PMCDENY		బిట్టు(16)
#నిర్వచించు SIFIVE_TMC_PGPREP_BUSERR		బిట్టు(15)
#నిర్వచించు SIFIVE_TMC_PGPREP_WAKE_DETECT		బిట్టు(12)
#నిర్వచించు SIFIVE_TMC_PGPREP_అంతర్గత_ABORT	బిట్టు(2)
#నిర్వచించు SIFIVE_TMC_PGPREP_ENARSP		(SIFIVE_TMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_TMC_PGPREP_PMCENAERR | \
						 SIFIVE_TMC_PGPREP_PMCDENY | \
						 SIFIVE_TMC_PGPREP_BUSERR | \
						 SIFIVE_TMC_PGPREP_WAKE_DETECT)

/* TMC.PG */
#నిర్వచించు SIFIVE_TMC_PG_OFF			0x4
#నిర్వచించు SIFIVE_TMC_PG_ENA_REQ			బిట్టు(31)
#నిర్వచించు SIFIVE_TMC_PG_ENA_ACK			బిట్టు(30)
#నిర్వచించు SIFIVE_TMC_PG_DIS_REQ			బిట్టు(29)
#నిర్వచించు SIFIVE_TMC_PG_DIS_ACK			బిట్టు(28)
#నిర్వచించు SIFIVE_TMC_PG_PMC_ENA_ERR		బిట్టు(17)
#నిర్వచించు SIFIVE_TMC_PG_PMC_DENY			బిట్టు(16)
#నిర్వచించు SIFIVE_TMC_PG_BUS_ERR			బిట్టు(15)
#నిర్వచించు SIFIVE_TMC_PG_MASTNOTQ			బిట్టు(14)
#నిర్వచించు SIFIVE_TMC_PG_WARM_RESET		బిట్టు(1)
#నిర్వచించు SIFIVE_TMC_PG_ENARSP			(SIFIVE_TMC_PG_PMC_ENA_ERR | \
						 SIFIVE_TMC_PG_PMC_DENY | \
						 SIFIVE_TMC_PG_BUS_ERR | \
						 SIFIVE_TMC_PG_MASTNOTQ)

/* TMC.RESUMEPC */
#నిర్వచించు SIFIVE_TMC_RESUMEPC_LO			0x10
#నిర్వచించు SIFIVE_TMC_RESUMEPC_HI			0x14

/* TMC.WAKEMASK */
#నిర్వచించు SIFIVE_TMC_WAKE_ముసుగు_OFF		0x20
#నిర్వచించు SIFIVE_TMC_WAKE_ముసుగు_WREQ		బిట్టు(31)
#నిర్వచించు SIFIVE_TMC_WAKE_ముసుగు_ACK		బిట్టు(30)

పూర్ణాంకం sifive_tmc0_సమితి_wakemask_enareq(u32 hartid)
{
	నిర్మాణం sbi_scratch *scratch = sbi_hartid_గమ్యం_scratch(hartid);
	నిర్మాణం sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	గుర్తులేని దీర్ఘ addr;
	u32 v;

	ఒకవేళ (!tmc0)
		తిరిగి SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_ముసుగు_OFF;
	v = readl((శూన్య *)addr);
	writel(v | SIFIVE_TMC_WAKE_ముసుగు_WREQ, (శూన్య *)addr);

	అయితే (!(readl((శూన్య *)addr) & SIFIVE_TMC_WAKE_ముసుగు_ACK));

	తిరిగి SBI_OK;
}

శూన్య sifive_tmc0_సమితి_wakemask_disreq(u32 hartid)
{
	నిర్మాణం sbi_scratch *scratch = sbi_hartid_గమ్యం_scratch(hartid);
	నిర్మాణం sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	గుర్తులేని దీర్ఘ addr;
	u32 v;

	ఒకవేళ (!tmc0)
		తిరిగి;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_ముసుగు_OFF;
	v = readl((శూన్య *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_ముసుగు_WREQ, (శూన్య *)addr);

	అయితే (readl((శూన్య *)addr) & SIFIVE_TMC_WAKE_ముసుగు_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	నిర్మాణం sbi_scratch *scratch = sbi_hartid_గమ్యం_scratch(hartid);
	నిర్మాణం sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	గుర్తులేని దీర్ఘ addr;
	u32 v;

	ఒకవేళ (!tmc0)
		తిరిగి false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((శూన్య *)addr);
	ఒకవేళ (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		తిరిగి false;

	తిరిగి true;
}

స్థిర శూన్య sifive_tmc0_సమితి_resumepc(physical_addr_t addr)
{
	నిర్మాణం sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (శూన్య *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#ఒకవేళ __riscv_xlen > 32
	writel((u32)(addr >> 32), (శూన్య *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#అయితే_ముగింపు
}

స్థిర u32 sifive_tmc0_సమితి_pgprep_enareq(శూన్య)
{
	నిర్మాణం sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	గుర్తులేని దీర్ఘ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((శూన్య *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (శూన్య *)reg);
	అయితే (!(readl((శూన్య *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((శూన్య *)reg);
	తిరిగి v & SIFIVE_TMC_PGPREP_అంతర్గత_ABORT;
}

స్థిర శూన్య sifive_tmc0_సమితి_pgprep_disreq(శూన్య)
{
	నిర్మాణం sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	గుర్తులేని దీర్ఘ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((శూన్య *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (శూన్య *)reg);
	అయితే (!(readl((శూన్య *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

స్థిర u32 sifive_tmc0_get_pgprep_enarsp(శూన్య)
{
	నిర్మాణం sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	గుర్తులేని దీర్ఘ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((శూన్య *)reg);

	తిరిగి v & SIFIVE_TMC_PGPREP_ENARSP;
}

స్థిర శూన్య sifive_tmc0_సమితి_pg_enareq(శూన్య)
{
	నిర్మాణం sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	గుర్తులేని దీర్ఘ reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((శూన్య *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (శూన్య *)reg);
}

స్థిర పూర్ణాంకం sifive_tmc0_prep(శూన్య)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	ఒకవేళ (!tmc0_ptr_get(scratch))
		తిరిగి SBI_ENODEV;

	rc = sifive_tmc0_సమితి_pgprep_enareq();
	ఒకవేళ (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		వెళ్ళు fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	ఒకవేళ (rc) {
		sifive_tmc0_సమితి_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		వెళ్ళు fail;
	}

	sifive_tmc0_సమితి_resumepc(scratch->warmboot_addr);

	తిరిగి SBI_OK;

fail:
	తిరిగి SBI_EFAIL;
}

స్థిర పూర్ణాంకం sifive_tmc0_enter(శూన్య)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	ఒకవేళ (fdt_cmo_వ్యక్తిగత_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	ఒకవేళ (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		వెళ్ళు fail;
	}

	ఒకవేళ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_సమితి_pg_enareq();
		అయితే (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_సమితి_pgprep_disreq();
	తిరిగి rc;
}

స్థిర పూర్ణాంకం sifive_tmc0_tile_pg(శూన్య)
{
	పూర్ణాంకం rc;

	rc = sifive_tmc0_prep();
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి sifive_tmc0_enter();
}

స్థిర పూర్ణాంకం sifive_tmc0_ప్రారంభం(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_గమ్యం_hartindex(hartid), true);

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం sifive_tmc0_stop(శూన్య)
{
	గుర్తులేని దీర్ఘ mie = csr_read(CSR_MIE);
	పూర్ణాంకం rc;
	/* Set IPI as wake up source */
	csr_సమితి(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	ఒకవేళ (rc) {
		csr_write(CSR_MIE, mie);
		తిరిగి rc;
	}

	తిరిగి SBI_OK;
}

స్థిర నిర్మాణం sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_ప్రారంభం = sifive_tmc0_ప్రారంభం,
	.hart_stop = sifive_tmc0_stop,
};

స్థిర పూర్ణాంకం sifive_tmc0_bind_cpu(నిర్మాణం sifive_tmc0 *tmc0)
{
	స్థిరరాశి శూన్య *fdt = fdt_get_చిరునామా();
	నిర్మాణం sbi_scratch *scratch;
	పూర్ణాంకం cpus_off, cpu_off, rc;
	స్థిరరాశి fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_మార్గం_స్థానభ్రంశం(fdt, "/cpus");
	ఒకవేళ (cpus_off < 0)
		తిరిగి SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		ఒకవేళ (rc)
			కొనసాగు;

		scratch = sbi_hartid_గమ్యం_scratch(hartid);
		ఒకవేళ (!scratch)
			కొనసాగు;

		val = fdt_getprop(fdt, cpu_off, "power-domains", ఖాళీ);
		ఒకవేళ (!val)
			తిరిగి SBI_ENOENT;

		ఒకవేళ (tmc0->id == fdt32_గమ్యం_cpu(val[0])) {
			tmc0_ptr_సమితి(scratch, tmc0);
			తిరిగి SBI_OK;
		}
	}

	తిరిగి SBI_ENODEV;
}

స్థిర పూర్ణాంకం sifive_tmc0_probe(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	నిర్మాణం sifive_tmc0 *tmc0;
	u64 addr;
	పూర్ణాంకం rc;

	ఒకవేళ (!tmc0_స్థానభ్రంశం) {
		tmc0_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(నిర్మాణం sifive_tmc0 *);
		ఒకవేళ (!tmc0_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;

		sbi_hsm_సమితి_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(పరిమాణం(*tmc0));
	ఒకవేళ (!tmc0)
		తిరిగి SBI_ENOMEM;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, ఖాళీ);
	ఒకవేళ (rc)
		వెళ్ళు free_tmc0;

	tmc0->reg = (గుర్తులేని దీర్ఘ)addr;
	tmc0->id = fdt_get_phandle(fdt_get_చిరునామా(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	ఒకవేళ (rc)
		వెళ్ళు free_tmc0;

	తిరిగి SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	తిరిగి rc;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sifive_tmc0_సరిపోలిక[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_hsm_sifive_tmc0 = {
	.సరిపోలిక_పట్టిక = sifive_tmc0_సరిపోలిక,
	.init = sifive_tmc0_probe,
};
