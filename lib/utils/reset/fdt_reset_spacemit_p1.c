/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <platform_override.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

நிலையான கட்டமைப்பு i2c_adapter *p1_adapter = வெற்று;
நிலையான uint32_t p1_reg = 0;

நிலையான முழு p1_system_reset_check(uint32_t type, uint32_t reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
		திரும்பு 1;
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		திரும்பு 255;
	}

	திரும்பு 0;
}

நிலையான வெறுமை p1_ops(uint32_t type)
{
	uint8_t byte;
	முழு rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	எனில் (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		திரும்பு;
	}

	எனில் (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	இல்லையெனில்
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	எனில் (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

நிலையான வெறுமை p1_system_reset(uint32_t type, uint32_t reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		நிறுத்து;
	}
}

நிலையான கட்டமைப்பு sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

நிலையான முழு p1_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			 மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, வெற்று);
	எனில் (rc)
		திரும்பு rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	எனில் (i2c_bus < 0)
		திரும்பு i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	எனில் (rc)
		திரும்பு rc;

	sbi_system_reset_add_device(&p1_reset);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
