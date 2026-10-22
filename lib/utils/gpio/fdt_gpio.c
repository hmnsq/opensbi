/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
বাহ্যিক ধ্রুবক কাঠামো fdt_driver *ধ্রুবক fdt_gpio_drivers[];

স্থির পূর্ণসংখ্যা fdt_gpio_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff)
{
	/* Check "gpio-controller" property */
	যদি (!fdt_getprop(fdt, nodeoff, "gpio-controller", ফাঁকা))
		ফেরত SBI_EINVAL;

	ফেরত fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

স্থির পূর্ণসংখ্যা fdt_gpio_chip_find(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			      কাঠামো gpio_chip **out_chip)
{
	পূর্ণসংখ্যা rc;
	কাঠামো gpio_chip *chip = gpio_chip_find(nodeoff);

	যদি (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		যদি (rc)
			ফেরত rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		যদি (!chip)
			ফেরত SBI_ENOSYS;
	}

	যদি (out_chip)
		*out_chip = chip;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_gpio_pin_get(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, পূর্ণসংখ্যা সূচক_2,
		     কাঠামো gpio_pin *out_pin)
{
	পূর্ণসংখ্যা rc;
	ধ্রুবক কাঠামো fdt_gpio *drv;
	কাঠামো gpio_chip *chip = ফাঁকা;
	কাঠামো fdt_phandle_args pargs;

	যদি (!fdt || (nodeoff < 0) || (সূচক_2 < 0) || !out_pin)
		ফেরত SBI_EINVAL;

	pargs.গ্রন্থি_offset = pargs.args_গুনতি = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 সূচক_2, &pargs);
	যদি (rc)
		ফেরত rc;

	rc = fdt_gpio_chip_find(fdt, pargs.গ্রন্থি_offset, &chip);
	যদি (rc)
		ফেরত rc;

	drv = chip->driver;
	যদি (!drv || !drv->xlate)
		ফেরত SBI_ENOSYS;

	ফেরত drv->xlate(chip, &pargs, out_pin);
}

পূর্ণসংখ্যা fdt_gpio_simple_xlate(কাঠামো gpio_chip *chip,
			  ধ্রুবক কাঠামো fdt_phandle_args *pargs,
			  কাঠামো gpio_pin *out_pin)
{
	যদি ((pargs->args_গুনতি < 2) || (chip->ngpio <= pargs->args[0]))
		ফেরত SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ফেরত 0;
}
