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

static ចំនួនគត់ fdt_gpio_init(const void *fdt, ចំនួនគត់ nodeoff)
{
	/* Check "gpio-controller" property */
	ប្រសិនបើ (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		ត្រឡប់ SBI_EINVAL;

	ត្រឡប់ fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static ចំនួនគត់ fdt_gpio_chip_find(const void *fdt, ចំនួនគត់ nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	ចំនួនគត់ rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	ប្រសិនបើ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ប្រសិនបើ (!chip)
			ត្រឡប់ SBI_ENOSYS;
	}

	ប្រសិនបើ (out_chip)
		*out_chip = chip;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_gpio_pin_get(const void *fdt, ចំនួនគត់ nodeoff, ចំនួនគត់ index,
		     sa_nracanaa gpio_pin *out_pin)
{
	ចំនួនគត់ rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	ប្រសិនបើ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ត្រឡប់ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	drv = chip->driver;
	ប្រសិនបើ (!drv || !drv->xlate)
		ត្រឡប់ SBI_ENOSYS;

	ត្រឡប់ drv->xlate(chip, &pargs, out_pin);
}

ចំនួនគត់ fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	ប្រសិនបើ ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ត្រឡប់ SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ត្រឡប់ 0;
}
