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

#تعریف_کن SIFIVE_GPIO_PINS_MIN	1
#تعریف_کن SIFIVE_GPIO_PINS_MAX	32
#تعریف_کن SIFIVE_GPIO_PINS_DEF	16

#تعریف_کن SIFIVE_GPIO_OUTEN	0x8
#تعریف_کن SIFIVE_GPIO_OUTVAL	0xc
#تعریف_کن SIFIVE_GPIO_بیت(b)	(1U << (b))

بنية sifive_gpio_chip {
	بیعلامت دراز addr;
	بنية gpio_chip chip;
};

ساكن صحيح sifive_gpio_direction_output(بنية gpio_pin *gp, صحيح مقدار)
{
	بیعلامت صحيح v;
	بنية sifive_gpio_chip *chip =
		container_از(gp->chip, بنية sifive_gpio_chip, chip);

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_بیت(gp->جابه_جایی);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	إذا (!مقدار)
		v &= ~SIFIVE_GPIO_بیت(gp->جابه_جایی);
	وإلا
		v |= SIFIVE_GPIO_بیت(gp->جابه_جایی);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	أرجع 0;
}

ساكن فراغ sifive_gpio_مجموعه(بنية gpio_pin *gp, صحيح مقدار)
{
	بیعلامت صحيح v;
	بنية sifive_gpio_chip *chip =
		container_از(gp->chip, بنية sifive_gpio_chip, chip);

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	إذا (!مقدار)
		v &= ~SIFIVE_GPIO_بیت(gp->جابه_جایی);
	وإلا
		v |= SIFIVE_GPIO_بیت(gp->جابه_جایی);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ثابت بنية fdt_gpio fdt_gpio_sifive;

ساكن صحيح sifive_gpio_init(ثابت فراغ *fdt, صحيح nodeoff,
			    ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بنية sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(حجم(*chip));
	إذا (!chip)
		أرجع SBI_ENOMEM;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.مجموعه = sifive_gpio_مجموعه;
	rc = gpio_chip_جمع_کن(&chip->chip);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_تطابق sifive_gpio_تطابق[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.تطابق_جدول = sifive_gpio_تطابق,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
