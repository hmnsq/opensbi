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

#ضمن <libfdt.h>

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

بنية dw_gpio_chip {
	فراغ *dr;
	فراغ *ext;
	بنية gpio_chip chip;
};

ثابت بنية fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, بنية dw_gpio_chip, chip);

ساكن صحيح dw_gpio_direction_output(بنية gpio_pin *gp, صحيح value)
{
	بنية dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	𐡄𐡍 (!value)
		v &= ~DW_GPIO_BIT(gp->offset);
	وإلا
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	أرجع 0;
}

ساكن فراغ dw_gpio_set(بنية gpio_pin *gp, صحيح value)
{
	بنية dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	𐡄𐡍 (!value)
		v &= ~DW_GPIO_BIT(gp->offset);
	وإلا
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

ساكن صحيح dw_gpio_init_bank(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_match *match)
{
	بنية dw_gpio_chip *chip;
	ثابت fdt32_t *val;
	uint64_t addr;
	صحيح rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	𐡄𐡍 (poff < 0)
		أرجع SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, poff, 0, &addr, عدم);
	𐡄𐡍 (rc)
		أرجع rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	𐡄𐡍 (!val || len <= 0)
		أرجع SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	𐡄𐡍 (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	𐡄𐡍 (!val || len <= 0)
		أرجع SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(حجم(*chip));
	𐡄𐡍 (!chip)
		أرجع SBI_ENOMEM;

	chip->dr = (فراغ *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (فراغ *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	𐡄𐡍 (rc)
		اذهب err_free_chip;

	أرجع 0;

err_free_chip:
	sbi_free(chip);
	أرجع rc;
}

/* since we're only probed when used, match on port not main controller node */
ساكن ثابت بنية fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
