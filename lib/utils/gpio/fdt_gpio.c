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

static целое fdt_gpio_init(const void *fdt, целое nodeoff)
{
	/* Check "gpio-controller" property */
	если (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		вернуть SBI_EINVAL;

	вернуть fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static целое fdt_gpio_chip_find(const void *fdt, целое nodeoff,
			      struct gpio_chip **out_chip)
{
	целое rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	если (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		если (rc)
			вернуть rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		если (!chip)
			вернуть SBI_ENOSYS;
	}

	если (out_chip)
		*out_chip = chip;

	вернуть 0;
}

целое fdt_gpio_pin_get(const void *fdt, целое nodeoff, целое index,
		     struct gpio_pin *out_pin)
{
	целое rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_args pargs;

	если (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		вернуть SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	если (rc)
		вернуть rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	если (rc)
		вернуть rc;

	drv = chip->driver;
	если (!drv || !drv->xlate)
		вернуть SBI_ENOSYS;

	вернуть drv->xlate(chip, &pargs, out_pin);
}

целое fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_args *pargs,
			  struct gpio_pin *out_pin)
{
	если ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		вернуть SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	вернуть 0;
}
