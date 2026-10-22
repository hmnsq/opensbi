/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
بیرونی ثابت ساختار fdt_driver *ثابت fdt_gpio_drivers[];

ایستا صحیح fdt_gpio_init(ثابت تهی *fdt, صحیح nodeoff)
{
	/* Check "gpio-controller" property */
	اگر (!fdt_getprop(fdt, nodeoff, "gpio-controller", پوچ))
		بازگشت SBI_EINVAL;

	بازگشت fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

ایستا صحیح fdt_gpio_chip_find(ثابت تهی *fdt, صحیح nodeoff,
			      ساختار gpio_chip **out_chip)
{
	صحیح rc;
	ساختار gpio_chip *chip = gpio_chip_find(nodeoff);

	اگر (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		اگر (rc)
			بازگشت rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		اگر (!chip)
			بازگشت SBI_ENOSYS;
	}

	اگر (out_chip)
		*out_chip = chip;

	بازگشت 0;
}

صحیح fdt_gpio_pin_get(ثابت تهی *fdt, صحیح nodeoff, صحیح index,
		     ساختار gpio_pin *out_pin)
{
	صحیح rc;
	ثابت ساختار fdt_gpio *drv;
	ساختار gpio_chip *chip = پوچ;
	ساختار fdt_phandle_args pargs;

	اگر (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		بازگشت SBI_EINVAL;

	pargs.گره_offset = pargs.args_شمار = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	اگر (rc)
		بازگشت rc;

	rc = fdt_gpio_chip_find(fdt, pargs.گره_offset, &chip);
	اگر (rc)
		بازگشت rc;

	drv = chip->driver;
	اگر (!drv || !drv->xlate)
		بازگشت SBI_ENOSYS;

	بازگشت drv->xlate(chip, &pargs, out_pin);
}

صحیح fdt_gpio_simple_xlate(ساختار gpio_chip *chip,
			  ثابت ساختار fdt_phandle_args *pargs,
			  ساختار gpio_pin *out_pin)
{
	اگر ((pargs->args_شمار < 2) || (chip->ngpio <= pargs->args[0]))
		بازگشت SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	بازگشت 0;
}
