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

#تعریف_کرو DW_GPIO_PINS_MAX	32

#تعریف_کرو DW_GPIO_DDR	0x4
#تعریف_کرو DW_GPIO_DR	0x0
#تعریف_کرو DW_GPIO_ثنائی_ہندسہ(_b)	(1UL << (_b))

ساخت dw_gpio_chip {
	خالی *dr;
	خالی *ext;
	ساخت gpio_chip chip;
};

ثابت ساخت fdt_gpio fdt_gpio_designware;

#تعریف_کرو pin_مقصد_chip(__p) container_کا((__p)->chip, ساخت dw_gpio_chip, chip);

ساکن صحیح dw_gpio_direction_output(ساخت gpio_pin *gp, صحیح قدر)
{
	ساخت dw_gpio_chip *chip = pin_مقصد_chip(gp);
	غیرعلامتی طویل v;

	v = readl(chip->dr + DW_GPIO_DR);
	اگر (!قدر)
		v &= ~DW_GPIO_ثنائی_ہندسہ(gp->انحراف);
	ورنہ
		v |= DW_GPIO_ثنائی_ہندسہ(gp->انحراف);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_ثنائی_ہندسہ(gp->انحراف);
	writel(v, chip->dr + DW_GPIO_DDR);

	واپس 0;
}

ساکن خالی dw_gpio_سیٹ(ساخت gpio_pin *gp, صحیح قدر)
{
	ساخت dw_gpio_chip *chip = pin_مقصد_chip(gp);
	غیرعلامتی طویل v;

	v = readl(chip->dr + DW_GPIO_DR);
	اگر (!قدر)
		v &= ~DW_GPIO_ثنائی_ہندسہ(gp->انحراف);
	ورنہ
		v |= DW_GPIO_ثنائی_ہندسہ(gp->انحراف);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

ساکن صحیح dw_gpio_init_bank(ثابت خالی *fdt, صحیح nodeoff,
			     ثابت ساخت fdt_مطابقت *مطابقت)
{
	ساخت dw_gpio_chip *chip;
	ثابت fdt32_t *val;
	uint64_t addr;
	صحیح rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_والد_انحراف(fdt, nodeoff);
	اگر (poff < 0)
		واپس SBI_EINVAL;

	rc = fdt_get_گرہ_addr_سائز(fdt, poff, 0, &addr, تہی);
	اگر (rc)
		واپس rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	اگر (!val || len <= 0)
		واپس SBI_EINVAL;
	bank = fdt32_مقصد_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	اگر (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	اگر (!val || len <= 0)
		واپس SBI_EINVAL;
	nr_pins = fdt32_مقصد_cpu(*val);

	chip = sbi_zalloc(حجم(*chip));
	اگر (!chip)
		واپس SBI_ENOMEM;

	chip->dr = (خالی *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (خالی *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.سیٹ = dw_gpio_سیٹ;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_جوڑو(&chip->chip);
	اگر (rc)
		جاؤ err_free_chip;

	واپس 0;

err_free_chip:
	sbi_free(chip);
	واپس rc;
}

/* since we're only probed when used, match on port not main controller node */
ساکن ثابت ساخت fdt_مطابقت dw_gpio_مطابقت[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ثابت ساخت fdt_gpio fdt_gpio_designware = {
	.driver = {
		.مطابقت_جدول = dw_gpio_مطابقت,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
