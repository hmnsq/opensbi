/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ fdt_driver *ਅਚਲ fdt_gpio_drivers[];

ਸਥਿਰ ਪੂਰਨ fdt_gpio_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff)
{
	/* Check "gpio-controller" property */
	ਜੇ (!fdt_getprop(fdt, nodeoff, "gpio-controller", ਨਿਰਾ))
		ਵਾਪਸ SBI_EINVAL;

	ਵਾਪਸ fdt_driver_init_by_ਵਿੱਥ(fdt, nodeoff, fdt_gpio_drivers);
}

ਸਥਿਰ ਪੂਰਨ fdt_gpio_chip_ਲੱਭੋ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			      ਬਣਤਰ gpio_chip **ਬਾਹਰਲਾ_chip)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ gpio_chip *chip = gpio_chip_ਲੱਭੋ(nodeoff);

	ਜੇ (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_ਲੱਭੋ(nodeoff);
		ਜੇ (!chip)
			ਵਾਪਸ SBI_ENOSYS;
	}

	ਜੇ (ਬਾਹਰਲਾ_chip)
		*ਬਾਹਰਲਾ_chip = chip;

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_gpio_pin_get(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਪੂਰਨ ਸੂਚਕਾਂਕ_2,
		     ਬਣਤਰ gpio_pin *ਬਾਹਰਲਾ_pin)
{
	ਪੂਰਨ rc;
	ਅਚਲ ਬਣਤਰ fdt_gpio *drv;
	ਬਣਤਰ gpio_chip *chip = ਨਿਰਾ;
	ਬਣਤਰ fdt_phandle_ਦਲੀਲਾਂ pargs;

	ਜੇ (!fdt || (nodeoff < 0) || (ਸੂਚਕਾਂਕ_2 < 0) || !ਬਾਹਰਲਾ_pin)
		ਵਾਪਸ SBI_EINVAL;

	pargs.ਗੰਢ_ਵਿੱਥ = pargs.ਦਲੀਲਾਂ_ਗਿਣਤੀ = 0;
	rc = fdt_parse_phandle_with_ਦਲੀਲਾਂ(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 ਸੂਚਕਾਂਕ_2, &pargs);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = fdt_gpio_chip_ਲੱਭੋ(fdt, pargs.ਗੰਢ_ਵਿੱਥ, &chip);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	drv = chip->driver;
	ਜੇ (!drv || !drv->xlate)
		ਵਾਪਸ SBI_ENOSYS;

	ਵਾਪਸ drv->xlate(chip, &pargs, ਬਾਹਰਲਾ_pin);
}

ਪੂਰਨ fdt_gpio_simple_xlate(ਬਣਤਰ gpio_chip *chip,
			  ਅਚਲ ਬਣਤਰ fdt_phandle_ਦਲੀਲਾਂ *pargs,
			  ਬਣਤਰ gpio_pin *ਬਾਹਰਲਾ_pin)
{
	ਜੇ ((pargs->ਦਲੀਲਾਂ_ਗਿਣਤੀ < 2) || (chip->ngpio <= pargs->ਦਲੀਲਾਂ[0]))
		ਵਾਪਸ SBI_EINVAL;

	ਬਾਹਰਲਾ_pin->chip = chip;
	ਬਾਹਰਲਾ_pin->ਵਿੱਥ = pargs->ਦਲੀਲਾਂ[0];
	ਬਾਹਰਲਾ_pin->flags = pargs->ਦਲੀਲਾਂ[1];
	ਵਾਪਸ 0;
}
