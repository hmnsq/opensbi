/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ fdt_driver *ಧ್ರುವ fdt_gpio_drivers[];

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_gpio_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff)
{
	/* Check "gpio-controller" property */
	ಯದಿ (!fdt_getprop(fdt, nodeoff, "gpio-controller", ರಿಕ್ತ))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಪ್ರತ್ಯಾಗಮ fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_gpio_chip_find(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			      ಸಂರಚನಾ gpio_chip **out_chip)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ gpio_chip *chip = gpio_chip_find(nodeoff);

	ಯದಿ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ಯದಿ (!chip)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;
	}

	ಯದಿ (out_chip)
		*out_chip = chip;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_gpio_pin_get(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಪೂರ್ಣಾಙ್ಕ index,
		     ಸಂರಚನಾ gpio_pin *out_pin)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಧ್ರುವ ಸಂರಚನಾ fdt_gpio *drv;
	ಸಂರಚನಾ gpio_chip *chip = ರಿಕ್ತ;
	ಸಂರಚನಾ fdt_phandle_args pargs;

	ಯದಿ (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	pargs.ಗಂಟು_offset = pargs.args_ಎಣಿಕೆ = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	rc = fdt_gpio_chip_find(fdt, pargs.ಗಂಟು_offset, &chip);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	drv = chip->driver;
	ಯದಿ (!drv || !drv->xlate)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಪ್ರತ್ಯಾಗಮ drv->xlate(chip, &pargs, out_pin);
}

ಪೂರ್ಣಾಙ್ಕ fdt_gpio_simple_xlate(ಸಂರಚನಾ gpio_chip *chip,
			  ಧ್ರುವ ಸಂರಚನಾ fdt_phandle_args *pargs,
			  ಸಂರಚನಾ gpio_pin *out_pin)
{
	ಯದಿ ((pargs->args_ಎಣಿಕೆ < 2) || (chip->ngpio <= pargs->args[0]))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ಪ್ರತ್ಯಾಗಮ 0;
}
