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

static int fdt_gpio_init(const void *fdt, int nodeoff)
{
	/* Check "gpio-controller" property */
	if (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		return SBI_EINVAL;

	return fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static int fdt_gpio_chip_find(const void *fdt, int nodeoff,
			      struct gpio_chip **out_chip)
{
	int rc;
	struct gpio_chip *chip = gpio_chip_find(nodeoff);

	if (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		if (rc)
			return rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		if (!chip)
			return SBI_ENOSYS;
	}

	if (out_chip)
		*out_chip = chip;

	return 0;
}

int fdt_gpio_pin_get(const void *fdt, int nodeoff, int index,
		     struct gpio_pin *out_pin)
{
	int rc;
	const struct fdt_gpio *drv;
	struct gpio_chip *chip = NULL;
	struct fdt_phandle_मगा_कातविरान pargs;

	if (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		return SBI_EINVAL;

	pargs.बुहोल_offset = pargs.मगा_कातविरान_बिलांग = 0;
	rc = fdt_parse_phandle_with_मगा_कातविरान(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	if (rc)
		return rc;

	rc = fdt_gpio_chip_find(fdt, pargs.बुहोल_offset, &chip);
	if (rc)
		return rc;

	drv = chip->driver;
	if (!drv || !drv->xlate)
		return SBI_ENOSYS;

	return drv->xlate(chip, &pargs, out_pin);
}

int fdt_gpio_simple_xlate(struct gpio_chip *chip,
			  const struct fdt_phandle_मगा_कातविरान *pargs,
			  struct gpio_pin *out_pin)
{
	if ((pargs->मगा_कातविरान_बिलांग < 2) || (chip->ngpio <= pargs->मगा_कातविरान[0]))
		return SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->मगा_कातविरान[0];
	out_pin->flags = pargs->मगा_कातविरान[1];
	return 0;
}
