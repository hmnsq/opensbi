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
extern const struct fdt_driver *const fdt_gpio_drivers[];

static цео fdt_gpio_init(const void *fdt, цео nodeoff)
{
	/* Check "gpio-controller" property */
	ако (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		врати SBI_EINVAL;

	врати fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static цео fdt_gpio_chip_find(const void *fdt, цео nodeoff,
			      struct gpio_chip **out_chip)
{
	цео rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	ако (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ако (rc)
			врати rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ако (!chip)
			врати SBI_ENOSYS;
	}

	ако (out_chip)
		*out_chip = chip;

	врати 0;
}

цео fdt_gpio_pin_get(const void *fdt, цео nodeoff, цео index,
		     struct gpio_pin *out_pin)
{
	цео rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_args pargs;

	ако (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		врати SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ако (rc)
		врати rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	ако (rc)
		врати rc;

	drv = chip->driver;
	ако (!drv || !drv->xlate)
		врати SBI_ENOSYS;

	врати drv->xlate(chip, &pargs, out_pin);
}

цео fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_args *pargs,
			  struct gpio_pin *out_pin)
{
	ако ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		врати SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	врати 0;
}
