/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/gpio/fdt_gpio.h>

/* List of FDT gpio drivers generated at compile time */
ବାହ୍ୟ ଅଚଳ ସଂରଚନା fdt_driver *ଅଚଳ fdt_gpio_drivers[];

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_gpio_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff)
{
	/* Check "gpio-controller" property */
	ଯଦି (!fdt_getprop(fdt, nodeoff, "gpio-controller", ଖାଲି))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଫେରନ୍ତୁ fdt_driver_init_by_offset(fdt, nodeoff, fdt_gpio_drivers);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_gpio_chip_find(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			      ସଂରଚନା gpio_chip **out_chip)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା gpio_chip *chip = gpio_chip_find(nodeoff);

	ଯଦି (!chip) {
		/* GPIO chip not found so initialize matching driver */
		rc = fdt_gpio_init(fdt, nodeoff);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		/* Try to find GPIO chip again */
		chip = gpio_chip_find(nodeoff);
		ଯଦି (!chip)
			ଫେରନ୍ତୁ SBI_ENOSYS;
	}

	ଯଦି (out_chip)
		*out_chip = chip;

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_gpio_pin_get(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
		     ସଂରଚନା gpio_pin *out_pin)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅଚଳ ସଂରଚନା fdt_gpio *drv;
	ସଂରଚନା gpio_chip *chip = ଖାଲି;
	ସଂରଚନା fdt_phandle_args pargs;

	ଯଦି (!fdt || (nodeoff < 0) || (index < 0) || !out_pin)
		ଫେରନ୍ତୁ SBI_EINVAL;

	pargs.ଗଣ୍ଠି_offset = pargs.args_ଗଣନା = 0;
	rc = fdt_parse_phandle_with_args(fdt, nodeoff,
					 "gpios", "#gpio-cells",
					 index, &pargs);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = fdt_gpio_chip_find(fdt, pargs.ଗଣ୍ଠି_offset, &chip);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	drv = chip->driver;
	ଯଦି (!drv || !drv->xlate)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଫେରନ୍ତୁ drv->xlate(chip, &pargs, out_pin);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_gpio_simple_xlate(ସଂରଚନା gpio_chip *chip,
			  ଅଚଳ ସଂରଚନା fdt_phandle_args *pargs,
			  ସଂରଚନା gpio_pin *out_pin)
{
	ଯଦି ((pargs->args_ଗଣନା < 2) || (chip->ngpio <= pargs->args[0]))
		ଫେରନ୍ତୁ SBI_EINVAL;

	out_pin->chip = chip;
	out_pin->offset = pargs->args[0];
	out_pin->flags = pargs->args[1];
	ଫେରନ୍ତୁ 0;
}
