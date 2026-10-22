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

ಸಂರಚನಾ syscon_reset {
	ಸಂರಚನಾ regmap *rmap;
	u32 priority;
	u32 offset;
	u32 ಮೌಲ್ಯ;
	u32 mask;
};

ಸ್ಥಿರ ಸಂರಚನಾ syscon_reset poweroff;
ಸ್ಥಿರ ಸಂರಚನಾ syscon_reset reboot;

ಸ್ಥಿರ ಸಂರಚನಾ syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	ಸಂರಚನಾ syscon_reset *reset = ರಿಕ್ತ;

	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಯದಿ (!is_reboot)
			reset = &poweroff;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಯದಿ (is_reboot)
			reset = &reboot;
		ಭಙ್ಗ;
	}

	ಯದಿ (reset && !reset->rmap)
		reset = ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ reset;
}

ಸ್ಥಿರ ಶೂನ್ಯ syscon_reset_exec(ಸಂರಚನಾ syscon_reset *reset)
{
	/* Issue the reset through regmap */
	ಯದಿ (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->ಮೌಲ್ಯ);

	/* hang !!! */
	sbi_hart_hang();
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syscon_poweroff_check(u32 type, u32 reason)
{
	ಸಂರಚನಾ syscon_reset *reset = syscon_reset_get(false, type);

	ಪ್ರತ್ಯಾಗಮ (reset) ? reset->priority : 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syscon_reboot_check(u32 type, u32 reason)
{
	ಸಂರಚನಾ syscon_reset *reset = syscon_reset_get(true, type);

	ಪ್ರತ್ಯಾಗಮ (reset) ? reset->priority : 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syscon_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			     ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc, len;
	ಧ್ರುವ fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	ಸಂರಚನಾ syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	ಯದಿ (reset->rmap)
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	ಯದಿ (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	ಯದಿ (!val && !mask)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!val) {
		/* support old binding */
		reset->ಮೌಲ್ಯ = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} ಅನ್ಯಥಾ ಯದಿ (!mask) {
		/* support value without mask */
		reset->ಮೌಲ್ಯ = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} ಅನ್ಯಥಾ {
		reset->ಮೌಲ್ಯ = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	ಯದಿ (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	ಅನ್ಯಥಾ
		sbi_system_reset_add_device(&syscon_poweroff);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (ಧ್ರುವ ಶೂನ್ಯ *)false },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (ಧ್ರುವ ಶೂನ್ಯ *)true },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
