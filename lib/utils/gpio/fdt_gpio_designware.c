/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 SiFive
 *
 * GPIO driver for Synopsys DesignWare APB GPIO
 *
 * Authors:
 *   Ben Dooks <ben.dooks@sifive.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/gpio/fdt_gpio.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_GPIO_PINS_MAX	32

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_GPIO_DDR	0x4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_GPIO_DR	0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(_b)	(1UL << (_b))

ਬਣਤਰ dw_gpio_chip {
	ਖਾਲੀ *dr;
	ਖਾਲੀ *ext;
	ਬਣਤਰ gpio_chip chip;
};

ਅਚਲ ਬਣਤਰ fdt_gpio fdt_gpio_designware;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ pin_ਮੰਜ਼ਿਲ_chip(__p) container_ਦਾ((__p)->chip, ਬਣਤਰ dw_gpio_chip, chip);

ਸਥਿਰ ਪੂਰਨ dw_gpio_direction_output(ਬਣਤਰ gpio_pin *gp, ਪੂਰਨ ਮੁੱਲ)
{
	ਬਣਤਰ dw_gpio_chip *chip = pin_ਮੰਜ਼ਿਲ_chip(gp);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ v;

	v = readl(chip->dr + DW_GPIO_DR);
	ਜੇ (!ਮੁੱਲ)
		v &= ~DW_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	ਨਹੀਂਤਾਂ
		v |= DW_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	writel(v, chip->dr + DW_GPIO_DDR);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ dw_gpio_ਸੈੱਟ(ਬਣਤਰ gpio_pin *gp, ਪੂਰਨ ਮੁੱਲ)
{
	ਬਣਤਰ dw_gpio_chip *chip = pin_ਮੰਜ਼ਿਲ_chip(gp);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ v;

	v = readl(chip->dr + DW_GPIO_DR);
	ਜੇ (!ਮੁੱਲ)
		v &= ~DW_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	ਨਹੀਂਤਾਂ
		v |= DW_GPIO_ਦੋਆਧਾਰੀ_ਅੰਕ(gp->ਵਿੱਥ);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

ਸਥਿਰ ਪੂਰਨ dw_gpio_init_bank(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			     ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਬਣਤਰ dw_gpio_chip *chip;
	ਅਚਲ fdt32_t *val;
	uint64_t addr;
	ਪੂਰਨ rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, nodeoff);
	ਜੇ (poff < 0)
		ਵਾਪਸ SBI_EINVAL;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, poff, 0, &addr, ਨਿਰਾ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	ਜੇ (!val || len <= 0)
		ਵਾਪਸ SBI_EINVAL;
	bank = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	ਜੇ (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	ਜੇ (!val || len <= 0)
		ਵਾਪਸ SBI_EINVAL;
	nr_pins = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);

	chip = sbi_zalloc(ਆਕਾਰ(*chip));
	ਜੇ (!chip)
		ਵਾਪਸ SBI_ENOMEM;

	chip->dr = (ਖਾਲੀ *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (ਖਾਲੀ *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.ਸੈੱਟ = dw_gpio_ਸੈੱਟ;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_ਜੋੜੋ(&chip->chip);
	ਜੇ (rc)
		ਜਾਓ err_free_chip;

	ਵਾਪਸ 0;

err_free_chip:
	sbi_free(chip);
	ਵਾਪਸ rc;
}

/* since we're only probed when used, match on port not main controller node */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ dw_gpio_ਮੇਲ[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_gpio fdt_gpio_designware = {
	.driver = {
		.ਮੇਲ_ਸਾਰਣੀ = dw_gpio_ਮੇਲ,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
