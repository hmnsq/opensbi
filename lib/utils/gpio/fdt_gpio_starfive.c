/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/gpio/fdt_gpio.h>

#تعریف_کرو STARFIVE_GPIO_PINS_DEF		64
#تعریف_کرو STARFIVE_GPIO_OUTVAL		0x40
#تعریف_کرو STARFIVE_GPIO_نقاب		0xff
#تعریف_کرو STARFIVE_GPIO_REG_کھسکاؤ_نقاب	0x3
#تعریف_کرو STARFIVE_GPIO_کھسکاؤ_بٹ	0x3

بنية starfive_gpio_chip {
	بنانشانی لما addr;
	بنية gpio_chip chip;
};

ساكن صحيح starfive_gpio_direction_output(بنية gpio_pin *gp, صحيح قدر)
{
	u32 val;
	بنانشانی لما reg_addr;
	u32 ثنائی_ہندسہ_نقاب_2, کھسکاؤ_بٹ;
	بنية starfive_gpio_chip *chip =
		container_دا(gp->chip, بنية starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->وتھ;
	reg_addr &= ~(STARFIVE_GPIO_REG_کھسکاؤ_نقاب);

	کھسکاؤ_بٹ = (gp->وتھ & STARFIVE_GPIO_REG_کھسکاؤ_نقاب)
		<< STARFIVE_GPIO_کھسکاؤ_بٹ;
	ثنائی_ہندسہ_نقاب_2 = STARFIVE_GPIO_نقاب << کھسکاؤ_بٹ;

	val = readl((فراغ *)reg_addr);
	val &= ~ثنائی_ہندسہ_نقاب_2;
	writel(val, (فراغ *)reg_addr);

	أرجع 0;
}

ساكن فراغ starfive_gpio_سیٹ(بنية gpio_pin *gp, صحيح قدر)
{
	u32 val;
	بنانشانی لما reg_addr;
	u32 ثنائی_ہندسہ_نقاب_2, کھسکاؤ_بٹ;
	بنية starfive_gpio_chip *chip =
		container_دا(gp->chip, بنية starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->وتھ;
	reg_addr &= ~(STARFIVE_GPIO_REG_کھسکاؤ_نقاب);

	کھسکاؤ_بٹ = (gp->وتھ & STARFIVE_GPIO_REG_کھسکاؤ_نقاب)
		<< STARFIVE_GPIO_کھسکاؤ_بٹ;
	ثنائی_ہندسہ_نقاب_2 = STARFIVE_GPIO_نقاب << کھسکاؤ_بٹ;
	/* set output value */
	val = readl((فراغ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~ثنائی_ہندسہ_نقاب_2;
	val |= قدر << کھسکاؤ_بٹ;
	writel(val, (فراغ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ثابت بنية fdt_gpio fdt_gpio_starfive;

ساكن صحيح starfive_gpio_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_میل *میل)
{
	صحيح rc;
	بنية starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(حجم(*chip));
	إذا (!chip)
		أرجع SBI_ENOMEM;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.سیٹ = starfive_gpio_سیٹ;
	rc = gpio_chip_جوڑو(&chip->chip);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_میل starfive_gpio_میل[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.میل_جدول = starfive_gpio_میل,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
