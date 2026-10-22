/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <libfdt.h>
#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
extern const sa_nracanaa fdt_driver *const fdt_gpio_drivers[];

static ཧྲིལ fdt_gpio_init(const void *fdt, ཧྲིལ nodeoff)
{
	/* Check "gpio-controller" property */
	གལ་ཏེ (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		ཕྱིར SBI_EINVAL;

	ཕྱིར fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static ཧྲིལ fdt_gpio_chip_find(const void *fdt, ཧྲིལ nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	ཧྲིལ rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	གལ་ཏེ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		གལ་ཏེ (rc)
			ཕྱིར rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		གལ་ཏེ (!chip)
			ཕྱིར SBI_ENOSYS;
	}

	གལ་ཏེ (out_chip)
		*out_chip = chip;

	ཕྱིར 0;
}

ཧྲིལ fdt_gpio_pin_get(const void *fdt, ཧྲིལ nodeoff, ཧྲིལ index,
		     sa_nracanaa gpio_pin *out_pin)
{
	ཧྲིལ rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	གལ་ཏེ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ཕྱིར SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	གལ་ཏེ (rc)
		ཕྱིར rc;

	drv = chip->driver;
	གལ་ཏེ (!drv || !drv->xlate)
		ཕྱིར SBI_ENOSYS;

	ཕྱིར drv->xlate(chip, &pargs, out_pin);
}

ཧྲིལ fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	གལ་ཏེ ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ཕྱིར SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ཕྱིར 0;
}
