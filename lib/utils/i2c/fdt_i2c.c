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

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ fdt_i2c_adapter_drivers[];

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_i2c_adapter_find(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				ಸಂರಚನಾ i2c_adapter **out_adapter)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	ಯದಿ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		ಯದಿ (!adapter)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;
	}

	ಯದಿ (out_adapter)
		*out_adapter = adapter;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_i2c_adapter_get(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			ಸಂರಚನಾ i2c_adapter **out_adapter)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ i2c_adapter *adapter;

	ಯದಿ (!fdt || (nodeoff < 0) || !out_adapter)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	*out_adapter = adapter;

	ಪ್ರತ್ಯಾಗಮ 0;
}
