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

	أرجع fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

ساكن صحيح fdt_gpio_chip_find(ثابت فراغ *fdt, صحيح nodeoff,
			      بنية gpio_chip **out_chip)
{
	صحيح rc;
	بنية gpio_chip *chip = gpio_chip_find(nodeoff);

	إذا (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		إذا (rc)
			أرجع rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		إذا (!chip)
			أرجع SBI_ENOSYS;
	}

	إذا (out_chip)
		*out_chip = chip;

	أرجع 0;
}

صحيح fdt_gpio_pin_get(ثابت فراغ *fdt, صحيح nodeoff, صحيح index,
		     بنية gpio_pin *out_pin)
{
	صحيح rc;
	ثابت بنية fdt_gpio *drv;
	بنية gpio_chip *chip = عدم;
	بنية fdt_phandle_حجوجي pargs;

	إذا (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		أرجع SBI_EINVAL;

	pargs.كلي_offset = pargs.حجوجي_كدي = 0;
	rc = fdt_parse_phandle_with_حجوجي(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	إذا (rc)
		أرجع rc;

	rc = fdt_gpio_chip_find(fdt, pargs.كلي_offset, &chip);
	إذا (rc)
		أرجع rc;

	drv = chip->driver;
	إذا (!drv || !drv->xlate)
		أرجع SBI_ENOSYS;

	أرجع drv->xlate(chip, &pargs, out_pin);
}

صحيح fdt_gpio_simple_xlate(بنية gpio_chip *chip,
			  ثابت بنية fdt_phandle_حجوجي *pargs,
			  بنية gpio_pin *out_pin)
{
	إذا ((pargs->حجوجي_كدي < 2) || (chip->ngpio <= pargs->حجوجي[0]))
		أرجع SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->حجوجي[0];
	out_pin->flags = pargs->حجوجي[1];
	أرجع 0;
}
