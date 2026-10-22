/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 *
 * derivate: lib/utils/gpio/fdt_gpio.c
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
خارجی ثابت ساخت fdt_driver *ثابت fdt_i2c_adapter_drivers[];

ساکن صحیح fdt_i2c_adapter_ڈھونڈو(ثابت خالی *fdt, صحیح nodeoff,
				ساخت i2c_adapter **برآمد_adapter)
{
	صحیح rc;
	ساخت i2c_adapter *adapter = i2c_adapter_ڈھونڈو(nodeoff);

	اگر (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_انحراف(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		اگر (rc)
			واپس rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_ڈھونڈو(nodeoff);
		اگر (!adapter)
			واپس SBI_ENOSYS;
	}

	اگر (برآمد_adapter)
		*برآمد_adapter = adapter;

	واپس 0;
}

صحیح fdt_i2c_adapter_get(ثابت خالی *fdt, صحیح nodeoff,
			ساخت i2c_adapter **برآمد_adapter)
{
	صحیح rc;
	ساخت i2c_adapter *adapter;

	اگر (!fdt || (nodeoff < 0) || !برآمد_adapter)
		واپس SBI_EINVAL;

	rc = fdt_i2c_adapter_ڈھونڈو(fdt, nodeoff, &adapter);
	اگر (rc)
		واپس rc;

	*برآمد_adapter = adapter;

	واپس 0;
}
