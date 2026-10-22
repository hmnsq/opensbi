/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
బాహ్య స్థిరరాశి నిర్మాణం fdt_driver *స్థిరరాశి fdt_gpio_drivers[];

స్థిర పూర్ణాంకం fdt_gpio_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff)
{
	/* Check "gpio-controller" property */
	ఒకవేళ (!fdt_getprop(fdt, nodeoff, "gpio-controller", ఖాళీ))
		తిరిగి SBI_EINVAL;

	తిరిగి fdt_driver_init_by_స్థానభ్రంశం(fdt, nodeoff, fdt_gpio_drivers);
}

స్థిర పూర్ణాంకం fdt_gpio_chip_కనుగొను(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			      నిర్మాణం gpio_chip **బహిర్గతం_chip)
{
	పూర్ణాంకం rc;
	నిర్మాణం gpio_chip *chip = gpio_chip_కనుగొను(nodeoff);

	ఒకవేళ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ఒకవేళ (rc)
			తిరిగి rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_కనుగొను(nodeoff);
		ఒకవేళ (!chip)
			తిరిగి SBI_ENOSYS;
	}

	ఒకవేళ (బహిర్గతం_chip)
		*బహిర్గతం_chip = chip;

	తిరిగి 0;
}

పూర్ణాంకం fdt_gpio_pin_get(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, పూర్ణాంకం సూచీ_2,
		     నిర్మాణం gpio_pin *బహిర్గతం_pin)
{
	పూర్ణాంకం rc;
	స్థిరరాశి నిర్మాణం fdt_gpio *drv;
	నిర్మాణం gpio_chip *chip = ఖాళీ;
	నిర్మాణం fdt_phandle_వాదనలు pargs;

	ఒకవేళ (!fdt || (nodeoff < 0) || (సూచీ_2 < 0) || !బహిర్గతం_pin)
		తిరిగి SBI_EINVAL;

	pargs.కణుపు_స్థానభ్రంశం = pargs.వాదనలు_లెక్క = 0;
	rc = fdt_parse_phandle_with_వాదనలు(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 సూచీ_2, &pargs);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = fdt_gpio_chip_కనుగొను(fdt, pargs.కణుపు_స్థానభ్రంశం, &chip);
	ఒకవేళ (rc)
		తిరిగి rc;

	drv = chip->driver;
	ఒకవేళ (!drv || !drv->xlate)
		తిరిగి SBI_ENOSYS;

	తిరిగి drv->xlate(chip, &pargs, బహిర్గతం_pin);
}

పూర్ణాంకం fdt_gpio_simple_xlate(నిర్మాణం gpio_chip *chip,
			  స్థిరరాశి నిర్మాణం fdt_phandle_వాదనలు *pargs,
			  నిర్మాణం gpio_pin *బహిర్గతం_pin)
{
	ఒకవేళ ((pargs->వాదనలు_లెక్క < 2) || (chip->ngpio <= pargs->వాదనలు[0]))
		తిరిగి SBI_EINVAL;

	బహిర్గతం_pin->chip = chip;
	బహిర్గతం_pin->స్థానభ్రంశం = pargs->వాదనలు[0];
	బహిర్గతం_pin->flags = pargs->వాదనలు[1];
	తిరిగి 0;
}
