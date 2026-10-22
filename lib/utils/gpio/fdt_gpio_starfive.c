/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/gpio/fdt_gpio.h>

#परिभाषय STARFIVE_GPIO_PINS_DEF		64
#परिभाषय STARFIVE_GPIO_OUTVAL		0x40
#परिभाषय STARFIVE_GPIO_आवरणम्		0xff
#परिभाषय STARFIVE_GPIO_REG_सरणम्_आवरणम्	0x3
#परिभाषय STARFIVE_GPIO_सरणम्_द्वयङ्काः	0x3

संरचना starfive_gpio_chip {
	अचिह्नित दीर्घ addr;
	संरचना gpio_chip chip;
};

स्थिर पूर्णाङ्क starfive_gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	u32 val;
	अचिह्नित दीर्घ reg_addr;
	u32 bit_आवरणम्, सरणम्_द्वयङ्काः;
	संरचना starfive_gpio_chip *chip =
		container_सम्बन्धि(gp->chip, संरचना starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->विस्थापनम्;
	reg_addr &= ~(STARFIVE_GPIO_REG_सरणम्_आवरणम्);

	सरणम्_द्वयङ्काः = (gp->विस्थापनम् & STARFIVE_GPIO_REG_सरणम्_आवरणम्)
		<< STARFIVE_GPIO_सरणम्_द्वयङ्काः;
	bit_आवरणम् = STARFIVE_GPIO_आवरणम् << सरणम्_द्वयङ्काः;

	val = readl((शून्य *)reg_addr);
	val &= ~bit_आवरणम्;
	writel(val, (शून्य *)reg_addr);

	प्रत्यागम 0;
}

स्थिर शून्य starfive_gpio_समुच्चयः(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	u32 val;
	अचिह्नित दीर्घ reg_addr;
	u32 bit_आवरणम्, सरणम्_द्वयङ्काः;
	संरचना starfive_gpio_chip *chip =
		container_सम्बन्धि(gp->chip, संरचना starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->विस्थापनम्;
	reg_addr &= ~(STARFIVE_GPIO_REG_सरणम्_आवरणम्);

	सरणम्_द्वयङ्काः = (gp->विस्थापनम् & STARFIVE_GPIO_REG_सरणम्_आवरणम्)
		<< STARFIVE_GPIO_सरणम्_द्वयङ्काः;
	bit_आवरणम् = STARFIVE_GPIO_आवरणम् << सरणम्_द्वयङ्काः;
	/* set output value */
	val = readl((शून्य *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_आवरणम्;
	val |= मानम् << सरणम्_द्वयङ्काः;
	writel(val, (शून्य *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ध्रुव संरचना fdt_gpio fdt_gpio_starfive;

स्थिर पूर्णाङ्क starfive_gpio_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(परिमाण(*chip));
	यदि (!chip)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc) {
		sbi_free(chip);
		प्रत्यागम rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.समुच्चयः = starfive_gpio_समुच्चयः;
	rc = gpio_chip_सङ्कलय(&chip->chip);
	यदि (rc) {
		sbi_free(chip);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् starfive_gpio_मेलनम्[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ध्रुव संरचना fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.मेलनम्_सारणी = starfive_gpio_मेलनम्,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
