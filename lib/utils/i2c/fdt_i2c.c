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

static số_nguyên fdt_i2c_adapter_find(const void *fdt, số_nguyên nodeoff,
				struct i2c_adapter **out_adapter)
{
	số_nguyên rc;
	struct i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	nếu (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		nếu (rc)
			trả_về rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		nếu (!adapter)
			trả_về SBI_ENOSYS;
	}

	nếu (out_adapter)
		*out_adapter = adapter;

	trả_về 0;
}

số_nguyên fdt_i2c_adapter_get(const void *fdt, số_nguyên nodeoff,
			struct i2c_adapter **out_adapter)
{
	số_nguyên rc;
	struct i2c_adapter *adapter;

	nếu (!fdt || (nodeoff < 0) || !out_adapter)
		trả_về SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	nếu (rc)
		trả_về rc;

	*out_adapter = adapter;

	trả_về 0;
}
