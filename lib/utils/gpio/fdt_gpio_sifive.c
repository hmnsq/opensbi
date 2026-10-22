/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/gpio/fdt_gpio.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_GPIO_PINS_MIN	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_GPIO_PINS_MAX	32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_GPIO_PINS_DEF	16

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_GPIO_OUTEN	0x8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_GPIO_OUTVAL	0xc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(b)	(1U << (b))

ਬਣਤਰ sifive_gpio_chip {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr;
	ਬਣਤਰ gpio_chip chip;
};

ਸਥਿਰ ਪੂਰਨ sifive_gpio_direction_output(ਬਣਤਰ gpio_pin *gp, ਪੂਰਨ ਮੁੱਲ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ v;
	ਬਣਤਰ sifive_gpio_chip *chip =
		container_ਦਾ(gp->chip, ਬਣਤਰ sifive_gpio_chip, chip);

	v = readl((ਅਸਥਿਰ ਖਾਲੀ *)(chip->addr + SIFIVE_GPIO_OUTEN));
	v |= SIFIVE_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	writel(v, (ਅਸਥਿਰ ਖਾਲੀ *)(chip->addr + SIFIVE_GPIO_OUTEN));

	v = readl((ਅਸਥਿਰ ਖਾਲੀ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ਜੇ (!ਮੁੱਲ)
		v &= ~SIFIVE_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	ਨਹੀਂਤਾਂ
		v |= SIFIVE_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	writel(v, (ਅਸਥਿਰ ਖਾਲੀ *)(chip->addr + SIFIVE_GPIO_OUTVAL));

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ sifive_gpio_ਸੈੱਟ(ਬਣਤਰ gpio_pin *gp, ਪੂਰਨ ਮੁੱਲ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ v;
	ਬਣਤਰ sifive_gpio_chip *chip =
		container_ਦਾ(gp->chip, ਬਣਤਰ sifive_gpio_chip, chip);

	v = readl((ਅਸਥਿਰ ਖਾਲੀ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
	ਜੇ (!ਮੁੱਲ)
		v &= ~SIFIVE_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	ਨਹੀਂਤਾਂ
		v |= SIFIVE_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	writel(v, (ਅਸਥਿਰ ਖਾਲੀ *)(chip->addr + SIFIVE_GPIO_OUTVAL));
}

ਅਚਲ ਬਣਤਰ fdt_gpio fdt_gpio_sifive;

ਸਥਿਰ ਪੂਰਨ sifive_gpio_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			    ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ sifive_gpio_chip *chip;
	uint64_t addr;

	chip = sbi_zalloc(ਆਕਾਰ(*chip));
	ਜੇ (!chip)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, ਨਿਰਾ);
	ਜੇ (rc) {
		sbi_free(chip);
		ਵਾਪਸ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_sifive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = SIFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = sifive_gpio_direction_output;
	chip->chip.ਸੈੱਟ = sifive_gpio_ਸੈੱਟ;
	rc = gpio_chip_ਜੋੜੋ(&chip->chip);
	ਜੇ (rc) {
		sbi_free(chip);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sifive_gpio_ਮੇਲ[] = {
	{ .compatible = "sifive,gpio0" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_gpio fdt_gpio_sifive = {
	.driver = {
		.ਮੇਲ_ਸਾਰਣੀ = sifive_gpio_ਮੇਲ,
		.init = sifive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
