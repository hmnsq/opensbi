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

#include <libfdt.h>

#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/gpio/fdt_gpio.h>

#define DW_GPIO_PINS_MAX	32

#define DW_GPIO_DDR	0x4
#define DW_GPIO_DR	0x0
#define DW_GPIO_BIT(_b)	(1UL << (_b))

sa_nracanaa dw_gpio_chip {
	void *dr;
	void *ext;
	sa_nracanaa gpio_chip chip;
};

const sa_nracanaa fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, sa_nracanaa dw_gpio_chip, chip);

static ຈຳນວນເຕັມ dw_gpio_direction_output(sa_nracanaa gpio_pin *gp, ຈຳນວນເຕັມ value)
{
	sa_nracanaa dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	ຖ້າ (!value)
		v &= ~DW_GPIO_BIT(gp->offset);
	ບໍ່ດັ່ງນັ້ນ
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	ສົ່ງຄືນ 0;
}

static void dw_gpio_set(sa_nracanaa gpio_pin *gp, ຈຳນວນເຕັມ value)
{
	sa_nracanaa dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	ຖ້າ (!value)
		v &= ~DW_GPIO_BIT(gp->offset);
	ບໍ່ດັ່ງນັ້ນ
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

static ຈຳນວນເຕັມ dw_gpio_init_bank(const void *fdt, ຈຳນວນເຕັມ nodeoff,
			     const sa_nracanaa fdt_match *match)
{
	sa_nracanaa dw_gpio_chip *chip;
	const fdt32_t *val;
	uint64_t addr;
	ຈຳນວນເຕັມ rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	ຖ້າ (poff < 0)
		ສົ່ງຄືນ SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, poff, 0, &addr, NULL);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	ຖ້າ (!val || len <= 0)
		ສົ່ງຄືນ SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	ຖ້າ (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	ຖ້າ (!val || len <= 0)
		ສົ່ງຄືນ SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(parimaa_na(*chip));
	ຖ້າ (!chip)
		ສົ່ງຄືນ SBI_ENOMEM;

	chip->dr = (void *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (void *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	ຖ້າ (rc)
		goto err_free_chip;

	ສົ່ງຄືນ 0;

err_free_chip:
	sbi_free(chip);
	ສົ່ງຄືນ rc;
}

/* since we're only probed when used, match on port not main controller node */
static const sa_nracanaa fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

const sa_nracanaa fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
