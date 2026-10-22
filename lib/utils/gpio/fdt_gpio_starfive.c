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

#تعریف_کړه STARFIVE_GPIO_PINS_DEF		64
#تعریف_کړه STARFIVE_GPIO_OUTVAL		0x40
#تعریف_کړه STARFIVE_GPIO_ماسک		0xff
#تعریف_کړه STARFIVE_GPIO_REG_ښويدنه_ماسک	0x3
#تعریف_کړه STARFIVE_GPIO_ښويدنه_بټونه	0x3

بنية starfive_gpio_chip {
	بینښه اوږد addr;
	بنية gpio_chip chip;
};

ساكن صحيح starfive_gpio_direction_output(بنية gpio_pin *gp, صحيح ارزښت)
{
	u32 val;
	بینښه اوږد reg_addr;
	u32 بټ_ماسک_2, ښويدنه_بټونه;
	بنية starfive_gpio_chip *chip =
		container_د(gp->chip, بنية starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->دننه_واټن;
	reg_addr &= ~(STARFIVE_GPIO_REG_ښويدنه_ماسک);

	ښويدنه_بټونه = (gp->دننه_واټن & STARFIVE_GPIO_REG_ښويدنه_ماسک)
		<< STARFIVE_GPIO_ښويدنه_بټونه;
	بټ_ماسک_2 = STARFIVE_GPIO_ماسک << ښويدنه_بټونه;

	val = readl((فراغ *)reg_addr);
	val &= ~بټ_ماسک_2;
	writel(val, (فراغ *)reg_addr);

	أرجع 0;
}

ساكن فراغ starfive_gpio_سېټ(بنية gpio_pin *gp, صحيح ارزښت)
{
	u32 val;
	بینښه اوږد reg_addr;
	u32 بټ_ماسک_2, ښويدنه_بټونه;
	بنية starfive_gpio_chip *chip =
		container_د(gp->chip, بنية starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->دننه_واټن;
	reg_addr &= ~(STARFIVE_GPIO_REG_ښويدنه_ماسک);

	ښويدنه_بټونه = (gp->دننه_واټن & STARFIVE_GPIO_REG_ښويدنه_ماسک)
		<< STARFIVE_GPIO_ښويدنه_بټونه;
	بټ_ماسک_2 = STARFIVE_GPIO_ماسک << ښويدنه_بټونه;
	/* set output value */
	val = readl((فراغ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~بټ_ماسک_2;
	val |= ارزښت << ښويدنه_بټونه;
	writel(val, (فراغ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ثابت بنية fdt_gpio fdt_gpio_starfive;

ساكن صحيح starfive_gpio_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بنية starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(حجم(*chip));
	إذا (!chip)
		أرجع SBI_ENOMEM;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.سېټ = starfive_gpio_سېټ;
	rc = gpio_chip_جمع_کړه(&chip->chip);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_سمون starfive_gpio_سمون[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.سمون_جدول = starfive_gpio_سمون,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
