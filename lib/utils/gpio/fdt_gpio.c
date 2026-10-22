/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
வெளி மாறிலி கட்டமைப்பு fdt_driver *மாறிலி fdt_gpio_drivers[];

நிலையான முழு fdt_gpio_init(மாறிலி வெறுமை *fdt, முழு nodeoff)
{
	/* Check "gpio-controller" property */
	எனில் (!fdt_getprop(fdt, nodeoff, "gpio-controller", வெற்று))
		திரும்பு SBI_EINVAL;

	திரும்பு fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

நிலையான முழு fdt_gpio_chip_find(மாறிலி வெறுமை *fdt, முழு nodeoff,
			      கட்டமைப்பு gpio_chip **out_chip)
{
	முழு rc;
	கட்டமைப்பு gpio_chip *chip = gpio_chip_find(nodeoff);

	எனில் (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		எனில் (rc)
			திரும்பு rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		எனில் (!chip)
			திரும்பு SBI_ENOSYS;
	}

	எனில் (out_chip)
		*out_chip = chip;

	திரும்பு 0;
}

முழு fdt_gpio_pin_get(மாறிலி வெறுமை *fdt, முழு nodeoff, முழு index,
		     கட்டமைப்பு gpio_pin *out_pin)
{
	முழு rc;
	மாறிலி கட்டமைப்பு fdt_gpio *drv;
	கட்டமைப்பு gpio_chip *chip = வெற்று;
	கட்டமைப்பு fdt_phandle_args pargs;

	எனில் (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		திரும்பு SBI_EINVAL;

	pargs.முனை_offset = pargs.args_மொத்தம் = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	எனில் (rc)
		திரும்பு rc;

	rc = fdt_gpio_chip_find(fdt, pargs.முனை_offset, &chip);
	எனில் (rc)
		திரும்பு rc;

	drv = chip->driver;
	எனில் (!drv || !drv->xlate)
		திரும்பு SBI_ENOSYS;

	திரும்பு drv->xlate(chip, &pargs, out_pin);
}

முழு fdt_gpio_simple_xlate(கட்டமைப்பு gpio_chip *chip,
			  மாறிலி கட்டமைப்பு fdt_phandle_args *pargs,
			  கட்டமைப்பு gpio_pin *out_pin)
{
	எனில் ((pargs->args_மொத்தம் < 2) || (chip->ngpio <= pargs->args[0]))
		திரும்பு SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	திரும்பு 0;
}
