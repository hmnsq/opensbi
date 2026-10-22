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

#تعریف_کړه DW_GPIO_PINS_MAX	32

#تعریف_کړه DW_GPIO_DDR	0x4
#تعریف_کړه DW_GPIO_DR	0x0
#تعریف_کړه DW_GPIO_بټ(_b)	(1UL << (_b))

بنية dw_gpio_chip {
	فراغ *dr;
	فراغ *ext;
	بنية gpio_chip chip;
};

ثابت بنية fdt_gpio fdt_gpio_designware;

#تعریف_کړه pin_منزل_chip(__p) container_د((__p)->chip, بنية dw_gpio_chip, chip);

ساكن صحيح dw_gpio_direction_output(بنية gpio_pin *gp, صحيح ارزښت)
{
	بنية dw_gpio_chip *chip = pin_منزل_chip(gp);
	بینښه اوږد v;

	v = readl(chip->dr + DW_GPIO_DR);
	إذا (!ارزښت)
		v &= ~DW_GPIO_بټ(gp->دننه_واټن);
	وإلا
		v |= DW_GPIO_بټ(gp->دننه_واټن);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_بټ(gp->دننه_واټن);
	writel(v, chip->dr + DW_GPIO_DDR);

	أرجع 0;
}

ساكن فراغ dw_gpio_سېټ(بنية gpio_pin *gp, صحيح ارزښت)
{
	بنية dw_gpio_chip *chip = pin_منزل_chip(gp);
	بینښه اوږد v;

	v = readl(chip->dr + DW_GPIO_DR);
	إذا (!ارزښت)
		v &= ~DW_GPIO_بټ(gp->دننه_واټن);
	وإلا
		v |= DW_GPIO_بټ(gp->دننه_واټن);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

ساكن صحيح dw_gpio_init_bank(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_سمون *سمون)
{
	بنية dw_gpio_chip *chip;
	ثابت fdt32_t *val;
	uint64_t addr;
	صحيح rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_پلار_دننه_واټن(fdt, nodeoff);
	إذا (poff < 0)
		أرجع SBI_EINVAL;

	rc = fdt_get_غوټه_addr_اندازه(fdt, poff, 0, &addr, عدم);
	إذا (rc)
		أرجع rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	إذا (!val || len <= 0)
		أرجع SBI_EINVAL;
	bank = fdt32_منزل_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	إذا (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	إذا (!val || len <= 0)
		أرجع SBI_EINVAL;
	nr_pins = fdt32_منزل_cpu(*val);

	chip = sbi_zalloc(حجم(*chip));
	إذا (!chip)
		أرجع SBI_ENOMEM;

	chip->dr = (فراغ *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (فراغ *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.سېټ = dw_gpio_سېټ;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_جمع_کړه(&chip->chip);
	إذا (rc)
		اذهب err_free_chip;

	أرجع 0;

err_free_chip:
	sbi_free(chip);
	أرجع rc;
}

/* since we're only probed when used, match on port not main controller node */
ساكن ثابت بنية fdt_سمون dw_gpio_سمون[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ثابت بنية fdt_gpio fdt_gpio_designware = {
	.driver = {
		.سمون_جدول = dw_gpio_سمون,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
