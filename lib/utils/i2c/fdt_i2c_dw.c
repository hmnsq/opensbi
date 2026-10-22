/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/i2c/dw_i2c.h>
#അന്തര്ഭാവ <sbi_utils/i2c/fdt_i2c.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_dw_i2c_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			   സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(വലുപ്പം(*adapter));
	എങ്കിൽ (!adapter)
		തിരികെ SBI_ENOMEM;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (rc) {
		sbi_free(adapter);
		തിരികെ rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	എങ്കിൽ (rc) {
		sbi_free(adapter);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
