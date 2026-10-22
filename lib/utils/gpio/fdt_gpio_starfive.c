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

#وصف_ڪريو STARFIVE_GPIO_PINS_DEF		64
#وصف_ڪريو STARFIVE_GPIO_OUTVAL		0x40
#وصف_ڪريو STARFIVE_GPIO_نقاب		0xff
#وصف_ڪريو STARFIVE_GPIO_REG_سرڪايو_نقاب	0x3
#وصف_ڪريو STARFIVE_GPIO_سرڪايو_بٽ	0x3

بنية starfive_gpio_chip {
	بنانشان ڊگهو addr;
	بنية gpio_chip chip;
};

ساكن صحيح starfive_gpio_direction_output(بنية gpio_pin *gp, صحيح قدر)
{
	u32 val;
	بنانشان ڊگهو reg_addr;
	u32 ٻٽو_انگ_نقاب_2, سرڪايو_بٽ;
	بنية starfive_gpio_chip *chip =
		container_جو(gp->chip, بنية starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->وٿي;
	reg_addr &= ~(STARFIVE_GPIO_REG_سرڪايو_نقاب);

	سرڪايو_بٽ = (gp->وٿي & STARFIVE_GPIO_REG_سرڪايو_نقاب)
		<< STARFIVE_GPIO_سرڪايو_بٽ;
	ٻٽو_انگ_نقاب_2 = STARFIVE_GPIO_نقاب << سرڪايو_بٽ;

	val = readl((فراغ *)reg_addr);
	val &= ~ٻٽو_انگ_نقاب_2;
	writel(val, (فراغ *)reg_addr);

	أرجع 0;
}

ساكن فراغ starfive_gpio_سيٽ(بنية gpio_pin *gp, صحيح قدر)
{
	u32 val;
	بنانشان ڊگهو reg_addr;
	u32 ٻٽو_انگ_نقاب_2, سرڪايو_بٽ;
	بنية starfive_gpio_chip *chip =
		container_جو(gp->chip, بنية starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->وٿي;
	reg_addr &= ~(STARFIVE_GPIO_REG_سرڪايو_نقاب);

	سرڪايو_بٽ = (gp->وٿي & STARFIVE_GPIO_REG_سرڪايو_نقاب)
		<< STARFIVE_GPIO_سرڪايو_بٽ;
	ٻٽو_انگ_نقاب_2 = STARFIVE_GPIO_نقاب << سرڪايو_بٽ;
	/* set output value */
	val = readl((فراغ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~ٻٽو_انگ_نقاب_2;
	val |= قدر << سرڪايو_بٽ;
	writel(val, (فراغ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ثابت بنية fdt_gpio fdt_gpio_starfive;

ساكن صحيح starfive_gpio_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنية starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(حجم(*chip));
	إذا (!chip)
		أرجع SBI_ENOMEM;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.سيٽ = starfive_gpio_سيٽ;
	rc = gpio_chip_جوڙيو(&chip->chip);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ starfive_gpio_ميلاپ[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.ميلاپ_جدول = starfive_gpio_ميلاپ,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
