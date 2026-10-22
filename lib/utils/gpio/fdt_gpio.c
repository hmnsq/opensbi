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

	أرجع fdt_driver_init_by_جابه_جایی(fdt, nodeoff, fdt_gpio_drivers);
}

ساكن صحيح fdt_gpio_chip_پیدا_کن(ثابت فراغ *fdt, صحيح nodeoff,
			      بنية gpio_chip **بیرون_chip)
{
	صحيح rc;
	بنية gpio_chip *chip = gpio_chip_پیدا_کن(nodeoff);

	إذا (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		إذا (rc)
			أرجع rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_پیدا_کن(nodeoff);
		إذا (!chip)
			أرجع SBI_ENOSYS;
	}

	إذا (بیرون_chip)
		*بیرون_chip = chip;

	أرجع 0;
}

صحيح fdt_gpio_pin_get(ثابت فراغ *fdt, صحيح nodeoff, صحيح شاخص_2,
		     بنية gpio_pin *بیرون_pin)
{
	صحيح rc;
	ثابت بنية fdt_gpio *drv;
	بنية gpio_chip *chip = عدم;
	بنية fdt_phandle_آرگومانها pargs;

	إذا (!fdt || (nodeoff < 0) || (شاخص_2 < 0) || !بیرون_pin)
		أرجع SBI_EINVAL;

	pargs.گره_جابه_جایی = pargs.آرگومانها_شمار = 0;
	rc = fdt_parse_phandle_with_آرگومانها(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 شاخص_2, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_gpio_chip_پیدا_کن(fdt, pargs.گره_جابه_جایی, &chip);
	إذا (rc)
		أرجع rc;

	drv = chip->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	أرجع drv->xlate(chip, &pargs, بیرون_pin);
}

صحيح fdt_gpio_simple_xlate(بنية gpio_chip *chip,
			  ثابت بنية fdt_phandle_آرگومانها *pargs,
			  بنية gpio_pin *بیرون_pin)
{
	إذا ((pargs->آرگومانها_شمار < 2) || (chip->ngpio <= pargs->آرگومانها[0]))
		أرجع SBI_EINVAL;

	بیرون_pin->chip = chip;
	بیرون_pin->جابه_جایی = pargs->آرگومانها[0];
	بیرون_pin->flags = pargs->آرگومانها[1];
	أرجع 0;
}
