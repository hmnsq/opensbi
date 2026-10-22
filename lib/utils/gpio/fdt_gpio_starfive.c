/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/gpio/fdt_gpio.h>

#تعریف_کرو STARFIVE_GPIO_PINS_DEF		64
#تعریف_کرو STARFIVE_GPIO_OUTVAL		0x40
#تعریف_کرو STARFIVE_GPIO_نقاب		0xff
#تعریف_کرو STARFIVE_GPIO_REG_سرکاؤ_نقاب	0x3
#تعریف_کرو STARFIVE_GPIO_سرکاؤ_بٹ	0x3

ساخت starfive_gpio_chip {
	غیرعلامتی طویل addr;
	ساخت gpio_chip chip;
};

ساکن صحیح starfive_gpio_direction_output(ساخت gpio_pin *gp, صحیح قدر)
{
	u32 val;
	غیرعلامتی طویل reg_addr;
	u32 ثنائی_ہندسہ_نقاب_2, سرکاؤ_بٹ;
	ساخت starfive_gpio_chip *chip =
		container_کا(gp->chip, ساخت starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->انحراف;
	reg_addr &= ~(STARFIVE_GPIO_REG_سرکاؤ_نقاب);

	سرکاؤ_بٹ = (gp->انحراف & STARFIVE_GPIO_REG_سرکاؤ_نقاب)
		<< STARFIVE_GPIO_سرکاؤ_بٹ;
	ثنائی_ہندسہ_نقاب_2 = STARFIVE_GPIO_نقاب << سرکاؤ_بٹ;

	val = readl((خالی *)reg_addr);
	val &= ~ثنائی_ہندسہ_نقاب_2;
	writel(val, (خالی *)reg_addr);

	واپس 0;
}

ساکن خالی starfive_gpio_سیٹ(ساخت gpio_pin *gp, صحیح قدر)
{
	u32 val;
	غیرعلامتی طویل reg_addr;
	u32 ثنائی_ہندسہ_نقاب_2, سرکاؤ_بٹ;
	ساخت starfive_gpio_chip *chip =
		container_کا(gp->chip, ساخت starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->انحراف;
	reg_addr &= ~(STARFIVE_GPIO_REG_سرکاؤ_نقاب);

	سرکاؤ_بٹ = (gp->انحراف & STARFIVE_GPIO_REG_سرکاؤ_نقاب)
		<< STARFIVE_GPIO_سرکاؤ_بٹ;
	ثنائی_ہندسہ_نقاب_2 = STARFIVE_GPIO_نقاب << سرکاؤ_بٹ;
	/* set output value */
	val = readl((خالی *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~ثنائی_ہندسہ_نقاب_2;
	val |= قدر << سرکاؤ_بٹ;
	writel(val, (خالی *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ثابت ساخت fdt_gpio fdt_gpio_starfive;

ساکن صحیح starfive_gpio_init(ثابت خالی *fdt, صحیح nodeoff,
			      ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(حجم(*chip));
	اگر (!chip)
		واپس SBI_ENOMEM;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, تہی);
	اگر (rc) {
		sbi_free(chip);
		واپس rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.سیٹ = starfive_gpio_سیٹ;
	rc = gpio_chip_جوڑو(&chip->chip);
	اگر (rc) {
		sbi_free(chip);
		واپس rc;
	}

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت starfive_gpio_مطابقت[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ثابت ساخت fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.مطابقت_جدول = starfive_gpio_مطابقت,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
