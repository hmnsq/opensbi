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

#define CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ	0x10
#define CCACHE_ALL_OP_REQ_BATCH_MASK (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ)

ಸಂರಚನಾ sifive_ccache {
	ಸಂರಚನಾ cache_device dev;
	ಶೂನ್ಯ *addr;
	u64 total_ಸಾಲುಗಳು;
};

#define to_ccache(_dev) container_of(_dev, ಸಂರಚನಾ sifive_ccache, dev)

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned ಪೂರ್ಣಾಙ್ಕ sifive_ccache_read_status(ಶೂನ್ಯ *status_addr)
{
	ಪ್ರತ್ಯಾಗಮ readl_relaxed(status_addr);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ sifive_ccache_write_cmd(u64 cmd, ಶೂನ್ಯ *cmd_csr_addr)
{
#ಯದಿ __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#ಅನ್ಯಥಾ
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_ccache_flush_all(ಸಂರಚನಾ cache_device *dev)
{
	ಸಂರಚನಾ sifive_ccache *ccache = to_ccache(dev);
	ಶೂನ್ಯ *status_addr = (ವರ್ಣ *)ccache->addr + CCACHE_STATUS_CSR;
	ಶೂನ್ಯ *cmd_csr_addr = (ವರ್ಣ *)ccache->addr + CCACHE_CMD_CSR;
	u64 total_cnt = ccache->total_ಸಾಲುಗಳು;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_OFFSET;
	ಪೂರ್ಣಾಙ್ಕ loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_MASK;

	ccache_mb_b();
send_cmd:
	total_cnt -= loop_cnt;
	ಯಾವತ್ (loop_cnt > 0) {
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
		cmd += CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ << CMD_CSR_BANK_OFFSET;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ;
	}
	ಯದಿ (!total_cnt)
		ಗಚ್ಛ done;

	/* Ensure the ccache is able receive more than 16 requests */
	ಕುರು {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_status(status_addr));
	} ಯಾವತ್ (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_MASK;

	ಯದಿ (total_cnt < loop_cnt) {
		loop_cnt = (total_cnt + CCACHE_ALL_OP_REQ_BATCH_ಅಂಕೆ) & CCACHE_ALL_OP_REQ_BATCH_MASK;
		cmd -= ((loop_cnt - total_cnt) << CMD_CSR_BANK_OFFSET);
		total_cnt = loop_cnt;
	}
	ಗಚ್ಛ send_cmd;
done:
	ಕುರು {} ಯಾವತ್ (sifive_ccache_read_status(status_addr));
	ccache_mb_a();

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಸಂರಚನಾ cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_ccache_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಸಂರಚನಾ sifive_ccache *ccache;
	ಸಂರಚನಾ cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* find the ccache base control address */
	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, ರಿಕ್ತ);
	ಯದಿ (rc < 0 && reg_addr)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ccache = sbi_zalloc(ಪರಿಮಾಣ(*ccache));
	ಯದಿ (!ccache)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	ಯದಿ (rc) {
		sbi_free(ccache);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ccache->addr = (ಶೂನ್ಯ *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_MASK;

	sets = (config_csr & CFG_CSR_SET_MASK) >> CFG_CSR_SET_OFFSET;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_MASK) >> CFG_CSR_WAY_OFFSET;

	ccache->total_ಸಾಲುಗಳು = sets * ways * banks;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sifive_ccache_match[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_sifive_ccache = {
	.match_table = sifive_ccache_match,
	.init = sifive_ccache_cold_init,
};
