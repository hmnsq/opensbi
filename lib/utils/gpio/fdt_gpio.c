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

static 정수 fdt_gpio_init(const void *fdt, 정수 nodeoff)
{
	/* Check "gpio-controller" property */
	만약 (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		반환 SBI_EINVAL;

	반환 fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static 정수 fdt_gpio_chip_find(const void *fdt, 정수 nodeoff,
			      struct gpio_chip **out_chip)
{
	정수 rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	만약 (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		만약 (rc)
			반환 rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		만약 (!chip)
			반환 SBI_ENOSYS;
	}

	만약 (out_chip)
		*out_chip = chip;

	반환 0;
}

정수 fdt_gpio_pin_get(const void *fdt, 정수 nodeoff, 정수 index,
		     struct gpio_pin *out_pin)
{
	정수 rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_args pargs;

	만약 (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		반환 SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	만약 (rc)
		반환 rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	만약 (rc)
		반환 rc;

	drv = chip->driver;
	만약 (!drv || !drv->xlate)
		반환 SBI_ENOSYS;

	반환 drv->xlate(chip, &pargs, out_pin);
}

정수 fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_args *pargs,
			  struct gpio_pin *out_pin)
{
	만약 ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		반환 SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	반환 0;
}
