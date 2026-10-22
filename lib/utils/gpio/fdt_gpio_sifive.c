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

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_GPIO_PINS_MIN	1
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_GPIO_PINS_MAX	32
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_GPIO_PINS_DEF	16

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_GPIO_OUTEN	0x8
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_GPIO_OUTVAL	0xc
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_GPIO_ಬಿಟ್(b)	(1U << (b))

ರಚನೆ sifive_gpio_chip {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr;
	ರಚನೆ gpio_chip chip;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_gpio_direction_output(ರಚನೆ gpio_pin *gp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ v;
	ರಚನೆ sifive_gpio_chip *chip =
		container_ದ(gp->chip, ರಚನೆ sifive_gpio_chip, chip);

	v = readl((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	writel(v, (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ಆದರೆ (!ಮೌಲ್ಯ)
		v &= ~SIFIVE_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ
		v |= SIFIVE_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	writel(v, (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_gpio_ಗಣ(ರಚನೆ gpio_pin *gp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ v;
	ರಚನೆ sifive_gpio_chip *chip =
		container_ದ(gp->chip, ರಚನೆ sifive_gpio_chip, chip);

	v = readl((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ಆದರೆ (!ಮೌಲ್ಯ)
		v &= ~SIFIVE_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ
		v |= SIFIVE_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	writel(v, (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_gpio fdt_gpio_sifive;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_gpio_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			    ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(ಗಾತ್ರ(*chip));
	ಆದರೆ (!chip)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &addr, ಖಾಲಿ);
	ಆದರೆ (rc) {
		sbi_free(chip);
		ಹಿಂತಿರುಗಿ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.ಗಣ = sifive_gpio_ಗಣ;
	rc = gpio_chip_ಕೂಡಿಸು(&chip->chip);
	ಆದರೆ (rc) {
		sbi_free(chip);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sifive_gpio_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sifive_gpio_ಹೊಂದಿಕೆ,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
