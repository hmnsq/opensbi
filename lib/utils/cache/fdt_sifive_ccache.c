/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/cache/fdt_cache.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>

#تعریف_کرو CCACHE_CFG_CSR			0
#تعریف_کرو CCACHE_CMD_CSR			0x280
#تعریف_کرو CCACHE_حالت_CSR		0x288

#تعریف_کرو CFG_CSR_BANK_نقاب		0xff
#تعریف_کرو CFG_CSR_WAY_نقاب		0xff00
#تعریف_کرو CFG_CSR_WAY_وتھ		8
#تعریف_کرو CFG_CSR_سیٹ_نقاب		0xff0000
#تعریف_کرو CFG_CSR_سیٹ_وتھ		16

#تعریف_کرو CMD_CSR_CMD_وتھ	56
#تعریف_کرو CMD_CSR_BANK_وتھ	6

#تعریف_کرو CMD_OPCODE_SETWAY	0x1ULL
#تعریف_کرو CMD_OPCODE_وتھ	0x2ULL

#تعریف_کرو CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_وتھ) | 0x3)

#تعریف_کرو CCACHE_CMD_QLEN	0xff

#تعریف_کرو ccache_mb_b()		RISCV_FENCE(rw, o)
#تعریف_کرو ccache_mb_a()		RISCV_FENCE(o, rw)

#تعریف_کرو CCACHE_ALL_OP_REQ_BATCH_عدد	0x10
#تعریف_کرو CCACHE_ALL_OP_REQ_BATCH_نقاب (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_عدد)

بنية sifive_ccache {
	بنية cache_device dev;
	فراغ *addr;
	u64 کل_سطریں;
};

#تعریف_کرو منزل_ccache(_dev) container_دا(_dev, بنية sifive_ccache, dev)

ساكن مضمن بنانشانی صحيح sifive_ccache_read_حالت(فراغ *حالت_addr)
{
	أرجع readl_relaxed(حالت_addr);
}

ساكن مضمن فراغ sifive_ccache_write_cmd(u64 cmd, فراغ *cmd_csr_addr)
{
#إذا __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#وإلا
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#جے_انت
}

ساكن صحيح sifive_ccache_flush_all(بنية cache_device *dev)
{
	بنية sifive_ccache *ccache = منزل_ccache(dev);
	فراغ *حالت_addr = (حرف *)ccache->addr + CCACHE_حالت_CSR;
	فراغ *cmd_csr_addr = (حرف *)ccache->addr + CCACHE_CMD_CSR;
	u64 کل_cnt = ccache->کل_سطریں;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_وتھ;
	صحيح loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_نقاب;

	ccache_mb_b();
send_cmd:
	کل_cnt -= loop_cnt;
	طالما (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_وتھ), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_عدد << CMD_CSR_BANK_وتھ;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_عدد;
	}
	إذا (!کل_cnt)
		اذهب پورا;

	/* Ensure the ccache is able receive more than 16 requests */
	افعل {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_حالت(حالت_addr));
	} طالما (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_عدد);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_نقاب;

	إذا (کل_cnt < loop_cnt) {
		loop_cnt = (کل_cnt + CCACHE_ALL_OP_REQ_BATCH_عدد) & CCACHE_ALL_OP_REQ_BATCH_نقاب;
		cmd -= ((loop_cnt - کل_cnt) << CMD_CSR_BANK_وتھ);
		کل_cnt = loop_cnt;
	}
	اذهب send_cmd;
پورا:
	افعل {} طالما (sifive_ccache_read_حالت(حالت_addr));
	ccache_mb_a();

	أرجع 0;
}

ساكن بنية cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

ساكن صحيح sifive_ccache_cold_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_میل *میل)
{
	بنية sifive_ccache *ccache;
	بنية cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	صحيح rc;

	/* find the ccache base control address */
	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 && reg_addr)
		أرجع SBI_ENODEV;

	ccache = sbi_zalloc(حجم(*ccache));
	إذا (!ccache)
		أرجع SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_جوڑو(fdt, nodeoff, dev);
	إذا (rc) {
		sbi_free(ccache);
		أرجع rc;
	}

	ccache->addr = (فراغ *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_نقاب;

	sets = (config_csr & CFG_CSR_سیٹ_نقاب) >> CFG_CSR_سیٹ_وتھ;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_نقاب) >> CFG_CSR_WAY_وتھ;

	ccache->کل_سطریں = sets * ways * banks;

	أرجع SBI_OK;
}

ساكن ثابت بنية fdt_میل sifive_ccache_میل[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ثابت بنية fdt_driver fdt_sifive_ccache = {
	.میل_جدول = sifive_ccache_میل,
	.init = sifive_ccache_cold_init,
};
