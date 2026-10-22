/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cache.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>

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

#define CCACHE_ALL_OP_REQ_BATCH_അക്കം	0x10
#define CCACHE_ALL_OP_REQ_BATCH_MASK (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_അക്കം)

ഘടന sifive_ccache {
	ഘടന cache_device dev;
	ശൂന്യം *addr;
	u64 total_വരികൾ;
};

#define to_ccache(_dev) container_of(_dev, ഘടന sifive_ccache, dev)

സ്ഥിതം വരിയിൽ unsigned പൂർണ്ണസംഖ്യ sifive_ccache_read_status(ശൂന്യം *status_addr)
{
	തിരികെ readl_relaxed(status_addr);
}

സ്ഥിതം വരിയിൽ ശൂന്യം sifive_ccache_write_cmd(u64 cmd, ശൂന്യം *cmd_csr_addr)
{
#എങ്കിൽ __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#അല്ലെങ്കിൽ
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_ccache_flush_all(ഘടന cache_device *dev)
{
	ഘടന sifive_ccache *ccache = to_ccache(dev);
	ശൂന്യം *status_addr = (അക്ഷരം *)ccache->addr + CCACHE_STATUS_CSR;
	ശൂന്യം *cmd_csr_addr = (അക്ഷരം *)ccache->addr + CCACHE_CMD_CSR;
	u64 total_cnt = ccache->total_വരികൾ;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_OFFSET;
	പൂർണ്ണസംഖ്യ loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_MASK;

	ccache_mb_b();
send_cmd:
	total_cnt -= loop_cnt;
	ആകുമ്പോൾ (loop_cnt > 0) {
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
		cmd += CCACHE_ALL_OP_REQ_BATCH_അക്കം << CMD_CSR_BANK_OFFSET;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_അക്കം;
	}
	എങ്കിൽ (!total_cnt)
		പോകുക done;

	/* Ensure the ccache is able receive more than 16 requests */
	ചെയ്യുക {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_status(status_addr));
	} ആകുമ്പോൾ (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_അക്കം);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_MASK;

	എങ്കിൽ (total_cnt < loop_cnt) {
		loop_cnt = (total_cnt + CCACHE_ALL_OP_REQ_BATCH_അക്കം) & CCACHE_ALL_OP_REQ_BATCH_MASK;
		cmd -= ((loop_cnt - total_cnt) << CMD_CSR_BANK_OFFSET);
		total_cnt = loop_cnt;
	}
	പോകുക send_cmd;
done:
	ചെയ്യുക {} ആകുമ്പോൾ (sifive_ccache_read_status(status_addr));
	ccache_mb_a();

	തിരികെ 0;
}

സ്ഥിതം ഘടന cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_ccache_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	ഘടന sifive_ccache *ccache;
	ഘടന cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	പൂർണ്ണസംഖ്യ rc;

	/* find the ccache base control address */
	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, ശൂന്യ);
	എങ്കിൽ (rc < 0 && reg_addr)
		തിരികെ SBI_ENODEV;

	ccache = sbi_zalloc(വലുപ്പം(*ccache));
	എങ്കിൽ (!ccache)
		തിരികെ SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	എങ്കിൽ (rc) {
		sbi_free(ccache);
		തിരികെ rc;
	}

	ccache->addr = (ശൂന്യം *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_MASK;

	sets = (config_csr & CFG_CSR_SET_MASK) >> CFG_CSR_SET_OFFSET;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_MASK) >> CFG_CSR_WAY_OFFSET;

	ccache->total_വരികൾ = sets * ways * banks;

	തിരികെ SBI_OK;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sifive_ccache_match[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_sifive_ccache = {
	.match_table = sifive_ccache_match,
	.init = sifive_ccache_cold_init,
};
