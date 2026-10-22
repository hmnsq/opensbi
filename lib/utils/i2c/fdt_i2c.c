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

static puur_naa_nka fdt_i2c_adapter_find(const void *fdt, puur_naa_nka nodeoff,
				sa_nracanaa i2c_adapter **out_adapter)
{
	puur_naa_nka rc;
	sa_nracanaa i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	ᨀᨘ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		ᨀᨘ (rc)
			ᨀᨘᨒ rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		ᨀᨘ (!adapter)
			ᨀᨘᨒ SBI_ENOSYS;
	}

	ᨀᨘ (out_adapter)
		*out_adapter = adapter;

	ᨀᨘᨒ 0;
}

puur_naa_nka fdt_i2c_adapter_get(const void *fdt, puur_naa_nka nodeoff,
			sa_nracanaa i2c_adapter **out_adapter)
{
	puur_naa_nka rc;
	sa_nracanaa i2c_adapter *adapter;

	ᨀᨘ (!fdt || (nodeoff < 0) || !out_adapter)
		ᨀᨘᨒ SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	ᨀᨘ (rc)
		ᨀᨘᨒ rc;

	*out_adapter = adapter;

	ᨀᨘᨒ 0;
}
