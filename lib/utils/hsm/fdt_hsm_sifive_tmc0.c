/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cmo_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

ਬਣਤਰ sifive_tmc0 {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg;
	ਬਣਤਰ sbi_dlist ਗੰਢ;
	u32 id;
};

ਸਥਿਰ SBI_LIST_ਸਿਰ(tmc0_list);
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmc0_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), ਬਣਤਰ sifive_tmc0 *, tmc0_ਵਿੱਥ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ tmc0_ptr_ਸੈੱਟ(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), ਬਣਤਰ sifive_tmc0 *, tmc0_ਵਿੱਥ, (__tmc0))

/* TMC.PGPREP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_OFF			0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_ENA_REQ		ਦੋਆਧਾਰੀ_ਅੰਕ(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_ENA_ACK		ਦੋਆਧਾਰੀ_ਅੰਕ(30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_DIS_REQ		ਦੋਆਧਾਰੀ_ਅੰਕ(29)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_DIS_ACK		ਦੋਆਧਾਰੀ_ਅੰਕ(28)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_CLFPNOTQ		ਦੋਆਧਾਰੀ_ਅੰਕ(18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_PMCENAERR		ਦੋਆਧਾਰੀ_ਅੰਕ(17)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_PMCDENY		ਦੋਆਧਾਰੀ_ਅੰਕ(16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_BUSERR		ਦੋਆਧਾਰੀ_ਅੰਕ(15)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_WAKE_DETECT		ਦੋਆਧਾਰੀ_ਅੰਕ(12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_ਅੰਦਰੂਨੀ_ABORT	ਦੋਆਧਾਰੀ_ਅੰਕ(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PGPREP_ENARSP		(SIFIVE_TMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_TMC_PGPREP_PMCENAERR | \
						 SIFIVE_TMC_PGPREP_PMCDENY | \
						 SIFIVE_TMC_PGPREP_BUSERR | \
						 SIFIVE_TMC_PGPREP_WAKE_DETECT)

/* TMC.PG */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_OFF			0x4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_ENA_REQ			ਦੋਆਧਾਰੀ_ਅੰਕ(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_ENA_ACK			ਦੋਆਧਾਰੀ_ਅੰਕ(30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_DIS_REQ			ਦੋਆਧਾਰੀ_ਅੰਕ(29)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_DIS_ACK			ਦੋਆਧਾਰੀ_ਅੰਕ(28)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_PMC_ENA_ERR		ਦੋਆਧਾਰੀ_ਅੰਕ(17)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_PMC_DENY			ਦੋਆਧਾਰੀ_ਅੰਕ(16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_BUS_ERR			ਦੋਆਧਾਰੀ_ਅੰਕ(15)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_MASTNOTQ			ਦੋਆਧਾਰੀ_ਅੰਕ(14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_WARM_RESET		ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_PG_ENARSP			(SIFIVE_TMC_PG_PMC_ENA_ERR | \
						 SIFIVE_TMC_PG_PMC_DENY | \
						 SIFIVE_TMC_PG_BUS_ERR | \
						 SIFIVE_TMC_PG_MASTNOTQ)

/* TMC.RESUMEPC */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_RESUMEPC_LO			0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_RESUMEPC_HI			0x14

/* TMC.WAKEMASK */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_WAKE_ਨਕਾਬ_OFF		0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_WAKE_ਨਕਾਬ_WREQ		ਦੋਆਧਾਰੀ_ਅੰਕ(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_TMC_WAKE_ਨਕਾਬ_ACK		ਦੋਆਧਾਰੀ_ਅੰਕ(30)

ਪੂਰਨ sifive_tmc0_ਸੈੱਟ_wakemask_enareq(u32 hartid)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
	ਬਣਤਰ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr;
	u32 v;

	ਜੇ (!tmc0)
		ਵਾਪਸ SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_ਨਕਾਬ_OFF;
	v = readl((ਖਾਲੀ *)addr);
	writel(v | SIFIVE_TMC_WAKE_ਨਕਾਬ_WREQ, (ਖਾਲੀ *)addr);

	ਜਦੋਂ (!(readl((ਖਾਲੀ *)addr) & SIFIVE_TMC_WAKE_ਨਕਾਬ_ACK));

	ਵਾਪਸ SBI_OK;
}

ਖਾਲੀ sifive_tmc0_ਸੈੱਟ_wakemask_disreq(u32 hartid)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
	ਬਣਤਰ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr;
	u32 v;

	ਜੇ (!tmc0)
		ਵਾਪਸ;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_ਨਕਾਬ_OFF;
	v = readl((ਖਾਲੀ *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_ਨਕਾਬ_WREQ, (ਖਾਲੀ *)addr);

	ਜਦੋਂ (readl((ਖਾਲੀ *)addr) & SIFIVE_TMC_WAKE_ਨਕਾਬ_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
	ਬਣਤਰ sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr;
	u32 v;

	ਜੇ (!tmc0)
		ਵਾਪਸ false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((ਖਾਲੀ *)addr);
	ਜੇ (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		ਵਾਪਸ false;

	ਵਾਪਸ true;
}

ਸਥਿਰ ਖਾਲੀ sifive_tmc0_ਸੈੱਟ_resumepc(physical_addr_t addr)
{
	ਬਣਤਰ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (ਖਾਲੀ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#ਜੇ __riscv_xlen > 32
	writel((u32)(addr >> 32), (ਖਾਲੀ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ u32 sifive_tmc0_ਸੈੱਟ_pgprep_enareq(ਖਾਲੀ)
{
	ਬਣਤਰ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ਖਾਲੀ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (ਖਾਲੀ *)reg);
	ਜਦੋਂ (!(readl((ਖਾਲੀ *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((ਖਾਲੀ *)reg);
	ਵਾਪਸ v & SIFIVE_TMC_PGPREP_ਅੰਦਰੂਨੀ_ABORT;
}

ਸਥਿਰ ਖਾਲੀ sifive_tmc0_ਸੈੱਟ_pgprep_disreq(ਖਾਲੀ)
{
	ਬਣਤਰ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ਖਾਲੀ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (ਖਾਲੀ *)reg);
	ਜਦੋਂ (!(readl((ਖਾਲੀ *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

ਸਥਿਰ u32 sifive_tmc0_get_pgprep_enarsp(ਖਾਲੀ)
{
	ਬਣਤਰ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ਖਾਲੀ *)reg);

	ਵਾਪਸ v & SIFIVE_TMC_PGPREP_ENARSP;
}

ਸਥਿਰ ਖਾਲੀ sifive_tmc0_ਸੈੱਟ_pg_enareq(ਖਾਲੀ)
{
	ਬਣਤਰ sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((ਖਾਲੀ *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (ਖਾਲੀ *)reg);
}

ਸਥਿਰ ਪੂਰਨ sifive_tmc0_prep(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	ਜੇ (!tmc0_ptr_get(scratch))
		ਵਾਪਸ SBI_ENODEV;

	rc = sifive_tmc0_ਸੈੱਟ_pgprep_enareq();
	ਜੇ (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		ਜਾਓ fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	ਜੇ (rc) {
		sifive_tmc0_ਸੈੱਟ_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		ਜਾਓ fail;
	}

	sifive_tmc0_ਸੈੱਟ_resumepc(scratch->warmboot_addr);

	ਵਾਪਸ SBI_OK;

fail:
	ਵਾਪਸ SBI_EFAIL;
}

ਸਥਿਰ ਪੂਰਨ sifive_tmc0_enter(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	ਜੇ (fdt_cmo_ਨਿੱਜੀ_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	ਜੇ (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		ਜਾਓ fail;
	}

	ਜੇ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_ਸੈੱਟ_pg_enareq();
		ਜਦੋਂ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_ਸੈੱਟ_pgprep_disreq();
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ sifive_tmc0_tile_pg(ਖਾਲੀ)
{
	ਪੂਰਨ rc;

	rc = sifive_tmc0_prep();
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ sifive_tmc0_enter();
}

ਸਥਿਰ ਪੂਰਨ sifive_tmc0_ਸ਼ੁਰੂ(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid), true);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ sifive_tmc0_stop(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mie = csr_read(CSR_MIE);
	ਪੂਰਨ rc;
	/* Set IPI as wake up source */
	csr_ਸੈੱਟ(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	ਜੇ (rc) {
		csr_write(CSR_MIE, mie);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਬਣਤਰ sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_ਸ਼ੁਰੂ = sifive_tmc0_ਸ਼ੁਰੂ,
	.hart_stop = sifive_tmc0_stop,
};

ਸਥਿਰ ਪੂਰਨ sifive_tmc0_bind_cpu(ਬਣਤਰ sifive_tmc0 *tmc0)
{
	ਅਚਲ ਖਾਲੀ *fdt = fdt_get_ਪਤਾ();
	ਬਣਤਰ sbi_scratch *scratch;
	ਪੂਰਨ cpus_off, cpu_off, rc;
	ਅਚਲ fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/cpus");
	ਜੇ (cpus_off < 0)
		ਵਾਪਸ SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		ਜੇ (rc)
			ਜਾਰੀ;

		scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(hartid);
		ਜੇ (!scratch)
			ਜਾਰੀ;

		val = fdt_getprop(fdt, cpu_off, "power-domains", ਨਿਰਾ);
		ਜੇ (!val)
			ਵਾਪਸ SBI_ENOENT;

		ਜੇ (tmc0->id == fdt32_ਮੰਜ਼ਿਲ_cpu(val[0])) {
			tmc0_ptr_ਸੈੱਟ(scratch, tmc0);
			ਵਾਪਸ SBI_OK;
		}
	}

	ਵਾਪਸ SBI_ENODEV;
}

ਸਥਿਰ ਪੂਰਨ sifive_tmc0_probe(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਬਣਤਰ sifive_tmc0 *tmc0;
	u64 addr;
	ਪੂਰਨ rc;

	ਜੇ (!tmc0_ਵਿੱਥ) {
		tmc0_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਬਣਤਰ sifive_tmc0 *);
		ਜੇ (!tmc0_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;

		sbi_hsm_ਸੈੱਟ_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(ਆਕਾਰ(*tmc0));
	ਜੇ (!tmc0)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, ਨਿਰਾ);
	ਜੇ (rc)
		ਜਾਓ free_tmc0;

	tmc0->reg = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)addr;
	tmc0->id = fdt_get_phandle(fdt_get_ਪਤਾ(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	ਜੇ (rc)
		ਜਾਓ free_tmc0;

	ਵਾਪਸ SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sifive_tmc0_ਮੇਲ[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_hsm_sifive_tmc0 = {
	.ਮੇਲ_ਸਾਰਣੀ = sifive_tmc0_ਮੇਲ,
	.init = sifive_tmc0_probe,
};
