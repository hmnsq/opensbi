/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/regmap/fdt_regmap.h>

ਬਣਤਰ syscon_reset {
	ਬਣਤਰ regmap *rmap;
	u32 ਤਰਜੀਹ;
	u32 ਵਿੱਥ;
	u32 ਮੁੱਲ;
	u32 ਨਕਾਬ;
};

ਸਥਿਰ ਬਣਤਰ syscon_reset poweroff;
ਸਥਿਰ ਬਣਤਰ syscon_reset reboot;

ਸਥਿਰ ਬਣਤਰ syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	ਬਣਤਰ syscon_reset *reset = ਨਿਰਾ;

	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ਜੇ (!is_reboot)
			reset = &poweroff;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ਜੇ (is_reboot)
			reset = &reboot;
		ਰੋਕੋ;
	}

	ਜੇ (reset && !reset->rmap)
		reset = ਨਿਰਾ;

	ਵਾਪਸ reset;
}

ਸਥਿਰ ਖਾਲੀ syscon_reset_exec(ਬਣਤਰ syscon_reset *reset)
{
	/* Issue the reset through regmap */
	ਜੇ (reset)
		regmap_update_ਬਿੱਟ(reset->rmap, reset->ਵਿੱਥ,
				   reset->ਨਕਾਬ, reset->ਮੁੱਲ);

	/* hang !!! */
	sbi_hart_hang();
}

ਸਥਿਰ ਪੂਰਨ syscon_poweroff_check(u32 type, u32 reason)
{
	ਬਣਤਰ syscon_reset *reset = syscon_reset_get(false, type);

	ਵਾਪਸ (reset) ? reset->ਤਰਜੀਹ : 0;
}

ਸਥਿਰ ਖਾਲੀ syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

ਸਥਿਰ ਪੂਰਨ syscon_reboot_check(u32 type, u32 reason)
{
	ਬਣਤਰ syscon_reset *reset = syscon_reset_get(true, type);

	ਵਾਪਸ (reset) ? reset->ਤਰਜੀਹ : 0;
}

ਸਥਿਰ ਖਾਲੀ syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

ਸਥਿਰ ਪੂਰਨ syscon_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			     ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc, len;
	ਅਚਲ fdt32_t *val, *ਨਕਾਬ;
	bool is_reboot = (ulong)ਮੇਲ->ਅੰਕੜੇ;
	ਬਣਤਰ syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	ਜੇ (reset->rmap)
		ਵਾਪਸ SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->ਤਰਜੀਹ = (val && len > 0) ? fdt32_ਮੰਜ਼ਿਲ_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	ਜੇ (val && len > 0)
		reset->ਵਿੱਥ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	ਨਕਾਬ = fdt_getprop(fdt, nodeoff, "mask", &len);
	ਜੇ (!val && !ਨਕਾਬ)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!val) {
		/* support old binding */
		reset->ਮੁੱਲ = fdt32_ਮੰਜ਼ਿਲ_cpu(*ਨਕਾਬ);
		reset->ਨਕਾਬ = 0xFFFFFFFF;
	} ਨਹੀਂਤਾਂ ਜੇ (!ਨਕਾਬ) {
		/* support value without mask */
		reset->ਮੁੱਲ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
		reset->ਨਕਾਬ = 0xFFFFFFFF;
	} ਨਹੀਂਤਾਂ {
		reset->ਮੁੱਲ = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);
		reset->ਨਕਾਬ = fdt32_ਮੰਜ਼ਿਲ_cpu(*ਨਕਾਬ);
	}

	ਜੇ (is_reboot)
		sbi_system_reset_ਜੋੜੋ_device(&syscon_reboot);
	ਨਹੀਂਤਾਂ
		sbi_system_reset_ਜੋੜੋ_device(&syscon_poweroff);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ syscon_poweroff_ਮੇਲ[] = {
	{ .compatible = "syscon-poweroff", .ਅੰਕੜੇ = (ਅਚਲ ਖਾਲੀ *)false },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_syscon_poweroff = {
	.ਮੇਲ_ਸਾਰਣੀ = syscon_poweroff_ਮੇਲ,
	.init = syscon_reset_init,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ syscon_reboot_ਮੇਲ[] = {
	{ .compatible = "syscon-reboot", .ਅੰਕੜੇ = (ਅਚਲ ਖਾਲੀ *)true },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_syscon_reboot = {
	.ਮੇਲ_ਸਾਰਣੀ = syscon_reboot_ਮੇਲ,
	.init = syscon_reset_init,
};
