/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATCWDT200_WP_ਅੰਕ 0x5aa5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WREN_REG 0x18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_REG 0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RST_ਸਮਾਂ_OFF 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RST_ਸਮਾਂ_MSK (0x3 << RST_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RST_CLK_128 (0 << RST_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RST_CLK_256 (1 << RST_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RST_CLK_512 (2 << RST_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RST_CLK_1024 (3 << RST_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_ਸਮਾਂ_OFF 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_ਸਮਾਂ_MSK (0xf << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_CLK_64 (0 << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_CLK_256 (1 << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_CLK_1024 (2 << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_CLK_2048 (3 << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_CLK_4096 (4 << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_CLK_8192 (5 << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_CLK_16384 (6 << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_CLK_32768 (7 << INT_ਸਮਾਂ_OFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RST_EN (1 << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_EN (1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLK_PCLK (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_EN (1 << 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AE350_FLASH_ਅਧਾਰ 0x80000000

ਸਥਿਰ ਅਸਥਿਰ ਅੱਖਰ *wdt_addr = ਨਿਰਾ;

ਸਥਿਰ ਪੂਰਨ ae350_system_reset_check(u32 type, u32 reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ਵਾਪਸ 1;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ਮੂਲ:
		ਵਾਪਸ 0;
	}
}

ਸਥਿਰ ਖਾਲੀ ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		ਜੇ (atcsmu_ਸੈੱਟ_reset_ਸਦਿਸ਼(AE350_FLASH_ਅਧਾਰ, i))
			ਜਾਓ fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_ਅੰਕ, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

ਸਥਿਰ ਪੂਰਨ atcwdt200_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	uint64_t reg_addr;
	ਪੂਰਨ rc;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, ਨਿਰਾ);
	ਜੇ (rc < 0 || !reg_addr)
		ਵਾਪਸ SBI_ENODEV;

	wdt_addr = (ਅਸਥਿਰ ਅੱਖਰ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)reg_addr;
	sbi_system_reset_ਜੋੜੋ_device(&atcwdt200_reset);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ atcwdt200_reset_ਮੇਲ[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_atcwdt200 = {
	.ਮੇਲ_ਸਾਰਣੀ = atcwdt200_reset_ਮੇਲ,
	.init	     = atcwdt200_reset_init,
};
