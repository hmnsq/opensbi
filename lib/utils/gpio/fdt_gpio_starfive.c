/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/gpio/fdt_gpio.h>

#define STARFIVE_GPIO_PINS_DEF		64
#define STARFIVE_GPIO_OUTVAL		0x40
#define STARFIVE_GPIO_MASK		0xff
#define STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#define STARFIVE_GPIO_SHIFT_BITS	0x3

ସଂରଚନା starfive_gpio_chip {
	unsigned long addr;
	ସଂରଚନା gpio_chip chip;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା starfive_gpio_direction_output(ସଂରଚନା gpio_pin *gp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	ସଂରଚନା starfive_gpio_chip *chip =
		container_of(gp->chip, ସଂରଚନା starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((ଶୂନ୍ୟ *)reg_addr);
	val &= ~bit_mask;
	writel(val, (ଶୂନ୍ୟ *)reg_addr);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ starfive_gpio_set(ସଂରଚନା gpio_pin *gp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	ସଂରଚନା starfive_gpio_chip *chip =
		container_of(gp->chip, ସଂରଚନା starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((ଶୂନ୍ୟ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= ମୂଲ୍ୟ << shift_bits;
	writel(val, (ଶୂନ୍ୟ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ଅଚଳ ସଂରଚନା fdt_gpio fdt_gpio_starfive;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା starfive_gpio_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			      ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(ଆକାର(*chip));
	ଯଦି (!chip)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, ଖାଲି);
	ଯଦି (rc) {
		sbi_free(chip);
		ଫେରନ୍ତୁ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	ଯଦି (rc) {
		sbi_free(chip);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
