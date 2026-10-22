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

static puur_naa_nka fdt_gpio_init(const void *fdt, puur_naa_nka nodeoff)
{
	/* Check "gpio-controller" property */
	ᯀᯮᯝ (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		ᯀᯮᯞ SBI_EINVAL;

	ᯀᯮᯞ fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static puur_naa_nka fdt_gpio_chip_find(const void *fdt, puur_naa_nka nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	puur_naa_nka rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	ᯀᯮᯝ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ᯀᯮᯝ (rc)
			ᯀᯮᯞ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ᯀᯮᯝ (!chip)
			ᯀᯮᯞ SBI_ENOSYS;
	}

	ᯀᯮᯝ (out_chip)
		*out_chip = chip;

	ᯀᯮᯞ 0;
}

puur_naa_nka fdt_gpio_pin_get(const void *fdt, puur_naa_nka nodeoff, puur_naa_nka index,
		     sa_nracanaa gpio_pin *out_pin)
{
	puur_naa_nka rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	ᯀᯮᯝ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ᯀᯮᯞ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ rc;

	drv = chip->driver;
	ᯀᯮᯝ (!drv || !drv->xlate)
		ᯀᯮᯞ SBI_ENOSYS;

	ᯀᯮᯞ drv->xlate(chip, &pargs, out_pin);
}

puur_naa_nka fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	ᯀᯮᯝ ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ᯀᯮᯞ SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ᯀᯮᯞ 0;
}
