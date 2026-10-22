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

#تعریف_کن STARFIVE_GPIO_PINS_DEF		64
#تعریف_کن STARFIVE_GPIO_OUTVAL		0x40
#تعریف_کن STARFIVE_GPIO_ماسک		0xff
#تعریف_کن STARFIVE_GPIO_REG_لغزش_ماسک	0x3
#تعریف_کن STARFIVE_GPIO_لغزش_بیتها	0x3

بنية starfive_gpio_chip {
	بیعلامت دراز addr;
	بنية gpio_chip chip;
};

ساكن صحيح starfive_gpio_direction_output(بنية gpio_pin *gp, صحيح مقدار)
{
	u32 val;
	بیعلامت دراز reg_addr;
	u32 بیت_ماسک_2, لغزش_بیتها;
	بنية starfive_gpio_chip *chip =
		container_از(gp->chip, بنية starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->جابه_جایی;
	reg_addr &= ~(STARFIVE_GPIO_REG_لغزش_ماسک);

	لغزش_بیتها = (gp->جابه_جایی & STARFIVE_GPIO_REG_لغزش_ماسک)
		<< STARFIVE_GPIO_لغزش_بیتها;
	بیت_ماسک_2 = STARFIVE_GPIO_ماسک << لغزش_بیتها;

	val = readl((فراغ *)reg_addr);
	val &= ~بیت_ماسک_2;
	writel(val, (فراغ *)reg_addr);

	أرجع 0;
}

ساكن فراغ starfive_gpio_مجموعه(بنية gpio_pin *gp, صحيح مقدار)
{
	u32 val;
	بیعلامت دراز reg_addr;
	u32 بیت_ماسک_2, لغزش_بیتها;
	بنية starfive_gpio_chip *chip =
		container_از(gp->chip, بنية starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->جابه_جایی;
	reg_addr &= ~(STARFIVE_GPIO_REG_لغزش_ماسک);

	لغزش_بیتها = (gp->جابه_جایی & STARFIVE_GPIO_REG_لغزش_ماسک)
		<< STARFIVE_GPIO_لغزش_بیتها;
	بیت_ماسک_2 = STARFIVE_GPIO_ماسک << لغزش_بیتها;
	/* set output value */
	val = readl((فراغ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~بیت_ماسک_2;
	val |= مقدار << لغزش_بیتها;
	writel(val, (فراغ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ثابت بنية fdt_gpio fdt_gpio_starfive;

ساكن صحيح starfive_gpio_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بنية starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(حجم(*chip));
	إذا (!chip)
		أرجع SBI_ENOMEM;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.مجموعه = starfive_gpio_مجموعه;
	rc = gpio_chip_جمع_کن(&chip->chip);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_تطابق starfive_gpio_تطابق[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.تطابق_جدول = starfive_gpio_تطابق,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
