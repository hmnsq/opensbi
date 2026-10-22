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

#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

ಸಂರಚನಾ dw_gpio_chip {
	ಶೂನ್ಯ *dr;
	ಶೂನ್ಯ *ext;
	ಸಂರಚನಾ gpio_chip chip;
};

ಧ್ರುವ ಸಂರಚನಾ fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, ಸಂರಚನಾ dw_gpio_chip, chip);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dw_gpio_direction_output(ಸಂರಚನಾ gpio_pin *gp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	ಸಂರಚನಾ dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	ಯದಿ (!ಮೌಲ್ಯ)
		v &= ~DW_GPIO_BIT(gp->offset);
	ಅನ್ಯಥಾ
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ dw_gpio_set(ಸಂರಚನಾ gpio_pin *gp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	ಸಂರಚನಾ dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	ಯದಿ (!ಮೌಲ್ಯ)
		v &= ~DW_GPIO_BIT(gp->offset);
	ಅನ್ಯಥಾ
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dw_gpio_init_bank(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			     ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಸಂರಚನಾ dw_gpio_chip *chip;
	ಧ್ರುವ fdt32_t *val;
	uint64_t addr;
	ಪೂರ್ಣಾಙ್ಕ rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	ಯದಿ (poff < 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, poff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	ಯದಿ (!val || len <= 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	ಯದಿ (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	ಯದಿ (!val || len <= 0)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(ಪರಿಮಾಣ(*chip));
	ಯದಿ (!chip)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	chip->dr = (ಶೂನ್ಯ *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (ಶೂನ್ಯ *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	ಯದಿ (rc)
		ಗಚ್ಛ err_free_chip;

	ಪ್ರತ್ಯಾಗಮ 0;

err_free_chip:
	sbi_free(chip);
	ಪ್ರತ್ಯಾಗಮ rc;
}

/* since we're only probed when used, match on port not main controller node */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
