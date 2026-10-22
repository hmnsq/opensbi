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
بیرونی ثابت ساختار fdt_driver *ثابت fdt_i2c_adapter_drivers[];

ایستا صحیح fdt_i2c_adapter_find(ثابت تهی *fdt, صحیح nodeoff,
				ساختار i2c_adapter **out_adapter)
{
	صحیح rc;
	ساختار i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	اگر (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		اگر (rc)
			بازگشت rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		اگر (!adapter)
			بازگشت SBI_ENOSYS;
	}

	اگر (out_adapter)
		*out_adapter = adapter;

	بازگشت 0;
}

صحیح fdt_i2c_adapter_get(ثابت تهی *fdt, صحیح nodeoff,
			ساختار i2c_adapter **out_adapter)
{
	صحیح rc;
	ساختار i2c_adapter *adapter;

	اگر (!fdt || (nodeoff < 0) || !out_adapter)
		بازگشت SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	اگر (rc)
		بازگشت rc;

	*out_adapter = adapter;

	بازگشت 0;
}
