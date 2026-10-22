/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
বাহ্য ধ্রুৱ সংরচনা fdt_driver *ধ্রুৱ fdt_gpio_drivers[];

স্থির পূর্ণাঙ্ক fdt_gpio_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff)
{
	/* Check "gpio-controller" property */
	যদি (!fdt_getprop(fdt, nodeoff, "gpio-controller", রিক্ত))
		ঘূৰাই SBI_EINVAL;

	ঘূৰাই fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

স্থির পূর্ণাঙ্ক fdt_gpio_chip_find(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      সংরচনা gpio_chip **out_chip)
{
	পূর্ণাঙ্ক rc;
	সংরচনা gpio_chip *chip = gpio_chip_find(nodeoff);

	যদি (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		যদি (rc)
			ঘূৰাই rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		যদি (!chip)
			ঘূৰাই SBI_ENOSYS;
	}

	যদি (out_chip)
		*out_chip = chip;

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_gpio_pin_get(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff, পূর্ণাঙ্ক index,
		     সংরচনা gpio_pin *out_pin)
{
	পূর্ণাঙ্ক rc;
	ধ্রুৱ সংরচনা fdt_gpio *drv;
	সংরচনা gpio_chip *chip = রিক্ত;
	সংরচনা fdt_phandle_args pargs;

	যদি (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ঘূৰাই SBI_EINVAL;

	pargs.গ্ৰন্থি_offset = pargs.args_গুনতি = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	যদি (rc)
		ঘূৰাই rc;

	rc = fdt_gpio_chip_find(fdt, pargs.গ্ৰন্থি_offset, &chip);
	যদি (rc)
		ঘূৰাই rc;

	drv = chip->driver;
	যদি (!drv || !drv->xlate)
		ঘূৰাই SBI_ENOSYS;

	ঘূৰাই drv->xlate(chip, &pargs, out_pin);
}

পূর্ণাঙ্ক fdt_gpio_simple_xlate(সংরচনা gpio_chip *chip,
			  ধ্রুৱ সংরচনা fdt_phandle_args *pargs,
			  সংরচনা gpio_pin *out_pin)
{
	যদি ((pargs->args_গুনতি < 2) || (chip->ngpio <= pargs->args[0]))
		ঘূৰাই SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ঘূৰাই 0;
}
