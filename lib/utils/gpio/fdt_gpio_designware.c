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

#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>

#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

கட்டமைப்பு dw_gpio_chip {
	வெறுமை *dr;
	வெறுமை *ext;
	கட்டமைப்பு gpio_chip chip;
};

மாறிலி கட்டமைப்பு fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, கட்டமைப்பு dw_gpio_chip, chip);

நிலையான முழு dw_gpio_direction_output(கட்டமைப்பு gpio_pin *gp, முழு மதிப்பு)
{
	கட்டமைப்பு dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	எனில் (!மதிப்பு)
		v &= ~DW_GPIO_BIT(gp->offset);
	இல்லையெனில்
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	திரும்பு 0;
}

நிலையான வெறுமை dw_gpio_set(கட்டமைப்பு gpio_pin *gp, முழு மதிப்பு)
{
	கட்டமைப்பு dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	எனில் (!மதிப்பு)
		v &= ~DW_GPIO_BIT(gp->offset);
	இல்லையெனில்
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

நிலையான முழு dw_gpio_init_bank(மாறிலி வெறுமை *fdt, முழு nodeoff,
			     மாறிலி கட்டமைப்பு fdt_match *match)
{
	கட்டமைப்பு dw_gpio_chip *chip;
	மாறிலி fdt32_t *val;
	uint64_t addr;
	முழு rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	எனில் (poff < 0)
		திரும்பு SBI_EINVAL;

	rc = fdt_get_முனை_addr_size(fdt, poff, 0, &addr, வெற்று);
	எனில் (rc)
		திரும்பு rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	எனில் (!val || len <= 0)
		திரும்பு SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	எனில் (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	எனில் (!val || len <= 0)
		திரும்பு SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(அளவு(*chip));
	எனில் (!chip)
		திரும்பு SBI_ENOMEM;

	chip->dr = (வெறுமை *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (வெறுமை *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	எனில் (rc)
		செல் err_free_chip;

	திரும்பு 0;

err_free_chip:
	sbi_free(chip);
	திரும்பு rc;
}

/* since we're only probed when used, match on port not main controller node */
நிலையான மாறிலி கட்டமைப்பு fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
