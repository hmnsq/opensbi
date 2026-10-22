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

	أرجع fdt_driver_init_by_دننه_واټن(fdt, nodeoff, fdt_gpio_drivers);
}

ساكن صحيح fdt_gpio_chip_پیدا_کړه(ثابت فراغ *fdt, صحيح nodeoff,
			      بنية gpio_chip **بهر_chip)
{
	صحيح rc;
	بنية gpio_chip *chip = gpio_chip_پیدا_کړه(nodeoff);

	إذا (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		إذا (rc)
			أرجع rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_پیدا_کړه(nodeoff);
		إذا (!chip)
			أرجع SBI_ENOSYS;
	}

	إذا (بهر_chip)
		*بهر_chip = chip;

	أرجع 0;
}

صحيح fdt_gpio_pin_get(ثابت فراغ *fdt, صحيح nodeoff, صحيح شاخص_2,
		     بنية gpio_pin *بهر_pin)
{
	صحيح rc;
	ثابت بنية fdt_gpio *drv;
	بنية gpio_chip *chip = عدم;
	بنية fdt_phandle_دلیلونه pargs;

	إذا (!fdt || (nodeoff < 0) || (شاخص_2 < 0) || !بهر_pin)
		أرجع SBI_EINVAL;

	pargs.غوټه_دننه_واټن = pargs.دلیلونه_شمېر = 0;
	rc = fdt_parse_phandle_with_دلیلونه(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 شاخص_2, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_gpio_chip_پیدا_کړه(fdt, pargs.غوټه_دننه_واټن, &chip);
	إذا (rc)
		أرجع rc;

	drv = chip->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	أرجع drv->xlate(chip, &pargs, بهر_pin);
}

صحيح fdt_gpio_simple_xlate(بنية gpio_chip *chip,
			  ثابت بنية fdt_phandle_دلیلونه *pargs,
			  بنية gpio_pin *بهر_pin)
{
	إذا ((pargs->دلیلونه_شمېر < 2) || (chip->ngpio <= pargs->دلیلونه[0]))
		أرجع SBI_EINVAL;

	بهر_pin->chip = chip;
	بهر_pin->دننه_واټن = pargs->دلیلونه[0];
	بهر_pin->flags = pargs->دلیلونه[1];
	أرجع 0;
}
