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

#تعریف_کن CCACHE_CFG_CSR			0
#تعریف_کن CCACHE_CMD_CSR			0x280
#تعریف_کن CCACHE_وضعیت_CSR		0x288

#تعریف_کن CFG_CSR_BANK_ماسک		0xff
#تعریف_کن CFG_CSR_WAY_ماسک		0xff00
#تعریف_کن CFG_CSR_WAY_جابه_جایی		8
#تعریف_کن CFG_CSR_مجموعه_ماسک		0xff0000
#تعریف_کن CFG_CSR_مجموعه_جابه_جایی		16

#تعریف_کن CMD_CSR_CMD_جابه_جایی	56
#تعریف_کن CMD_CSR_BANK_جابه_جایی	6

#تعریف_کن CMD_OPCODE_SETWAY	0x1ULL
#تعریف_کن CMD_OPCODE_جابه_جایی	0x2ULL

#تعریف_کن CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_جابه_جایی) | 0x3)

#تعریف_کن CCACHE_CMD_QLEN	0xff

#تعریف_کن ccache_mb_b()		RISCV_FENCE(rw, o)
#تعریف_کن ccache_mb_a()		RISCV_FENCE(o, rw)

#تعریف_کن CCACHE_ALL_OP_REQ_BATCH_رقم	0x10
#تعریف_کن CCACHE_ALL_OP_REQ_BATCH_ماسک (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_رقم)

بنية sifive_ccache {
	بنية cache_device dev;
	فراغ *addr;
	u64 مجموع_سطرها;
};

#تعریف_کن مقصد_ccache(_dev) container_از(_dev, بنية sifive_ccache, dev)

ساكن مضمن بیعلامت صحيح sifive_ccache_read_وضعیت(فراغ *وضعیت_addr)
{
	أرجع readl_relaxed(وضعیت_addr);
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
#پایان_اگر
}

ساكن صحيح sifive_ccache_flush_all(بنية cache_device *dev)
{
	بنية sifive_ccache *ccache = مقصد_ccache(dev);
	فراغ *وضعیت_addr = (حرف *)ccache->addr + CCACHE_وضعیت_CSR;
	فراغ *cmd_csr_addr = (حرف *)ccache->addr + CCACHE_CMD_CSR;
	u64 مجموع_cnt = ccache->مجموع_سطرها;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_جابه_جایی;
	صحيح loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_ماسک;

	ccache_mb_b();
send_cmd:
	مجموع_cnt -= loop_cnt;
	طالما (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_جابه_جایی), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_رقم << CMD_CSR_BANK_جابه_جایی;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_رقم;
	}
	إذا (!مجموع_cnt)
		اذهب انجامشده;

	/* Ensure the ccache is able receive more than 16 requests */
	افعل {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_وضعیت(وضعیت_addr));
	} طالما (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_رقم);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_ماسک;

	إذا (مجموع_cnt < loop_cnt) {
		loop_cnt = (مجموع_cnt + CCACHE_ALL_OP_REQ_BATCH_رقم) & CCACHE_ALL_OP_REQ_BATCH_ماسک;
		cmd -= ((loop_cnt - مجموع_cnt) << CMD_CSR_BANK_جابه_جایی);
		مجموع_cnt = loop_cnt;
	}
	اذهب send_cmd;
انجامشده:
	افعل {} طالما (sifive_ccache_read_وضعیت(وضعیت_addr));
	ccache_mb_a();

	أرجع 0;
}

ساكن بنية cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

ساكن صحيح sifive_ccache_cold_init(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_تطابق *تطابق)
{
	بنية sifive_ccache *ccache;
	بنية cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	صحيح rc;

	/* find the ccache base control address */
	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 && reg_addr)
		أرجع SBI_ENODEV;

	ccache = sbi_zalloc(حجم(*ccache));
	إذا (!ccache)
		أرجع SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_جمع_کن(fdt, nodeoff, dev);
	إذا (rc) {
		sbi_free(ccache);
		أرجع rc;
	}

	ccache->addr = (فراغ *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_ماسک;

	sets = (config_csr & CFG_CSR_مجموعه_ماسک) >> CFG_CSR_مجموعه_جابه_جایی;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_ماسک) >> CFG_CSR_WAY_جابه_جایی;

	ccache->مجموع_سطرها = sets * ways * banks;

	أرجع SBI_OK;
}

ساكن ثابت بنية fdt_تطابق sifive_ccache_تطابق[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ثابت بنية fdt_driver fdt_sifive_ccache = {
	.تطابق_جدول = sifive_ccache_تطابق,
	.init = sifive_ccache_cold_init,
};
