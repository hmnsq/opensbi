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

static පූර්ණ_සංඛ්‍යා fdt_gpio_init(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff)
{
	/* Check "gpio-controller" property */
	නම් (!fdt_getprop(fdt, nodeoff, "gpio-controller", NULL))
		ආපසු SBI_EINVAL;

	ආපසු fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

static පූර්ණ_සංඛ්‍යා fdt_gpio_chip_find(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
			      sa_nracanaa gpio_chip **out_chip)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa gpio_chip *chip = gpio_chip_find(nodeoff);

	නම් (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		නම් (rc)
			ආපසු rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		නම් (!chip)
			ආපසු SBI_ENOSYS;
	}

	නම් (out_chip)
		*out_chip = chip;

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_gpio_pin_get(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff, පූර්ණ_සංඛ්‍යා index,
		     sa_nracanaa gpio_pin *out_pin)
{
	පූර්ණ_සංඛ්‍යා rc;
	const sa_nracanaa fdt_gpio *drv;
	sa_nracanaa gpio_chip *chip = NULL;
	sa_nracanaa fdt_phandle_args pargs;

	නම් (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ආපසු SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	නම් (rc)
		ආපසු rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	නම් (rc)
		ආපසු rc;

	drv = chip->driver;
	නම් (!drv || !drv->xlate)
		ආපසු SBI_ENOSYS;

	ආපසු drv->xlate(chip, &pargs, out_pin);
}

පූර්ණ_සංඛ්‍යා fdt_gpio_simple_xlate(sa_nracanaa gpio_chip *chip,
			  const sa_nracanaa fdt_phandle_args *pargs,
			  sa_nracanaa gpio_pin *out_pin)
{
	නම් ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ආපසු SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ආපසු 0;
}
