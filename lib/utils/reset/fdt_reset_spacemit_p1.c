/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <platform_override.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

સ્થિર સંરચના i2c_adapter *p1_adapter = ખાલી;
સ્થિર uint32_t p1_reg = 0;

સ્થિર પૂર્ણાંક p1_system_reset_check(uint32_t type, uint32_t reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
		પાછા 1;
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		પાછા 255;
	}

	પાછા 0;
}

સ્થિર શૂન્ય p1_ops(uint32_t type)
{
	uint8_t byte;
	પૂર્ણાંક rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	જો (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		પાછા;
	}

	જો (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	નહીંતર
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	જો (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

સ્થિર શૂન્ય p1_system_reset(uint32_t type, uint32_t reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		તોડો;
	}
}

સ્થિર સંરચના sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

સ્થિર પૂર્ણાંક p1_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			 અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, ખાલી);
	જો (rc)
		પાછા rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	જો (i2c_bus < 0)
		પાછા i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	જો (rc)
		પાછા rc;

	sbi_system_reset_add_device(&p1_reset);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

અચલ સંરચના fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
