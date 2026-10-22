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

#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>

#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

સંરચના dw_gpio_chip {
	શૂન્ય *dr;
	શૂન્ય *ext;
	સંરચના gpio_chip chip;
};

અચલ સંરચના fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, સંરચના dw_gpio_chip, chip);

સ્થિર પૂર્ણાંક dw_gpio_direction_output(સંરચના gpio_pin *gp, પૂર્ણાંક કિંમત)
{
	સંરચના dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	જો (!કિંમત)
		v &= ~DW_GPIO_BIT(gp->offset);
	નહીંતર
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	પાછા 0;
}

સ્થિર શૂન્ય dw_gpio_set(સંરચના gpio_pin *gp, પૂર્ણાંક કિંમત)
{
	સંરચના dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	જો (!કિંમત)
		v &= ~DW_GPIO_BIT(gp->offset);
	નહીંતર
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

સ્થિર પૂર્ણાંક dw_gpio_init_bank(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			     અચલ સંરચના fdt_match *match)
{
	સંરચના dw_gpio_chip *chip;
	અચલ fdt32_t *val;
	uint64_t addr;
	પૂર્ણાંક rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	જો (poff < 0)
		પાછા SBI_EINVAL;

	rc = fdt_get_ગાંઠ_addr_size(fdt, poff, 0, &addr, ખાલી);
	જો (rc)
		પાછા rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	જો (!val || len <= 0)
		પાછા SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	જો (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	જો (!val || len <= 0)
		પાછા SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(કદ(*chip));
	જો (!chip)
		પાછા SBI_ENOMEM;

	chip->dr = (શૂન્ય *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (શૂન્ય *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	જો (rc)
		જાઓ err_free_chip;

	પાછા 0;

err_free_chip:
	sbi_free(chip);
	પાછા rc;
}

/* since we're only probed when used, match on port not main controller node */
સ્થિર અચલ સંરચના fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

અચલ સંરચના fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
