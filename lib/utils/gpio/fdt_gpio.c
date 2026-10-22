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
	གལ་སྲིད (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		ལོག SBI_EINVAL;

	ལོག fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static ཧྲིལ fdt_gpio_chip_find(const void *fdt, ཧྲིལ nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	ཧྲིལ rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	གལ་སྲིད (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		གལ་སྲིད (rc)
			ལོག rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		གལ་སྲིད (!chip)
			ལོག SBI_ENOSYS;
	}

	གལ་སྲིད (out_chip)
		*out_chip = chip;

	ལོག 0;
}

ཧྲིལ fdt_gpio_pin_get(const void *fdt, ཧྲིལ nodeoff, ཧྲིལ index,
		     sa_nracanaa gpio_pin *out_pin)
{
	ཧྲིལ rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	གལ་སྲིད (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ལོག SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	གལ་སྲིད (rc)
		ལོག rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	གལ་སྲིད (rc)
		ལོག rc;

	drv = chip->driver;
	གལ་སྲིད (!drv || !drv->xlate)
		ལོག SBI_ENOSYS;

	ལོག drv->xlate(chip, &pargs, out_pin);
}

ཧྲིལ fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	གལ་སྲིད ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ལོག SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ལོག 0;
}
