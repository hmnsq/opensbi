/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/gpio/fdt_gpio.h>

#घोषणा SIFIVE_GPIO_PINS_MIN	1
#घोषणा SIFIVE_GPIO_PINS_MAX	32
#घोषणा SIFIVE_GPIO_PINS_DEF	16

#घोषणा SIFIVE_GPIO_OUTEN	0x8
#घोषणा SIFIVE_GPIO_OUTVAL	0xc
#घोषणा SIFIVE_GPIO_BIT(b)	(1U << (b))

काष्ठा sifive_gpio_chip {
	अचिन्हित दीर्घ addr;
	काष्ठा gpio_chip chip;
};

अटल पूर्णांक sifive_gpio_direction_output(काष्ठा gpio_pin *gp, पूर्णांक मूल्य)
{
	अचिन्हित पूर्णांक v;
	काष्ठा sifive_gpio_chip *chip =
		container_of(gp->chip, काष्ठा sifive_gpio_chip, chip);

	v = readl((अस्थिर व्योम *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (अस्थिर व्योम *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((अस्थिर व्योम *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	अगर (!मूल्य)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	अन्यथा
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (अस्थिर व्योम *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	वापस 0;
}

अटल व्योम sifive_gpio_set(काष्ठा gpio_pin *gp, पूर्णांक मूल्य)
{
	अचिन्हित पूर्णांक v;
	काष्ठा sifive_gpio_chip *chip =
		container_of(gp->chip, काष्ठा sifive_gpio_chip, chip);

	v = readl((अस्थिर व्योम *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	अगर (!मूल्य)
		v &= ~SIFIVE_GPIO_BIT(gp->offset);
	अन्यथा
		v |= SIFIVE_GPIO_BIT(gp->offset);
	writel(v, (अस्थिर व्योम *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

स्थिर काष्ठा fdt_gpio fdt_gpio_sifive;

अटल पूर्णांक sifive_gpio_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			    स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(माप(*chip));
	अगर (!chip)
		वापस SBI_ENOMEM;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, शून्य);
	अगर (rc) {
		sbi_free(chip);
		वापस rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.set = sifive_gpio_set;
	rc = gpio_chip_add(&chip->chip);
	अगर (rc) {
		sbi_free(chip);
		वापस rc;
	}

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match sifive_gpio_match[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

स्थिर काष्ठा fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.match_table = sifive_gpio_match,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
