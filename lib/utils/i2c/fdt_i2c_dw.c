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

static puur_naa_nka fdt_dw_i2c_init(const void *fdt, puur_naa_nka nodeoff,
			   const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc;
	sa_nracanaa dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(parimaa_na(*adapter));
	ᬬᬾᬦ᭄ (!adapter)
		ᬯᬮᬶ SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	ᬬᬾᬦ᭄ (rc) {
		sbi_free(adapter);
		ᬯᬮᬶ rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	ᬬᬾᬦ᭄ (rc) {
		sbi_free(adapter);
		ᬯᬮᬶ rc;
	}

	ᬯᬮᬶ 0;
}

static const sa_nracanaa fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

const sa_nracanaa fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
