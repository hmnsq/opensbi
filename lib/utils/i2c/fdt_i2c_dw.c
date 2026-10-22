/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_string.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/i2c/dw_i2c.h>
#include <sbi_utils/i2c/fdt_i2c.h>

static 整数 fdt_dw_i2c_init(const void *fdt, 整数 nodeoff,
			   const struct fdt_match *match)
{
	整数 rc;
	struct dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(sizeof(*adapter));
	如果 (!adapter)
		返回 SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	如果 (rc) {
		sbi_free(adapter);
		返回 rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	如果 (rc) {
		sbi_free(adapter);
		返回 rc;
	}

	返回 0;
}

static const struct fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

const struct fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
