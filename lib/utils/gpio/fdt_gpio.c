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
	もし (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		戻る SBI_EINVAL;

	戻る fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static 整数 fdt_gpio_chip_find(const void *fdt, 整数 nodeoff,
			      struct gpio_chip **out_chip)
{
	整数 rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	もし (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		もし (rc)
			戻る rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		もし (!chip)
			戻る SBI_ENOSYS;
	}

	もし (out_chip)
		*out_chip = chip;

	戻る 0;
}

整数 fdt_gpio_pin_get(const void *fdt, 整数 nodeoff, 整数 index,
		     struct gpio_pin *out_pin)
{
	整数 rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_args pargs;

	もし (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		戻る SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	もし (rc)
		戻る rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	もし (rc)
		戻る rc;

	drv = chip->driver;
	もし (!drv || !drv->xlate)
		戻る SBI_ENOSYS;

	戻る drv->xlate(chip, &pargs, out_pin);
}

整数 fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_args *pargs,
			  struct gpio_pin *out_pin)
{
	もし ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		戻る SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	戻る 0;
}
