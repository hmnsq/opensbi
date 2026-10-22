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

#ವ್ಯಾಖ್ಯಾನಿಸು DW_GPIO_PINS_MAX	32

#ವ್ಯಾಖ್ಯಾನಿಸು DW_GPIO_DDR	0x4
#ವ್ಯಾಖ್ಯಾನಿಸು DW_GPIO_DR	0x0
#ವ್ಯಾಖ್ಯಾನಿಸು DW_GPIO_ಬಿಟ್(_b)	(1UL << (_b))

ರಚನೆ dw_gpio_chip {
	ಶೂನ್ಯ *dr;
	ಶೂನ್ಯ *ext;
	ರಚನೆ gpio_chip chip;
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_gpio fdt_gpio_designware;

#ವ್ಯಾಖ್ಯಾನಿಸು pin_ಗಮ್ಯ_chip(__p) container_ದ((__p)->chip, ರಚನೆ dw_gpio_chip, chip);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dw_gpio_direction_output(ರಚನೆ gpio_pin *gp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	ರಚನೆ dw_gpio_chip *chip = pin_ಗಮ್ಯ_chip(gp);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ v;

	v = readl(chip->dr + DW_GPIO_DR);
	ಆದರೆ (!ಮೌಲ್ಯ)
		v &= ~DW_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ
		v |= DW_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	writel(v, chip->dr + DW_GPIO_DDR);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ dw_gpio_ಗಣ(ರಚನೆ gpio_pin *gp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	ರಚನೆ dw_gpio_chip *chip = pin_ಗಮ್ಯ_chip(gp);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ v;

	v = readl(chip->dr + DW_GPIO_DR);
	ಆದರೆ (!ಮೌಲ್ಯ)
		v &= ~DW_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ
		v |= DW_GPIO_ಬಿಟ್(gp->ಸ್ಥಾನಾಂತರ);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dw_gpio_init_bank(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			     ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ರಚನೆ dw_gpio_chip *chip;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	uint64_t addr;
	ಪೂರ್ಣಾಂಕ rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, nodeoff);
	ಆದರೆ (poff < 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, poff, 0, &addr, ಖಾಲಿ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	ಆದರೆ (!val || len <= 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	bank = fdt32_ಗಮ್ಯ_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	ಆದರೆ (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	ಆದರೆ (!val || len <= 0)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	nr_pins = fdt32_ಗಮ್ಯ_cpu(*val);

	chip = sbi_zalloc(ಗಾತ್ರ(*chip));
	ಆದರೆ (!chip)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	chip->dr = (ಶೂನ್ಯ *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (ಶೂನ್ಯ *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.ಗಣ = dw_gpio_ಗಣ;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_ಕೂಡಿಸು(&chip->chip);
	ಆದರೆ (rc)
		ಹೋಗು err_free_chip;

	ಹಿಂತಿರುಗಿ 0;

err_free_chip:
	sbi_free(chip);
	ಹಿಂತಿರುಗಿ rc;
}

/* since we're only probed when used, match on port not main controller node */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ dw_gpio_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_gpio fdt_gpio_designware = {
	.driver = {
		.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = dw_gpio_ಹೊಂದಿಕೆ,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
