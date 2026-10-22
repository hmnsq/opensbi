/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>

#تعریف_کرو CCACHE_CFG_CSR			0
#تعریف_کرو CCACHE_CMD_CSR			0x280
#تعریف_کرو CCACHE_حالت_CSR		0x288

#تعریف_کرو CFG_CSR_BANK_نقاب		0xff
#تعریف_کرو CFG_CSR_WAY_نقاب		0xff00
#تعریف_کرو CFG_CSR_WAY_انحراف		8
#تعریف_کرو CFG_CSR_سیٹ_نقاب		0xff0000
#تعریف_کرو CFG_CSR_سیٹ_انحراف		16

#تعریف_کرو CMD_CSR_CMD_انحراف	56
#تعریف_کرو CMD_CSR_BANK_انحراف	6

#تعریف_کرو CMD_OPCODE_SETWAY	0x1ULL
#تعریف_کرو CMD_OPCODE_انحراف	0x2ULL

#تعریف_کرو CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_انحراف) | 0x3)

#تعریف_کرو CCACHE_CMD_QLEN	0xff

#تعریف_کرو ccache_mb_b()		RISCV_FENCE(rw, o)
#تعریف_کرو ccache_mb_a()		RISCV_FENCE(o, rw)

#تعریف_کرو CCACHE_ALL_OP_REQ_BATCH_عدد	0x10
#تعریف_کرو CCACHE_ALL_OP_REQ_BATCH_نقاب (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_عدد)

ساخت sifive_ccache {
	ساخت cache_device dev;
	خالی *addr;
	u64 کل_سطریں;
};

#تعریف_کرو مقصد_ccache(_dev) container_کا(_dev, ساخت sifive_ccache, dev)

ساکن اندرونی غیرعلامتی صحیح sifive_ccache_read_حالت(خالی *حالت_addr)
{
	واپس readl_relaxed(حالت_addr);
}

ساکن اندرونی خالی sifive_ccache_write_cmd(u64 cmd, خالی *cmd_csr_addr)
{
#اگر __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#ورنہ
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#اگر_ختم
}

ساکن صحیح sifive_ccache_flush_all(ساخت cache_device *dev)
{
	ساخت sifive_ccache *ccache = مقصد_ccache(dev);
	خالی *حالت_addr = (حرف *)ccache->addr + CCACHE_حالت_CSR;
	خالی *cmd_csr_addr = (حرف *)ccache->addr + CCACHE_CMD_CSR;
	u64 کل_cnt = ccache->کل_سطریں;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_انحراف;
	صحیح loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_نقاب;

	ccache_mb_b();
send_cmd:
	کل_cnt -= loop_cnt;
	جب (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_انحراف), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_عدد << CMD_CSR_BANK_انحراف;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_عدد;
	}
	اگر (!کل_cnt)
		جاؤ مکمل;

	/* Ensure the ccache is able receive more than 16 requests */
	کرو {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_حالت(حالت_addr));
	} جب (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_عدد);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_نقاب;

	اگر (کل_cnt < loop_cnt) {
		loop_cnt = (کل_cnt + CCACHE_ALL_OP_REQ_BATCH_عدد) & CCACHE_ALL_OP_REQ_BATCH_نقاب;
		cmd -= ((loop_cnt - کل_cnt) << CMD_CSR_BANK_انحراف);
		کل_cnt = loop_cnt;
	}
	جاؤ send_cmd;
مکمل:
	کرو {} جب (sifive_ccache_read_حالت(حالت_addr));
	ccache_mb_a();

	واپس 0;
}

ساکن ساخت cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

ساکن صحیح sifive_ccache_cold_init(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	ساخت sifive_ccache *ccache;
	ساخت cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	صحیح rc;

	/* find the ccache base control address */
	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, تہی);
	اگر (rc < 0 && reg_addr)
		واپس SBI_ENODEV;

	ccache = sbi_zalloc(حجم(*ccache));
	اگر (!ccache)
		واپس SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_جوڑو(fdt, nodeoff, dev);
	اگر (rc) {
		sbi_free(ccache);
		واپس rc;
	}

	ccache->addr = (خالی *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_نقاب;

	sets = (config_csr & CFG_CSR_سیٹ_نقاب) >> CFG_CSR_سیٹ_انحراف;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_نقاب) >> CFG_CSR_WAY_انحراف;

	ccache->کل_سطریں = sets * ways * banks;

	واپس SBI_OK;
}

ساکن ثابت ساخت fdt_مطابقت sifive_ccache_مطابقت[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ثابت ساخت fdt_driver fdt_sifive_ccache = {
	.مطابقت_جدول = sifive_ccache_مطابقت,
	.init = sifive_ccache_cold_init,
};
