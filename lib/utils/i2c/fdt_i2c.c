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

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_i2c_adapter_drivers[];

સ્થિર પૂર્ણાંક fdt_i2c_adapter_find(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				સંરચના i2c_adapter **out_adapter)
{
	પૂર્ણાંક rc;
	સંરચના i2c_adapter *adapter = i2c_adapter_find(nodeoff);

	જો (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_offset(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		જો (rc)
			પાછા rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_find(nodeoff);
		જો (!adapter)
			પાછા SBI_ENOSYS;
	}

	જો (out_adapter)
		*out_adapter = adapter;

	પાછા 0;
}

પૂર્ણાંક fdt_i2c_adapter_get(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			સંરચના i2c_adapter **out_adapter)
{
	પૂર્ણાંક rc;
	સંરચના i2c_adapter *adapter;

	જો (!fdt || (nodeoff < 0) || !out_adapter)
		પાછા SBI_EINVAL;

	rc = fdt_i2c_adapter_find(fdt, nodeoff, &adapter);
	જો (rc)
		પાછા rc;

	*out_adapter = adapter;

	પાછા 0;
}
