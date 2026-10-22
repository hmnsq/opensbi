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
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_i2c_adapter_drivers[];

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_i2c_adapter_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				ರಚನೆ i2c_adapter **ಹೊರಹೋಗು_adapter)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ i2c_adapter *adapter = i2c_adapter_ಕಂಡುಹಿಡಿ(nodeoff);

	ಆದರೆ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_ಸ್ಥಾನಾಂತರ(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_ಕಂಡುಹಿಡಿ(nodeoff);
		ಆದರೆ (!adapter)
			ಹಿಂತಿರುಗಿ SBI_ENOSYS;
	}

	ಆದರೆ (ಹೊರಹೋಗು_adapter)
		*ಹೊರಹೋಗು_adapter = adapter;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_i2c_adapter_get(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			ರಚನೆ i2c_adapter **ಹೊರಹೋಗು_adapter)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ i2c_adapter *adapter;

	ಆದರೆ (!fdt || (nodeoff < 0) || !ಹೊರಹೋಗು_adapter)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	rc = fdt_i2c_adapter_ಕಂಡುಹಿಡಿ(fdt, nodeoff, &adapter);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	*ಹೊರಹೋಗು_adapter = adapter;

	ಹಿಂತಿರುಗಿ 0;
}
