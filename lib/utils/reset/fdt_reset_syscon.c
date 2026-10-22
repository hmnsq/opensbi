/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/regmap/fdt_regmap.h>

بنية syscon_reset {
	بنية regmap *rmap;
	u32 priority;
	u32 offset;
	u32 value;
	u32 mask;
};

ساكن بنية syscon_reset poweroff;
ساكن بنية syscon_reset reboot;

ساكن بنية syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	بنية syscon_reset *reset = عدم;

	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
		אם (!is_reboot)
			reset = &poweroff;
		اكسر;
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		אם (is_reboot)
			reset = &reboot;
		اكسر;
	}

	אם (reset && !reset->rmap)
		reset = عدم;

	החזר reset;
}

ساكن فراغ syscon_reset_exec(بنية syscon_reset *reset)
{
	/* Issue the reset through regmap */
	אם (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->value);

	/* hang !!! */
	sbi_hart_hang();
}

ساكن صحيح syscon_poweroff_check(u32 type, u32 reason)
{
	بنية syscon_reset *reset = syscon_reset_get(false, type);

	החזר (reset) ? reset->priority : 0;
}

ساكن فراغ syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

ساكن بنية sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

ساكن صحيح syscon_reboot_check(u32 type, u32 reason)
{
	بنية syscon_reset *reset = syscon_reset_get(true, type);

	החזר (reset) ? reset->priority : 0;
}

ساكن فراغ syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

ساكن بنية sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

ساكن صحيح syscon_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_match *match)
{
	صحيح rc, len;
	ثابت fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	بنية syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	אם (reset->rmap)
		החזר SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	אם (rc)
		החזר rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	אם (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	אחרת
		החזר SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	אם (!val && !mask)
		החזר SBI_EINVAL;

	אם (!val) {
		/* support old binding */
		reset->value = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} אחרת אם (!mask) {
		/* support value without mask */
		reset->value = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} אחרת {
		reset->value = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	אם (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	אחרת
		sbi_system_reset_add_device(&syscon_poweroff);

	החזר 0;
}

ساكن ثابت بنية fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (ثابت فراغ *)false },
	{ },
};

ثابت بنية fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

ساكن ثابت بنية fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (ثابت فراغ *)true },
	{ },
};

ثابت بنية fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
