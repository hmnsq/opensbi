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

static sa_nracanaa i2c_adapter *p1_adapter = NULL;
static uint32_t p1_reg = 0;

static puur_naa_nka p1_system_reset_check(uint32_t type, uint32_t reason)
{
	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
		ᬯᬮᬶ 1;
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ᬯᬮᬶ 255;
	}

	ᬯᬮᬶ 0;
}

static void p1_ops(uint32_t type)
{
	uint8_t byte;
	puur_naa_nka rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	ᬬᬾᬦ᭄ (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		ᬯᬮᬶ;
	}

	ᬬᬾᬦ᭄ (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	ᬮᬶᬬ
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	ᬬᬾᬦ᭄ (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

static void p1_system_reset(uint32_t type, uint32_t reason)
{
	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		bha_nga;
	}
}

static sa_nracanaa sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

static puur_naa_nka p1_reset_init(const void *fdt, puur_naa_nka nodeoff,
			 const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	ᬬᬾᬦ᭄ (rc)
		ᬯᬮᬶ rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	ᬬᬾᬦ᭄ (i2c_bus < 0)
		ᬯᬮᬶ i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	ᬬᬾᬦ᭄ (rc)
		ᬯᬮᬶ rc;

	sbi_system_reset_add_device(&p1_reset);

	ᬯᬮᬶ 0;
}

static const sa_nracanaa fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

const sa_nracanaa fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
