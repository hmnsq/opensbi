/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/gpio/fdt_gpio.h>

#define STARFIVE_GPIO_PINS_DEF		64
#define STARFIVE_GPIO_OUTVAL		0x40
#define STARFIVE_GPIO_MASK		0xff
#define STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#define STARFIVE_GPIO_SHIFT_BITS	0x3

ഘടന starfive_gpio_chip {
	unsigned long addr;
	ഘടന gpio_chip chip;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ starfive_gpio_direction_output(ഘടന gpio_pin *gp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	ഘടന starfive_gpio_chip *chip =
		container_of(gp->chip, ഘടന starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((ശൂന്യം *)reg_addr);
	val &= ~bit_mask;
	writel(val, (ശൂന്യം *)reg_addr);

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം starfive_gpio_set(ഘടന gpio_pin *gp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	ഘടന starfive_gpio_chip *chip =
		container_of(gp->chip, ഘടന starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((ശൂന്യം *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= മൂല്യം << shift_bits;
	writel(val, (ശൂന്യം *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

സ്ഥിരം ഘടന fdt_gpio fdt_gpio_starfive;

സ്ഥിതം പൂർണ്ണസംഖ്യ starfive_gpio_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			      സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(വലുപ്പം(*chip));
	എങ്കിൽ (!chip)
		തിരികെ SBI_ENOMEM;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (rc) {
		sbi_free(chip);
		തിരികെ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	എങ്കിൽ (rc) {
		sbi_free(chip);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

സ്ഥിരം ഘടന fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
