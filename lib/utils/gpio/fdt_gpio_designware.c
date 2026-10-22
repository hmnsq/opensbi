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

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

संरचना dw_gpio_chip {
	शून्य *dr;
	शून्य *ext;
	संरचना gpio_chip chip;
};

अचर संरचना fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, संरचना dw_gpio_chip, chip);

स्थिर पूर्णाङ्क dw_gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मान)
{
	संरचना dw_gpio_chip *chip = pin_to_chip(gp);
	अचिन्हित दीर्घ v;

	v = readl(chip->dr + DW_GPIO_DR);
	यदि (!मान)
		v &= ~DW_GPIO_BIT(gp->offset);
	अन्यथा
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	फर्काऊ 0;
}

स्थिर शून्य dw_gpio_set(संरचना gpio_pin *gp, पूर्णाङ्क मान)
{
	संरचना dw_gpio_chip *chip = pin_to_chip(gp);
	अचिन्हित दीर्घ v;

	v = readl(chip->dr + DW_GPIO_DR);
	यदि (!मान)
		v &= ~DW_GPIO_BIT(gp->offset);
	अन्यथा
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

स्थिर पूर्णाङ्क dw_gpio_init_bank(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			     अचर संरचना fdt_match *match)
{
	संरचना dw_gpio_chip *chip;
	अचर fdt32_t *val;
	uint64_t addr;
	पूर्णाङ्क rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	यदि (poff < 0)
		फर्काऊ SBI_EINVAL;

	rc = fdt_get_गाँठो_addr_size(fdt, poff, 0, &addr, रिक्त);
	यदि (rc)
		फर्काऊ rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	यदि (!val || len <= 0)
		फर्काऊ SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	यदि (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	यदि (!val || len <= 0)
		फर्काऊ SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(परिमाण(*chip));
	यदि (!chip)
		फर्काऊ SBI_ENOMEM;

	chip->dr = (शून्य *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (शून्य *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	यदि (rc)
		गच्छ err_free_chip;

	फर्काऊ 0;

err_free_chip:
	sbi_free(chip);
	फर्काऊ rc;
}

/* since we're only probed when used, match on port not main controller node */
स्थिर अचर संरचना fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

अचर संरचना fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
