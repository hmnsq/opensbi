/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

ഘടന sifive_gpio_chip {
	unsigned long addr;
	ഘടന gpio_chip chip;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_gpio_direction_output(ഘടന gpio_pin *gp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	unsigned പൂർണ്ണസംഖ്യ v;
	ഘടന sifive_gpio_chip *chip =
		container_of(gp->chip, ഘടന sifive_gpio_chip, chip);

	v = readl((അസ്ഥിര ശൂന്യം *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (അസ്ഥിര ശൂന്യം *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((അസ്ഥിര ശൂന്യം *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	എങ്കിൽ (!മൂല്യം)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	അല്ലെങ്കിൽ
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (അസ്ഥിര ശൂന്യം *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം sifive_gpio_set(ഘടന gpio_pin *gp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	unsigned പൂർണ്ണസംഖ്യ v;
	ഘടന sifive_gpio_chip *chip =
		container_of(gp->chip, ഘടന sifive_gpio_chip, chip);

	v = readl((അസ്ഥിര ശൂന്യം *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	എങ്കിൽ (!മൂല്യം)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	അല്ലെങ്കിൽ
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (അസ്ഥിര ശൂന്യം *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

സ്ഥിരം ഘടന fdt_gpio fdt_gpio_sifive;

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_gpio_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			    സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(വലുപ്പം(*chip));
	എങ്കിൽ (!chip)
		തിരികെ SBI_ENOMEM;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (rc) {
		sbi_free(chip);
		തിരികെ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	എങ്കിൽ (rc) {
		sbi_free(chip);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

സ്ഥിരം ഘടന fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
