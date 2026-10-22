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

#تعریف_کړه CCACHE_CFG_CSR			0
#تعریف_کړه CCACHE_CMD_CSR			0x280
#تعریف_کړه CCACHE_حالت_CSR		0x288

#تعریف_کړه CFG_CSR_BANK_ماسک		0xff
#تعریف_کړه CFG_CSR_WAY_ماسک		0xff00
#تعریف_کړه CFG_CSR_WAY_دننه_واټن		8
#تعریف_کړه CFG_CSR_سېټ_ماسک		0xff0000
#تعریف_کړه CFG_CSR_سېټ_دننه_واټن		16

#تعریف_کړه CMD_CSR_CMD_دننه_واټن	56
#تعریف_کړه CMD_CSR_BANK_دننه_واټن	6

#تعریف_کړه CMD_OPCODE_SETWAY	0x1ULL
#تعریف_کړه CMD_OPCODE_دننه_واټن	0x2ULL

#تعریف_کړه CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_دننه_واټن) | 0x3)

#تعریف_کړه CCACHE_CMD_QLEN	0xff

#تعریف_کړه ccache_mb_b()		RISCV_FENCE(rw, o)
#تعریف_کړه ccache_mb_a()		RISCV_FENCE(o, rw)

#تعریف_کړه CCACHE_ALL_OP_REQ_BATCH_عدد	0x10
#تعریف_کړه CCACHE_ALL_OP_REQ_BATCH_ماسک (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_عدد)

بنية sifive_ccache {
	بنية cache_device dev;
	فراغ *addr;
	u64 ټول_کرښې;
};

#تعریف_کړه منزل_ccache(_dev) container_د(_dev, بنية sifive_ccache, dev)

ساكن مضمن بینښه صحيح sifive_ccache_read_حالت(فراغ *حالت_addr)
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
#د_که_پای
}

ساكن صحيح sifive_ccache_flush_all(بنية cache_device *dev)
{
	بنية sifive_ccache *ccache = منزل_ccache(dev);
	فراغ *حالت_addr = (حرف *)ccache->addr + CCACHE_حالت_CSR;
	فراغ *cmd_csr_addr = (حرف *)ccache->addr + CCACHE_CMD_CSR;
	u64 ټول_cnt = ccache->ټول_کرښې;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_دننه_واټن;
	صحيح loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_ماسک;

	ccache_mb_b();
send_cmd:
	ټول_cnt -= loop_cnt;
	طالما (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_دننه_واټن), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_عدد << CMD_CSR_BANK_دننه_واټن;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_عدد;
	}
	إذا (!ټول_cnt)
		اذهب بشپړ;

	/* Ensure the ccache is able receive more than 16 requests */
	افعل {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_حالت(حالت_addr));
	} طالما (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_عدد);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_ماسک;

	إذا (ټول_cnt < loop_cnt) {
		loop_cnt = (ټول_cnt + CCACHE_ALL_OP_REQ_BATCH_عدد) & CCACHE_ALL_OP_REQ_BATCH_ماسک;
		cmd -= ((loop_cnt - ټول_cnt) << CMD_CSR_BANK_دننه_واټن);
		ټول_cnt = loop_cnt;
	}
	اذهب send_cmd;
بشپړ:
	افعل {} طالما (sifive_ccache_read_حالت(حالت_addr));
	ccache_mb_a();

	أرجع 0;
}

ساكن بنية cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

ساكن صحيح sifive_ccache_cold_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_سمون *سمون)
{
	بنية sifive_ccache *ccache;
	بنية cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	صحيح rc;

	/* find the ccache base control address */
	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 && reg_addr)
		أرجع SBI_ENODEV;

	ccache = sbi_zalloc(حجم(*ccache));
	إذا (!ccache)
		أرجع SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_جمع_کړه(fdt, nodeoff, dev);
	إذا (rc) {
		sbi_free(ccache);
		أرجع rc;
	}

	ccache->addr = (فراغ *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_ماسک;

	sets = (config_csr & CFG_CSR_سېټ_ماسک) >> CFG_CSR_سېټ_دننه_واټن;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_ماسک) >> CFG_CSR_WAY_دننه_واټن;

	ccache->ټول_کرښې = sets * ways * banks;

	أرجع SBI_OK;
}

ساكن ثابت بنية fdt_سمون sifive_ccache_سمون[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ثابت بنية fdt_driver fdt_sifive_ccache = {
	.سمون_جدول = sifive_ccache_سمون,
	.init = sifive_ccache_cold_init,
};
