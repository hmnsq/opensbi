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

#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>

#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

সংরচনা dw_gpio_chip {
	শূন্য *dr;
	শূন্য *ext;
	সংরচনা gpio_chip chip;
};

ধ্রুৱ সংরচনা fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, সংরচনা dw_gpio_chip, chip);

স্থির পূর্ণাঙ্ক dw_gpio_direction_output(সংরচনা gpio_pin *gp, পূর্ণাঙ্ক মান)
{
	সংরচনা dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	যদি (!মান)
		v &= ~DW_GPIO_BIT(gp->offset);
	অন্যথা
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	ঘূৰাই 0;
}

স্থির শূন্য dw_gpio_set(সংরচনা gpio_pin *gp, পূর্ণাঙ্ক মান)
{
	সংরচনা dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	যদি (!মান)
		v &= ~DW_GPIO_BIT(gp->offset);
	অন্যথা
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

স্থির পূর্ণাঙ্ক dw_gpio_init_bank(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			     ধ্রুৱ সংরচনা fdt_match *match)
{
	সংরচনা dw_gpio_chip *chip;
	ধ্রুৱ fdt32_t *val;
	uint64_t addr;
	পূর্ণাঙ্ক rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	যদি (poff < 0)
		ঘূৰাই SBI_EINVAL;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, poff, 0, &addr, রিক্ত);
	যদি (rc)
		ঘূৰাই rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	যদি (!val || len <= 0)
		ঘূৰাই SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	যদি (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	যদি (!val || len <= 0)
		ঘূৰাই SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(পরিমাণ(*chip));
	যদি (!chip)
		ঘূৰাই SBI_ENOMEM;

	chip->dr = (শূন্য *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (শূন্য *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	যদি (rc)
		গচ্ছ err_free_chip;

	ঘূৰাই 0;

err_free_chip:
	sbi_free(chip);
	ঘূৰাই rc;
}

/* since we're only probed when used, match on port not main controller node */
স্থির ধ্রুৱ সংরচনা fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ধ্রুৱ সংরচনা fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
