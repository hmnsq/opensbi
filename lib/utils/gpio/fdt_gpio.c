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

static ຈຳນວນເຕັມ fdt_gpio_init(const void *fdt, ຈຳນວນເຕັມ nodeoff)
{
	/* Check "gpio-controller" property */
	ຖ້າ (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		ສົ່ງຄືນ SBI_EINVAL;

	ສົ່ງຄືນ fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static ຈຳນວນເຕັມ fdt_gpio_chip_find(const void *fdt, ຈຳນວນເຕັມ nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	ຈຳນວນເຕັມ rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	ຖ້າ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ຖ້າ (!chip)
			ສົ່ງຄືນ SBI_ENOSYS;
	}

	ຖ້າ (out_chip)
		*out_chip = chip;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_gpio_pin_get(const void *fdt, ຈຳນວນເຕັມ nodeoff, ຈຳນວນເຕັມ index,
		     sa_nracanaa gpio_pin *out_pin)
{
	ຈຳນວນເຕັມ rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	ຖ້າ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ສົ່ງຄືນ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	drv = chip->driver;
	ຖ້າ (!drv || !drv->xlate)
		ສົ່ງຄືນ SBI_ENOSYS;

	ສົ່ງຄືນ drv->xlate(chip, &pargs, out_pin);
}

ຈຳນວນເຕັມ fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	ຖ້າ ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ສົ່ງຄືນ SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ສົ່ງຄືນ 0;
}
