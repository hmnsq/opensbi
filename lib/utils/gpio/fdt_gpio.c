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

	أرجع fdt_driver_init_by_وٿي(fdt, nodeoff, fdt_gpio_drivers);
}

ساكن صحيح fdt_gpio_chip_ڳوليو(ثابت فراغ *fdt, صحيح nodeoff,
			      بنية gpio_chip **ٻاهريون_chip)
{
	صحيح rc;
	بنية gpio_chip *chip = gpio_chip_ڳوليو(nodeoff);

	إذا (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		إذا (rc)
			أرجع rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_ڳوليو(nodeoff);
		إذا (!chip)
			أرجع SBI_ENOSYS;
	}

	إذا (ٻاهريون_chip)
		*ٻاهريون_chip = chip;

	أرجع 0;
}

صحيح fdt_gpio_pin_get(ثابت فراغ *fdt, صحيح nodeoff, صحيح اشاريو_2,
		     بنية gpio_pin *ٻاهريون_pin)
{
	صحيح rc;
	ثابت بنية fdt_gpio *drv;
	بنية gpio_chip *chip = عدم;
	بنية fdt_phandle_دليلون pargs;

	إذا (!fdt || (nodeoff < 0) || (اشاريو_2 < 0) || !ٻاهريون_pin)
		أرجع SBI_EINVAL;

	pargs.ڳنڍ_وٿي = pargs.دليلون_ڳڻپ = 0;
	rc = fdt_parse_phandle_with_دليلون(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 اشاريو_2, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_gpio_chip_ڳوليو(fdt, pargs.ڳنڍ_وٿي, &chip);
	إذا (rc)
		أرجع rc;

	drv = chip->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	أرجع drv->xlate(chip, &pargs, ٻاهريون_pin);
}

صحيح fdt_gpio_simple_xlate(بنية gpio_chip *chip,
			  ثابت بنية fdt_phandle_دليلون *pargs,
			  بنية gpio_pin *ٻاهريون_pin)
{
	إذا ((pargs->دليلون_ڳڻپ < 2) || (chip->ngpio <= pargs->دليلون[0]))
		أرجع SBI_EINVAL;

	ٻاهريون_pin->chip = chip;
	ٻاهريون_pin->وٿي = pargs->دليلون[0];
	ٻاهريون_pin->flags = pargs->دليلون[1];
	أرجع 0;
}
