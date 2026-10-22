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

struct dw_gpio_chip {
	void *dr;
	void *ext;
	struct gpio_chip chip;
};

const struct fdt_gpio fdt_gpio_designware;

#define pin_to_chip(__p) container_of((__p)->chip, struct dw_gpio_chip, chip);

static цео dw_gpio_direction_output(struct gpio_pin *gp, цео value)
{
	struct dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	ако (!value)
		v &= ~DW_GPIO_BIT(gp->offset);
	инако
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);

	/* the DR is output only so we can set it then the DDR to set
	 * the data direction, to avoid glitches.
	 */
	v = readl(chip->dr + DW_GPIO_DDR);
	v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DDR);

	врати 0;
}

static void dw_gpio_set(struct gpio_pin *gp, цео value)
{
	struct dw_gpio_chip *chip = pin_to_chip(gp);
	unsigned long v;

	v = readl(chip->dr + DW_GPIO_DR);
	ако (!value)
		v &= ~DW_GPIO_BIT(gp->offset);
	инако
		v |= DW_GPIO_BIT(gp->offset);
	writel(v, chip->dr + DW_GPIO_DR);
}

/* notes:
 * each sub node is a bank and has ngpios or snpns,nr-gpios and a reg property
 * with the compatible `snps,dw-apb-gpio-port`.
 * bank A is the only one with irq support but we're not using it here
*/

static цео dw_gpio_init_bank(const void *fdt, цео nodeoff,
			     const struct fdt_match *match)
{
	struct dw_gpio_chip *chip;
	const fdt32_t *val;
	uint64_t addr;
	цео rc, poff, nr_pins, bank, len;

	/* need to get parent for the address property  */
	poff = fdt_parent_offset(fdt, nodeoff);
	ако (poff < 0)
		врати SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, poff, 0, &addr, NULL);
	ако (rc)
		врати rc;

	val = fdt_getprop(fdt, nodeoff, "reg", &len);
	ако (!val || len <= 0)
		врати SBI_EINVAL;
	bank = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "snps,nr-gpios", &len);
	ако (!val)
		val = fdt_getprop(fdt, nodeoff, "ngpios", &len);
	ако (!val || len <= 0)
		врати SBI_EINVAL;
	nr_pins = fdt32_to_cpu(*val);

	chip = sbi_zalloc(sizeof(*chip));
	ако (!chip)
		врати SBI_ENOMEM;

	chip->dr = (void *)(uintptr_t)addr + (bank * 0xc);
	chip->ext = (void *)(uintptr_t)addr + (bank * 4) + 0x50;
	chip->chip.driver = &fdt_gpio_designware;
	chip->chip.id = nodeoff;
	chip->chip.ngpio = nr_pins;
	chip->chip.set = dw_gpio_set;
	chip->chip.direction_output = dw_gpio_direction_output;
	rc = gpio_chip_add(&chip->chip);
	ако (rc)
		goto err_free_chip;

	врати 0;

err_free_chip:
	sbi_free(chip);
	врати rc;
}

/* since we're only probed when used, match on port not main controller node */
static const struct fdt_match dw_gpio_match[] = {
	{ .compatible = "snps,dw-apb-gpio-port" },
	{ },
};

const struct fdt_gpio fdt_gpio_designware = {
	.driver = {
		.match_table = dw_gpio_match,
		.init = dw_gpio_init_bank,
	},
	.xlate = fdt_gpio_simple_xlate,
};
