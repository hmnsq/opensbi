/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/i2c/dw_i2c.h>
#અન્તર્ભાવ <sbi_utils/i2c/fdt_i2c.h>

સ્થિર પૂર્ણાંક fdt_dw_i2c_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			   અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(કદ(*adapter));
	જો (!adapter)
		પાછા SBI_ENOMEM;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, ખાલી);
	જો (rc) {
		sbi_free(adapter);
		પાછા rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	જો (rc) {
		sbi_free(adapter);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

અચલ સંરચના fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
