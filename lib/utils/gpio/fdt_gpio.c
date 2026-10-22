/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
ബാഹ്യ സ്ഥിരം ഘടന fdt_driver *സ്ഥിരം fdt_gpio_drivers[];

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_gpio_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff)
{
	/* Check "gpio-controller" property */
	എങ്കിൽ (!fdt_getprop(fdt, nodeoff, "gpio-controller", ശൂന്യ))
		തിരികെ SBI_EINVAL;

	തിരികെ fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_gpio_chip_find(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			      ഘടന gpio_chip **out_chip)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന gpio_chip *chip = gpio_chip_find(nodeoff);

	എങ്കിൽ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		എങ്കിൽ (rc)
			തിരികെ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		എങ്കിൽ (!chip)
			തിരികെ SBI_ENOSYS;
	}

	എങ്കിൽ (out_chip)
		*out_chip = chip;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_gpio_pin_get(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, പൂർണ്ണസംഖ്യ index,
		     ഘടന gpio_pin *out_pin)
{
	പൂർണ്ണസംഖ്യ rc;
	സ്ഥിരം ഘടന fdt_gpio *drv;
	ഘടന gpio_chip *chip = ശൂന്യ;
	ഘടന fdt_phandle_args pargs;

	എങ്കിൽ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		തിരികെ SBI_EINVAL;

	pargs.കണ്ണി_offset = pargs.args_എണ്ണം = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = fdt_gpio_chip_find(fdt, pargs.കണ്ണി_offset, &chip);
	എങ്കിൽ (rc)
		തിരികെ rc;

	drv = chip->driver;
	എങ്കിൽ (!drv || !drv->xlate)
		തിരികെ SBI_ENOSYS;

	തിരികെ drv->xlate(chip, &pargs, out_pin);
}

പൂർണ്ണസംഖ്യ fdt_gpio_simple_xlate(ഘടന gpio_chip *chip,
			  സ്ഥിരം ഘടന fdt_phandle_args *pargs,
			  ഘടന gpio_pin *out_pin)
{
	എങ്കിൽ ((pargs->args_എണ്ണം < 2) || (chip->ngpio <= pargs->args[0]))
		തിരികെ SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	തിരികെ 0;
}
