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

#include <sbi/sbi_error.h>
#include <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
extern const struct fdt_driver *const fdt_i2c_adapter_drivers[];

static 정수 fdt_i2c_adapter_find(const void *fdt, 정수 nodeoff,
				struct i2c_adapter **out_adapter)
{
	정수 rc;
	struct i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	만약 (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		만약 (rc)
			반환 rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		만약 (!adapter)
			반환 SBI_ENOSYS;
	}

	만약 (out_adapter)
		*out_adapter = adapter;

	반환 0;
}

정수 fdt_i2c_adapter_get(const void *fdt, 정수 nodeoff,
			struct i2c_adapter **out_adapter)
{
	정수 rc;
	struct i2c_adapter *adapter;

	만약 (!fdt || (nodeoff < 0) || !out_adapter)
		반환 SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	만약 (rc)
		반환 rc;

	*out_adapter = adapter;

	반환 0;
}
