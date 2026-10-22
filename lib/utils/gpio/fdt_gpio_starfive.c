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

#ವ್ಯಾಖ್ಯಾನಿಸು STARFIVE_GPIO_PINS_DEF		64
#ವ್ಯಾಖ್ಯಾನಿಸು STARFIVE_GPIO_OUTVAL		0x40
#ವ್ಯಾಖ್ಯಾನಿಸು STARFIVE_GPIO_ಮುಖವಾಡ		0xff
#ವ್ಯಾಖ್ಯಾನಿಸು STARFIVE_GPIO_REG_ಸರಿಸು_ಮುಖವಾಡ	0x3
#ವ್ಯಾಖ್ಯಾನಿಸು STARFIVE_GPIO_ಸರಿಸು_ಬಿಟ್ಟುಗಳು	0x3

ರಚನೆ starfive_gpio_chip {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr;
	ರಚನೆ gpio_chip chip;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ starfive_gpio_direction_output(ರಚನೆ gpio_pin *gp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	u32 val;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg_addr;
	u32 ಬಿಟ್_ಮುಖವಾಡ_2, ಸರಿಸು_ಬಿಟ್ಟುಗಳು;
	ರಚನೆ starfive_gpio_chip *chip =
		container_ದ(gp->chip, ರಚನೆ starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->ಸ್ಥಾನಾಂತರ;
	reg_addr &= ~(STARFIVE_GPIO_REG_ಸರಿಸು_ಮುಖವಾಡ);

	ಸರಿಸು_ಬಿಟ್ಟುಗಳು = (gp->ಸ್ಥಾನಾಂತರ & STARFIVE_GPIO_REG_ಸರಿಸು_ಮುಖವಾಡ)
		<< STARFIVE_GPIO_ಸರಿಸು_ಬಿಟ್ಟುಗಳು;
	ಬಿಟ್_ಮುಖವಾಡ_2 = STARFIVE_GPIO_ಮುಖವಾಡ << ಸರಿಸು_ಬಿಟ್ಟುಗಳು;

	val = readl((ಶೂನ್ಯ *)reg_addr);
	val &= ~ಬಿಟ್_ಮುಖವಾಡ_2;
	writel(val, (ಶೂನ್ಯ *)reg_addr);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ starfive_gpio_ಗಣ(ರಚನೆ gpio_pin *gp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	u32 val;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ reg_addr;
	u32 ಬಿಟ್_ಮುಖವಾಡ_2, ಸರಿಸು_ಬಿಟ್ಟುಗಳು;
	ರಚನೆ starfive_gpio_chip *chip =
		container_ದ(gp->chip, ರಚನೆ starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->ಸ್ಥಾನಾಂತರ;
	reg_addr &= ~(STARFIVE_GPIO_REG_ಸರಿಸು_ಮುಖವಾಡ);

	ಸರಿಸು_ಬಿಟ್ಟುಗಳು = (gp->ಸ್ಥಾನಾಂತರ & STARFIVE_GPIO_REG_ಸರಿಸು_ಮುಖವಾಡ)
		<< STARFIVE_GPIO_ಸರಿಸು_ಬಿಟ್ಟುಗಳು;
	ಬಿಟ್_ಮುಖವಾಡ_2 = STARFIVE_GPIO_ಮುಖವಾಡ << ಸರಿಸು_ಬಿಟ್ಟುಗಳು;
	/* set output value */
	val = readl((ಶೂನ್ಯ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~ಬಿಟ್_ಮುಖವಾಡ_2;
	val |= ಮೌಲ್ಯ << ಸರಿಸು_ಬಿಟ್ಟುಗಳು;
	writel(val, (ಶೂನ್ಯ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_gpio fdt_gpio_starfive;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ starfive_gpio_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			      ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(ಗಾತ್ರ(*chip));
	ಆದರೆ (!chip)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &addr, ಖಾಲಿ);
	ಆದರೆ (rc) {
		sbi_free(chip);
		ಹಿಂತಿರುಗಿ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.ಗಣ = starfive_gpio_ಗಣ;
	rc = gpio_chip_ಕೂಡಿಸು(&chip->chip);
	ಆದರೆ (rc) {
		sbi_free(chip);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ starfive_gpio_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = starfive_gpio_ಹೊಂದಿಕೆ,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
