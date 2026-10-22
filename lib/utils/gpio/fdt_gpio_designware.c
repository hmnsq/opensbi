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

#समावेश <libfdt.h>

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>

#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/gpio/fdt_gpio.h>

#घोषणा DW_GPIO_PINS_MAX	32

#घोषणा DW_GPIO_DDR	0x4
#घोषणा DW_GPIO_DR	0x0
#घोषणा DW_GPIO_BIT(_b)	(1UL << (_b))

काष्ठा dw_gpio_chip {
	व्योम *dr;
	व्योम *ext;
	काष्ठा gpio_chip chip;
};

स्थिर काष्ठा fdt_gpio fdt_gpio_designware;

#घोषणा pin_to_chip(__p) container_of((__p)->chip, काष्ठा dw_gpio_chip, chip);

अटल पूर्णांक dw_gpio_direction_output(काष्ठा gpio_pin *gp, पूर्णांक मूल्य)
{
	काष्ठा dw_gpio_chip *chip = pin_to_chip(gp);
	अचिन्हित दीर्घ v;

	v = readl(chip->dr + DW_GPIO_DR);
	अगर (!मूल्य)
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

	वापस 0;
}

अटल व्योम dw_gpio_set(काष्ठा gpio_pin *gp, पूर्णांक मूल्य)
{
	काष्ठा dw_gpio_chip *chip = pin_to_chip(gp);
	अचिन्हित दीर्घ v;

	v = readl(chip->dr + DW_GPIO_DR);
	अगर (!मूल्य)
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

अटल पूर्णांक dw_gpio_init_bank(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			     स्थिर काष्ठा fdt_match *match)
{
	काष्ठा dw_gpio_chip *chip;
	स्थिर fdt32_t *val;
	uint64_t addr;
	पूर्णांक rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	अगर (poff < 0)
		वापस SBI_EINVAL;

	rc = fdt_get_गाँठ_addr_आकार(fdt, poff, 0, &addr, शून्य);
	अगर (rc)
		वापस rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	अगर (!val || len <= 0)
		वापस SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	अगर (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	अगर (!val || len <= 0)
		वापस SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(माप(*chip));
	अगर (!chip)
		वापस SBI_ENOMEM;

	chip->dr = (व्योम *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (व्योम *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	अगर (rc)
		जाओ err_free_chip;

	वापस 0;

err_free_chip:
	sbi_free(chip);
	वापस rc;
}

/* since we're only probed when used, match on port not main controller node */
अटल स्थिर काष्ठा fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

स्थिर काष्ठा fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
