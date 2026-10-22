/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

संरचना sifive_gpio_chip {
	अचिन्हित दीर्घ addr;
	संरचना gpio_chip chip;
};

स्थिर पूर्णाङ्क sifive_gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मूल्य)
{
	अचिन्हित पूर्णाङ्क v;
	संरचना sifive_gpio_chip *chip =
		container_of(gp->chip, संरचना sifive_gpio_chip, chip);

	v = readl((चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	यदि (!मूल्य)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	अन्यथा
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	प्रत्यागम 0;
}

स्थिर शून्य sifive_gpio_set(संरचना gpio_pin *gp, पूर्णाङ्क मूल्य)
{
	अचिन्हित पूर्णाङ्क v;
	संरचना sifive_gpio_chip *chip =
		container_of(gp->chip, संरचना sifive_gpio_chip, chip);

	v = readl((चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	यदि (!मूल्य)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	अन्यथा
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ध्रुव संरचना fdt_gpio fdt_gpio_sifive;

स्थिर पूर्णाङ्क sifive_gpio_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			    ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(परिमाण(*chip));
	यदि (!chip)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_get_गाँठ_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc) {
		sbi_free(chip);
		प्रत्यागम rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	यदि (rc) {
		sbi_free(chip);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ध्रुव संरचना fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
