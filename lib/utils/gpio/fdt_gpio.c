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

static цяло fdt_gpio_init(const void *fdt, цяло nodeoff)
{
	/* Check "gpio-controller" property */
	ако (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		върни SBI_EINVAL;

	върни fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static цяло fdt_gpio_chip_find(const void *fdt, цяло nodeoff,
			      struct gpio_chip **out_chip)
{
	цяло rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	ако (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ако (rc)
			върни rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ако (!chip)
			върни SBI_ENOSYS;
	}

	ако (out_chip)
		*out_chip = chip;

	върни 0;
}

цяло fdt_gpio_pin_get(const void *fdt, цяло nodeoff, цяло index,
		     struct gpio_pin *out_pin)
{
	цяло rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_args pargs;

	ако (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		върни SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ако (rc)
		върни rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	ако (rc)
		върни rc;

	drv = chip->driver;
	ако (!drv || !drv->xlate)
		върни SBI_ENOSYS;

	върни drv->xlate(chip, &pargs, out_pin);
}

цяло fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_args *pargs,
			  struct gpio_pin *out_pin)
{
	ако ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		върни SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	върни 0;
}
