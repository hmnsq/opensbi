/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/gpio/fdt_gpio.h>

#घोषणा STARFIVE_GPIO_PINS_DEF		64
#घोषणा STARFIVE_GPIO_OUTVAL		0x40
#घोषणा STARFIVE_GPIO_MASK		0xff
#घोषणा STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#घोषणा STARFIVE_GPIO_SHIFT_BITS	0x3

काष्ठा starfive_gpio_chip {
	अचिन्हित दीर्घ addr;
	काष्ठा gpio_chip chip;
};

अटल पूर्णांक starfive_gpio_direction_output(काष्ठा gpio_pin *gp, पूर्णांक मूल्य)
{
	u32 val;
	अचिन्हित दीर्घ reg_addr;
	u32 bit_mask, shift_bits;
	काष्ठा starfive_gpio_chip *chip =
		container_of(gp->chip, काष्ठा starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((व्योम *)reg_addr);
	val &= ~bit_mask;
	writel(val, (व्योम *)reg_addr);

	वापस 0;
}

अटल व्योम starfive_gpio_set(काष्ठा gpio_pin *gp, पूर्णांक मूल्य)
{
	u32 val;
	अचिन्हित दीर्घ reg_addr;
	u32 bit_mask, shift_bits;
	काष्ठा starfive_gpio_chip *chip =
		container_of(gp->chip, काष्ठा starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((व्योम *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= मूल्य << shift_bits;
	writel(val, (व्योम *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

स्थिर काष्ठा fdt_gpio fdt_gpio_starfive;

अटल पूर्णांक starfive_gpio_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			      स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(माप(*chip));
	अगर (!chip)
		वापस SBI_ENOMEM;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, शून्य);
	अगर (rc) {
		sbi_free(chip);
		वापस rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	अगर (rc) {
		sbi_free(chip);
		वापस rc;
	}

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

स्थिर काष्ठा fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
