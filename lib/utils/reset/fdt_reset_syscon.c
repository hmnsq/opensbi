/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/regmap/fdt_regmap.h>

ഘടന syscon_reset {
	ഘടന regmap *rmap;
	u32 priority;
	u32 offset;
	u32 മൂല്യം;
	u32 mask;
};

സ്ഥിതം ഘടന syscon_reset poweroff;
സ്ഥിതം ഘടന syscon_reset reboot;

സ്ഥിതം ഘടന syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	ഘടന syscon_reset *reset = ശൂന്യ;

	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
		എങ്കിൽ (!is_reboot)
			reset = &poweroff;
		നിർത്തുക;
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		എങ്കിൽ (is_reboot)
			reset = &reboot;
		നിർത്തുക;
	}

	എങ്കിൽ (reset && !reset->rmap)
		reset = ശൂന്യ;

	തിരികെ reset;
}

സ്ഥിതം ശൂന്യം syscon_reset_exec(ഘടന syscon_reset *reset)
{
	/* Issue the reset through regmap */
	എങ്കിൽ (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->മൂല്യം);

	/* hang !!! */
	sbi_hart_hang();
}

സ്ഥിതം പൂർണ്ണസംഖ്യ syscon_poweroff_check(u32 type, u32 reason)
{
	ഘടന syscon_reset *reset = syscon_reset_get(false, type);

	തിരികെ (reset) ? reset->priority : 0;
}

സ്ഥിതം ശൂന്യം syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

സ്ഥിതം ഘടന sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

സ്ഥിതം പൂർണ്ണസംഖ്യ syscon_reboot_check(u32 type, u32 reason)
{
	ഘടന syscon_reset *reset = syscon_reset_get(true, type);

	തിരികെ (reset) ? reset->priority : 0;
}

സ്ഥിതം ശൂന്യം syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

സ്ഥിതം ഘടന sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

സ്ഥിതം പൂർണ്ണസംഖ്യ syscon_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			     സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc, len;
	സ്ഥിരം fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	ഘടന syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	എങ്കിൽ (reset->rmap)
		തിരികെ SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	എങ്കിൽ (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		തിരികെ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	എങ്കിൽ (!val && !mask)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!val) {
		/* support old binding */
		reset->മൂല്യം = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} അല്ലെങ്കിൽ എങ്കിൽ (!mask) {
		/* support value without mask */
		reset->മൂല്യം = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} അല്ലെങ്കിൽ {
		reset->മൂല്യം = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	എങ്കിൽ (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	അല്ലെങ്കിൽ
		sbi_system_reset_add_device(&syscon_poweroff);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (സ്ഥിരം ശൂന്യം *)false },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

സ്ഥിതം സ്ഥിരം ഘടന fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (സ്ഥിരം ശൂന്യം *)true },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
