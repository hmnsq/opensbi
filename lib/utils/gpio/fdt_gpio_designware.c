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

#అన్తర్భావ <libfdt.h>

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>

#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/gpio/fdt_gpio.h>

#నిర్వచించు DW_GPIO_PINS_MAX	32

#నిర్వచించు DW_GPIO_DDR	0x4
#నిర్వచించు DW_GPIO_DR	0x0
#నిర్వచించు DW_GPIO_బిట్టు(_b)	(1UL << (_b))

నిర్మాణం dw_gpio_chip {
	శూన్య *dr;
	శూన్య *ext;
	నిర్మాణం gpio_chip chip;
};

స్థిరరాశి నిర్మాణం fdt_gpio fdt_gpio_designware;

#నిర్వచించు pin_గమ్యం_chip(__p) container_యొక్క((__p)->chip, నిర్మాణం dw_gpio_chip, chip);

స్థిర పూర్ణాంకం dw_gpio_direction_output(నిర్మాణం gpio_pin *gp, పూర్ణాంకం విలువ)
{
	నిర్మాణం dw_gpio_chip *chip = pin_గమ్యం_chip(gp);
	గుర్తులేని దీర్ఘ v;

	v = readl(chip->dr + DW_GPIO_DR);
	ఒకవేళ (!విలువ)
		v &= ~DW_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	లేకపోతే
		v |= DW_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	writel(v, chip->dr + DW_GPIO_DDR);

	తిరిగి 0;
}

స్థిర శూన్య dw_gpio_సమితి(నిర్మాణం gpio_pin *gp, పూర్ణాంకం విలువ)
{
	నిర్మాణం dw_gpio_chip *chip = pin_గమ్యం_chip(gp);
	గుర్తులేని దీర్ఘ v;

	v = readl(chip->dr + DW_GPIO_DR);
	ఒకవేళ (!విలువ)
		v &= ~DW_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	లేకపోతే
		v |= DW_GPIO_బిట్టు(gp->స్థానభ్రంశం);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

స్థిర పూర్ణాంకం dw_gpio_init_bank(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			     స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	నిర్మాణం dw_gpio_chip *chip;
	స్థిరరాశి fdt32_t *val;
	uint64_t addr;
	పూర్ణాంకం rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_మాతృ_స్థానభ్రంశం(fdt, nodeoff);
	ఒకవేళ (poff < 0)
		తిరిగి SBI_EINVAL;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, poff, 0, &addr, ఖాళీ);
	ఒకవేళ (rc)
		తిరిగి rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	ఒకవేళ (!val || len <= 0)
		తిరిగి SBI_EINVAL;
	bank = fdt32_గమ్యం_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	ఒకవేళ (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	ఒకవేళ (!val || len <= 0)
		తిరిగి SBI_EINVAL;
	nr_pins = fdt32_గమ్యం_cpu(*val);

	chip = sbi_zalloc(పరిమాణం(*chip));
	ఒకవేళ (!chip)
		తిరిగి SBI_ENOMEM;

	chip->dr = (శూన్య *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (శూన్య *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.సమితి = dw_gpio_సమితి;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_కూడు(&chip->chip);
	ఒకవేళ (rc)
		వెళ్ళు err_free_chip;

	తిరిగి 0;

err_free_chip:
	sbi_free(chip);
	తిరిగి rc;
}

/* since we're only probed when used, match on port not main controller node */
స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక dw_gpio_సరిపోలిక[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_gpio fdt_gpio_designware = {
	.driver = {
		.సరిపోలిక_పట్టిక = dw_gpio_సరిపోలిక,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
