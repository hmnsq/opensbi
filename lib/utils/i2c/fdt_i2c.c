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
extern const sa_nracanaa fdt_driver *const fdt_i2c_adapter_drivers[];

static ཧྲིལ fdt_i2c_adapter_find(const void *fdt, ཧྲིལ nodeoff,
				sa_nracanaa i2c_adapter **out_adapter)
{
	ཧྲིལ rc;
	sa_nracanaa i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	གལ་ཏེ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		གལ་ཏེ (rc)
			ཕྱིར rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		གལ་ཏེ (!adapter)
			ཕྱིར SBI_ENOSYS;
	}

	གལ་ཏེ (out_adapter)
		*out_adapter = adapter;

	ཕྱིར 0;
}

ཧྲིལ fdt_i2c_adapter_get(const void *fdt, ཧྲིལ nodeoff,
			sa_nracanaa i2c_adapter **out_adapter)
{
	ཧྲིལ rc;
	sa_nracanaa i2c_adapter *adapter;

	གལ་ཏེ (!fdt || (nodeoff < 0) || !out_adapter)
		ཕྱིར SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	*out_adapter = adapter;

	ཕྱིར 0;
}
