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

#परिभाषय SIFIVE_GPIO_PINS_MIN	1
#परिभाषय SIFIVE_GPIO_PINS_MAX	32
#परिभाषय SIFIVE_GPIO_PINS_DEF	16

#परिभाषय SIFIVE_GPIO_OUTEN	0x8
#परिभाषय SIFIVE_GPIO_OUTVAL	0xc
#परिभाषय SIFIVE_GPIO_BIT(b)	(1U << (b))

संरचना sifive_gpio_chip {
	अचिह्नित दीर्घ addr;
	संरचना gpio_chip chip;
};

स्थिर पूर्णाङ्क sifive_gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	अचिह्नित पूर्णाङ्क v;
	संरचना sifive_gpio_chip *chip =
		container_सम्बन्धि(gp->chip, संरचना sifive_gpio_chip, chip);

	v = readl((चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->विस्थापनम्);
	writel(v, (चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	यदि (!मानम्)
		v &= ~SIFIVE_GPIO_BIT(gp->विस्थापनम्);
	अन्यथा
		v |= SIFIVE_GPIO_BIT(gp->विस्थापनम्);
	writel(v, (चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	प्रत्यागम 0;
}

स्थिर शून्य sifive_gpio_समुच्चयः(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	अचिह्नित पूर्णाङ्क v;
	संरचना sifive_gpio_chip *chip =
		container_सम्बन्धि(gp->chip, संरचना sifive_gpio_chip, chip);

	v = readl((चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	यदि (!मानम्)
		v &= ~SIFIVE_GPIO_BIT(gp->विस्थापनम्);
	अन्यथा
		v |= SIFIVE_GPIO_BIT(gp->विस्थापनम्);
	writel(v, (चञ्चल शून्य *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ध्रुव संरचना fdt_gpio fdt_gpio_sifive;

स्थिर पूर्णाङ्क sifive_gpio_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			    ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(परिमाण(*chip));
	यदि (!chip)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc) {
		sbi_free(chip);
		प्रत्यागम rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.समुच्चयः = sifive_gpio_समुच्चयः;
	rc = gpio_chip_सङ्कलय(&chip->chip);
	यदि (rc) {
		sbi_free(chip);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् sifive_gpio_मेलनम्[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ध्रुव संरचना fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.मेलनम्_सारणी = sifive_gpio_मेलनम्,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
