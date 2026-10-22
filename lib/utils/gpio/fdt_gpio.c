/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
बाह्य ध्रुव संरचना fdt_driver *ध्रुव fdt_gpio_drivers[];

स्थिर पूर्णाङ्क fdt_gpio_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff)
{
	/* Check "gpio-controller" property */
	यदि (!fdt_getprop(fdt, nodeoff, "gpio-controller", रिक्त))
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम fdt_driver_init_by_विस्थापनम्(fdt, nodeoff, fdt_gpio_drivers);
}

स्थिर पूर्णाङ्क fdt_gpio_chip_find(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      संरचना gpio_chip **बहिर्गमनम्_chip)
{
	पूर्णाङ्क rc;
	संरचना gpio_chip *chip = gpio_chip_find(nodeoff);

	यदि (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		यदि (rc)
			प्रत्यागम rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		यदि (!chip)
			प्रत्यागम SBI_ENOSYS;
	}

	यदि (बहिर्गमनम्_chip)
		*बहिर्गमनम्_chip = chip;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_gpio_pin_get(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, पूर्णाङ्क सूचकांक_2,
		     संरचना gpio_pin *बहिर्गमनम्_pin)
{
	पूर्णाङ्क rc;
	ध्रुव संरचना fdt_gpio *drv;
	संरचना gpio_chip *chip = रिक्त;
	संरचना fdt_phandle_तर्काः pargs;

	यदि (!fdt || (nodeoff < 0) || (सूचकांक_2 < 0) || !बहिर्गमनम्_pin)
		प्रत्यागम SBI_EINVAL;

	pargs.ग्रन्थिः_विस्थापनम् = pargs.तर्काः_गणनम् = 0;
	rc = fdt_parse_phandle_with_तर्काः(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 सूचकांक_2, &pargs);
	यदि (rc)
		प्रत्यागम rc;

	rc = fdt_gpio_chip_find(fdt, pargs.ग्रन्थिः_विस्थापनम्, &chip);
	यदि (rc)
		प्रत्यागम rc;

	drv = chip->driver;
	यदि (!drv || !drv->xlate)
		प्रत्यागम SBI_ENOSYS;

	प्रत्यागम drv->xlate(chip, &pargs, बहिर्गमनम्_pin);
}

पूर्णाङ्क fdt_gpio_simple_xlate(संरचना gpio_chip *chip,
			  ध्रुव संरचना fdt_phandle_तर्काः *pargs,
			  संरचना gpio_pin *बहिर्गमनम्_pin)
{
	यदि ((pargs->तर्काः_गणनम् < 2) || (chip->ngpio <= pargs->तर्काः[0]))
		प्रत्यागम SBI_EINVAL;

	बहिर्गमनम्_pin->chip = chip;
	बहिर्गमनम्_pin->विस्थापनम् = pargs->तर्काः[0];
	बहिर्गमनम्_pin->flags = pargs->तर्काः[1];
	प्रत्यागम 0;
}
