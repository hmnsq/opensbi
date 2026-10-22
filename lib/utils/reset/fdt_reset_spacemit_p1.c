/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <platform_override.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

സ്ഥിതം ഘടന i2c_adapter *p1_adapter = ശൂന്യ;
സ്ഥിതം uint32_t p1_reg = 0;

സ്ഥിതം പൂർണ്ണസംഖ്യ p1_system_reset_check(uint32_t type, uint32_t reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
		തിരികെ 1;
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		തിരികെ 255;
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം p1_ops(uint32_t type)
{
	uint8_t byte;
	പൂർണ്ണസംഖ്യ rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	എങ്കിൽ (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		തിരികെ;
	}

	എങ്കിൽ (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	അല്ലെങ്കിൽ
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	എങ്കിൽ (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

സ്ഥിതം ശൂന്യം p1_system_reset(uint32_t type, uint32_t reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		നിർത്തുക;
	}
}

സ്ഥിതം ഘടന sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

സ്ഥിതം പൂർണ്ണസംഖ്യ p1_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			 സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (rc)
		തിരികെ rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	എങ്കിൽ (i2c_bus < 0)
		തിരികെ i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	എങ്കിൽ (rc)
		തിരികെ rc;

	sbi_system_reset_add_device(&p1_reset);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
