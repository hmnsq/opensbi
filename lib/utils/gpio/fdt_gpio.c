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
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver *ಸ್ಥಿರಾಂಕ fdt_gpio_drivers[];

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_gpio_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff)
{
	/* Check "gpio-controller" property */
	ಆದರೆ (!fdt_getprop(fdt, nodeoff, "gpio-controller", ಖಾಲಿ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹಿಂತಿರುಗಿ fdt_driver_init_by_ಸ್ಥಾನಾಂತರ(fdt, nodeoff, fdt_gpio_drivers);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_gpio_chip_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			      ರಚನೆ gpio_chip **ಹೊರಹೋಗು_chip)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ gpio_chip *chip = gpio_chip_ಕಂಡುಹಿಡಿ(nodeoff);

	ಆದರೆ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_ಕಂಡುಹಿಡಿ(nodeoff);
		ಆದರೆ (!chip)
			ಹಿಂತಿರುಗಿ SBI_ENOSYS;
	}

	ಆದರೆ (ಹೊರಹೋಗು_chip)
		*ಹೊರಹೋಗು_chip = chip;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_gpio_pin_get(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_2,
		     ರಚನೆ gpio_pin *ಹೊರಹೋಗು_pin)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_gpio *drv;
	ರಚನೆ gpio_chip *chip = ಖಾಲಿ;
	ರಚನೆ fdt_phandle_ವಾದಗಳು pargs;

	ಆದರೆ (!fdt || (nodeoff < 0) || (ಸೂಚ್ಯಂಕ_2 < 0) || !ಹೊರಹೋಗು_pin)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	pargs.ಗಂಟು_ಸ್ಥಾನಾಂತರ = pargs.ವಾದಗಳು_ಎಣಿಕೆ = 0;
	rc = fdt_parse_phandle_with_ವಾದಗಳು(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 ಸೂಚ್ಯಂಕ_2, &pargs);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = fdt_gpio_chip_ಕಂಡುಹಿಡಿ(fdt, pargs.ಗಂಟು_ಸ್ಥಾನಾಂತರ, &chip);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	drv = chip->driver;
	ಆದರೆ (!drv || !drv->xlate)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಹಿಂತಿರುಗಿ drv->xlate(chip, &pargs, ಹೊರಹೋಗು_pin);
}

ಪೂರ್ಣಾಂಕ fdt_gpio_simple_xlate(ರಚನೆ gpio_chip *chip,
			  ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_phandle_ವಾದಗಳು *pargs,
			  ರಚನೆ gpio_pin *ಹೊರಹೋಗು_pin)
{
	ಆದರೆ ((pargs->ವಾದಗಳು_ಎಣಿಕೆ < 2) || (chip->ngpio <= pargs->ವಾದಗಳು[0]))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಹೊರಹೋಗು_pin->chip = chip;
	ಹೊರಹೋಗು_pin->ಸ್ಥಾನಾಂತರ = pargs->ವಾದಗಳು[0];
	ಹೊರಹೋಗು_pin->flags = pargs->ವಾದಗಳು[1];
	ಹಿಂತಿರುಗಿ 0;
}
