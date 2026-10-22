/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

ସଂରଚନା sifive_gpio_chip {
	unsigned long addr;
	ସଂରଚନା gpio_chip chip;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_gpio_direction_output(ସଂରଚନା gpio_pin *gp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;
	ସଂରଚନା sifive_gpio_chip *chip =
		container_of(gp->chip, ସଂରଚନା sifive_gpio_chip, chip);

	v = readl((ଅସ୍ଥିର ଶୂନ୍ୟ *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ଅସ୍ଥିର ଶୂନ୍ୟ *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((ଅସ୍ଥିର ଶୂନ୍ୟ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ଯଦି (!ମୂଲ୍ୟ)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	ଅନ୍ୟଥା
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ଅସ୍ଥିର ଶୂନ୍ୟ *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_gpio_set(ସଂରଚନା gpio_pin *gp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;
	ସଂରଚନା sifive_gpio_chip *chip =
		container_of(gp->chip, ସଂରଚନା sifive_gpio_chip, chip);

	v = readl((ଅସ୍ଥିର ଶୂନ୍ୟ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ଯଦି (!ମୂଲ୍ୟ)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	ଅନ୍ୟଥା
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ଅସ୍ଥିର ଶୂନ୍ୟ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ଅଚଳ ସଂରଚନା fdt_gpio fdt_gpio_sifive;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_gpio_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			    ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(ଆକାର(*chip));
	ଯଦି (!chip)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, ଖାଲି);
	ଯଦି (rc) {
		sbi_free(chip);
		ଫେରନ୍ତୁ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	ଯଦି (rc) {
		sbi_free(chip);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
