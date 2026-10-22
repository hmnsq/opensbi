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

static ကိန်းပြည့် fdt_i2c_adapter_find(const void *fdt, ကိန်းပြည့် nodeoff,
				sa_nracanaa i2c_adapter **out_adapter)
{
	ကိန်းပြည့် rc;
	sa_nracanaa i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	အကယ်၍ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		အကယ်၍ (!adapter)
			ပြန်ပေး SBI_ENOSYS;
	}

	အကယ်၍ (out_adapter)
		*out_adapter = adapter;

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_i2c_adapter_get(const void *fdt, ကိန်းပြည့် nodeoff,
			sa_nracanaa i2c_adapter **out_adapter)
{
	ကိန်းပြည့် rc;
	sa_nracanaa i2c_adapter *adapter;

	အကယ်၍ (!fdt || (nodeoff < 0) || !out_adapter)
		ပြန်ပေး SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	*out_adapter = adapter;

	ပြန်ပေး 0;
}
