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

#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>

#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

ഘടന dw_gpio_chip {
	ശൂന്യം *dr;
	ശൂന്യം *ext;
	ഘടന gpio_chip chip;
};

സ്ഥിരം ഘടന fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, ഘടന dw_gpio_chip, chip);

സ്ഥിതം പൂർണ്ണസംഖ്യ dw_gpio_direction_output(ഘടന gpio_pin *gp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	ഘടന dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	എങ്കിൽ (!മൂല്യം)
		v &= ~DW_GPIO_BIT(gp->offset);
	അല്ലെങ്കിൽ
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം dw_gpio_set(ഘടന gpio_pin *gp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	ഘടന dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	എങ്കിൽ (!മൂല്യം)
		v &= ~DW_GPIO_BIT(gp->offset);
	അല്ലെങ്കിൽ
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

സ്ഥിതം പൂർണ്ണസംഖ്യ dw_gpio_init_bank(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			     സ്ഥിരം ഘടന fdt_match *match)
{
	ഘടന dw_gpio_chip *chip;
	സ്ഥിരം fdt32_t *val;
	uint64_t addr;
	പൂർണ്ണസംഖ്യ rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	എങ്കിൽ (poff < 0)
		തിരികെ SBI_EINVAL;

	rc = fdt_get_കണ്ണി_addr_size(fdt, poff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (rc)
		തിരികെ rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	എങ്കിൽ (!val || len <= 0)
		തിരികെ SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	എങ്കിൽ (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	എങ്കിൽ (!val || len <= 0)
		തിരികെ SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(വലുപ്പം(*chip));
	എങ്കിൽ (!chip)
		തിരികെ SBI_ENOMEM;

	chip->dr = (ശൂന്യം *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (ശൂന്യം *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	എങ്കിൽ (rc)
		പോകുക err_free_chip;

	തിരികെ 0;

err_free_chip:
	sbi_free(chip);
	തിരികെ rc;
}

/* since we're only probed when used, match on port not main controller node */
സ്ഥിതം സ്ഥിരം ഘടന fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

സ്ഥിരം ഘടന fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
