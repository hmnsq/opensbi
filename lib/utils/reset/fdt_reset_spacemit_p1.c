/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#include <libfdt.h>
#include <platform_override.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_system.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

static struct i2c_adapter *p1_adapter = NULL;
static uint32_t p1_reg = 0;

static цяло p1_system_reset_check(uint32_t type, uint32_t reason)
{
	switch (type) {
	case SBI_SRST_RESET_TYPE_SHUTDOWN:
		върни 1;
	case SBI_SRST_RESET_TYPE_COLD_REBOOT:
	case SBI_SRST_RESET_TYPE_WARM_REBOOT:
		върни 255;
	}

	върни 0;
}

static void p1_ops(uint32_t type)
{
	uint8_t byte;
	цяло rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	ако (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		върни;
	}

	ако (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	иначе
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	ако (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

static void p1_system_reset(uint32_t type, uint32_t reason)
{
	switch (type) {
	case SBI_SRST_RESET_TYPE_SHUTDOWN:
	case SBI_SRST_RESET_TYPE_COLD_REBOOT:
	case SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		break;
	}
}

static struct sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

static цяло p1_reset_init(const void *fdt, цяло nodeoff,
			 const struct fdt_match *match)
{
	цяло rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	ако (rc)
		върни rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	ако (i2c_bus < 0)
		върни i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	ако (rc)
		върни rc;

	sbi_system_reset_add_device(&p1_reset);

	върни 0;
}

static const struct fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

const struct fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
