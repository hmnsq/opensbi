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

static จำนวนเต็ม fdt_gpio_init(const void *fdt, จำนวนเต็ม nodeoff)
{
	/* Check "gpio-controller" property */
	ถ้า (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		คืน SBI_EINVAL;

	คืน fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static จำนวนเต็ม fdt_gpio_chip_find(const void *fdt, จำนวนเต็ม nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	จำนวนเต็ม rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	ถ้า (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ถ้า (rc)
			คืน rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ถ้า (!chip)
			คืน SBI_ENOSYS;
	}

	ถ้า (out_chip)
		*out_chip = chip;

	คืน 0;
}

จำนวนเต็ม fdt_gpio_pin_get(const void *fdt, จำนวนเต็ม nodeoff, จำนวนเต็ม index,
		     sa_nracanaa gpio_pin *out_pin)
{
	จำนวนเต็ม rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	ถ้า (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		คืน SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ถ้า (rc)
		คืน rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	ถ้า (rc)
		คืน rc;

	drv = chip->driver;
	ถ้า (!drv || !drv->xlate)
		คืน SBI_ENOSYS;

	คืน drv->xlate(chip, &pargs, out_pin);
}

จำนวนเต็ม fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	ถ้า ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		คืน SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	คืน 0;
}
