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

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_i2c_adapter_drivers[];

स्थिर पूर्णाङ्क fdt_i2c_adapter_find(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				संरचना i2c_adapter **out_adapter)
{
	पूर्णाङ्क rc;
	संरचना i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	जइ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		जइ (rc)
			प्रत्यागम rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		जइ (!adapter)
			प्रत्यागम SBI_ENOSYS;
	}

	जइ (out_adapter)
		*out_adapter = adapter;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_i2c_adapter_get(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			संरचना i2c_adapter **out_adapter)
{
	पूर्णाङ्क rc;
	संरचना i2c_adapter *adapter;

	जइ (!fdt || (nodeoff < 0) || !out_adapter)
		प्रत्यागम SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	जइ (rc)
		प्रत्यागम rc;

	*out_adapter = adapter;

	प्रत्यागम 0;
}
