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

ساكن صحيح fdt_i2c_adapter_پیدا_کن(ثابت فراغ *fdt, صحيح nodeoff,
				بنية i2c_adapter **بیرون_adapter)
{
	صحيح rc;
	بنية i2c_adapter *adapter = i2c_adapter_پیدا_کن(nodeoff);

	إذا (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_جابه_جایی(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		إذا (rc)
			أرجع rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_پیدا_کن(nodeoff);
		إذا (!adapter)
			أرجع SBI_ENOSYS;
	}

	إذا (بیرون_adapter)
		*بیرون_adapter = adapter;

	أرجع 0;
}

صحيح fdt_i2c_adapter_get(ثابت فراغ *fdt, صحيح nodeoff,
			بنية i2c_adapter **بیرون_adapter)
{
	صحيح rc;
	بنية i2c_adapter *adapter;

	إذا (!fdt || (nodeoff < 0) || !بیرون_adapter)
		أرجع SBI_EINVAL;

	rc = fdt_i2c_adapter_پیدا_کن(fdt, nodeoff, &adapter);
	إذا (rc)
		أرجع rc;

	*بیرون_adapter = adapter;

	أرجع 0;
}
