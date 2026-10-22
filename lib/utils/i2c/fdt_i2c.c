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

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_i2c_adapter_drivers[];

ਸਥਿਰ ਪੂਰਨ fdt_i2c_adapter_ਲੱਭੋ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				ਬਣਤਰ i2c_adapter **ਬਾਹਰਲਾ_adapter)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ i2c_adapter *adapter = i2c_adapter_ਲੱਭੋ(nodeoff);

	ਜੇ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_ਵਿੱਥ(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_ਲੱਭੋ(nodeoff);
		ਜੇ (!adapter)
			ਵਾਪਸ SBI_ENOSYS;
	}

	ਜੇ (ਬਾਹਰਲਾ_adapter)
		*ਬਾਹਰਲਾ_adapter = adapter;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_i2c_adapter_get(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			ਬਣਤਰ i2c_adapter **ਬਾਹਰਲਾ_adapter)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ i2c_adapter *adapter;

	ਜੇ (!fdt || (nodeoff < 0) || !ਬਾਹਰਲਾ_adapter)
		ਵਾਪਸ SBI_EINVAL;

	rc = fdt_i2c_adapter_ਲੱਭੋ(fdt, nodeoff, &adapter);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	*ਬਾਹਰਲਾ_adapter = adapter;

	ਵਾਪਸ 0;
}
