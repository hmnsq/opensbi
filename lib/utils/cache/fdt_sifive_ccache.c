/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/cache/fdt_cache.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>

#घोषणा CCACHE_CFG_CSR			0
#घोषणा CCACHE_CMD_CSR			0x280
#घोषणा CCACHE_STATUS_CSR		0x288

#घोषणा CFG_CSR_BANK_MASK		0xff
#घोषणा CFG_CSR_WAY_MASK		0xff00
#घोषणा CFG_CSR_WAY_OFFSET		8
#घोषणा CFG_CSR_SET_MASK		0xff0000
#घोषणा CFG_CSR_SET_OFFSET		16

#घोषणा CMD_CSR_CMD_OFFSET	56
#घोषणा CMD_CSR_BANK_OFFSET	6

#घोषणा CMD_OPCODE_SETWAY	0x1ULL
#घोषणा CMD_OPCODE_OFFSET	0x2ULL

#घोषणा CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_OFFSET) | 0x3)

#घोषणा CCACHE_CMD_QLEN	0xff

#घोषणा ccache_mb_b()		RISCV_FENCE(rw, o)
#घोषणा ccache_mb_a()		RISCV_FENCE(o, rw)

#घोषणा CCACHE_ALL_OP_REQ_BATCH_अंक	0x10
#घोषणा CCACHE_ALL_OP_REQ_BATCH_MASK (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_अंक)

काष्ठा sifive_ccache {
	काष्ठा cache_device dev;
	व्योम *addr;
	u64 योग_पंक्तियाँ;
};

#घोषणा to_ccache(_dev) container_of(_dev, काष्ठा sifive_ccache, dev)

अटल अंतरभूत अचिन्हित पूर्णांक sifive_ccache_read_status(व्योम *status_addr)
{
	वापस readl_relaxed(status_addr);
}

अटल अंतरभूत व्योम sifive_ccache_write_cmd(u64 cmd, व्योम *cmd_csr_addr)
{
#अगर __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#अन्यथा
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#पूर्ण_अगर
}

अटल पूर्णांक sifive_ccache_flush_all(काष्ठा cache_device *dev)
{
	काष्ठा sifive_ccache *ccache = to_ccache(dev);
	व्योम *status_addr = (अक्षर *)ccache->addr + CCACHE_STATUS_CSR;
	व्योम *cmd_csr_addr = (अक्षर *)ccache->addr + CCACHE_CMD_CSR;
	u64 योग_cnt = ccache->योग_पंक्तियाँ;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_OFFSET;
	पूर्णांक loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_MASK;

	ccache_mb_b();
send_cmd:
	योग_cnt -= loop_cnt;
	जबतक (loop_cnt > 0) {
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
		cmd += CCACHE_ALL_OP_REQ_BATCH_अंक << CMD_CSR_BANK_OFFSET;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_अंक;
	}
	अगर (!योग_cnt)
		जाओ done;

	/* Ensure the ccache is able receive more than 16 requests */
	करो {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_status(status_addr));
	} जबतक (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_अंक);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_MASK;

	अगर (योग_cnt < loop_cnt) {
		loop_cnt = (योग_cnt + CCACHE_ALL_OP_REQ_BATCH_अंक) & CCACHE_ALL_OP_REQ_BATCH_MASK;
		cmd -= ((loop_cnt - योग_cnt) << CMD_CSR_BANK_OFFSET);
		योग_cnt = loop_cnt;
	}
	जाओ send_cmd;
done:
	करो {} जबतक (sifive_ccache_read_status(status_addr));
	ccache_mb_a();

	वापस 0;
}

अटल काष्ठा cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

अटल पूर्णांक sifive_ccache_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff, स्थिर काष्ठा fdt_match *match)
{
	काष्ठा sifive_ccache *ccache;
	काष्ठा cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	पूर्णांक rc;

	/* find the ccache base control address */
	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, शून्य);
	अगर (rc < 0 && reg_addr)
		वापस SBI_ENODEV;

	ccache = sbi_zalloc(माप(*ccache));
	अगर (!ccache)
		वापस SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_add(fdt, nodeoff, dev);
	अगर (rc) {
		sbi_free(ccache);
		वापस rc;
	}

	ccache->addr = (व्योम *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_MASK;

	sets = (config_csr & CFG_CSR_SET_MASK) >> CFG_CSR_SET_OFFSET;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_MASK) >> CFG_CSR_WAY_OFFSET;

	ccache->योग_पंक्तियाँ = sets * ways * banks;

	वापस SBI_OK;
}

अटल स्थिर काष्ठा fdt_match sifive_ccache_match[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_sifive_ccache = {
	.match_table = sifive_ccache_match,
	.init = sifive_ccache_cold_init,
};
