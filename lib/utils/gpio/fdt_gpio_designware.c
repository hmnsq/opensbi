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

#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>

#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

ସଂରଚନା dw_gpio_chip {
	ଶୂନ୍ୟ *dr;
	ଶୂନ୍ୟ *ext;
	ସଂରଚନା gpio_chip chip;
};

ଅଚଳ ସଂରଚନା fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, ସଂରଚନା dw_gpio_chip, chip);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dw_gpio_direction_output(ସଂରଚନା gpio_pin *gp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	ସଂରଚନା dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	ଯଦି (!ମୂଲ୍ୟ)
		v &= ~DW_GPIO_BIT(gp->offset);
	ଅନ୍ୟଥା
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ dw_gpio_set(ସଂରଚନା gpio_pin *gp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	ସଂରଚନା dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	ଯଦି (!ମୂଲ୍ୟ)
		v &= ~DW_GPIO_BIT(gp->offset);
	ଅନ୍ୟଥା
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dw_gpio_init_bank(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			     ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ସଂରଚନା dw_gpio_chip *chip;
	ଅଚଳ fdt32_t *val;
	uint64_t addr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	ଯଦି (poff < 0)
		ଫେରନ୍ତୁ SBI_EINVAL;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, poff, 0, &addr, ଖାଲି);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	ଯଦି (!val || len <= 0)
		ଫେରନ୍ତୁ SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	ଯଦି (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	ଯଦି (!val || len <= 0)
		ଫେରନ୍ତୁ SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(ଆକାର(*chip));
	ଯଦି (!chip)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	chip->dr = (ଶୂନ୍ୟ *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (ଶୂନ୍ୟ *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	ଯଦି (rc)
		ଯାଅ err_free_chip;

	ଫେରନ୍ତୁ 0;

err_free_chip:
	sbi_free(chip);
	ଫେରନ୍ତୁ rc;
}

/* since we're only probed when used, match on port not main controller node */
ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
