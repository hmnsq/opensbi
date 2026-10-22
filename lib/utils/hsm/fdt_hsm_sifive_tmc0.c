/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive
 */

#include <libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_ipi.h>
#include <sbi_utils/cache/fdt_cmo_helper.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/hsm/fdt_hsm_sifive_inst.h>
#include <sbi_utils/hsm/fdt_hsm_sifive_tmc0.h>

sa_nracanaa sifive_tmc0 {
	unsigned long reg;
	sa_nracanaa sbi_dlist node;
	u32 id;
};

static SBI_LIST_HEAD(tmc0_list);
static unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), sa_nracanaa sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), sa_nracanaa sifive_tmc0 *, tmc0_offset, (__tmc0))

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

ចំនួនគត់ sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	sa_nracanaa sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	sa_nracanaa sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ប្រសិនបើ (!tmc0)
		ត្រឡប់ SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((void *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (void *)addr);

	ខណៈពេល (!(readl((void *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	ត្រឡប់ SBI_OK;
}

void sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	sa_nracanaa sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	sa_nracanaa sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ប្រសិនបើ (!tmc0)
		ត្រឡប់;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((void *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (void *)addr);

	ខណៈពេល (readl((void *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	sa_nracanaa sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	sa_nracanaa sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	ប្រសិនបើ (!tmc0)
		ត្រឡប់ false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((void *)addr);
	ប្រសិនបើ (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		ត្រឡប់ false;

	ត្រឡប់ true;
}

static void sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	sa_nracanaa sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (void *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#ប្រសិនបើ __riscv_xlen > 32
	writel((u32)(addr >> 32), (void *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

static u32 sifive_tmc0_set_pgprep_enareq(void)
{
	sa_nracanaa sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((void *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (void *)reg);
	ខណៈពេល (!(readl((void *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((void *)reg);
	ត្រឡប់ v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

static void sifive_tmc0_set_pgprep_disreq(void)
{
	sa_nracanaa sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((void *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (void *)reg);
	ខណៈពេល (!(readl((void *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

static u32 sifive_tmc0_get_pgprep_enarsp(void)
{
	sa_nracanaa sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((void *)reg);

	ត្រឡប់ v & SIFIVE_TMC_PGPREP_ENARSP;
}

static void sifive_tmc0_set_pg_enareq(void)
{
	sa_nracanaa sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((void *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (void *)reg);
}

static ចំនួនគត់ sifive_tmc0_prep(void)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	ប្រសិនបើ (!tmc0_ptr_get(scratch))
		ត្រឡប់ SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	ប្រសិនបើ (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		goto fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	ប្រសិនបើ (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		goto fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	ត្រឡប់ SBI_OK;

fail:
	ត្រឡប់ SBI_EFAIL;
}

static ចំនួនគត់ sifive_tmc0_enter(void)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	ប្រសិនបើ (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	ប្រសិនបើ (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		goto fail;
	}

	ប្រសិនបើ (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		ខណៈពេល (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	ត្រឡប់ rc;
}

static ចំនួនគត់ sifive_tmc0_tile_pg(void)
{
	ចំនួនគត់ rc;

	rc = sifive_tmc0_prep();
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	ត្រឡប់ sifive_tmc0_enter();
}

static ចំនួនគត់ sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ sifive_tmc0_stop(void)
{
	unsigned long mie = csr_read(CSR_MIE);
	ចំនួនគត់ rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	ប្រសិនបើ (rc) {
		csr_write(CSR_MIE, mie);
		ត្រឡប់ rc;
	}

	ត្រឡប់ SBI_OK;
}

static sa_nracanaa sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

static ចំនួនគត់ sifive_tmc0_bind_cpu(sa_nracanaa sifive_tmc0 *tmc0)
{
	const void *fdt = fdt_get_address();
	sa_nracanaa sbi_scratch *scratch;
	ចំនួនគត់ cpus_off, cpu_off, rc;
	const fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	ប្រសិនបើ (cpus_off < 0)
		ត្រឡប់ SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		ប្រសិនបើ (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ប្រសិនបើ (!scratch)
			continue;

		val = fdt_getprop(fdt, cpu_off, "power-domains", NULL);
		ប្រសិនបើ (!val)
			ត្រឡប់ SBI_ENOENT;

		ប្រសិនបើ (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			ត្រឡប់ SBI_OK;
		}
	}

	ត្រឡប់ SBI_ENODEV;
}

static ចំនួនគត់ sifive_tmc0_probe(const void *fdt, ចំនួនគត់ nodeoff, const sa_nracanaa fdt_match *match)
{
	sa_nracanaa sifive_tmc0 *tmc0;
	u64 addr;
	ចំនួនគត់ rc;

	ប្រសិនបើ (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(sa_nracanaa sifive_tmc0 *);
		ប្រសិនបើ (!tmc0_offset)
			ត្រឡប់ SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(parimaa_na(*tmc0));
	ប្រសិនបើ (!tmc0)
		ត្រឡប់ SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	ប្រសិនបើ (rc)
		goto free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	ប្រសិនបើ (rc)
		goto free_tmc0;

	ត្រឡប់ SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	ត្រឡប់ rc;
}

static const sa_nracanaa fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

const sa_nracanaa fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
