/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/gpio/fdt_gpio.h>

#تعریف_کرو SIFIVE_GPIO_PINS_MIN	1
#تعریف_کرو SIFIVE_GPIO_PINS_MAX	32
#تعریف_کرو SIFIVE_GPIO_PINS_DEF	16

#تعریف_کرو SIFIVE_GPIO_OUTEN	0x8
#تعریف_کرو SIFIVE_GPIO_OUTVAL	0xc
#تعریف_کرو SIFIVE_GPIO_ثنائی_ہندسہ(b)	(1U << (b))

ساخت sifive_gpio_chip {
	غیرعلامتی طویل addr;
	ساخت gpio_chip chip;
};

ساکن صحیح sifive_gpio_direction_output(ساخت gpio_pin *gp, صحیح قدر)
{
	غیرعلامتی صحیح v;
	ساخت sifive_gpio_chip *chip =
		container_کا(gp->chip, ساخت sifive_gpio_chip, chip);

	v = readl((متلون خالی *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_ثنائی_ہندسہ(gp->انحراف);
	writel(v, (متلون خالی *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((متلون خالی *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	اگر (!قدر)
		v &= ~SIFIVE_GPIO_ثنائی_ہندسہ(gp->انحراف);
	ورنہ
		v |= SIFIVE_GPIO_ثنائی_ہندسہ(gp->انحراف);
	writel(v, (متلون خالی *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	واپس 0;
}

ساکن خالی sifive_gpio_سیٹ(ساخت gpio_pin *gp, صحیح قدر)
{
	غیرعلامتی صحیح v;
	ساخت sifive_gpio_chip *chip =
		container_کا(gp->chip, ساخت sifive_gpio_chip, chip);

	v = readl((متلون خالی *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	اگر (!قدر)
		v &= ~SIFIVE_GPIO_ثنائی_ہندسہ(gp->انحراف);
	ورنہ
		v |= SIFIVE_GPIO_ثنائی_ہندسہ(gp->انحراف);
	writel(v, (متلون خالی *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ثابت ساخت fdt_gpio fdt_gpio_sifive;

ساکن صحیح sifive_gpio_init(ثابت خالی *fdt, صحیح nodeoff,
			    ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(حجم(*chip));
	اگر (!chip)
		واپس SBI_ENOMEM;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, تہی);
	اگر (rc) {
		sbi_free(chip);
		واپس rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.سیٹ = sifive_gpio_سیٹ;
	rc = gpio_chip_جوڑو(&chip->chip);
	اگر (rc) {
		sbi_free(chip);
		واپس rc;
	}

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت sifive_gpio_مطابقت[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ثابت ساخت fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.مطابقت_جدول = sifive_gpio_مطابقت,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
