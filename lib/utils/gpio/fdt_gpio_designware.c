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

#شامل <libfdt.h>

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>

#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

ساختار dw_gpio_chip {
	تهی *dr;
	تهی *ext;
	ساختار gpio_chip chip;
};

ثابت ساختار fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, ساختار dw_gpio_chip, chip);

ایستا صحیح dw_gpio_direction_output(ساختار gpio_pin *gp, صحیح مقدار)
{
	ساختار dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	اگر (!مقدار)
		v &= ~DW_GPIO_BIT(gp->offset);
	وگرنه
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	بازگشت 0;
}

ایستا تهی dw_gpio_set(ساختار gpio_pin *gp, صحیح مقدار)
{
	ساختار dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	اگر (!مقدار)
		v &= ~DW_GPIO_BIT(gp->offset);
	وگرنه
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

ایستا صحیح dw_gpio_init_bank(ثابت تهی *fdt, صحیح nodeoff,
			     ثابت ساختار fdt_match *match)
{
	ساختار dw_gpio_chip *chip;
	ثابت fdt32_t *val;
	uint64_t addr;
	صحیح rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	اگر (poff < 0)
		بازگشت SBI_EINVAL;

	rc = fdt_get_گره_addr_size(fdt, poff, 0, &addr, پوچ);
	اگر (rc)
		بازگشت rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	اگر (!val || len <= 0)
		بازگشت SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	اگر (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	اگر (!val || len <= 0)
		بازگشت SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(اندازه(*chip));
	اگر (!chip)
		بازگشت SBI_ENOMEM;

	chip->dr = (تهی *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (تهی *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	اگر (rc)
		برو err_free_chip;

	بازگشت 0;

err_free_chip:
	sbi_free(chip);
	بازگشت rc;
}

/* since we're only probed when used, match on port not main controller node */
ایستا ثابت ساختار fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ثابت ساختار fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
