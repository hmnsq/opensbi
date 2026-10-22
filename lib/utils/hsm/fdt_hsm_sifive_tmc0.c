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

struct sifive_tmc0 {
	unsigned long reg;
	struct sbi_dlist node;
	u32 id;
};

static SBI_LIST_HEAD(tmc0_list);
static unsigned long tmc0_offset;

#define tmc0_ptr_get(__scratch)				\
	sbi_scratch_read_type((__scratch), struct sifive_tmc0 *, tmc0_offset)

#define tmc0_ptr_set(__scratch, __tmc0)			\
	sbi_scratch_write_type((__scratch), struct sifive_tmc0 *, tmc0_offset, (__tmc0))

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

整数 sifive_tmc0_set_wakemask_enareq(u32 hartid)
{
	struct sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	struct sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	もし (!tmc0)
		戻る SBI_ENODEV;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((void *)addr);
	writel(v | SIFIVE_TMC_WAKE_MASK_WREQ, (void *)addr);

	ながら (!(readl((void *)addr) & SIFIVE_TMC_WAKE_MASK_ACK));

	戻る SBI_OK;
}

void sifive_tmc0_set_wakemask_disreq(u32 hartid)
{
	struct sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	struct sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	もし (!tmc0)
		戻る;

	addr = tmc0->reg + SIFIVE_TMC_WAKE_MASK_OFF;
	v = readl((void *)addr);
	writel(v & ~SIFIVE_TMC_WAKE_MASK_WREQ, (void *)addr);

	ながら (readl((void *)addr) & SIFIVE_TMC_WAKE_MASK_ACK);
}

bool sifive_tmc0_is_pg(u32 hartid)
{
	struct sbi_scratch *scratch = sbi_hartid_to_scratch(hartid);
	struct sifive_tmc0 *tmc0 = tmc0_ptr_get(scratch);
	unsigned long addr;
	u32 v;

	もし (!tmc0)
		戻る false;

	addr = tmc0->reg + SIFIVE_TMC_PG_OFF;
	v = readl((void *)addr);
	もし (!(v & SIFIVE_TMC_PG_ENA_ACK) ||
	    (v & SIFIVE_TMC_PG_ENARSP) ||
	    (v & SIFIVE_TMC_PG_DIS_REQ))
		戻る false;

	戻る true;
}

static void sifive_tmc0_set_resumepc(physical_addr_t addr)
{
	struct sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());

	writel((u32)addr, (void *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_LO));
#もし __riscv_xlen > 32
	writel((u32)(addr >> 32), (void *)(tmc0->reg + SIFIVE_TMC_RESUMEPC_HI));
#endif
}

static u32 sifive_tmc0_set_pgprep_enareq(void)
{
	struct sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((void *)reg);

	writel(v | SIFIVE_TMC_PGPREP_ENA_REQ, (void *)reg);
	ながら (!(readl((void *)reg) & SIFIVE_TMC_PGPREP_ENA_ACK));

	v = readl((void *)reg);
	戻る v & SIFIVE_TMC_PGPREP_INTERNAL_ABORT;
}

static void sifive_tmc0_set_pgprep_disreq(void)
{
	struct sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((void *)reg);

	writel(v | SIFIVE_TMC_PGPREP_DIS_REQ, (void *)reg);
	ながら (!(readl((void *)reg) & SIFIVE_TMC_PGPREP_DIS_ACK));
}

static u32 sifive_tmc0_get_pgprep_enarsp(void)
{
	struct sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PGPREP_OFF;
	u32 v = readl((void *)reg);

	戻る v & SIFIVE_TMC_PGPREP_ENARSP;
}

static void sifive_tmc0_set_pg_enareq(void)
{
	struct sifive_tmc0 *tmc0 = tmc0_ptr_get(sbi_scratch_thishart_ptr());
	unsigned long reg = tmc0->reg + SIFIVE_TMC_PG_OFF;
	u32 v = readl((void *)reg);

	writel(v | SIFIVE_TMC_PG_ENA_REQ, (void *)reg);
}

static 整数 sifive_tmc0_prep(void)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	もし (!tmc0_ptr_get(scratch))
		戻る SBI_ENODEV;

	rc = sifive_tmc0_set_pgprep_enareq();
	もし (rc) {
		sbi_printf("TMC0 error: Internal Abort (Wake detect)\n");
		goto fail;
	}

	rc = sifive_tmc0_get_pgprep_enarsp();
	もし (rc) {
		sifive_tmc0_set_pgprep_disreq();
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		goto fail;
	}

	sifive_tmc0_set_resumepc(scratch->warmboot_addr);

	戻る SBI_OK;

