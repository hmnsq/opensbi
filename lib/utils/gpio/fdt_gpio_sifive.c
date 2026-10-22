/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/gpio/fdt_gpio.h>

#تعریف_کړه SIFIVE_GPIO_PINS_MIN	1
#تعریف_کړه SIFIVE_GPIO_PINS_MAX	32
#تعریف_کړه SIFIVE_GPIO_PINS_DEF	16

#تعریف_کړه SIFIVE_GPIO_OUTEN	0x8
#تعریف_کړه SIFIVE_GPIO_OUTVAL	0xc
#تعریف_کړه SIFIVE_GPIO_بټ(b)	(1U << (b))

بنية sifive_gpio_chip {
	بینښه اوږد addr;
	بنية gpio_chip chip;
};

ساكن صحيح sifive_gpio_direction_output(بنية gpio_pin *gp, صحيح ارزښت)
{
	بینښه صحيح v;
	بنية sifive_gpio_chip *chip =
		container_د(gp->chip, بنية sifive_gpio_chip, chip);

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_بټ(gp->دننه_واټن);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	إذا (!ارزښت)
		v &= ~SIFIVE_GPIO_بټ(gp->دننه_واټن);
	وإلا
		v |= SIFIVE_GPIO_بټ(gp->دننه_واټن);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	أرجع 0;
}

ساكن فراغ sifive_gpio_سېټ(بنية gpio_pin *gp, صحيح ارزښت)
{
	بینښه صحيح v;
	بنية sifive_gpio_chip *chip =
		container_د(gp->chip, بنية sifive_gpio_chip, chip);

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	إذا (!ارزښت)
		v &= ~SIFIVE_GPIO_بټ(gp->دننه_واټن);
	وإلا
		v |= SIFIVE_GPIO_بټ(gp->دننه_واټن);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ثابت بنية fdt_gpio fdt_gpio_sifive;

ساكن صحيح sifive_gpio_init(ثابت فراغ *fdt, صحيح nodeoff,
			    ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بنية sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(حجم(*chip));
	إذا (!chip)
		أرجع SBI_ENOMEM;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.سېټ = sifive_gpio_سېټ;
	rc = gpio_chip_جمع_کړه(&chip->chip);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_سمون sifive_gpio_سمون[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.سمون_جدول = sifive_gpio_سمون,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
