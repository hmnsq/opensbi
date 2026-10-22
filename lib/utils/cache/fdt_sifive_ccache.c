/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cache.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>

#परिभाषय CCACHE_CFG_CSR			0
#परिभाषय CCACHE_CMD_CSR			0x280
#परिभाषय CCACHE_स्थितिः_CSR		0x288

#परिभाषय CFG_CSR_BANK_आवरणम्		0xff
#परिभाषय CFG_CSR_WAY_आवरणम्		0xff00
#परिभाषय CFG_CSR_WAY_विस्थापनम्		8
#परिभाषय CFG_CSR_समुच्चयः_आवरणम्		0xff0000
#परिभाषय CFG_CSR_समुच्चयः_विस्थापनम्		16

#परिभाषय CMD_CSR_CMD_विस्थापनम्	56
#परिभाषय CMD_CSR_BANK_विस्थापनम्	6

#परिभाषय CMD_OPCODE_SETWAY	0x1ULL
#परिभाषय CMD_OPCODE_विस्थापनम्	0x2ULL

#परिभाषय CFLUSH_SETWAY_CLEANINV  ((CMD_OPCODE_SETWAY << CMD_OPCODE_विस्थापनम्) | 0x3)

#परिभाषय CCACHE_CMD_QLEN	0xff

#परिभाषय ccache_mb_b()		RISCV_FENCE(rw, o)
#परिभाषय ccache_mb_a()		RISCV_FENCE(o, rw)

#परिभाषय CCACHE_ALL_OP_REQ_BATCH_अङ्कः	0x10
#परिभाषय CCACHE_ALL_OP_REQ_BATCH_आवरणम् (CCACHE_CMD_QLEN + 1 - CCACHE_ALL_OP_REQ_BATCH_अङ्कः)

संरचना sifive_ccache {
	संरचना cache_device dev;
	शून्य *addr;
	u64 योग_पङ्क्तयः;
};

#परिभाषय गन्तव्यम्_ccache(_dev) container_सम्बन्धि(_dev, संरचना sifive_ccache, dev)

स्थिर अन्तःस्थ अचिह्नित पूर्णाङ्क sifive_ccache_read_स्थितिः(शून्य *स्थितिः_addr)
{
	प्रत्यागम readl_relaxed(स्थितिः_addr);
}

स्थिर अन्तःस्थ शून्य sifive_ccache_write_cmd(u64 cmd, शून्य *cmd_csr_addr)
{
#यदि __riscv_xlen != 32
	writeq_relaxed(cmd, cmd_csr_addr);
#अन्यथा
	/*
	 * The cache maintenance request is only generated when the "command"
	 * field (part of the high word) is written.
	 */
	writel_relaxed(cmd, cmd_csr_addr);
	writel(cmd >> 32, cmd_csr_addr + 4);
#यदि_अन्तः
}

स्थिर पूर्णाङ्क sifive_ccache_flush_all(संरचना cache_device *dev)
{
	संरचना sifive_ccache *ccache = गन्तव्यम्_ccache(dev);
	शून्य *स्थितिः_addr = (वर्ण *)ccache->addr + CCACHE_स्थितिः_CSR;
	शून्य *cmd_csr_addr = (वर्ण *)ccache->addr + CCACHE_CMD_CSR;
	u64 योग_cnt = ccache->योग_पङ्क्तयः;
	u64 cmd = CFLUSH_SETWAY_CLEANINV << CMD_CSR_CMD_विस्थापनम्;
	पूर्णाङ्क loop_cnt = CCACHE_CMD_QLEN & CCACHE_ALL_OP_REQ_BATCH_आवरणम्;

	ccache_mb_b();
send_cmd:
	योग_cnt -= loop_cnt;
	यावत् (loop_cnt > 0) {
		sifive_ccache_write_cmd(cmd + (0 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (1 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (2 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (3 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (4 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (5 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (6 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (7 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (8 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (9 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (10 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (11 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (12 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (13 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (14 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		sifive_ccache_write_cmd(cmd + (15 << CMD_CSR_BANK_विस्थापनम्), cmd_csr_addr);
		cmd += CCACHE_ALL_OP_REQ_BATCH_अङ्कः << CMD_CSR_BANK_विस्थापनम्;
		loop_cnt -= CCACHE_ALL_OP_REQ_BATCH_अङ्कः;
	}
	यदि (!योग_cnt)
		गच्छ समाप्तम्;

	/* Ensure the ccache is able receive more than 16 requests */
	कुरु {
		loop_cnt = (CCACHE_CMD_QLEN - sifive_ccache_read_स्थितिः(स्थितिः_addr));
	} यावत् (loop_cnt < CCACHE_ALL_OP_REQ_BATCH_अङ्कः);
	loop_cnt &= CCACHE_ALL_OP_REQ_BATCH_आवरणम्;

	यदि (योग_cnt < loop_cnt) {
		loop_cnt = (योग_cnt + CCACHE_ALL_OP_REQ_BATCH_अङ्कः) & CCACHE_ALL_OP_REQ_BATCH_आवरणम्;
		cmd -= ((loop_cnt - योग_cnt) << CMD_CSR_BANK_विस्थापनम्);
		योग_cnt = loop_cnt;
	}
	गच्छ send_cmd;
समाप्तम्:
	कुरु {} यावत् (sifive_ccache_read_स्थितिः(स्थितिः_addr));
	ccache_mb_a();

	प्रत्यागम 0;
}

स्थिर संरचना cache_ops sifive_ccache_ops = {
	.cache_flush_all = sifive_ccache_flush_all,
};

स्थिर पूर्णाङ्क sifive_ccache_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	संरचना sifive_ccache *ccache;
	संरचना cache_device *dev;
	u64 reg_addr = 0;
	u32 config_csr, banks, sets, ways;
	पूर्णाङ्क rc;

	/* find the ccache base control address */
	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &reg_addr, रिक्त);
	यदि (rc < 0 && reg_addr)
		प्रत्यागम SBI_ENODEV;

	ccache = sbi_zalloc(परिमाण(*ccache));
	यदि (!ccache)
		प्रत्यागम SBI_ENOMEM;

	dev = &ccache->dev;
	dev->ops = &sifive_ccache_ops;
	rc = fdt_cache_सङ्कलय(fdt, nodeoff, dev);
	यदि (rc) {
		sbi_free(ccache);
		प्रत्यागम rc;
	}

	ccache->addr = (शून्य *)(uintptr_t)reg_addr;

	/* get the info of ccache from config CSR */
	config_csr = readl(ccache->addr + CCACHE_CFG_CSR);
	banks = config_csr & CFG_CSR_BANK_आवरणम्;

	sets = (config_csr & CFG_CSR_समुच्चयः_आवरणम्) >> CFG_CSR_समुच्चयः_विस्थापनम्;
	sets = (1 << sets);

	ways = (config_csr & CFG_CSR_WAY_आवरणम्) >> CFG_CSR_WAY_विस्थापनम्;

	ccache->योग_पङ्क्तयः = sets * ways * banks;

	प्रत्यागम SBI_OK;
}

स्थिर ध्रुव संरचना fdt_मेलनम् sifive_ccache_मेलनम्[] = {
	{ .compatible = "sifive,ccache2" },
	{},
};

ध्रुव संरचना fdt_driver fdt_sifive_ccache = {
	.मेलनम्_सारणी = sifive_ccache_मेलनम्,
	.init = sifive_ccache_cold_init,
};
