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

static ціле fdt_gpio_init(const void *fdt, ціле nodeoff)
{
	/* Check "gpio-controller" property */
	якщо (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		повернути SBI_EINVAL;

	повернути fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static ціле fdt_gpio_chip_find(const void *fdt, ціле nodeoff,
			      struct gpio_chip **out_chip)
{
	ціле rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	якщо (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		якщо (rc)
			повернути rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		якщо (!chip)
			повернути SBI_ENOSYS;
	}

	якщо (out_chip)
		*out_chip = chip;

	повернути 0;
}

ціле fdt_gpio_pin_get(const void *fdt, ціле nodeoff, ціле index,
		     struct gpio_pin *out_pin)
{
	ціле rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_args pargs;

	якщо (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		повернути SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	якщо (rc)
		повернути rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	якщо (rc)
		повернути rc;

	drv = chip->driver;
	якщо (!drv || !drv->xlate)
		повернути SBI_ENOSYS;

	повернути drv->xlate(chip, &pargs, out_pin);
}

ціле fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_args *pargs,
			  struct gpio_pin *out_pin)
{
	якщо ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		повернути SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	повернути 0;
}
