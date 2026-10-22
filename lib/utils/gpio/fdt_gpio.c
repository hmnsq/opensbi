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

static số_nguyên fdt_gpio_init(const void *fdt, số_nguyên nodeoff)
{
	/* Check "gpio-controller" property */
	nếu (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		trả_về SBI_EINVAL;

	trả_về fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static số_nguyên fdt_gpio_chip_find(const void *fdt, số_nguyên nodeoff,
			      struct gpio_chip **out_chip)
{
	số_nguyên rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	nếu (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		nếu (rc)
			trả_về rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		nếu (!chip)
			trả_về SBI_ENOSYS;
	}

	nếu (out_chip)
		*out_chip = chip;

	trả_về 0;
}

số_nguyên fdt_gpio_pin_get(const void *fdt, số_nguyên nodeoff, số_nguyên index,
		     struct gpio_pin *out_pin)
{
	số_nguyên rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_args pargs;

	nếu (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		trả_về SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	nếu (rc)
		trả_về rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	nếu (rc)
		trả_về rc;

	drv = chip->driver;
	nếu (!drv || !drv->xlate)
		trả_về SBI_ENOSYS;

	trả_về drv->xlate(chip, &pargs, out_pin);
}

số_nguyên fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_args *pargs,
			  struct gpio_pin *out_pin)
{
	nếu ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		trả_về SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	trả_về 0;
}
