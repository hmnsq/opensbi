/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/regmap/fdt_regmap.h>

ರಚನೆ syscon_reset {
	ರಚನೆ regmap *rmap;
	u32 ಆದ್ಯತೆ;
	u32 ಸ್ಥಾನಾಂತರ;
	u32 ಮೌಲ್ಯ;
	u32 ಮುಖವಾಡ;
};

ಸ್ಥಿರ ರಚನೆ syscon_reset poweroff;
ಸ್ಥಿರ ರಚನೆ syscon_reset reboot;

ಸ್ಥಿರ ರಚನೆ syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	ರಚನೆ syscon_reset *reset = ಖಾಲಿ;

	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಆದರೆ (!is_reboot)
			reset = &poweroff;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಆದರೆ (is_reboot)
			reset = &reboot;
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (reset && !reset->rmap)
		reset = ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ reset;
}

ಸ್ಥಿರ ಶೂನ್ಯ syscon_reset_exec(ರಚನೆ syscon_reset *reset)
{
	/* Issue the reset through regmap */
	ಆದರೆ (reset)
		regmap_update_ಬಿಟ್ಟುಗಳು(reset->rmap, reset->ಸ್ಥಾನಾಂತರ,
				   reset->ಮುಖವಾಡ, reset->ಮೌಲ್ಯ);

	/* hang !!! */
	sbi_hart_hang();
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syscon_poweroff_check(u32 type, u32 reason)
{
	ರಚನೆ syscon_reset *reset = syscon_reset_get(false, type);

	ಹಿಂತಿರುಗಿ (reset) ? reset->ಆದ್ಯತೆ : 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syscon_reboot_check(u32 type, u32 reason)
{
	ರಚನೆ syscon_reset *reset = syscon_reset_get(true, type);

	ಹಿಂತಿರುಗಿ (reset) ? reset->ಆದ್ಯತೆ : 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syscon_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			     ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc, len;
	ಸ್ಥಿರಾಂಕ fdt32_t *val, *ಮುಖವಾಡ;
	bool is_reboot = (ulong)ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ;
	ರಚನೆ syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	ಆದರೆ (reset->rmap)
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->ಆದ್ಯತೆ = (val && len > 0) ? fdt32_ಗಮ್ಯ_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	ಆದರೆ (val && len > 0)
		reset->ಸ್ಥಾನಾಂತರ = fdt32_ಗಮ್ಯ_cpu(*val);
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	ಮುಖವಾಡ = fdt_getprop(fdt, nodeoff, "mask", &len);
	ಆದರೆ (!val && !ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!val) {
		/* support old binding */
		reset->ಮೌಲ್ಯ = fdt32_ಗಮ್ಯ_cpu(*ಮುಖವಾಡ);
		reset->ಮುಖವಾಡ = 0xFFFFFFFF;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಮುಖವಾಡ) {
		/* support value without mask */
		reset->ಮೌಲ್ಯ = fdt32_ಗಮ್ಯ_cpu(*val);
		reset->ಮುಖವಾಡ = 0xFFFFFFFF;
	} ಇಲ್ಲದಿದ್ದರೆ {
		reset->ಮೌಲ್ಯ = fdt32_ಗಮ್ಯ_cpu(*val);
		reset->ಮುಖವಾಡ = fdt32_ಗಮ್ಯ_cpu(*ಮುಖವಾಡ);
	}

	ಆದರೆ (is_reboot)
		sbi_system_reset_ಕೂಡಿಸು_device(&syscon_reboot);
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_system_reset_ಕೂಡಿಸು_device(&syscon_poweroff);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ syscon_poweroff_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "syscon-poweroff", .ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)false },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_syscon_poweroff = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = syscon_poweroff_ಹೊಂದಿಕೆ,
	.init = syscon_reset_init,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ syscon_reboot_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "syscon-reboot", .ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)true },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_syscon_reboot = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = syscon_reboot_ಹೊಂದಿಕೆ,
	.init = syscon_reset_init,
};
