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

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_i2c_adapter_drivers[];

ساكن صحيح fdt_i2c_adapter_find(ثابت فراغ *fdt, صحيح nodeoff,
				بنية i2c_adapter **out_adapter)
{
	صحيح rc;
	بنية i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	ܐܢ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		ܐܢ (rc)
			ܦܢܐ rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		ܐܢ (!adapter)
			ܦܢܐ SBI_ENOSYS;
	}

	ܐܢ (out_adapter)
		*out_adapter = adapter;

	ܦܢܐ 0;
}

صحيح fdt_i2c_adapter_get(ثابت فراغ *fdt, صحيح nodeoff,
			بنية i2c_adapter **out_adapter)
{
	صحيح rc;
	بنية i2c_adapter *adapter;

	ܐܢ (!fdt || (nodeoff < 0) || !out_adapter)
		ܦܢܐ SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	ܐܢ (rc)
		ܦܢܐ rc;

	*out_adapter = adapter;

	ܦܢܐ 0;
}
