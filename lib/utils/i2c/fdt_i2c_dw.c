/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/i2c/dw_i2c.h>
#অন্তর্ভাৱ <sbi_utils/i2c/fdt_i2c.h>

স্থির পূর্ণাঙ্ক fdt_dw_i2c_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			   ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(পরিমাণ(*adapter));
	যদি (!adapter)
		ঘূৰাই SBI_ENOMEM;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (rc) {
		sbi_free(adapter);
		ঘূৰাই rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	যদি (rc) {
		sbi_free(adapter);
		ঘূৰাই rc;
	}

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
