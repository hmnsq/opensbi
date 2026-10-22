/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/gpio/fdt_gpio.h>

#నిర్వచించు STARFIVE_GPIO_PINS_DEF		64
#నిర్వచించు STARFIVE_GPIO_OUTVAL		0x40
#నిర్వచించు STARFIVE_GPIO_ముసుగు		0xff
#నిర్వచించు STARFIVE_GPIO_REG_జరుపు_ముసుగు	0x3
#నిర్వచించు STARFIVE_GPIO_జరుపు_బిట్లు	0x3

నిర్మాణం starfive_gpio_chip {
	గుర్తులేని దీర్ఘ addr;
	నిర్మాణం gpio_chip chip;
};

స్థిర పూర్ణాంకం starfive_gpio_direction_output(నిర్మాణం gpio_pin *gp, పూర్ణాంకం విలువ)
{
	u32 val;
	గుర్తులేని దీర్ఘ reg_addr;
	u32 బిట్టు_ముసుగు_2, జరుపు_బిట్లు;
	నిర్మాణం starfive_gpio_chip *chip =
		container_యొక్క(gp->chip, నిర్మాణం starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->స్థానభ్రంశం;
	reg_addr &= ~(STARFIVE_GPIO_REG_జరుపు_ముసుగు);

	జరుపు_బిట్లు = (gp->స్థానభ్రంశం & STARFIVE_GPIO_REG_జరుపు_ముసుగు)
		<< STARFIVE_GPIO_జరుపు_బిట్లు;
	బిట్టు_ముసుగు_2 = STARFIVE_GPIO_ముసుగు << జరుపు_బిట్లు;

	val = readl((శూన్య *)reg_addr);
	val &= ~బిట్టు_ముసుగు_2;
	writel(val, (శూన్య *)reg_addr);

	తిరిగి 0;
}

స్థిర శూన్య starfive_gpio_సమితి(నిర్మాణం gpio_pin *gp, పూర్ణాంకం విలువ)
{
	u32 val;
	గుర్తులేని దీర్ఘ reg_addr;
	u32 బిట్టు_ముసుగు_2, జరుపు_బిట్లు;
	నిర్మాణం starfive_gpio_chip *chip =
		container_యొక్క(gp->chip, నిర్మాణం starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->స్థానభ్రంశం;
	reg_addr &= ~(STARFIVE_GPIO_REG_జరుపు_ముసుగు);

	జరుపు_బిట్లు = (gp->స్థానభ్రంశం & STARFIVE_GPIO_REG_జరుపు_ముసుగు)
		<< STARFIVE_GPIO_జరుపు_బిట్లు;
	బిట్టు_ముసుగు_2 = STARFIVE_GPIO_ముసుగు << జరుపు_బిట్లు;
	/* set output value */
	val = readl((శూన్య *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~బిట్టు_ముసుగు_2;
	val |= విలువ << జరుపు_బిట్లు;
	writel(val, (శూన్య *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

స్థిరరాశి నిర్మాణం fdt_gpio fdt_gpio_starfive;

స్థిర పూర్ణాంకం starfive_gpio_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			      స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(పరిమాణం(*chip));
	ఒకవేళ (!chip)
		తిరిగి SBI_ENOMEM;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, ఖాళీ);
	ఒకవేళ (rc) {
		sbi_free(chip);
		తిరిగి rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.సమితి = starfive_gpio_సమితి;
	rc = gpio_chip_కూడు(&chip->chip);
	ఒకవేళ (rc) {
		sbi_free(chip);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక starfive_gpio_సరిపోలిక[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.సరిపోలిక_పట్టిక = starfive_gpio_సరిపోలిక,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
