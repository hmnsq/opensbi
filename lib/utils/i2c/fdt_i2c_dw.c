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

ساکن صحیح fdt_dw_i2c_init(ثابت خالی *fdt, صحیح nodeoff,
			   ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت dw_i2c_adapter *adapter;
	u64 addr;

	adapter = sbi_zalloc(حجم(*adapter));
	اگر (!adapter)
		واپس SBI_ENOMEM;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, تہی);
	اگر (rc) {
		sbi_free(adapter);
		واپس rc;
	}

	adapter->addr = addr;

	rc = dw_i2c_init(&adapter->adapter, nodeoff);
	اگر (rc) {
		sbi_free(adapter);
		واپس rc;
	}

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت fdt_dw_i2c_مطابقت[] = {
	{ .compatible = "snps,designware-i2c" },
	{ .compatible = "starfive,jh7110-i2c" },
	{ },
};

ثابت ساخت fdt_driver fdt_i2c_adapter_dw = {
	.مطابقت_جدول = fdt_dw_i2c_مطابقت,
	.init = fdt_dw_i2c_init,
};
