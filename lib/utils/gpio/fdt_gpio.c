/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
خارجي ثابت بنية fdt_driver *ثابت fdt_gpio_drivers[];

ساكن صحيح fdt_gpio_init(ثابت فراغ *fdt, صحيح nodeoff)
{
	/* Check "gpio-controller" property */
	إذا (!fdt_getprop(fdt, nodeoff, "gpio-controller", عدم))
		أرجع SBI_EINVAL;

	أرجع fdt_driver_init_by_وتھ(fdt, nodeoff, fdt_gpio_drivers);
}

ساكن صحيح fdt_gpio_chip_لبھو(ثابت فراغ *fdt, صحيح nodeoff,
			      بنية gpio_chip **باہرلا_chip)
{
	صحيح rc;
	بنية gpio_chip *chip = gpio_chip_لبھو(nodeoff);

	إذا (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		إذا (rc)
			أرجع rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_لبھو(nodeoff);
		إذا (!chip)
			أرجع SBI_ENOSYS;
	}

	إذا (باہرلا_chip)
		*باہرلا_chip = chip;

	أرجع 0;
}

صحيح fdt_gpio_pin_get(ثابت فراغ *fdt, صحيح nodeoff, صحيح اشاریہ_2,
		     بنية gpio_pin *باہرلا_pin)
{
	صحيح rc;
	ثابت بنية fdt_gpio *drv;
	بنية gpio_chip *chip = عدم;
	بنية fdt_phandle_دلیلاں pargs;

	إذا (!fdt || (nodeoff < 0) || (اشاریہ_2 < 0) || !باہرلا_pin)
		أرجع SBI_EINVAL;

	pargs.گرہ_وتھ = pargs.دلیلاں_گنتی = 0;
	rc = fdt_parse_phandle_with_دلیلاں(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 اشاریہ_2, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_gpio_chip_لبھو(fdt, pargs.گرہ_وتھ, &chip);
	إذا (rc)
		أرجع rc;

	drv = chip->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	أرجع drv->xlate(chip, &pargs, باہرلا_pin);
}

صحيح fdt_gpio_simple_xlate(بنية gpio_chip *chip,
			  ثابت بنية fdt_phandle_دلیلاں *pargs,
			  بنية gpio_pin *باہرلا_pin)
{
	إذا ((pargs->دلیلاں_گنتی < 2) || (chip->ngpio <= pargs->دلیلاں[0]))
		أرجع SBI_EINVAL;

	باہرلا_pin->chip = chip;
	باہرلا_pin->وتھ = pargs->دلیلاں[0];
	باہرلا_pin->flags = pargs->دلیلاں[1];
	أرجع 0;
}
