/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/i2c/dw_i2c.h>
#অন্তর্ভাব <sbi_utils/i2c/fdt_i2c.h>

স্থির পূর্ণাঙ্ক fdt_dw_i2c_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			   ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(পরিমাণ(*adapter));
	যদি (!adapter)
		প্রত্যাগম SBI_ENOMEM;

	rc = fdt_get_গ্রন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (rc) {
		sbi_free(adapter);
		প্রত্যাগম rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	যদি (rc) {
		sbi_free(adapter);
		প্রত্যাগম rc;
	}

	প্রত্যাগম 0;
}

স্থির ধ্রুব সংরচনা fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
