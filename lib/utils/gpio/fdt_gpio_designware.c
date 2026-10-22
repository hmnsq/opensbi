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

#अन्तर्भाव <libfdt.h>

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/gpio/fdt_gpio.h>

#परिभाषय DW_GPIO_PINS_MAX	32

#परिभाषय DW_GPIO_DDR	0x4
#परिभाषय DW_GPIO_DR	0x0
#परिभाषय DW_GPIO_BIT(_b)	(1UL << (_b))

संरचना dw_gpio_chip {
	शून्य *dr;
	शून्य *ext;
	संरचना gpio_chip chip;
};

ध्रुव संरचना fdt_gpio fdt_gpio_designware;

#परिभाषय pin_गन्तव्यम्_chip(__p) container_सम्बन्धि((__p)->chip, संरचना dw_gpio_chip, chip);

स्थिर पूर्णाङ्क dw_gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	संरचना dw_gpio_chip *chip = pin_गन्तव्यम्_chip(gp);
	अचिह्नित दीर्घ v;

	v = readl(chip->dr + DW_GPIO_DR);
	यदि (!मानम्)
		v &= ~DW_GPIO_BIT(gp->विस्थापनम्);
	अन्यथा
		v |= DW_GPIO_BIT(gp->विस्थापनम्);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->विस्थापनम्);
	writel(v, chip->dr + DW_GPIO_DDR);

	प्रत्यागम 0;
}

स्थिर शून्य dw_gpio_समुच्चयः(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	संरचना dw_gpio_chip *chip = pin_गन्तव्यम्_chip(gp);
	अचिह्नित दीर्घ v;

	v = readl(chip->dr + DW_GPIO_DR);
	यदि (!मानम्)
		v &= ~DW_GPIO_BIT(gp->विस्थापनम्);
	अन्यथा
		v |= DW_GPIO_BIT(gp->विस्थापनम्);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

स्थिर पूर्णाङ्क dw_gpio_init_bank(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			     ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	संरचना dw_gpio_chip *chip;
	ध्रुव fdt32_t *val;
	uint64_t addr;
	पूर्णाङ्क rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_विस्थापनम्(fdt, nodeoff);
	यदि (poff < 0)
		प्रत्यागम SBI_EINVAL;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, poff, 0, &addr, रिक्त);
	यदि (rc)
		प्रत्यागम rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	यदि (!val || len <= 0)
		प्रत्यागम SBI_EINVAL;
	bank = fdt32_गन्तव्यम्_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	यदि (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	यदि (!val || len <= 0)
		प्रत्यागम SBI_EINVAL;
	nr_pins = fdt32_गन्तव्यम्_cpu(*val);

	chip = sbi_zalloc(परिमाण(*chip));
	यदि (!chip)
		प्रत्यागम SBI_ENOMEM;

	chip->dr = (शून्य *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (शून्य *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.समुच्चयः = dw_gpio_समुच्चयः;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_सङ्कलय(&chip->chip);
	यदि (rc)
		गच्छ err_free_chip;

	प्रत्यागम 0;

err_free_chip:
	sbi_free(chip);
	प्रत्यागम rc;
}

/* since we're only probed when used, match on port not main controller node */
स्थिर ध्रुव संरचना fdt_मेलनम् dw_gpio_मेलनम्[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ध्रुव संरचना fdt_gpio fdt_gpio_designware = {
	.driver = {
		.मेलनम्_सारणी = dw_gpio_मेलनम्,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
