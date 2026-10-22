/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

ساختار sifive_gpio_chip {
	unsigned long addr;
	ساختار gpio_chip chip;
};

ایستا صحیح sifive_gpio_direction_output(ساختار gpio_pin *gp, صحیح مقدار)
{
	unsigned صحیح v;
	ساختار sifive_gpio_chip *chip =
		container_of(gp->chip, ساختار sifive_gpio_chip, chip);

	v = readl((فرار تهی *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (فرار تهی *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((فرار تهی *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	اگر (!مقدار)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	وگرنه
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (فرار تهی *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	بازگشت 0;
}

ایستا تهی sifive_gpio_set(ساختار gpio_pin *gp, صحیح مقدار)
{
	unsigned صحیح v;
	ساختار sifive_gpio_chip *chip =
		container_of(gp->chip, ساختار sifive_gpio_chip, chip);

	v = readl((فرار تهی *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	اگر (!مقدار)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	وگرنه
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (فرار تهی *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ثابت ساختار fdt_gpio fdt_gpio_sifive;

ایستا صحیح sifive_gpio_init(ثابت تهی *fdt, صحیح nodeoff,
			    ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(اندازه(*chip));
	اگر (!chip)
		بازگشت SBI_ENOMEM;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &addr, پوچ);
	اگر (rc) {
		sbi_free(chip);
		بازگشت rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	اگر (rc) {
		sbi_free(chip);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ثابت ساختار fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
