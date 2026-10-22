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
बाह्य स्थिरांक संरचना fdt_driver *स्थिरांक fdt_gpio_drivers[];

स्थिर पूर्णांक fdt_gpio_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff)
{
	/* Check "gpio-controller" property */
	जर (!fdt_getprop(fdt, nodeoff, "gpio-controller", रिक्त))
		परत SBI_EINVAL;

	परत fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

स्थिर पूर्णांक fdt_gpio_chip_find(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			      संरचना gpio_chip **out_chip)
{
	पूर्णांक rc;
	संरचना gpio_chip *chip = gpio_chip_find(nodeoff);

	जर (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		जर (rc)
			परत rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		जर (!chip)
			परत SBI_ENOSYS;
	}

	जर (out_chip)
		*out_chip = chip;

	परत 0;
}

पूर्णांक fdt_gpio_pin_get(स्थिरांक शून्य *fdt, पूर्णांक nodeoff, पूर्णांक index,
		     संरचना gpio_pin *out_pin)
{
	पूर्णांक rc;
	स्थिरांक संरचना fdt_gpio *drv;
	संरचना gpio_chip *chip = रिक्त;
	संरचना fdt_phandle_args pargs;

	जर (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		परत SBI_EINVAL;

	pargs.गाठ_offset = pargs.args_मोजणी = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	जर (rc)
		परत rc;

	rc = fdt_gpio_chip_find(fdt, pargs.गाठ_offset, &chip);
	जर (rc)
		परत rc;

	drv = chip->driver;
	जर (!drv || !drv->xlate)
		परत SBI_ENOSYS;

	परत drv->xlate(chip, &pargs, out_pin);
}

पूर्णांक fdt_gpio_simple_xlate(संरचना gpio_chip *chip,
			  स्थिरांक संरचना fdt_phandle_args *pargs,
			  संरचना gpio_pin *out_pin)
{
	जर ((pargs->args_मोजणी < 2) || (chip->ngpio <= pargs->args[0]))
		परत SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	परत 0;
}
