/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cache.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>

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

#define CCACHE_ALL_OP_REQ_BATCH_ଅଙ୍କ	0x10
#define CCACHE_ALL_OP_REQ_BATCH_MASK (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_ଅଙ୍କ)

ସଂରଚନା sifive_ccache {
	ସଂରଚନା cache_device dev;
	ଶୂନ୍ୟ *addr;
	u64 total_ଧାଡ଼ିଗୁଡ଼ିକ;
};

#define to_ccache(_dev) container_of(_dev, ସଂରଚନା sifive_ccache, dev)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_ccache_read_status(ଶୂନ୍ୟ *status_addr)
{
	ଫେରନ୍ତୁ readl_relaxed(status_addr);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ sifive_ccache_write_cmd(u64 cmd, ଶୂନ୍ୟ *cmd_csr_addr)
{
#ଯଦି __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#ଅନ୍ୟଥା
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_ccache_flush_all(ସଂରଚନା cache_device *dev)
{
	ସଂରଚନା sifive_ccache *ccache = to_ccache(dev);
	ଶୂନ୍ୟ *status_addr = (ଅକ୍ଷର *)ccache->addr + CCACHE_STATUS_CSR;
	ଶୂନ୍ୟ *cmd_csr_addr = (ଅକ୍ଷର *)ccache->addr + CCACHE_CMD_CSR;
	u64 total_cnt = ccache->total_ଧାଡ଼ିଗୁଡ଼ିକ;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_OFFSET;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_MASK;

	ccache_mb_b();
send_cmd:
	total_cnt -= loop_cnt;
	ଯେତେବେଳେ (loop_cnt > 0) {
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
		cmd += CCACHE_ALL_OP_REQ_BATCH_ଅଙ୍କ << CMD_CSR_BANK_OFFSET;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_ଅଙ୍କ;
	}
	ଯଦି (!total_cnt)
		ଯାଅ done;

	/* Ensure the ccache is able receive more than 16 requests */
	କର {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_status(status_addr));
	} ଯେତେବେଳେ (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_ଅଙ୍କ);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_MASK;

	ଯଦି (total_cnt < loop_cnt) {
		loop_cnt = (total_cnt + CCACHE_ALL_OP_REQ_BATCH_ଅଙ୍କ) & CCACHE_ALL_OP_REQ_BATCH_MASK;
		cmd -= ((loop_cnt - total_cnt) << CMD_CSR_BANK_OFFSET);
		total_cnt = loop_cnt;
	}
	ଯାଅ send_cmd;
done:
	କର {} ଯେତେବେଳେ (sifive_ccache_read_status(status_addr));
	ccache_mb_a();

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_ccache_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ସଂରଚନା sifive_ccache *ccache;
	ସଂରଚନା cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* find the ccache base control address */
	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, ଖାଲି);
	ଯଦି (rc < 0 && reg_addr)
		ଫେରନ୍ତୁ SBI_ENODEV;

	ccache = sbi_zalloc(ଆକାର(*ccache));
	ଯଦି (!ccache)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ଯଦି (rc) {
		sbi_free(ccache);
		ଫେରନ୍ତୁ rc;
	}

	ccache->addr = (ଶୂନ୍ୟ *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_MASK;

	sets = (config_csr & CFG_CSR_SET_MASK) >> CFG_CSR_SET_OFFSET;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_MASK) >> CFG_CSR_WAY_OFFSET;

	ccache->total_ଧାଡ଼ିଗୁଡ଼ିକ = sets * ways * banks;

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sifive_ccache_match[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_sifive_ccache = {
	.match_table = sifive_ccache_match,
	.init = sifive_ccache_cold_init,
};
