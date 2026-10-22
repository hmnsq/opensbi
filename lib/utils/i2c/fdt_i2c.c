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

static ຈຳນວນເຕັມ fdt_i2c_adapter_find(const void *fdt, ຈຳນວນເຕັມ nodeoff,
				sa_nracanaa i2c_adapter **out_adapter)
{
	ຈຳນວນເຕັມ rc;
	sa_nracanaa i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	ຖ້າ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		ຖ້າ (!adapter)
			ສົ່ງຄືນ SBI_ENOSYS;
	}

	ຖ້າ (out_adapter)
		*out_adapter = adapter;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_i2c_adapter_get(const void *fdt, ຈຳນວນເຕັມ nodeoff,
			sa_nracanaa i2c_adapter **out_adapter)
{
	ຈຳນວນເຕັມ rc;
	sa_nracanaa i2c_adapter *adapter;

	ຖ້າ (!fdt || (nodeoff < 0) || !out_adapter)
		ສົ່ງຄືນ SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	*out_adapter = adapter;

	ສົ່ງຄືນ 0;
}
