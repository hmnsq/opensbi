/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/gpio/fdt_gpio.h>

#define STARFIVE_GPIO_PINS_DEF		64
#define STARFIVE_GPIO_OUTVAL		0x40
#define STARFIVE_GPIO_MASK		0xff
#define STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#define STARFIVE_GPIO_SHIFT_BITS	0x3

સંરચના starfive_gpio_chip {
	unsigned long addr;
	સંરચના gpio_chip chip;
};

સ્થિર પૂર્ણાંક starfive_gpio_direction_output(સંરચના gpio_pin *gp, પૂર્ણાંક કિંમત)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	સંરચના starfive_gpio_chip *chip =
		container_of(gp->chip, સંરચના starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((શૂન્ય *)reg_addr);
	val &= ~bit_mask;
	writel(val, (શૂન્ય *)reg_addr);

	પાછા 0;
}

સ્થિર શૂન્ય starfive_gpio_set(સંરચના gpio_pin *gp, પૂર્ણાંક કિંમત)
{
	u32 val;
	unsigned long reg_addr;
	u32 bit_mask, shift_bits;
	સંરચના starfive_gpio_chip *chip =
		container_of(gp->chip, સંરચના starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((શૂન્ય *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= કિંમત << shift_bits;
	writel(val, (શૂન્ય *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

અચલ સંરચના fdt_gpio fdt_gpio_starfive;

સ્થિર પૂર્ણાંક starfive_gpio_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			      અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(કદ(*chip));
	જો (!chip)
		પાછા SBI_ENOMEM;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, ખાલી);
	જો (rc) {
		sbi_free(chip);
		પાછા rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	જો (rc) {
		sbi_free(chip);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

અચલ સંરચના fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
