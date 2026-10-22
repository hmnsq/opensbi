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

struct sifive_gpio_chip {
	unsigned long addr;
	struct gpio_chip chip;
};

static 정수 sifive_gpio_direction_output(struct gpio_pin *gp, 정수 value)
{
	unsigned 정수 v;
	struct sifive_gpio_chip *chip =
		container_of(gp->chip, struct sifive_gpio_chip, chip);

	v = readl((volatile void *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (volatile void *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((volatile void *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	만약 (!value)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	아니면
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (volatile void *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	반환 0;
}

static void sifive_gpio_set(struct gpio_pin *gp, 정수 value)
{
	unsigned 정수 v;
	struct sifive_gpio_chip *chip =
		container_of(gp->chip, struct sifive_gpio_chip, chip);

	v = readl((volatile void *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	만약 (!value)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	아니면
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (volatile void *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

const struct fdt_gpio fdt_gpio_sifive;

static 정수 sifive_gpio_init(const void *fdt, 정수 nodeoff,
			    const struct fdt_match *match)
{
	정수 rc;
	struct sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(sizeof(*chip));
	만약 (!chip)
		반환 SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	만약 (rc) {
		sbi_free(chip);
		반환 rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	만약 (rc) {
		sbi_free(chip);
		반환 rc;
	}

	반환 0;
}

static const struct fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

const struct fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
