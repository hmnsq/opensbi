/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/gpio/fdt_gpio.h>

#define STARFIVE_GPIO_PINS_DEF		64
#define STARFIVE_GPIO_OUTVAL		0x40
#define STARFIVE_GPIO_MASK		0xff
#define STARFIVE_GPIO_REG_SHIFT_MASK	0x3
#define STARFIVE_GPIO_SHIFT_BITS	0x3

संरचना starfive_gpio_chip {
	अचिन्हित दीर्घ addr;
	संरचना gpio_chip chip;
};

स्थिर पूर्णाङ्क starfive_gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मान)
{
	u32 val;
	अचिन्हित दीर्घ reg_addr;
	u32 bit_mask, shift_bits;
	संरचना starfive_gpio_chip *chip =
		container_of(gp->chip, संरचना starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;

	val = readl((शून्य *)reg_addr);
	val &= ~bit_mask;
	writel(val, (शून्य *)reg_addr);

	फर्काऊ 0;
}

स्थिर शून्य starfive_gpio_set(संरचना gpio_pin *gp, पूर्णाङ्क मान)
{
	u32 val;
	अचिन्हित दीर्घ reg_addr;
	u32 bit_mask, shift_bits;
	संरचना starfive_gpio_chip *chip =
		container_of(gp->chip, संरचना starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->offset;
	reg_addr &= ~(STARFIVE_GPIO_REG_SHIFT_MASK);

	shift_bits = (gp->offset & STARFIVE_GPIO_REG_SHIFT_MASK)
		<< STARFIVE_GPIO_SHIFT_BITS;
	bit_mask = STARFIVE_GPIO_MASK << shift_bits;
	/* set output value */
	val = readl((शून्य *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~bit_mask;
	val |= मान << shift_bits;
	writel(val, (शून्य *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

अचर संरचना fdt_gpio fdt_gpio_starfive;

स्थिर पूर्णाङ्क starfive_gpio_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			      अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(परिमाण(*chip));
	यदि (!chip)
		फर्काऊ SBI_ENOMEM;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc) {
		sbi_free(chip);
		फर्काऊ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.set = starfive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	यदि (rc) {
		sbi_free(chip);
		फर्काऊ rc;
	}

	फर्काऊ 0;
}

स्थिर अचर संरचना fdt_match starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

अचर संरचना fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.match_table = starfive_gpio_match,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
