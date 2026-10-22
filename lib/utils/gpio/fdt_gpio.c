/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
बाह्य स्थिर काष्ठा fdt_driver *स्थिर fdt_gpio_drivers[];

अटल पूर्णांक fdt_gpio_init(स्थिर व्योम *fdt, पूर्णांक nodeoff)
{
	/* Check "gpio-controller" property */
	अगर (!fdt_getprop(fdt, nodeoff, "gpio-controller", शून्य))
		वापस SBI_EINVAL;

	वापस fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

अटल पूर्णांक fdt_gpio_chip_find(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			      काष्ठा gpio_chip **out_chip)
{
	पूर्णांक rc;
	काष्ठा gpio_chip *chip = gpio_chip_find(nodeoff);

	अगर (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		अगर (rc)
			वापस rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		अगर (!chip)
			वापस SBI_ENOSYS;
	}

	अगर (out_chip)
		*out_chip = chip;

	वापस 0;
}

पूर्णांक fdt_gpio_pin_get(स्थिर व्योम *fdt, पूर्णांक nodeoff, पूर्णांक सूचकांक_2,
		     काष्ठा gpio_pin *out_pin)
{
	पूर्णांक rc;
	स्थिर काष्ठा fdt_gpio *drv;
	काष्ठा gpio_chip *chip = शून्य;
	काष्ठा fdt_phandle_args pargs;

	अगर (!fdt || (nodeoff < 0) || (सूचकांक_2 < 0) || !out_pin)
		वापस SBI_EINVAL;

	pargs.गाँठ_offset = pargs.args_गिनती = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 सूचकांक_2, &pargs);
	अगर (rc)
		वापस rc;

	rc = fdt_gpio_chip_find(fdt, pargs.गाँठ_offset, &chip);
	अगर (rc)
		वापस rc;

	drv = chip->driver;
	अगर (!drv || !drv->xlate)
		वापस SBI_ENOSYS;

	वापस drv->xlate(chip, &pargs, out_pin);
}

पूर्णांक fdt_gpio_simple_xlate(काष्ठा gpio_chip *chip,
			  स्थिर काष्ठा fdt_phandle_args *pargs,
			  काष्ठा gpio_pin *out_pin)
{
	अगर ((pargs->args_गिनती < 2) || (chip->ngpio <= pargs->args[0]))
		वापस SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	वापस 0;
}
