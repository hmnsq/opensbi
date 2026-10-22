/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Starfive
 *
 * Authors:
 *   Minda.chen <Minda.chen@starfivetech.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/gpio/fdt_gpio.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STARFIVE_GPIO_PINS_DEF		64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STARFIVE_GPIO_OUTVAL		0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STARFIVE_GPIO_ਨਕਾਬ		0xff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STARFIVE_GPIO_REG_ਖਿਸਕਾਓ_ਨਕਾਬ	0x3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STARFIVE_GPIO_ਖਿਸਕਾਓ_ਬਿੱਟ	0x3

ਬਣਤਰ starfive_gpio_chip {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr;
	ਬਣਤਰ gpio_chip chip;
};

ਸਥਿਰ ਪੂਰਨ starfive_gpio_direction_output(ਬਣਤਰ gpio_pin *gp, ਪੂਰਨ ਮੁੱਲ)
{
	u32 val;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg_addr;
	u32 ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ_2, ਖਿਸਕਾਓ_ਬਿੱਟ;
	ਬਣਤਰ starfive_gpio_chip *chip =
		container_ਦਾ(gp->chip, ਬਣਤਰ starfive_gpio_chip, chip);

	/* set out en*/
	reg_addr = chip->addr + gp->ਵਿੱਥ;
	reg_addr &= ~(STARFIVE_GPIO_REG_ਖਿਸਕਾਓ_ਨਕਾਬ);

	ਖਿਸਕਾਓ_ਬਿੱਟ = (gp->ਵਿੱਥ & STARFIVE_GPIO_REG_ਖਿਸਕਾਓ_ਨਕਾਬ)
		<< STARFIVE_GPIO_ਖਿਸਕਾਓ_ਬਿੱਟ;
	ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ_2 = STARFIVE_GPIO_ਨਕਾਬ << ਖਿਸਕਾਓ_ਬਿੱਟ;

	val = readl((ਖਾਲੀ *)reg_addr);
	val &= ~ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ_2;
	writel(val, (ਖਾਲੀ *)reg_addr);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ starfive_gpio_ਸੈੱਟ(ਬਣਤਰ gpio_pin *gp, ਪੂਰਨ ਮੁੱਲ)
{
	u32 val;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ reg_addr;
	u32 ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ_2, ਖਿਸਕਾਓ_ਬਿੱਟ;
	ਬਣਤਰ starfive_gpio_chip *chip =
		container_ਦਾ(gp->chip, ਬਣਤਰ starfive_gpio_chip, chip);

	reg_addr = chip->addr + gp->ਵਿੱਥ;
	reg_addr &= ~(STARFIVE_GPIO_REG_ਖਿਸਕਾਓ_ਨਕਾਬ);

	ਖਿਸਕਾਓ_ਬਿੱਟ = (gp->ਵਿੱਥ & STARFIVE_GPIO_REG_ਖਿਸਕਾਓ_ਨਕਾਬ)
		<< STARFIVE_GPIO_ਖਿਸਕਾਓ_ਬਿੱਟ;
	ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ_2 = STARFIVE_GPIO_ਨਕਾਬ << ਖਿਸਕਾਓ_ਬਿੱਟ;
	/* set output value */
	val = readl((ਖਾਲੀ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
	val &= ~ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ_2;
	val |= ਮੁੱਲ << ਖਿਸਕਾਓ_ਬਿੱਟ;
	writel(val, (ਖਾਲੀ *)(reg_addr + STARFIVE_GPIO_OUTVAL));
}

ਅਚਲ ਬਣਤਰ fdt_gpio fdt_gpio_starfive;

ਸਥਿਰ ਪੂਰਨ starfive_gpio_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			      ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ starfive_gpio_chip *chip;
	u64 addr;

	chip = sbi_zalloc(ਆਕਾਰ(*chip));
	ਜੇ (!chip)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, ਨਿਰਾ);
	ਜੇ (rc) {
		sbi_free(chip);
		ਵਾਪਸ rc;
	}

	chip->addr = addr;
	chip->chip.driver = &fdt_gpio_starfive;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = STARFIVE_GPIO_PINS_DEF;
	chip->chip.direction_output = starfive_gpio_direction_output;
	chip->chip.ਸੈੱਟ = starfive_gpio_ਸੈੱਟ;
	rc = gpio_chip_ਜੋੜੋ(&chip->chip);
	ਜੇ (rc) {
		sbi_free(chip);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ starfive_gpio_ਮੇਲ[] = {
	{ .compatible = "starfive,jh7110-sys-pinctrl" },
	{ .compatible = "starfive,iomux-pinctrl" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_gpio fdt_gpio_starfive = {
	.driver = {
		.ਮੇਲ_ਸਾਰਣੀ = starfive_gpio_ਮੇਲ,
		.init = starfive_gpio_init,
	},
	.xlate = fdt_gpio_simple_xlate,
};
