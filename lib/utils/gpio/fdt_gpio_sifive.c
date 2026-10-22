/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/gpio/fdt_gpio.h>

#నిర్వచించు SIFIVE_GPIO_PINS_MIN	1
#నిర్వచించు SIFIVE_GPIO_PINS_MAX	32
#నిర్వచించు SIFIVE_GPIO_PINS_DEF	16

#నిర్వచించు SIFIVE_GPIO_OUTEN	0x8
#నిర్వచించు SIFIVE_GPIO_OUTVAL	0xc
#నిర్వచించు SIFIVE_GPIO_బిట్టు(b)	(1U << (b))

నిర్మాణం sifive_gpio_chip {
	గుర్తులేని దీర్ఘ addr;
	నిర్మాణం gpio_chip chip;
};

స్థిర పూర్ణాంకం sifive_gpio_direction_output(నిర్మాణం gpio_pin *gp, పూర్ణాంకం విలువ)
{
	గుర్తులేని పూర్ణాంకం v;
	నిర్మాణం sifive_gpio_chip *chip =
		container_యొక్క(gp->chip, నిర్మాణం sifive_gpio_chip, chip);

	v = readl((అస్థిర శూన్య *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	writel(v, (అస్థిర శూన్య *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((అస్థిర శూన్య *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ఒకవేళ (!విలువ)
		v &= ~SIFIVE_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	లేకపోతే
		v |= SIFIVE_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	writel(v, (అస్థిర శూన్య *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	తిరిగి 0;
}

స్థిర శూన్య sifive_gpio_సమితి(నిర్మాణం gpio_pin *gp, పూర్ణాంకం విలువ)
{
	గుర్తులేని పూర్ణాంకం v;
	నిర్మాణం sifive_gpio_chip *chip =
		container_యొక్క(gp->chip, నిర్మాణం sifive_gpio_chip, chip);

	v = readl((అస్థిర శూన్య *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ఒకవేళ (!విలువ)
		v &= ~SIFIVE_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	లేకపోతే
		v |= SIFIVE_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	writel(v, (అస్థిర శూన్య *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

స్థిరరాశి నిర్మాణం fdt_gpio fdt_gpio_sifive;

స్థిర పూర్ణాంకం sifive_gpio_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			    స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(పరిమాణం(*chip));
	ఒకవేళ (!chip)
		తిరిగి SBI_ENOMEM;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, ఖాళీ);
	ఒకవేళ (rc) {
		sbi_free(chip);
		తిరిగి rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.సమితి = sifive_gpio_సమితి;
	rc = gpio_chip_కూడు(&chip->chip);
	ఒకవేళ (rc) {
		sbi_free(chip);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sifive_gpio_సరిపోలిక[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.సరిపోలిక_పట్టిక = sifive_gpio_సరిపోలిక,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
