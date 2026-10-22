/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/cache/fdt_cache.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCACHE_CFG_CSR			0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCACHE_CMD_CSR			0x280
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCACHE_ਸਥਿਤੀ_CSR		0x288

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFG_CSR_BANK_ਨਕਾਬ		0xff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFG_CSR_WAY_ਨਕਾਬ		0xff00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFG_CSR_WAY_ਵਿੱਥ		8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFG_CSR_ਸੈੱਟ_ਨਕਾਬ		0xff0000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFG_CSR_ਸੈੱਟ_ਵਿੱਥ		16

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_CSR_CMD_ਵਿੱਥ	56
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_CSR_BANK_ਵਿੱਥ	6

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_OPCODE_SETWAY	0x1ULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMD_OPCODE_ਵਿੱਥ	0x2ULL

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_ਵਿੱਥ) | 0x3)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCACHE_CMD_QLEN	0xff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ccache_mb_b()		RISCV_FENCE(rw, o)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ccache_mb_a()		RISCV_FENCE(o, rw)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCACHE_ALL_OP_REQ_BATCH_ਅੰਕ	0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCACHE_ALL_OP_REQ_BATCH_ਨਕਾਬ (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_ਅੰਕ)

ਬਣਤਰ sifive_ccache {
	ਬਣਤਰ cache_device dev;
	ਖਾਲੀ *addr;
	u64 ਕੁੱਲ_ਸਤਰਾਂ;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_ccache(_dev) container_ਦਾ(_dev, ਬਣਤਰ sifive_ccache, dev)

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ sifive_ccache_read_ਸਥਿਤੀ(ਖਾਲੀ *ਸਥਿਤੀ_addr)
{
	ਵਾਪਸ readl_relaxed(ਸਥਿਤੀ_addr);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ sifive_ccache_write_cmd(u64 cmd, ਖਾਲੀ *cmd_csr_addr)
{
#ਜੇ __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#ਨਹੀਂਤਾਂ
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ sifive_ccache_flush_all(ਬਣਤਰ cache_device *dev)
{
	ਬਣਤਰ sifive_ccache *ccache = ਮੰਜ਼ਿਲ_ccache(dev);
	ਖਾਲੀ *ਸਥਿਤੀ_addr = (ਅੱਖਰ *)ccache->addr + CCACHE_ਸਥਿਤੀ_CSR;
	ਖਾਲੀ *cmd_csr_addr = (ਅੱਖਰ *)ccache->addr + CCACHE_CMD_CSR;
	u64 ਕੁੱਲ_cnt = ccache->ਕੁੱਲ_ਸਤਰਾਂ;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_ਵਿੱਥ;
	ਪੂਰਨ loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_ਨਕਾਬ;

	ccache_mb_b();
send_cmd:
	ਕੁੱਲ_cnt -= loop_cnt;
	ਜਦੋਂ (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_ਵਿੱਥ), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_ਅੰਕ << CMD_CSR_BANK_ਵਿੱਥ;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_ਅੰਕ;
	}
	ਜੇ (!ਕੁੱਲ_cnt)
		ਜਾਓ ਪੂਰਾ;

	/* Ensure the ccache is able receive more than 16 requests */
	ਕਰੋ {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_ਸਥਿਤੀ(ਸਥਿਤੀ_addr));
	} ਜਦੋਂ (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_ਅੰਕ);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_ਨਕਾਬ;

	ਜੇ (ਕੁੱਲ_cnt < loop_cnt) {
		loop_cnt = (ਕੁੱਲ_cnt + CCACHE_ALL_OP_REQ_BATCH_ਅੰਕ) & CCACHE_ALL_OP_REQ_BATCH_ਨਕਾਬ;
		cmd -= ((loop_cnt - ਕੁੱਲ_cnt) << CMD_CSR_BANK_ਵਿੱਥ);
		ਕੁੱਲ_cnt = loop_cnt;
	}
	ਜਾਓ send_cmd;
ਪੂਰਾ:
	ਕਰੋ {} ਜਦੋਂ (sifive_ccache_read_ਸਥਿਤੀ(ਸਥਿਤੀ_addr));
	ccache_mb_a();

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

ਸਥਿਰ ਪੂਰਨ sifive_ccache_cold_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਬਣਤਰ sifive_ccache *ccache;
	ਬਣਤਰ cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	ਪੂਰਨ rc;

	/* find the ccache base control address */
	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, ਨਿਰਾ);
	ਜੇ (rc < 0 && reg_addr)
		ਵਾਪਸ SBI_ENODEV;

	ccache = sbi_zalloc(ਆਕਾਰ(*ccache));
	ਜੇ (!ccache)
		ਵਾਪਸ SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_ਜੋੜੋ(fdt, nodeoff, dev);
	ਜੇ (rc) {
		sbi_free(ccache);
		ਵਾਪਸ rc;
	}

	ccache->addr = (ਖਾਲੀ *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_ਨਕਾਬ;

	sets = (config_csr & CFG_CSR_ਸੈੱਟ_ਨਕਾਬ) >> CFG_CSR_ਸੈੱਟ_ਵਿੱਥ;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_ਨਕਾਬ) >> CFG_CSR_WAY_ਵਿੱਥ;

	ccache->ਕੁੱਲ_ਸਤਰਾਂ = sets * ways * banks;

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sifive_ccache_ਮੇਲ[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_sifive_ccache = {
	.ਮੇਲ_ਸਾਰਣੀ = sifive_ccache_ਮੇਲ,
	.init = sifive_ccache_cold_init,
};
