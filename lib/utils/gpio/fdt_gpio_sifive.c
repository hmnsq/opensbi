/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

கட்டமைப்பு sifive_gpio_chip {
	unsigned long addr;
	கட்டமைப்பு gpio_chip chip;
};

நிலையான முழு sifive_gpio_direction_output(கட்டமைப்பு gpio_pin *gp, முழு மதிப்பு)
{
	unsigned முழு v;
	கட்டமைப்பு sifive_gpio_chip *chip =
		container_of(gp->chip, கட்டமைப்பு sifive_gpio_chip, chip);

	v = readl((நிலையற்ற வெறுமை *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (நிலையற்ற வெறுமை *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((நிலையற்ற வெறுமை *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	எனில் (!மதிப்பு)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	இல்லையெனில்
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (நிலையற்ற வெறுமை *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	திரும்பு 0;
}

நிலையான வெறுமை sifive_gpio_set(கட்டமைப்பு gpio_pin *gp, முழு மதிப்பு)
{
	unsigned முழு v;
	கட்டமைப்பு sifive_gpio_chip *chip =
		container_of(gp->chip, கட்டமைப்பு sifive_gpio_chip, chip);

	v = readl((நிலையற்ற வெறுமை *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	எனில் (!மதிப்பு)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	இல்லையெனில்
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (நிலையற்ற வெறுமை *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

மாறிலி கட்டமைப்பு fdt_gpio fdt_gpio_sifive;

நிலையான முழு sifive_gpio_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			    மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(அளவு(*chip));
	எனில் (!chip)
		திரும்பு SBI_ENOMEM;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, வெற்று);
	எனில் (rc) {
		sbi_free(chip);
		திரும்பு rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	எனில் (rc) {
		sbi_free(chip);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
