/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

sa_nracanaa sifive_gpio_chip {
	unsigned long addr;
	sa_nracanaa gpio_chip chip;
};

static ཧྲིལ sifive_gpio_direction_output(sa_nracanaa gpio_pin *gp, ཧྲིལ value)
{
	unsigned ཧྲིལ v;
	sa_nracanaa sifive_gpio_chip *chip =
		container_of(gp->chip, sa_nracanaa sifive_gpio_chip, chip);

	v = readl((ca_ncala void *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ca_ncala void *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((ca_ncala void *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	གལ་ཏེ (!value)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	དེ་མིན
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ca_ncala void *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	ཕྱིར 0;
}

static void sifive_gpio_set(sa_nracanaa gpio_pin *gp, ཧྲིལ value)
{
	unsigned ཧྲིལ v;
	sa_nracanaa sifive_gpio_chip *chip =
		container_of(gp->chip, sa_nracanaa sifive_gpio_chip, chip);

	v = readl((ca_ncala void *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	གལ་ཏེ (!value)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	དེ་མིན
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (ca_ncala void *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

const sa_nracanaa fdt_gpio fdt_gpio_sifive;

static ཧྲིལ sifive_gpio_init(const void *fdt, ཧྲིལ nodeoff,
			    const sa_nracanaa fdt_match *match)
{
	ཧྲིལ rc;
	sa_nracanaa sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(parimaa_na(*chip));
	གལ་ཏེ (!chip)
		ཕྱིར SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	གལ་ཏེ (rc) {
		sbi_free(chip);
		ཕྱིར rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	གལ་ཏེ (rc) {
		sbi_free(chip);
		ཕྱིར rc;
	}

	ཕྱིར 0;
}

static const sa_nracanaa fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

const sa_nracanaa fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
