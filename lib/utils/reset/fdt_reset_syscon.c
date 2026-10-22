/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_system.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/regmap/fdt_regmap.h>

sa_nracanaa syscon_reset {
	sa_nracanaa regmap *rmap;
	u32 priority;
	u32 offset;
	u32 value;
	u32 mask;
};

static sa_nracanaa syscon_reset poweroff;
static sa_nracanaa syscon_reset reboot;

static sa_nracanaa syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	sa_nracanaa syscon_reset *reset = NULL;

	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
		ប្រសិនបើ (!is_reboot)
			reset = &poweroff;
		bha_nga;
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ប្រសិនបើ (is_reboot)
			reset = &reboot;
		bha_nga;
	}

	ប្រសិនបើ (reset && !reset->rmap)
		reset = NULL;

	ត្រឡប់ reset;
}

static void syscon_reset_exec(sa_nracanaa syscon_reset *reset)
{
	/* Issue the reset through regmap */
	ប្រសិនបើ (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->value);

	/* hang !!! */
	sbi_hart_hang();
}

static ចំនួនគត់ syscon_poweroff_check(u32 type, u32 reason)
{
	sa_nracanaa syscon_reset *reset = syscon_reset_get(false, type);

	ត្រឡប់ (reset) ? reset->priority : 0;
}

static void syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

static sa_nracanaa sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

static ចំនួនគត់ syscon_reboot_check(u32 type, u32 reason)
{
	sa_nracanaa syscon_reset *reset = syscon_reset_get(true, type);

	ត្រឡប់ (reset) ? reset->priority : 0;
}

static void syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

static sa_nracanaa sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

static ចំនួនគត់ syscon_reset_init(const void *fdt, ចំនួនគត់ nodeoff,
			     const sa_nracanaa fdt_match *match)
{
	ចំនួនគត់ rc, len;
	const fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	sa_nracanaa syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	ប្រសិនបើ (reset->rmap)
		ត្រឡប់ SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	ប្រសិនបើ (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		ត្រឡប់ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	ប្រសិនបើ (!val && !mask)
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (!val) {
		/* support old binding */
		reset->value = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} បើមិនដូច្នេះ ប្រសិនបើ (!mask) {
		/* support value without mask */
		reset->value = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} បើមិនដូច្នេះ {
		reset->value = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	ប្រសិនបើ (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	បើមិនដូច្នេះ
		sbi_system_reset_add_device(&syscon_poweroff);

	ត្រឡប់ 0;
}

static const sa_nracanaa fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (const void *)false },
	{ },
};

const sa_nracanaa fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

static const sa_nracanaa fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (const void *)true },
	{ },
};

const sa_nracanaa fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
