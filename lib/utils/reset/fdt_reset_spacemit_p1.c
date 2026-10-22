/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <platform_override.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PWR_CTRL2		0x7e
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PWR_CTRL2_SHUTDOWN	ਦੋਆਧਾਰੀ_ਅੰਕ(2)	/* Shutdown request */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PWR_CTRL2_RST		ਦੋਆਧਾਰੀ_ਅੰਕ(1)	/* Reset request */

ਸਥਿਰ ਬਣਤਰ i2c_adapter *p1_adapter = ਨਿਰਾ;
ਸਥਿਰ uint32_t p1_reg = 0;

ਸਥਿਰ ਪੂਰਨ p1_system_reset_check(uint32_t type, uint32_t reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ਵਾਪਸ 1;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ਵਾਪਸ 255;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ p1_ops(uint32_t type)
{
	uint8_t byte;
	ਪੂਰਨ rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	ਜੇ (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		ਵਾਪਸ;
	}

	ਜੇ (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	ਨਹੀਂਤਾਂ
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	ਜੇ (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

ਸਥਿਰ ਖਾਲੀ p1_system_reset(uint32_t type, uint32_t reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		ਰੋਕੋ;
	}
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

ਸਥਿਰ ਪੂਰਨ p1_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			 ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, ਨਿਰਾ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_ਮਾਪਾ_ਵਿੱਥ(fdt, nodeoff);
	ਜੇ (i2c_bus < 0)
		ਵਾਪਸ i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sbi_system_reset_ਜੋੜੋ_device(&p1_reset);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ p1_reset_ਮੇਲ[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_spacemit_p1 = {
	.ਮੇਲ_ਸਾਰਣੀ = p1_reset_ਮੇਲ,
	.init = p1_reset_init,
};
