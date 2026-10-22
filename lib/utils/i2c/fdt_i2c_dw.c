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

স্থির পূর্ণসংখ্যা fdt_dw_i2c_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			   ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(আকার(*adapter));
	যদি (!adapter)
		ফেরত SBI_ENOMEM;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &addr, ফাঁকা);
	যদি (rc) {
		sbi_free(adapter);
		ফেরত rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	যদি (rc) {
		sbi_free(adapter);
		ফেরত rc;
	}

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
