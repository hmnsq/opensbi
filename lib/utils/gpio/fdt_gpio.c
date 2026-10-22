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
خارجی ثابت ساخت fdt_driver *ثابت fdt_gpio_drivers[];

ساکن صحیح fdt_gpio_init(ثابت خالی *fdt, صحیح nodeoff)
{
	/* Check "gpio-controller" property */
	اگر (!fdt_getprop(fdt, nodeoff, "gpio-controller", تہی))
		واپس SBI_EINVAL;

	واپس fdt_driver_init_by_انحراف(fdt, nodeoff, fdt_gpio_drivers);
}

ساکن صحیح fdt_gpio_chip_ڈھونڈو(ثابت خالی *fdt, صحیح nodeoff,
			      ساخت gpio_chip **برآمد_chip)
{
	صحیح rc;
	ساخت gpio_chip *chip = gpio_chip_ڈھونڈو(nodeoff);

	اگر (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		اگر (rc)
			واپس rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_ڈھونڈو(nodeoff);
		اگر (!chip)
			واپس SBI_ENOSYS;
	}

	اگر (برآمد_chip)
		*برآمد_chip = chip;

	واپس 0;
}

صحیح fdt_gpio_pin_get(ثابت خالی *fdt, صحیح nodeoff, صحیح اشاریہ_2,
		     ساخت gpio_pin *برآمد_pin)
{
	صحیح rc;
	ثابت ساخت fdt_gpio *drv;
	ساخت gpio_chip *chip = تہی;
	ساخت fdt_phandle_دلائل pargs;

	اگر (!fdt || (nodeoff < 0) || (اشاریہ_2 < 0) || !برآمد_pin)
		واپس SBI_EINVAL;

	pargs.گرہ_انحراف = pargs.دلائل_گنتی = 0;
	rc = fdt_parse_phandle_with_دلائل(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 اشاریہ_2, &pargs);
	اگر (rc)
		واپس rc;

	rc = fdt_gpio_chip_ڈھونڈو(fdt, pargs.گرہ_انحراف, &chip);
	اگر (rc)
		واپس rc;

	drv = chip->driver;
	اگر (!drv || !drv->xlate)
		واپس SBI_ENOSYS;

	واپس drv->xlate(chip, &pargs, برآمد_pin);
}

صحیح fdt_gpio_simple_xlate(ساخت gpio_chip *chip,
			  ثابت ساخت fdt_phandle_دلائل *pargs,
			  ساخت gpio_pin *برآمد_pin)
{
	اگر ((pargs->دلائل_گنتی < 2) || (chip->ngpio <= pargs->دلائل[0]))
		واپس SBI_EINVAL;

	برآمد_pin->chip = chip;
	برآمد_pin->انحراف = pargs->دلائل[0];
	برآمد_pin->flags = pargs->دلائل[1];
	واپس 0;
}
