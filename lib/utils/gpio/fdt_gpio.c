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

static 整数 fdt_gpio_init(const void *fdt, 整数 nodeoff)
{
	/* Check "gpio-controller" property */
	如果 (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		返回 SBI_EINVAL;

	返回 fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static 整数 fdt_gpio_chip_find(const void *fdt, 整数 nodeoff,
			      struct gpio_chip **out_chip)
{
	整数 rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	如果 (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		如果 (rc)
			返回 rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		如果 (!chip)
			返回 SBI_ENOSYS;
	}

	如果 (out_chip)
		*out_chip = chip;

	返回 0;
}

整数 fdt_gpio_pin_get(const void *fdt, 整数 nodeoff, 整数 index,
		     struct gpio_pin *out_pin)
{
	整数 rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_args pargs;

	如果 (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		返回 SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	如果 (rc)
		返回 rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	如果 (rc)
		返回 rc;

	drv = chip->driver;
	如果 (!drv || !drv->xlate)
		返回 SBI_ENOSYS;

	返回 drv->xlate(chip, &pargs, out_pin);
}

整数 fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_args *pargs,
			  struct gpio_pin *out_pin)
{
	如果 ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		返回 SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	返回 0;
}
