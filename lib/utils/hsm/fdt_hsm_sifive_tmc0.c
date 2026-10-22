/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi_utils/cache/fdt_cmo_helper.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#ضمن <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

بنية sifive_tmc0 {
	بیعلامت دراز reg;
	بنية sbi_dlist گره;
	u32 id;
};

ساكن SBI_LIST_سر(tmc0_list);
ساكن بیعلامت دراز tmc0_جابه_جایی;

#تعریف_کن tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), بنية sifive_tmc0 *, tmc0_جابه_جایی)

#تعریف_کن tmc0_ptr_مجموعه(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), بنية sifive_tmc0 *, tmc0_جابه_جایی, (__tmc0))

/* TMC.PGPREP */
#تعریف_کن SIFIVE_TMC_PGPREP_OFF			0x0
#تعریف_کن SIFIVE_TMC_PGPREP_ENA_REQ		بیت(31)
#تعریف_کن SIFIVE_TMC_PGPREP_ENA_ACK		بیت(30)
#تعریف_کن SIFIVE_TMC_PGPREP_DIS_REQ		بیت(29)
#تعریف_کن SIFIVE_TMC_PGPREP_DIS_ACK		بیت(28)
#تعریف_کن SIFIVE_TMC_PGPREP_CLFPNOTQ		بیت(18)
#تعریف_کن SIFIVE_TMC_PGPREP_PMCENAERR		بیت(17)
#تعریف_کن SIFIVE_TMC_PGPREP_PMCDENY		بیت(16)
#تعریف_کن SIFIVE_TMC_PGPREP_BUSERR		بیت(15)
#تعریف_کن SIFIVE_TMC_PGPREP_WAKE_DETECT		بیت(12)
#تعریف_کن SIFIVE_TMC_PGPREP_داخلی_ABORT	بیت(2)
#تعریف_کن SIFIVE_TMC_PGPREP_ENARSP		(SIFIVE_TMC_PGPREP_CLFPNOTQ | \
						 SIFIVE_TMC_PGPREP_PMCENAERR | \
						 SIFIVE_TMC_PGPREP_PMCDENY | \
						 SIFIVE_TMC_PGPREP_BUSERR | \
						 SIFIVE_TMC_PGPREP_WAKE_DETECT)

/* TMC.PG */
#تعریف_کن SIFIVE_TMC_PG_OFF			0x4
#تعریف_کن SIFIVE_TMC_PG_ENA_REQ			بیت(31)
#تعریف_کن SIFIVE_TMC_PG_ENA_ACK			بیت(30)
#تعریف_کن SIFIVE_TMC_PG_DIS_REQ			بیت(29)
#تعریف_کن SIFIVE_TMC_PG_DIS_ACK			بیت(28)
#تعریف_کن SIFIVE_TMC_PG_PMC_ENA_ERR		بیت(17)
#تعریف_کن SIFIVE_TMC_PG_PMC_DENY			بیت(16)
#تعریف_کن SIFIVE_TMC_PG_BUS_ERR			بیت(15)
#تعریف_کن SIFIVE_TMC_PG_MASTNOTQ			بیت(14)
#تعریف_کن SIFIVE_TMC_PG_WARM_RESET		بیت(1)
#تعریف_کن SIFIVE_TMC_PG_ENARSP			(SIFIVE_TMC_PG_PMC_ENA_ERR | \
						 SIFIVE_TMC_PG_PMC_DENY | \
						 SIFIVE_TMC_PG_BUS_ERR | \
						 SIFIVE_TMC_PG_MASTNOTQ)

/* TMC.RESUMEPC */
#تعریف_کن SIFIVE_TMC_RESUMEPC_LO			0x10
#تعریف_کن SIFIVE_TMC_RESUMEPC_HI			0x14

/* TMC.WAKEMASK */
#تعریف_کن SIFIVE_TMC_WAKE_ماسک_OFF		0x20
#تعریف_کن SIFIVE_TMC_WAKE_ماسک_WREQ		بیت(31)
#تعریف_کن SIFIVE_TMC_WAKE_ماسک_ACK		بیت(30)

صحيح sifive_tmc0_مجموعه_wakemask_enareq(u32 hartid)
{
	بنية sbi_scratch *scratch = sbi_hartid_مقصد_scratch(hartid);
	بنية sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	بیعلامت دراز addr;
	u32 v;

	إذا (!tmc0)
		أرجع SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_ماسک_OFF;
	v = readl((فراغ *)addr);
	writel(v | SIFIVE_TMC_WAKE_ماسک_WREQ, (فراغ *)addr);

	طالما (!(readl((فراغ *)addr) & SIFIVE_TMC_WAKE_ماسک_ACK));

	أرجع SBI_OK;
}

