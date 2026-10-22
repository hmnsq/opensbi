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

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

بنية sifive_gpio_chip {
	unsigned long addr;
	بنية gpio_chip chip;
};

ساكن صحيح sifive_gpio_direction_output(بنية gpio_pin *gp, صحيح value)
{
	unsigned صحيح v;
	بنية sifive_gpio_chip *chip =
		container_of(gp->chip, بنية sifive_gpio_chip, chip);

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	𐡄𐡍 (!value)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	وإلا
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	أرجع 0;
}

ساكن فراغ sifive_gpio_set(بنية gpio_pin *gp, صحيح value)
{
	unsigned صحيح v;
	بنية sifive_gpio_chip *chip =
		container_of(gp->chip, بنية sifive_gpio_chip, chip);

	v = readl((متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	𐡄𐡍 (!value)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	وإلا
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (متطاير فراغ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ثابت بنية fdt_gpio fdt_gpio_sifive;

ساكن صحيح sifive_gpio_init(ثابت فراغ *fdt, صحيح nodeoff,
			    ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(حجم(*chip));
	𐡄𐡍 (!chip)
		أرجع SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, عدم);
	𐡄𐡍 (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	𐡄𐡍 (rc) {
		sbi_free(chip);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
