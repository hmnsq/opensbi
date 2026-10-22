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

static ကိန်းပြည့် fdt_gpio_init(const void *fdt, ကိန်းပြည့် nodeoff)
{
	/* Check "gpio-controller" property */
	အကယ်၍ (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		ပြန်ပေး SBI_EINVAL;

	ပြန်ပေး fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static ကိန်းပြည့် fdt_gpio_chip_find(const void *fdt, ကိန်းပြည့် nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	ကိန်းပြည့် rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	အကယ်၍ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		အကယ်၍ (!chip)
			ပြန်ပေး SBI_ENOSYS;
	}

	အကယ်၍ (out_chip)
		*out_chip = chip;

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_gpio_pin_get(const void *fdt, ကိန်းပြည့် nodeoff, ကိန်းပြည့် index,
		     sa_nracanaa gpio_pin *out_pin)
{
	ကိန်းပြည့် rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	အကယ်၍ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ပြန်ပေး SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	drv = chip->driver;
	အကယ်၍ (!drv || !drv->xlate)
		ပြန်ပေး SBI_ENOSYS;

	ပြန်ပေး drv->xlate(chip, &pargs, out_pin);
}

ကိန်းပြည့် fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	အကယ်၍ ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ပြန်ပေး SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ပြန်ပေး 0;
}
