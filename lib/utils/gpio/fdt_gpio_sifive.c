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

#وصف_ڪريو SIFIVE_GPIO_PINS_MIN	1
#وصف_ڪريو SIFIVE_GPIO_PINS_MAX	32
#وصف_ڪريو SIFIVE_GPIO_PINS_DEF	16

#وصف_ڪريو SIFIVE_GPIO_OUTEN	0x8
#وصف_ڪريو SIFIVE_GPIO_OUTVAL	0xc
#وصف_ڪريو SIFIVE_GPIO_ٻٽو_انگ(b)	(1U << (b))

بنية sifive_gpio_chip {
	بنانشان ڊگهو addr;
	بنية gpio_chip chip;
};

ساكن صحيح sifive_gpio_direction_output(بنية gpio_pin *gp, صحيح قدر)
{
	بنانشان صحيح v;
	بنية sifive_gpio_chip *chip =
		container_جو(gp->chip, بنية sifive_gpio_chip, chip);

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_ٻٽو_انگ(gp->وٿي);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	إذا (!قدر)
		v &= ~SIFIVE_GPIO_ٻٽو_انگ(gp->وٿي);
	وإلا
		v |= SIFIVE_GPIO_ٻٽو_انگ(gp->وٿي);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	أرجع 0;
}

ساكن فراغ sifive_gpio_سيٽ(بنية gpio_pin *gp, صحيح قدر)
{
	بنانشان صحيح v;
	بنية sifive_gpio_chip *chip =
		container_جو(gp->chip, بنية sifive_gpio_chip, chip);

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	إذا (!قدر)
		v &= ~SIFIVE_GPIO_ٻٽو_انگ(gp->وٿي);
	وإلا
		v |= SIFIVE_GPIO_ٻٽو_انگ(gp->وٿي);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ثابت بنية fdt_gpio fdt_gpio_sifive;

ساكن صحيح sifive_gpio_init(ثابت فراغ *fdt, صحيح nodeoff,
			    ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنية sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(حجم(*chip));
	إذا (!chip)
		أرجع SBI_ENOMEM;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.سيٽ = sifive_gpio_سيٽ;
	rc = gpio_chip_جوڙيو(&chip->chip);
	إذا (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ sifive_gpio_ميلاپ[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.ميلاپ_جدول = sifive_gpio_ميلاپ,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
