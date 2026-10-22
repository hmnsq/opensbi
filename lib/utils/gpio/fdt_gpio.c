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
	ܐܢ (!fdt_getprop(fdt, nodeoff, "gpio-controller", عدم))
		ܦܢܐ SBI_EINVAL;

	ܦܢܐ fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

ساكن صحيح fdt_gpio_chip_find(ثابت فراغ *fdt, صحيح nodeoff,
			      بنية gpio_chip **out_chip)
{
	صحيح rc;
	بنية gpio_chip *chip = gpio_chip_find(nodeoff);

	ܐܢ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ܐܢ (rc)
			ܦܢܐ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ܐܢ (!chip)
			ܦܢܐ SBI_ENOSYS;
	}

	ܐܢ (out_chip)
		*out_chip = chip;

	ܦܢܐ 0;
}

صحيح fdt_gpio_pin_get(ثابت فراغ *fdt, صحيح nodeoff, صحيح index,
		     بنية gpio_pin *out_pin)
{
	صحيح rc;
	ثابت بنية fdt_gpio *drv;
	بنية gpio_chip *chip = عدم;
	بنية fdt_phandle_args pargs;

	ܐܢ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ܦܢܐ SBI_EINVAL;

	pargs.node_offset = pargs.args_count = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ܐܢ (rc)
		ܦܢܐ rc;

	rc = fdt_gpio_chip_find(fdt, pargs.node_offset, &chip);
	ܐܢ (rc)
		ܦܢܐ rc;

	drv = chip->driver;
	ܐܢ (!drv || !drv->xlate)
		ܦܢܐ SBI_ENOSYS;

	ܦܢܐ drv->xlate(chip, &pargs, out_pin);
}

صحيح fdt_gpio_simple_xlate(بنية gpio_chip *chip,
			  ثابت بنية fdt_phandle_args *pargs,
			  بنية gpio_pin *out_pin)
{
	ܐܢ ((pargs->args_count < 2) || (chip->ngpio <= pargs->args[0]))
		ܦܢܐ SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ܦܢܐ 0;
}
