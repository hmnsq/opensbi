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

स्थिर पूर्णांक sifive_gpio_direction_output(संरचना gpio_pin *gp, पूर्णांक मूल्य)
{
	अचिन्हित पूर्णांक v;
	संरचना sifive_gpio_chip *chip =
		container_of(gp->chip, संरचना sifive_gpio_chip, chip);

	v = readl((अस्थिर शून्य *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (अस्थिर शून्य *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((अस्थिर शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	जर (!मूल्य)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	अन्यथा
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (अस्थिर शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	परत 0;
}

स्थिर शून्य sifive_gpio_set(संरचना gpio_pin *gp, पूर्णांक मूल्य)
{
	अचिन्हित पूर्णांक v;
	संरचना sifive_gpio_chip *chip =
		container_of(gp->chip, संरचना sifive_gpio_chip, chip);

	v = readl((अस्थिर शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	जर (!मूल्य)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	अन्यथा
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (अस्थिर शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

स्थिरांक संरचना fdt_gpio fdt_gpio_sifive;

स्थिर पूर्णांक sifive_gpio_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			    स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	संरचना sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(आकार(*chip));
	जर (!chip)
		परत SBI_ENOMEM;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	जर (rc) {
		sbi_free(chip);
		परत rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	जर (rc) {
		sbi_free(chip);
		परत rc;
	}

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

स्थिरांक संरचना fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