fail:
	戻る SBI_EFAIL;
}

static 整数 sifive_tmc0_enter(void)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	u32 rc;

	/* Flush cache and check if there is wake detect or bus error */
	もし (fdt_cmo_private_flc_flush_all() &&
	    sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CFLUSH_D_L1))
		sifive_cflush();

	rc = sifive_tmc0_get_pgprep_enarsp();
	もし (rc) {
		sbi_printf("TMC0 error: error response code: 0x%x\n", rc);
		goto fail;
	}

	もし (sbi_hart_has_extension(scratch, SBI_HART_EXT_XSIFIVE_CEASE)) {
		sifive_tmc0_set_pg_enareq();
		ながら (1)
			sifive_cease();
	}

	rc = SBI_ENOTSUPP;
fail:
	sifive_tmc0_set_pgprep_disreq();
	戻る rc;
}

static 整数 sifive_tmc0_tile_pg(void)
{
	整数 rc;

	rc = sifive_tmc0_prep();
	もし (rc)
		戻る rc;

	戻る sifive_tmc0_enter();
}

static 整数 sifive_tmc0_start(u32 hartid, ulong saddr)
{
	/*
	 * In system suspend, the IMSIC will be reset in SiFive platform so
	 * we use the CLINT IPI as the wake event.
	 */
	sbi_ipi_raw_send(sbi_hartid_to_hartindex(hartid), true);

	戻る SBI_OK;
}

static 整数 sifive_tmc0_stop(void)
{
	unsigned long mie = csr_read(CSR_MIE);
	整数 rc;
	/* Set IPI as wake up source */
	csr_set(CSR_MIE, MIP_MEIP | MIP_MSIP);

	rc = sifive_tmc0_tile_pg();
	もし (rc) {
		csr_write(CSR_MIE, mie);
		戻る rc;
	}

	戻る SBI_OK;
}

static struct sbi_hsm_device tmc0_hsm_dev = {
	.name = "SiFive TMC0",
	.hart_start = sifive_tmc0_start,
	.hart_stop = sifive_tmc0_stop,
};

static 整数 sifive_tmc0_bind_cpu(struct sifive_tmc0 *tmc0)
{
	const void *fdt = fdt_get_address();
	struct sbi_scratch *scratch;
	整数 cpus_off, cpu_off, rc;
	const fdt32_t *val;
	u32 hartid;

	cpus_off = fdt_path_offset(fdt, "/cpus");
	もし (cpus_off < 0)
		戻る SBI_ENOENT;

	fdt_for_each_subnode(cpu_off, fdt, cpus_off) {
		rc = fdt_parse_hart_id(fdt, cpu_off, &hartid);
		もし (rc)
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		もし (!scratch)
			continue;

		val = fdt_getprop(fdt, cpu_off, "power-domains", NULL);
		もし (!val)
			戻る SBI_ENOENT;

		もし (tmc0->id == fdt32_to_cpu(val[0])) {
			tmc0_ptr_set(scratch, tmc0);
			戻る SBI_OK;
		}
	}

	戻る SBI_ENODEV;
}

static 整数 sifive_tmc0_probe(const void *fdt, 整数 nodeoff, const struct fdt_match *match)
{
	struct sifive_tmc0 *tmc0;
	u64 addr;
	整数 rc;

	もし (!tmc0_offset) {
		tmc0_offset = sbi_scratch_alloc_type_offset(struct sifive_tmc0 *);
		もし (!tmc0_offset)
			戻る SBI_ENOMEM;

		sbi_hsm_set_device(&tmc0_hsm_dev);
	}

	tmc0 = sbi_zalloc(sizeof(*tmc0));
	もし (!tmc0)
		戻る SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	もし (rc)
		goto free_tmc0;

	tmc0->reg = (unsigned long)addr;
	tmc0->id = fdt_get_phandle(fdt_get_address(), nodeoff);

	rc = sifive_tmc0_bind_cpu(tmc0);
	もし (rc)
		goto free_tmc0;

	戻る SBI_OK;

free_tmc0:
	sbi_free(tmc0);
	戻る rc;
}

static const struct fdt_match sifive_tmc0_match[] = {
	{ .compatible = "sifive,tmc0" },
	{ },
};

const struct fdt_driver fdt_hsm_sifive_tmc0 = {
	.match_table = sifive_tmc0_match,
	.init = sifive_tmc0_probe,
};
