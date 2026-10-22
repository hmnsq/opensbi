/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cache.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>

#నిర్వచించు CCACHE_CFG_CSR			0
#నిర్వచించు CCACHE_CMD_CSR			0x280
#నిర్వచించు CCACHE_స్థితి_CSR		0x288

#నిర్వచించు CFG_CSR_BANK_ముసుగు		0xff
#నిర్వచించు CFG_CSR_WAY_ముసుగు		0xff00
#నిర్వచించు CFG_CSR_WAY_స్థానభ్రంశం		8
#నిర్వచించు CFG_CSR_సమితి_ముసుగు		0xff0000
#నిర్వచించు CFG_CSR_సమితి_స్థానభ్రంశం		16

#నిర్వచించు CMD_CSR_CMD_స్థానభ్రంశం	56
#నిర్వచించు CMD_CSR_BANK_స్థానభ్రంశం	6

#నిర్వచించు CMD_OPCODE_SETWAY	0x1ULL
#నిర్వచించు CMD_OPCODE_స్థానభ్రంశం	0x2ULL

#నిర్వచించు CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_స్థానభ్రంశం) | 0x3)

#నిర్వచించు CCACHE_CMD_QLEN	0xff

#నిర్వచించు ccache_mb_b()		RISCV_FENCE(rw, o)
#నిర్వచించు ccache_mb_a()		RISCV_FENCE(o, rw)

#నిర్వచించు CCACHE_ALL_OP_REQ_BATCH_అంకె	0x10
#నిర్వచించు CCACHE_ALL_OP_REQ_BATCH_ముసుగు (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_అంకె)

నిర్మాణం sifive_ccache {
	నిర్మాణం cache_device dev;
	శూన్య *addr;
	u64 మొత్తం_పంక్తులు;
};

#నిర్వచించు గమ్యం_ccache(_dev) container_యొక్క(_dev, నిర్మాణం sifive_ccache, dev)

స్థిర అంతఃవరుస గుర్తులేని పూర్ణాంకం sifive_ccache_read_స్థితి(శూన్య *స్థితి_addr)
{
	తిరిగి readl_relaxed(స్థితి_addr);
}

స్థిర అంతఃవరుస శూన్య sifive_ccache_write_cmd(u64 cmd, శూన్య *cmd_csr_addr)
{
#ఒకవేళ __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#లేకపోతే
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం sifive_ccache_flush_all(నిర్మాణం cache_device *dev)
{
	నిర్మాణం sifive_ccache *ccache = గమ్యం_ccache(dev);
	శూన్య *స్థితి_addr = (అక్షరం *)ccache->addr + CCACHE_స్థితి_CSR;
	శూన్య *cmd_csr_addr = (అక్షరం *)ccache->addr + CCACHE_CMD_CSR;
	u64 మొత్తం_cnt = ccache->మొత్తం_పంక్తులు;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_స్థానభ్రంశం;
	పూర్ణాంకం loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_ముసుగు;

	ccache_mb_b();
send_cmd:
	మొత్తం_cnt -= loop_cnt;
	అయితే (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_స్థానభ్రంశం), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_అంకె << CMD_CSR_BANK_స్థానభ్రంశం;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_అంకె;
	}
	ఒకవేళ (!మొత్తం_cnt)
		వెళ్ళు పూర్తి;

	/* Ensure the ccache is able receive more than 16 requests */
	చేయి {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_స్థితి(స్థితి_addr));
	} అయితే (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_అంకె);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_ముసుగు;

	ఒకవేళ (మొత్తం_cnt < loop_cnt) {
		loop_cnt = (మొత్తం_cnt + CCACHE_ALL_OP_REQ_BATCH_అంకె) & CCACHE_ALL_OP_REQ_BATCH_ముసుగు;
		cmd -= ((loop_cnt - మొత్తం_cnt) << CMD_CSR_BANK_స్థానభ్రంశం);
		మొత్తం_cnt = loop_cnt;
	}
	వెళ్ళు send_cmd;
పూర్తి:
	చేయి {} అయితే (sifive_ccache_read_స్థితి(స్థితి_addr));
	ccache_mb_a();

	తిరిగి 0;
}

స్థిర నిర్మాణం cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

స్థిర పూర్ణాంకం sifive_ccache_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	నిర్మాణం sifive_ccache *ccache;
	నిర్మాణం cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	పూర్ణాంకం rc;

	/* find the ccache base control address */
	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, ఖాళీ);
	ఒకవేళ (rc < 0 && reg_addr)
		తిరిగి SBI_ENODEV;

	ccache = sbi_zalloc(పరిమాణం(*ccache));
	ఒకవేళ (!ccache)
		తిరిగి SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_కూడు(fdt, nodeoff, dev);
	ఒకవేళ (rc) {
		sbi_free(ccache);
		తిరిగి rc;
	}

	ccache->addr = (శూన్య *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_ముసుగు;

	sets = (config_csr & CFG_CSR_సమితి_ముసుగు) >> CFG_CSR_సమితి_స్థానభ్రంశం;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_ముసుగు) >> CFG_CSR_WAY_స్థానభ్రంశం;

	ccache->మొత్తం_పంక్తులు = sets * ways * banks;

	తిరిగి SBI_OK;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sifive_ccache_సరిపోలిక[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_sifive_ccache = {
	.సరిపోలిక_పట్టిక = sifive_ccache_సరిపోలిక,
	.init = sifive_ccache_cold_init,
};
