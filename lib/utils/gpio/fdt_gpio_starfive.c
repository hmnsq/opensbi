/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/gpio/fdt_gpio.h>

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

	ঘূৰাই 0;
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

ধ্রুৱ সংরচনা fdt_gpio fdt_gpio_starfive;

স্থির পূর্ণাঙ্ক starfive_gpio_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(পরিমাণ(*chip));
	যদি (!chip)
		ঘূৰাই SBI_ENOMEM;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (rc) {
		sbi_free(chip);
		ঘূৰাই rc;
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
		ঘূৰাই rc;
	}

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ধ্রুৱ সংরচনা fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
