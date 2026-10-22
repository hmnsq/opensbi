/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cmo_helper.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#അന്തര്ഭാവ <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

ഘടന sifive_tmc0 {
	unsigned long reg;
	ഘടന sbi_dlist കണ്ണി;
	u32 id;
};

സ്ഥിതം SBI_LIST_HEAD(tmc0_list);
സ്ഥിതം unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), ഘടന sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), ഘടന sifive_tmc0 *, tmc0_offset, (__tmc0))

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

പൂർണ്ണസംഖ്യ sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	ഘടന sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ഘടന sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	എങ്കിൽ (!tmc0)
		തിരികെ SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((ശൂന്യം *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (ശൂന്യം *)addr);

	ആകുമ്പോൾ (!(readl((ശൂന്യം *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	തിരികെ SBI_OK;
}

ശൂന്യം sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	ഘടന sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ഘടന sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	എങ്കിൽ (!tmc0)
		തിരികെ;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((ശൂന്യം *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (ശൂന്യം *)addr);

	ആകുമ്പോൾ (readl((ശൂന്യം *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	ഘടന sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	ഘടന sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	എങ്കിൽ (!tmc0)
		തിരികെ false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((ശൂന്യം *)addr);
	എങ്കിൽ (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		തിരികെ false;

	തിരികെ true;
}

സ്ഥിതം ശൂന്യം sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	ഘടന sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (ശൂന്യം *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#എങ്കിൽ __riscv_xlen > 32
	writel((u32)(addr >> 32), (ശൂന്യം *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

സ്ഥിതം u32 sifive_tmc0_set_pgprep_enareq(ശൂന്യം)
{
	ഘടന sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ശൂന്യം *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (ശൂന്യം *)reg);
	ആകുമ്പോൾ (!(readl((ശൂന്യം *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((ശൂന്യം *)reg);
	തിരികെ v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

സ്ഥിതം ശൂന്യം sifive_tmc0_set_pgprep_disreq(ശൂന്യം)
{
	ഘടന sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ശൂന്യം *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (ശൂന്യം *)reg);
	ആകുമ്പോൾ (!(readl((ശൂന്യം *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

സ്ഥിതം u32 sifive_tmc0_get_pgprep_enarsp(ശൂന്യം)
{
	ഘടന sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((ശൂന്യം *)reg);

	തിരികെ v & SIFIVE_TMC_PGPREP_ENARSP;
}

സ്ഥിതം ശൂന്യം sifive_tmc0_set_pg_enareq(ശൂന്യം)
{
	ഘടന sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((ശൂന്യം *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (ശൂന്യം *)reg);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_tmc0_prep(ശൂന്യം)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	എങ്കിൽ (!tmc0_ptr_get(scratch))
		തിരികെ SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	എങ്കിൽ (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		പോകുക fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	എങ്കിൽ (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		പോകുക fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	തിരികെ SBI_OK;

fail:
	തിരികെ SBI_EFAIL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_tmc0_enter(ശൂന്യം)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	എങ്കിൽ (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	എങ്കിൽ (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		പോകുക fail;
	}

	എങ്കിൽ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		ആകുമ്പോൾ (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_tmc0_tile_pg(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = sifive_tmc0_prep();
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ sifive_tmc0_enter();
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_tmc0_stop(ശൂന്യം)
{
	unsigned long mie = csr_read(CSR_MIE);
	പൂർണ്ണസംഖ്യ rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	എങ്കിൽ (rc) {
		csr_write(CSR_MIE, mie);
		തിരികെ rc;
	}

	തിരികെ SBI_OK;
}

സ്ഥിതം ഘടന sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_tmc0_bind_cpu(ഘടന sifive_tmc0 *tmc0)
{
	സ്ഥിരം ശൂന്യം *fdt = fdt_get_address();
	ഘടന sbi_scratch *scratch;
	പൂർണ്ണസംഖ്യ cpus_off, cpu_off, rc;
	സ്ഥിരം fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	എങ്കിൽ (cpus_off < 0)
		തിരികെ SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		എങ്കിൽ (rc)
			തുടരുക;

		scratch = sbi_hartid_to_scratch(hartid);
		എങ്കിൽ (!scratch)
			തുടരുക;

		val = fdt_getprop(fdt, cpu_off, "power-domains", ശൂന്യ);
		എങ്കിൽ (!val)
			തിരികെ SBI_ENOENT;

		എങ്കിൽ (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			തിരികെ SBI_OK;
		}
	}

	തിരികെ SBI_ENODEV;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_tmc0_probe(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	ഘടന sifive_tmc0 *tmc0;
	u64 addr;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(ഘടന sifive_tmc0 *);
		എങ്കിൽ (!tmc0_offset)
			തിരികെ SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(വലുപ്പം(*tmc0));
	എങ്കിൽ (!tmc0)
		തിരികെ SBI_ENOMEM;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (rc)
		പോകുക free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	എങ്കിൽ (rc)
		പോകുക free_tmc0;

	തിരികെ SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	തിരികെ rc;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
