/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

ಸಂರಚನಾ sifive_gpio_chip {
	unsigned long addr;
	ಸಂರಚನಾ gpio_chip chip;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_gpio_direction_output(ಸಂರಚನಾ gpio_pin *gp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ v;
	ಸಂರಚನಾ sifive_gpio_chip *chip =
		container_of(gp->chip, ಸಂರಚನಾ sifive_gpio_chip, chip);

	v = readl((ಚಞ್ಚಲ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ಚಞ್ಚಲ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((ಚಞ್ಚಲ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ಯದಿ (!ಮೌಲ್ಯ)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	ಅನ್ಯಥಾ
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ಚಞ್ಚಲ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_gpio_set(ಸಂರಚನಾ gpio_pin *gp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ v;
	ಸಂರಚನಾ sifive_gpio_chip *chip =
		container_of(gp->chip, ಸಂರಚನಾ sifive_gpio_chip, chip);

	v = readl((ಚಞ್ಚಲ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ಯದಿ (!ಮೌಲ್ಯ)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	ಅನ್ಯಥಾ
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ಚಞ್ಚಲ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ಧ್ರುವ ಸಂರಚನಾ fdt_gpio fdt_gpio_sifive;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_gpio_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			    ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(ಪರಿಮಾಣ(*chip));
	ಯದಿ (!chip)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (rc) {
		sbi_free(chip);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	ಯದಿ (rc) {
		sbi_free(chip);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
