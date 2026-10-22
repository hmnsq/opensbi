/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

সংরচনা sifive_gpio_chip {
	unsigned long addr;
	সংরচনা gpio_chip chip;
};

স্থির পূর্ণাঙ্ক sifive_gpio_direction_output(সংরচনা gpio_pin *gp, পূর্ণাঙ্ক মান)
{
	unsigned পূর্ণাঙ্ক v;
	সংরচনা sifive_gpio_chip *chip =
		container_of(gp->chip, সংরচনা sifive_gpio_chip, chip);

	v = readl((চঞ্চল শূন্য *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (চঞ্চল শূন্য *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((চঞ্চল শূন্য *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	যদি (!মান)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	অন্যথা
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (চঞ্চল শূন্য *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	প্রত্যাগম 0;
}

স্থির শূন্য sifive_gpio_set(সংরচনা gpio_pin *gp, পূর্ণাঙ্ক মান)
{
	unsigned পূর্ণাঙ্ক v;
	সংরচনা sifive_gpio_chip *chip =
		container_of(gp->chip, সংরচনা sifive_gpio_chip, chip);

	v = readl((চঞ্চল শূন্য *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	যদি (!মান)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	অন্যথা
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (চঞ্চল শূন্য *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ধ্রুব সংরচনা fdt_gpio fdt_gpio_sifive;

স্থির পূর্ণাঙ্ক sifive_gpio_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			    ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(পরিমাণ(*chip));
	যদি (!chip)
		প্রত্যাগম SBI_ENOMEM;

	rc = fdt_get_গ্রন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (rc) {
		sbi_free(chip);
		প্রত্যাগম rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	যদি (rc) {
		sbi_free(chip);
		প্রত্যাগম rc;
	}

	প্রত্যাগম 0;
}

স্থির ধ্রুব সংরচনা fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ধ্রুব সংরচনা fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