فراغ sifive_tmc0_مجموعه_wakemask_disreq(u32 hartid)
{
	بنية sbi_scratch *scratch = sbi_hartid_مقصد_scratch(hartid);
	بنية sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	بیعلامت دراز addr;
	u32 v;

	إذا (!tmc0)
		أرجع;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_ماسک_OFF;
	v = readl((فراغ *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_ماسک_WREQ, (فراغ *)addr);

	طالما (readl((فراغ *)addr) & SIFIVE_TMC_WAKE_ماسک_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	بنية sbi_scratch *scratch = sbi_hartid_مقصد_scratch(hartid);
	بنية sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	بیعلامت دراز addr;
	u32 v;

	إذا (!tmc0)
		أرجع false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((فراغ *)addr);
	إذا (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		أرجع false;

	أرجع true;
}

ساكن فراغ sifive_tmc0_مجموعه_resumepc(physical_addr_t addr)
{
	بنية sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (فراغ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#إذا __riscv_xlen > 32
	writel((u32)(addr >> 32), (فراغ *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#پایان_اگر
}

ساكن u32 sifive_tmc0_مجموعه_pgprep_enareq(فراغ)
{
	بنية sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	بیعلامت دراز reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((فراغ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (فراغ *)reg);
	طالما (!(readl((فراغ *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((فراغ *)reg);
	أرجع v & SIFIVE_TMC_PGPREP_داخلی_ABORT;
}

ساكن فراغ sifive_tmc0_مجموعه_pgprep_disreq(فراغ)
{
	بنية sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	بیعلامت دراز reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((فراغ *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (فراغ *)reg);
	طالما (!(readl((فراغ *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

ساكن u32 sifive_tmc0_get_pgprep_enarsp(فراغ)
{
	بنية sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	بیعلامت دراز reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((فراغ *)reg);

	أرجع v & SIFIVE_TMC_PGPREP_ENARSP;
}

ساكن فراغ sifive_tmc0_مجموعه_pg_enareq(فراغ)
{
	بنية sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	بیعلامت دراز reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((فراغ *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (فراغ *)reg);
}

ساكن صحيح sifive_tmc0_prep(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	إذا (!tmc0_ptr_get(scratch))
		أرجع SBI_ENODEV;

	rc = sifive_tmc0_مجموعه_pgprep_enareq();
	إذا (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		اذهب fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	إذا (rc) {
		sifive_tmc0_مجموعه_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		اذهب fail;
	}

	sifive_tmc0_مجموعه_resumepc(scratch->warmboot_addr);

	أرجع SBI_OK;

fail:
	أرجع SBI_EFAIL;
}

ساكن صحيح sifive_tmc0_enter(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	إذا (fdt_cmo_خصوصی_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	إذا (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		اذهب fail;
	}

	إذا (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_مجموعه_pg_enareq();
		طالما (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_مجموعه_pgprep_disreq();
	أرجع rc;
}

ساكن صحيح sifive_tmc0_tile_pg(فراغ)
{
	صحيح rc;

	rc = sifive_tmc0_prep();
	إذا (rc)
		أرجع rc;

	أرجع sifive_tmc0_enter();
}

ساكن صحيح sifive_tmc0_شروع(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_مقصد_hartindex(hartid), true);

	أرجع SBI_OK;
}

ساكن صحيح sifive_tmc0_stop(فراغ)
{
	بیعلامت دراز mie = csr_read(CSR_MIE);
	صحيح rc;
	/* Set IPI as wake up source */
	csr_مجموعه(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	إذا (rc) {
		csr_write(CSR_MIE, mie);
		أرجع rc;
	}

	أرجع SBI_OK;
}

ساكن بنية sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_شروع = sifive_tmc0_شروع,
	.hart_stop = sifive_tmc0_stop,
};

ساكن صحيح sifive_tmc0_bind_cpu(بنية sifive_tmc0 *tmc0)
{
	ثابت فراغ *fdt = fdt_get_آدرس();
	بنية sbi_scratch *scratch;
	صحيح cpus_off, cpu_off, rc;
	ثابت fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_مسیر_جابه_جایی(fdt, "/cpus");
	إذا (cpus_off < 0)
		أرجع SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		إذا (rc)
			استمر;

		scratch = sbi_hartid_مقصد_scratch(hartid);
		إذا (!scratch)
			استمر;

		val = fdt_getprop(fdt, cpu_off, "power-domains", عدم);
		إذا (!val)
			أرجع SBI_ENOENT;

		إذا (tmc0->id == fdt32_مقصد_cpu(val[0])) {
			tmc0_ptr_مجموعه(scratch, tmc0);
			أرجع SBI_OK;
		}
	}

	أرجع SBI_ENODEV;
}

ساكن صحيح sifive_tmc0_probe(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_تطابق *تطابق)
{
	بنية sifive_tmc0 *tmc0;
	u64 addr;
	صحيح rc;

	إذا (!tmc0_جابه_جایی) {
		tmc0_جابه_جایی = sbi_scratch_alloc_type_جابه_جایی(بنية sifive_tmc0 *);
		إذا (!tmc0_جابه_جایی)
			أرجع SBI_ENOMEM;

		sbi_hsm_مجموعه_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(حجم(*tmc0));
	إذا (!tmc0)
		أرجع SBI_ENOMEM;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc)
		اذهب free_tmc0;

	tmc0->reg = (بیعلامت دراز)addr;
	tmc0->id = fdt_get_phandle(fdt_get_آدرس(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	إذا (rc)
		اذهب free_tmc0;

	أرجع SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	أرجع rc;
}

ساكن ثابت بنية fdt_تطابق sifive_tmc0_تطابق[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

ثابت بنية fdt_driver fdt_hsm_sifive_tmc0 = {
	.تطابق_جدول = sifive_tmc0_تطابق,
	.init = sifive_tmc0_probe,
};
