/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/gpio/fdt_gpio.h>

#define STARFIVE_GPIO_PINS_DEF		64
#define STARFIVE_GPIO_OUTVAL		0x40
#define STARFIVE_GPIO_MASK		0xff
#define STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#define STARFIVE_GPIO_SHIFT_BITS	0x3

sa_nracanaa starfive_gpio_chip {
	unsigned long addr;
	sa_nracanaa gpio_chip chip;
};

static පූර්ණ_සංඛ්‍යා starfive_gpio_direction_output(sa_nracanaa gpio_pin *gp, පූර්ණ_සංඛ්‍යා value)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	sa_nracanaa starfive_gpio_chip *chip =
		container_of(gp->chip, sa_nracanaa starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((void *)reg_addr);
	val &= ~bit_mask;
	writel(val, (void *)reg_addr);

	ආපසු 0;
}

static void starfive_gpio_set(sa_nracanaa gpio_pin *gp, පූර්ණ_සංඛ්‍යා value)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	sa_nracanaa starfive_gpio_chip *chip =
		container_of(gp->chip, sa_nracanaa starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((void *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= value << shift_bits;
	writel(val, (void *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

const sa_nracanaa fdt_gpio fdt_gpio_starfive;

static පූර්ණ_සංඛ්‍යා starfive_gpio_init(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
			      const sa_nracanaa fdt_match *match)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(parimaa_na(*chip));
	නම් (!chip)
		ආපසු SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	නම් (rc) {
		sbi_free(chip);
		ආපසු rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	නම් (rc) {
		sbi_free(chip);
		ආපසු rc;
	}

	ආපසු 0;
}

static const sa_nracanaa fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

const sa_nracanaa fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
