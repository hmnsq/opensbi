/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cache.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>

#define CCACHE_CFG_CSR			0
#define CCACHE_CMD_CSR			0x280
#define CCACHE_STATUS_CSR		0x288

#define CFG_CSR_BANK_MASK		0xff
#define CFG_CSR_WAY_MASK		0xff00
#define CFG_CSR_WAY_OFFSET		8
#define CFG_CSR_SET_MASK		0xff0000
#define CFG_CSR_SET_OFFSET		16

#define CMD_CSR_CMD_OFFSET	56
#define CMD_CSR_BANK_OFFSET	6

#define CMD_OPCODE_SETWAY	0x1ULL
#define CMD_OPCODE_OFFSET	0x2ULL

#define CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_OFFSET) | 0x3)

#define CCACHE_CMD_QLEN	0xff

#define ccache_mb_b()		RISCV_FENCE(rw, o)
#define ccache_mb_a()		RISCV_FENCE(o, rw)

#define CCACHE_ALL_OP_REQ_BATCH_અંક	0x10
#define CCACHE_ALL_OP_REQ_BATCH_MASK (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_અંક)

સંરચના sifive_ccache {
	સંરચના cache_device dev;
	શૂન્ય *addr;
	u64 total_લીટીઓ;
};

#define to_ccache(_dev) container_of(_dev, સંરચના sifive_ccache, dev)

સ્થિર અંતઃરેખ unsigned પૂર્ણાંક sifive_ccache_read_status(શૂન્ય *status_addr)
{
	પાછા readl_relaxed(status_addr);
}

સ્થિર અંતઃરેખ શૂન્ય sifive_ccache_write_cmd(u64 cmd, શૂન્ય *cmd_csr_addr)
{
#જો __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#નહીંતર
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#endif
}

સ્થિર પૂર્ણાંક sifive_ccache_flush_all(સંરચના cache_device *dev)
{
	સંરચના sifive_ccache *ccache = to_ccache(dev);
	શૂન્ય *status_addr = (અક્ષર *)ccache->addr + CCACHE_STATUS_CSR;
	શૂન્ય *cmd_csr_addr = (અક્ષર *)ccache->addr + CCACHE_CMD_CSR;
	u64 total_cnt = ccache->total_લીટીઓ;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_OFFSET;
	પૂર્ણાંક loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_MASK;

	ccache_mb_b();
send_cmd:
	total_cnt -= loop_cnt;
	જ્યારે (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_OFFSET), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_અંક << CMD_CSR_BANK_OFFSET;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_અંક;
	}
	જો (!total_cnt)
		જાઓ done;

	/* Ensure the ccache is able receive more than 16 requests */
	કરો {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_status(status_addr));
	} જ્યારે (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_અંક);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_MASK;

	જો (total_cnt < loop_cnt) {
		loop_cnt = (total_cnt + CCACHE_ALL_OP_REQ_BATCH_અંક) & CCACHE_ALL_OP_REQ_BATCH_MASK;
		cmd -= ((loop_cnt - total_cnt) << CMD_CSR_BANK_OFFSET);
		total_cnt = loop_cnt;
	}
	જાઓ send_cmd;
done:
	કરો {} જ્યારે (sifive_ccache_read_status(status_addr));
	ccache_mb_a();

	પાછા 0;
}

સ્થિર સંરચના cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

સ્થિર પૂર્ણાંક sifive_ccache_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	સંરચના sifive_ccache *ccache;
	સંરચના cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	પૂર્ણાંક rc;

	/* find the ccache base control address */
	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, ખાલી);
	જો (rc < 0 && reg_addr)
		પાછા SBI_ENODEV;

	ccache = sbi_zalloc(કદ(*ccache));
	જો (!ccache)
		પાછા SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	જો (rc) {
		sbi_free(ccache);
		પાછા rc;
	}

	ccache->addr = (શૂન્ય *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_MASK;

	sets = (config_csr & CFG_CSR_SET_MASK) >> CFG_CSR_SET_OFFSET;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_MASK) >> CFG_CSR_WAY_OFFSET;

	ccache->total_લીટીઓ = sets * ways * banks;

	પાછા SBI_OK;
}

સ્થિર અચલ સંરચના fdt_match sifive_ccache_match[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

અચલ સંરચના fdt_driver fdt_sifive_ccache = {
	.match_table = sifive_ccache_match,
	.init = sifive_ccache_cold_init,
};
