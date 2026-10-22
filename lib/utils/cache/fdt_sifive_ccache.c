/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/cache/fdt_cache.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>

#ವ್ಯಾಖ್ಯಾನಿಸು CCACHE_CFG_CSR			0
#ವ್ಯಾಖ್ಯಾನಿಸು CCACHE_CMD_CSR			0x280
#ವ್ಯಾಖ್ಯಾನಿಸು CCACHE_ಸ್ಥಿತಿ_CSR		0x288

#ವ್ಯಾಖ್ಯಾನಿಸು CFG_CSR_BANK_ಮುಖವಾಡ		0xff
#ವ್ಯಾಖ್ಯಾನಿಸು CFG_CSR_WAY_ಮುಖವಾಡ		0xff00
#ವ್ಯಾಖ್ಯಾನಿಸು CFG_CSR_WAY_ಸ್ಥಾನಾಂತರ		8
#ವ್ಯಾಖ್ಯಾನಿಸು CFG_CSR_ಗಣ_ಮುಖವಾಡ		0xff0000
#ವ್ಯಾಖ್ಯಾನಿಸು CFG_CSR_ಗಣ_ಸ್ಥಾನಾಂತರ		16

#ವ್ಯಾಖ್ಯಾನಿಸು CMD_CSR_CMD_ಸ್ಥಾನಾಂತರ	56
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ	6

#ವ್ಯಾಖ್ಯಾನಿಸು CMD_OPCODE_SETWAY	0x1ULL
#ವ್ಯಾಖ್ಯಾನಿಸು CMD_OPCODE_ಸ್ಥಾನಾಂತರ	0x2ULL

#ವ್ಯಾಖ್ಯಾನಿಸು CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_ಸ್ಥಾನಾಂತರ) | 0x3)

#ವ್ಯಾಖ್ಯಾನಿಸು CCACHE_CMD_QLEN	0xff

#ವ್ಯಾಖ್ಯಾನಿಸು ccache_mb_b()		RISCV_FENCE(rw, o)
#ವ್ಯಾಖ್ಯಾನಿಸು ccache_mb_a()		RISCV_FENCE(o, rw)

#ವ್ಯಾಖ್ಯಾನಿಸು CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ	0x10
#ವ್ಯಾಖ್ಯಾನಿಸು CCACHE_ALL_OP_REQ_BATCH_ಮುಖವಾಡ (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ)

ರಚನೆ sifive_ccache {
	ರಚನೆ cache_device dev;
	ಶೂನ್ಯ *addr;
	u64 ಒಟ್ಟು_ಸಾಲುಗಳು;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_ccache(_dev) container_ದ(_dev, ರಚನೆ sifive_ccache, dev)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sifive_ccache_read_ಸ್ಥಿತಿ(ಶೂನ್ಯ *ಸ್ಥಿತಿ_addr)
{
	ಹಿಂತಿರುಗಿ readl_relaxed(ಸ್ಥಿತಿ_addr);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ sifive_ccache_write_cmd(u64 cmd, ಶೂನ್ಯ *cmd_csr_addr)
{
#ಆದರೆ __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#ಇಲ್ಲದಿದ್ದರೆ
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_ccache_flush_all(ರಚನೆ cache_device *dev)
{
	ರಚನೆ sifive_ccache *ccache = ಗಮ್ಯ_ccache(dev);
	ಶೂನ್ಯ *ಸ್ಥಿತಿ_addr = (ಅಕ್ಷರ *)ccache->addr + CCACHE_ಸ್ಥಿತಿ_CSR;
	ಶೂನ್ಯ *cmd_csr_addr = (ಅಕ್ಷರ *)ccache->addr + CCACHE_CMD_CSR;
	u64 ಒಟ್ಟು_cnt = ccache->ಒಟ್ಟು_ಸಾಲುಗಳು;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_ಮುಖವಾಡ;

	ccache_mb_b();
send_cmd:
	ಒಟ್ಟು_cnt -= loop_cnt;
	ಆಗ (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ;
	}
	ಆದರೆ (!ಒಟ್ಟು_cnt)
		ಹೋಗು ಮುಗಿದ;

	/* Ensure the ccache is able receive more than 16 requests */
	ಮಾಡು {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_addr));
	} ಆಗ (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_ಮುಖವಾಡ;

	ಆದರೆ (ಒಟ್ಟು_cnt < loop_cnt) {
		loop_cnt = (ಒಟ್ಟು_cnt + CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ) & CCACHE_ALL_OP_REQ_BATCH_ಮುಖವಾಡ;
		cmd -= ((loop_cnt - ಒಟ್ಟು_cnt) << CMD_CSR_BANK_ಸ್ಥಾನಾಂತರ);
		ಒಟ್ಟು_cnt = loop_cnt;
	}
	ಹೋಗು send_cmd;
ಮುಗಿದ:
	ಮಾಡು {} ಆಗ (sifive_ccache_read_ಸ್ಥಿತಿ(ಸ್ಥಿತಿ_addr));
	ccache_mb_a();

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_ccache_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ರಚನೆ sifive_ccache *ccache;
	ರಚನೆ cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	ಪೂರ್ಣಾಂಕ rc;

	/* find the ccache base control address */
	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, ಖಾಲಿ);
	ಆದರೆ (rc < 0 && reg_addr)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ccache = sbi_zalloc(ಗಾತ್ರ(*ccache));
	ಆದರೆ (!ccache)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_ಕೂಡಿಸು(fdt, nodeoff, dev);
	ಆದರೆ (rc) {
		sbi_free(ccache);
		ಹಿಂತಿರುಗಿ rc;
	}

	ccache->addr = (ಶೂನ್ಯ *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_ಮುಖವಾಡ;

	sets = (config_csr & CFG_CSR_ಗಣ_ಮುಖವಾಡ) >> CFG_CSR_ಗಣ_ಸ್ಥಾನಾಂತರ;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_ಮುಖವಾಡ) >> CFG_CSR_WAY_ಸ್ಥಾನಾಂತರ;

	ccache->ಒಟ್ಟು_ಸಾಲುಗಳು = sets * ways * banks;

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sifive_ccache_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_sifive_ccache = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sifive_ccache_ಹೊಂದಿಕೆ,
	.init = sifive_ccache_cold_init,
};
