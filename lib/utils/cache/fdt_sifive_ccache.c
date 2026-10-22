/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_barrier.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cache.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>

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

#define CCACHE_ALL_OP_REQ_BATCH_অঙ্ক	0x10
#define CCACHE_ALL_OP_REQ_BATCH_MASK (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_অঙ্ক)

কাঠামো sifive_ccache {
	কাঠামো cache_device dev;
	শূন্য *addr;
	u64 যোগ_সারিগুলি;
};

#define to_ccache(_dev) container_of(_dev, কাঠামো sifive_ccache, dev)

স্থির অন্তঃরেখ unsigned পূর্ণসংখ্যা sifive_ccache_read_status(শূন্য *status_addr)
{
	ফেরত readl_relaxed(status_addr);
}

স্থির অন্তঃরেখ শূন্য sifive_ccache_write_cmd(u64 cmd, শূন্য *cmd_csr_addr)
{
#যদি __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#অন্যথা
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#endif
}

স্থির পূর্ণসংখ্যা sifive_ccache_flush_all(কাঠামো cache_device *dev)
{
	কাঠামো sifive_ccache *ccache = to_ccache(dev);
	শূন্য *status_addr = (অক্ষর *)ccache->addr + CCACHE_STATUS_CSR;
	শূন্য *cmd_csr_addr = (অক্ষর *)ccache->addr + CCACHE_CMD_CSR;
	u64 যোগ_cnt = ccache->যোগ_সারিগুলি;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_OFFSET;
	পূর্ণসংখ্যা loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_MASK;

	ccache_mb_b();
send_cmd:
	যোগ_cnt -= loop_cnt;
	যখন (loop_cnt > 0) {
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
		cmd += CCACHE_ALL_OP_REQ_BATCH_অঙ্ক << CMD_CSR_BANK_OFFSET;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_অঙ্ক;
	}
	যদি (!যোগ_cnt)
		যাও done;

	/* Ensure the ccache is able receive more than 16 requests */
	করো {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_status(status_addr));
	} যখন (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_অঙ্ক);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_MASK;

	যদি (যোগ_cnt < loop_cnt) {
		loop_cnt = (যোগ_cnt + CCACHE_ALL_OP_REQ_BATCH_অঙ্ক) & CCACHE_ALL_OP_REQ_BATCH_MASK;
		cmd -= ((loop_cnt - যোগ_cnt) << CMD_CSR_BANK_OFFSET);
		যোগ_cnt = loop_cnt;
	}
	যাও send_cmd;
done:
	করো {} যখন (sifive_ccache_read_status(status_addr));
	ccache_mb_a();

	ফেরত 0;
}

স্থির কাঠামো cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

স্থির পূর্ণসংখ্যা sifive_ccache_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, ধ্রুবক কাঠামো fdt_match *match)
{
	কাঠামো sifive_ccache *ccache;
	কাঠামো cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	পূর্ণসংখ্যা rc;

	/* find the ccache base control address */
	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, ফাঁকা);
	যদি (rc < 0 && reg_addr)
		ফেরত SBI_ENODEV;

	ccache = sbi_zalloc(আকার(*ccache));
	যদি (!ccache)
		ফেরত SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	যদি (rc) {
		sbi_free(ccache);
		ফেরত rc;
	}

	ccache->addr = (শূন্য *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_MASK;

	sets = (config_csr & CFG_CSR_SET_MASK) >> CFG_CSR_SET_OFFSET;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_MASK) >> CFG_CSR_WAY_OFFSET;

	ccache->যোগ_সারিগুলি = sets * ways * banks;

	ফেরত SBI_OK;
}

স্থির ধ্রুবক কাঠামো fdt_match sifive_ccache_match[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ধ্রুবক কাঠামো fdt_driver fdt_sifive_ccache = {
	.match_table = sifive_ccache_match,
	.init = sifive_ccache_cold_init,
};
