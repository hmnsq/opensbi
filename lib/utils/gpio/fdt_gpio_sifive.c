/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/gpio/fdt_gpio.h>

#define SIFIVE_GPIO_PINS_MIN	1
#define SIFIVE_GPIO_PINS_MAX	32
#define SIFIVE_GPIO_PINS_DEF	16

#define SIFIVE_GPIO_OUTEN	0x8
#define SIFIVE_GPIO_OUTVAL	0xc
#define SIFIVE_GPIO_BIT(b)	(1U << (b))

સંરચના sifive_gpio_chip {
	unsigned long addr;
	સંરચના gpio_chip chip;
};

સ્થિર પૂર્ણાંક sifive_gpio_direction_output(સંરચના gpio_pin *gp, પૂર્ણાંક કિંમત)
{
	unsigned પૂર્ણાંક v;
	સંરચના sifive_gpio_chip *chip =
		container_of(gp->chip, સંરચના sifive_gpio_chip, chip);

	v = readl((અસ્થિર શૂન્ય *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (અસ્થિર શૂન્ય *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((અસ્થિર શૂન્ય *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	જો (!કિંમત)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	નહીંતર
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (અસ્થિર શૂન્ય *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	પાછા 0;
}

સ્થિર શૂન્ય sifive_gpio_set(સંરચના gpio_pin *gp, પૂર્ણાંક કિંમત)
{
	unsigned પૂર્ણાંક v;
	સંરચના sifive_gpio_chip *chip =
		container_of(gp->chip, સંરચના sifive_gpio_chip, chip);

	v = readl((અસ્થિર શૂન્ય *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	જો (!કિંમત)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	નહીંતર
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (અસ્થિર શૂન્ય *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

અચલ સંરચના fdt_gpio fdt_gpio_sifive;

સ્થિર પૂર્ણાંક sifive_gpio_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			    અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(કદ(*chip));
	જો (!chip)
		પાછા SBI_ENOMEM;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, ખાલી);
	જો (rc) {
		sbi_free(chip);
		પાછા rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	જો (rc) {
		sbi_free(chip);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

અચલ સંરચના fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
