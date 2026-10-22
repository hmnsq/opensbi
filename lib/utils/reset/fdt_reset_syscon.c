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
		ᬬᬾᬦ᭄ (!is_reboot)
			reset = &poweroff;
		bha_nga;
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ᬬᬾᬦ᭄ (is_reboot)
			reset = &reboot;
		bha_nga;
	}

	ᬬᬾᬦ᭄ (reset && !reset->rmap)
		reset = NULL;

	ᬯᬮᬶ reset;
}

static void syscon_reset_exec(sa_nracanaa syscon_reset *reset)
{
	/* Issue the reset through regmap */
	ᬬᬾᬦ᭄ (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->value);

	/* hang !!! */
	sbi_hart_hang();
}

static puur_naa_nka syscon_poweroff_check(u32 type, u32 reason)
{
	sa_nracanaa syscon_reset *reset = syscon_reset_get(false, type);

	ᬯᬮᬶ (reset) ? reset->priority : 0;
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

static puur_naa_nka syscon_reboot_check(u32 type, u32 reason)
{
	sa_nracanaa syscon_reset *reset = syscon_reset_get(true, type);

	ᬯᬮᬶ (reset) ? reset->priority : 0;
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

static puur_naa_nka syscon_reset_init(const void *fdt, puur_naa_nka nodeoff,
			     const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc, len;
	const fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	sa_nracanaa syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	ᬬᬾᬦ᭄ (reset->rmap)
		ᬯᬮᬶ SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	ᬬᬾᬦ᭄ (rc)
		ᬯᬮᬶ rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	ᬬᬾᬦ᭄ (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	ᬮᬶᬬ
		ᬯᬮᬶ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	ᬬᬾᬦ᭄ (!val && !mask)
		ᬯᬮᬶ SBI_EINVAL;

	ᬬᬾᬦ᭄ (!val) {
		/* support old binding */
		reset->value = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (!mask) {
		/* support value without mask */
		reset->value = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} ᬮᬶᬬ {
		reset->value = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	ᬬᬾᬦ᭄ (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	ᬮᬶᬬ
		sbi_system_reset_add_device(&syscon_poweroff);

	ᬯᬮᬶ 0;
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
