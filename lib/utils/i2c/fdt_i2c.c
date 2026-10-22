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

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/i2c/fdt_i2c.h>

/* List of FDT i2c adapter drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_i2c_adapter_drivers[];

స్థిర పూర్ణాంకం fdt_i2c_adapter_కనుగొను(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				నిర్మాణం i2c_adapter **బహిర్గతం_adapter)
{
	పూర్ణాంకం rc;
	నిర్మాణం i2c_adapter *adapter = i2c_adapter_కనుగొను(nodeoff);

	ఒకవేళ (!adapter) {
		/* I2C adapter not found so initialize matching driver */
		rc = fdt_driver_init_by_స్థానభ్రంశం(fdt, nodeoff,
					       fdt_i2c_adapter_drivers);
		ఒకవేళ (rc)
			తిరిగి rc;

		/* Try to find I2C adapter again */
		adapter = i2c_adapter_కనుగొను(nodeoff);
		ఒకవేళ (!adapter)
			తిరిగి SBI_ENOSYS;
	}

	ఒకవేళ (బహిర్గతం_adapter)
		*బహిర్గతం_adapter = adapter;

	తిరిగి 0;
}

పూర్ణాంకం fdt_i2c_adapter_get(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			నిర్మాణం i2c_adapter **బహిర్గతం_adapter)
{
	పూర్ణాంకం rc;
	నిర్మాణం i2c_adapter *adapter;

	ఒకవేళ (!fdt || (nodeoff < 0) || !బహిర్గతం_adapter)
		తిరిగి SBI_EINVAL;

	rc = fdt_i2c_adapter_కనుగొను(fdt, nodeoff, &adapter);
	ఒకవేళ (rc)
		తిరిగి rc;

	*బహిర్గతం_adapter = adapter;

	తిరిగి 0;
}
