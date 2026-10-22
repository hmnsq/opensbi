/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/i2c/dw_i2c.h>
#شامل <sbi_utils/i2c/fdt_i2c.h>

ایستا صحیح fdt_dw_i2c_init(ثابت تهی *fdt, صحیح nodeoff,
			   ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(اندازه(*adapter));
	اگر (!adapter)
		بازگشت SBI_ENOMEM;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &addr, پوچ);
	اگر (rc) {
		sbi_free(adapter);
		بازگشت rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	اگر (rc) {
		sbi_free(adapter);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match fdt_dw_i2c_match[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ثابت ساختار fdt_driver fdt_i2c_adapter_dw = {
	.match_table = fdt_dw_i2c_match,
	.init = fdt_dw_i2c_init,
};
