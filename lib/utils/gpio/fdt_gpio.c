/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_gpio_drivers[];

સ્થિર પૂર્ણાંક fdt_gpio_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff)
{
	/* Check "gpio-controller" property */
	જો (!fdt_getprop(fdt, nodeoff, "gpio-controller", ખાલી))
		પાછા SBI_EINVAL;

	પાછા fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

સ્થિર પૂર્ણાંક fdt_gpio_chip_find(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			      સંરચના gpio_chip **out_chip)
{
	પૂર્ણાંક rc;
	સંરચના gpio_chip *chip = gpio_chip_find(nodeoff);

	જો (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		જો (rc)
			પાછા rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		જો (!chip)
			પાછા SBI_ENOSYS;
	}

	જો (out_chip)
		*out_chip = chip;

	પાછા 0;
}

પૂર્ણાંક fdt_gpio_pin_get(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, પૂર્ણાંક index,
		     સંરચના gpio_pin *out_pin)
{
	પૂર્ણાંક rc;
	અચલ સંરચના fdt_gpio *drv;
	સંરચના gpio_chip *chip = ખાલી;
	સંરચના fdt_phandle_args pargs;

	જો (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		પાછા SBI_EINVAL;

	pargs.ગાંઠ_offset = pargs.args_ગણતરી = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	જો (rc)
		પાછા rc;

	rc = fdt_gpio_chip_find(fdt, pargs.ગાંઠ_offset, &chip);
	જો (rc)
		પાછા rc;

	drv = chip->driver;
	જો (!drv || !drv->xlate)
		પાછા SBI_ENOSYS;

	પાછા drv->xlate(chip, &pargs, out_pin);
}

પૂર્ણાંક fdt_gpio_simple_xlate(સંરચના gpio_chip *chip,
			  અચલ સંરચના fdt_phandle_args *pargs,
			  સંરચના gpio_pin *out_pin)
{
	જો ((pargs->args_ગણતરી < 2) || (chip->ngpio <= pargs->args[0]))
		પાછા SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	પાછા 0;
}
