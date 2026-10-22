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
	ꦪꦼꦤ꧀ (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		ꦮꦭꦶ SBI_EINVAL;

	ꦮꦭꦶ fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static puur_naa_nka fdt_gpio_chip_find(const void *fdt, puur_naa_nka nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	puur_naa_nka rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	ꦪꦼꦤ꧀ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ꦪꦼꦤ꧀ (rc)
			ꦮꦭꦶ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ꦪꦼꦤ꧀ (!chip)
			ꦮꦭꦶ SBI_ENOSYS;
	}

	ꦪꦼꦤ꧀ (out_chip)
		*out_chip = chip;

	ꦮꦭꦶ 0;
}

puur_naa_nka fdt_gpio_pin_get(const void *fdt, puur_naa_nka nodeoff, puur_naa_nka index,
		     sa_nracanaa gpio_pin *out_pin)
{
	puur_naa_nka rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	ꦪꦼꦤ꧀ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ꦮꦭꦶ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ꦪꦼꦤ꧀ (rc)
		ꦮꦭꦶ rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	ꦪꦼꦤ꧀ (rc)
		ꦮꦭꦶ rc;

	drv = chip->driver;
	ꦪꦼꦤ꧀ (!drv || !drv->xlate)
		ꦮꦭꦶ SBI_ENOSYS;

	ꦮꦭꦶ drv->xlate(chip, &pargs, out_pin);
}

puur_naa_nka fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	ꦪꦼꦤ꧀ ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ꦮꦭꦶ SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ꦮꦭꦶ 0;
}
