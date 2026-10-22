/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/gpio/fdt_gpio.h>

#define STARFIVE_GPIO_PINS_DEF		64
#define STARFIVE_GPIO_OUTVAL		0x40
#define STARFIVE_GPIO_MASK		0xff
#define STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#define STARFIVE_GPIO_SHIFT_BITS	0x3

ಸಂರಚನಾ starfive_gpio_chip {
	unsigned long addr;
	ಸಂರಚನಾ gpio_chip chip;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ starfive_gpio_direction_output(ಸಂರಚನಾ gpio_pin *gp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	ಸಂರಚನಾ starfive_gpio_chip *chip =
		container_of(gp->chip, ಸಂರಚನಾ starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((ಶೂನ್ಯ *)reg_addr);
	val &= ~bit_mask;
	writel(val, (ಶೂನ್ಯ *)reg_addr);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ starfive_gpio_set(ಸಂರಚನಾ gpio_pin *gp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	ಸಂರಚನಾ starfive_gpio_chip *chip =
		container_of(gp->chip, ಸಂರಚನಾ starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((ಶೂನ್ಯ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= ಮೌಲ್ಯ << shift_bits;
	writel(val, (ಶೂನ್ಯ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ಧ್ರುವ ಸಂರಚನಾ fdt_gpio fdt_gpio_starfive;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ starfive_gpio_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			      ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(ಪರಿಮಾಣ(*chip));
	ಯದಿ (!chip)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (rc) {
		sbi_free(chip);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	ಯದಿ (rc) {
		sbi_free(chip);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
