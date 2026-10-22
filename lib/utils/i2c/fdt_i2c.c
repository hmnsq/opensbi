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

static จำนวนเต็ม fdt_i2c_adapter_find(const void *fdt, จำนวนเต็ม nodeoff,
				sa_nracanaa i2c_adapter **out_adapter)
{
	จำนวนเต็ม rc;
	sa_nracanaa i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	ถ้า (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		ถ้า (rc)
			คืน rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		ถ้า (!adapter)
			คืน SBI_ENOSYS;
	}

	ถ้า (out_adapter)
		*out_adapter = adapter;

	คืน 0;
}

จำนวนเต็ม fdt_i2c_adapter_get(const void *fdt, จำนวนเต็ม nodeoff,
			sa_nracanaa i2c_adapter **out_adapter)
{
	จำนวนเต็ม rc;
	sa_nracanaa i2c_adapter *adapter;

	ถ้า (!fdt || (nodeoff < 0) || !out_adapter)
		คืน SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	ถ้า (rc)
		คืน rc;

	*out_adapter = adapter;

	คืน 0;
}
