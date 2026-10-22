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
	सचे (!fdt_getprop(fdt, nodeoff, "gpio-controller", रिक्त))
		पटिनिवत्त SBI_EINVAL;

	पटिनिवत्त fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

स्थिर पूर्णाङ्क fdt_gpio_chip_find(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      संरचना gpio_chip **out_chip)
{
	पूर्णाङ्क rc;
	संरचना gpio_chip *chip = gpio_chip_find(nodeoff);

	सचे (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		सचे (rc)
			पटिनिवत्त rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		सचे (!chip)
			पटिनिवत्त SBI_ENOSYS;
	}

	सचे (out_chip)
		*out_chip = chip;

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_gpio_pin_get(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, पूर्णाङ्क index,
		     संरचना gpio_pin *out_pin)
{
	पूर्णाङ्क rc;
	ध्रुव संरचना fdt_gpio *drv;
	संरचना gpio_chip *chip = रिक्त;
	संरचना fdt_phandle_args pargs;

	सचे (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		पटिनिवत्त SBI_EINVAL;

	pargs.ग्रन्थिः_offset = pargs.args_गणनम् = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	सचे (rc)
		पटिनिवत्त rc;

	rc = fdt_gpio_chip_find(fdt, pargs.ग्रन्थिः_offset, &chip);
	सचे (rc)
		पटिनिवत्त rc;

	drv = chip->driver;
	सचे (!drv || !drv->xlate)
		पटिनिवत्त SBI_ENOSYS;

	पटिनिवत्त drv->xlate(chip, &pargs, out_pin);
}

पूर्णाङ्क fdt_gpio_simple_xlate(संरचना gpio_chip *chip,
			  ध्रुव संरचना fdt_phandle_args *pargs,
			  संरचना gpio_pin *out_pin)
{
	सचे ((pargs->args_गणनम् < 2) || (chip->ngpio <= pargs->args[0]))
		पटिनिवत्त SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	पटिनिवत्त 0;
}
