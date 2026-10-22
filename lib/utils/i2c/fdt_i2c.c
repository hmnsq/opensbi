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
বাহ্যিক ধ্রুবক কাঠামো fdt_driver *ধ্রুবক fdt_i2c_adapter_drivers[];

স্থির পূর্ণসংখ্যা fdt_i2c_adapter_find(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				কাঠামো i2c_adapter **out_adapter)
{
	পূর্ণসংখ্যা rc;
	কাঠামো i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	যদি (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		যদি (rc)
			ফেরত rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		যদি (!adapter)
			ফেরত SBI_ENOSYS;
	}

	যদি (out_adapter)
		*out_adapter = adapter;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_i2c_adapter_get(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			কাঠামো i2c_adapter **out_adapter)
{
	পূর্ণসংখ্যা rc;
	কাঠামো i2c_adapter *adapter;

	যদি (!fdt || (nodeoff < 0) || !out_adapter)
		ফেরত SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	যদি (rc)
		ফেরত rc;

	*out_adapter = adapter;

	ফেরত 0;
}
