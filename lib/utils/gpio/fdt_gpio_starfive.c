/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/gpio/fdt_gpio.h>

#define STARFIVE_GPIO_PINS_DEF		64
#define STARFIVE_GPIO_OUTVAL		0x40
#define STARFIVE_GPIO_MASK		0xff
#define STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#define STARFIVE_GPIO_SHIFT_BITS	0x3

கட்டமைப்பு starfive_gpio_chip {
	unsigned long addr;
	கட்டமைப்பு gpio_chip chip;
};

நிலையான முழு starfive_gpio_direction_output(கட்டமைப்பு gpio_pin *gp, முழு மதிப்பு)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	கட்டமைப்பு starfive_gpio_chip *chip =
		container_of(gp->chip, கட்டமைப்பு starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((வெறுமை *)reg_addr);
	val &= ~bit_mask;
	writel(val, (வெறுமை *)reg_addr);

	திரும்பு 0;
}

நிலையான வெறுமை starfive_gpio_set(கட்டமைப்பு gpio_pin *gp, முழு மதிப்பு)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	கட்டமைப்பு starfive_gpio_chip *chip =
		container_of(gp->chip, கட்டமைப்பு starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((வெறுமை *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= மதிப்பு << shift_bits;
	writel(val, (வெறுமை *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

மாறிலி கட்டமைப்பு fdt_gpio fdt_gpio_starfive;

நிலையான முழு starfive_gpio_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			      மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(அளவு(*chip));
	எனில் (!chip)
		திரும்பு SBI_ENOMEM;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, வெற்று);
	எனில் (rc) {
		sbi_free(chip);
		திரும்பு rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	எனில் (rc) {
		sbi_free(chip);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
