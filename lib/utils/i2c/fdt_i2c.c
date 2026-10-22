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

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
বাহ্য ধ্রুব সংরচনা fdt_driver *ধ্রুব fdt_i2c_adapter_drivers[];

স্থির পূর্ণাঙ্ক fdt_i2c_adapter_find(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				সংরচনা i2c_adapter **out_adapter)
{
	পূর্ণাঙ্ক rc;
	সংরচনা i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	যদি (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		যদি (rc)
			প্রত্যাগম rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		যদি (!adapter)
			প্রত্যাগম SBI_ENOSYS;
	}

	যদি (out_adapter)
		*out_adapter = adapter;

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_i2c_adapter_get(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			সংরচনা i2c_adapter **out_adapter)
{
	পূর্ণাঙ্ক rc;
	সংরচনা i2c_adapter *adapter;

	যদি (!fdt || (nodeoff < 0) || !out_adapter)
		প্রত্যাগম SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	যদি (rc)
		প্রত্যাগম rc;

	*out_adapter = adapter;

	প্রত্যাগম 0;
}
