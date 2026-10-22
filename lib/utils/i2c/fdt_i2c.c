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

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_i2c_adapter_drivers[];

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_i2c_adapter_find(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				ഘടന i2c_adapter **out_adapter)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	എങ്കിൽ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		എങ്കിൽ (rc)
			തിരികെ rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		എങ്കിൽ (!adapter)
			തിരികെ SBI_ENOSYS;
	}

	എങ്കിൽ (out_adapter)
		*out_adapter = adapter;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_i2c_adapter_get(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			ഘടന i2c_adapter **out_adapter)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന i2c_adapter *adapter;

	എങ്കിൽ (!fdt || (nodeoff < 0) || !out_adapter)
		തിരികെ SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	എങ്കിൽ (rc)
		തിരികെ rc;

	*out_adapter = adapter;

	തിരികെ 0;
}
