/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SWRSTREQ_CTRL_REG_ਵਿੱਥ 0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SWRSTREQ_REG_ਵਿੱਥ 0x04

ਬਣਤਰ reset_metanoia_ਅੰਕੜੇ {
	u8 *reg_ਅਧਾਰ;
};

ਸਥਿਰ ਬਣਤਰ reset_metanoia_ਅੰਕੜੇ reset_ਅੰਕੜੇ;

ਸਥਿਰ ਪੂਰਨ metanoia_system_reset_check(u32 type, u32 reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ਵਾਪਸ 1;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ਮੂਲ:
		ਵਾਪਸ 0;
	}
}

ਸਥਿਰ ਖਾਲੀ metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_ਅੰਕੜੇ.reg_ਅਧਾਰ + SWRSTREQ_CTRL_REG_ਵਿੱਥ);
	writew(0x1, reset_ਅੰਕੜੇ.reg_ਅਧਾਰ + SWRSTREQ_REG_ਵਿੱਥ);
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ਸਥਿਰ ਪੂਰਨ metanoia_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			       ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	u64 reg_addr;
	ਪੂਰਨ rc;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, ਨਿਰਾ);
	ਜੇ (rc < 0 || !reg_addr)
		ਵਾਪਸ SBI_ENODEV;

	reset_ਅੰਕੜੇ.reg_ਅਧਾਰ = (u8 *)(ulong)reg_addr;

	sbi_system_reset_ਜੋੜੋ_device(&metanoia_reset);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ metanoia_reset_ਮੇਲ[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_metanoia = {
	.ਮੇਲ_ਸਾਰਣੀ = metanoia_reset_ਮੇਲ,
	.init	     = metanoia_reset_init,
};
