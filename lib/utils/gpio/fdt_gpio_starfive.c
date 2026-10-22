/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/gpio/fdt_gpio.h>

#define STARFIVE_GPIO_PINS_DEF		64
#define STARFIVE_GPIO_OUTVAL		0x40
#define STARFIVE_GPIO_MASK		0xff
#define STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#define STARFIVE_GPIO_SHIFT_BITS	0x3

সংরচনা starfive_gpio_chip {
	unsigned long addr;
	সংরচনা gpio_chip chip;
};

স্থির পূর্ণাঙ্ক starfive_gpio_direction_output(সংরচনা gpio_pin *gp, পূর্ণাঙ্ক মান)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	সংরচনা starfive_gpio_chip *chip =
		container_of(gp->chip, সংরচনা starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((শূন্য *)reg_addr);
	val &= ~bit_mask;
	writel(val, (শূন্য *)reg_addr);

	প্রত্যাগম 0;
}

স্থির শূন্য starfive_gpio_set(সংরচনা gpio_pin *gp, পূর্ণাঙ্ক মান)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	সংরচনা starfive_gpio_chip *chip =
		container_of(gp->chip, সংরচনা starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((শূন্য *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= মান << shift_bits;
	writel(val, (শূন্য *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ধ্রুব সংরচনা fdt_gpio fdt_gpio_starfive;

স্থির পূর্ণাঙ্ক starfive_gpio_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(পরিমাণ(*chip));
	যদি (!chip)
		প্রত্যাগম SBI_ENOMEM;

	rc = fdt_get_গ্রন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (rc) {
		sbi_free(chip);
		প্রত্যাগম rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	যদি (rc) {
		sbi_free(chip);
		প্রত্যাগম rc;
	}

	প্রত্যাগম 0;
}

স্থির ধ্রুব সংরচনা fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ধ্রুব সংরচনা fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
