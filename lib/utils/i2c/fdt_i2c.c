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

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_i2c_adapter_drivers[];

நிலையான முழு fdt_i2c_adapter_find(மாறிலி வெறுமை *fdt, முழு nodeoff,
				கட்டமைப்பு i2c_adapter **out_adapter)
{
	முழு rc;
	கட்டமைப்பு i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	எனில் (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		எனில் (rc)
			திரும்பு rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		எனில் (!adapter)
			திரும்பு SBI_ENOSYS;
	}

	எனில் (out_adapter)
		*out_adapter = adapter;

	திரும்பு 0;
}

முழு fdt_i2c_adapter_get(மாறிலி வெறுமை *fdt, முழு nodeoff,
			கட்டமைப்பு i2c_adapter **out_adapter)
{
	முழு rc;
	கட்டமைப்பு i2c_adapter *adapter;

	எனில் (!fdt || (nodeoff < 0) || !out_adapter)
		திரும்பு SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	எனில் (rc)
		திரும்பு rc;

	*out_adapter = adapter;

	திரும்பு 0;
}
