/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/regmap/fdt_regmap.h>

ساختار syscon_reset {
	ساختار regmap *rmap;
	u32 priority;
	u32 offset;
	u32 مقدار;
	u32 mask;
};

ایستا ساختار syscon_reset poweroff;
ایستا ساختار syscon_reset reboot;

ایستا ساختار syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	ساختار syscon_reset *reset = پوچ;

	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		اگر (!is_reboot)
			reset = &poweroff;
		بشکن;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		اگر (is_reboot)
			reset = &reboot;
		بشکن;
	}

	اگر (reset && !reset->rmap)
		reset = پوچ;

	بازگشت reset;
}

ایستا تهی syscon_reset_exec(ساختار syscon_reset *reset)
{
	/* Issue the reset through regmap */
	اگر (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->مقدار);

	/* hang !!! */
	sbi_hart_hang();
}

ایستا صحیح syscon_poweroff_check(u32 type, u32 reason)
{
	ساختار syscon_reset *reset = syscon_reset_get(false, type);

	بازگشت (reset) ? reset->priority : 0;
}

ایستا تهی syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

ایستا ساختار sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

ایستا صحیح syscon_reboot_check(u32 type, u32 reason)
{
	ساختار syscon_reset *reset = syscon_reset_get(true, type);

	بازگشت (reset) ? reset->priority : 0;
}

ایستا تهی syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

ایستا ساختار sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

ایستا صحیح syscon_reset_init(ثابت تهی *fdt, صحیح nodeoff,
			     ثابت ساختار fdt_match *match)
{
	صحیح rc, len;
	ثابت fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	ساختار syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	اگر (reset->rmap)
		بازگشت SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	اگر (rc)
		بازگشت rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	اگر (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	وگرنه
		بازگشت SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	اگر (!val && !mask)
		بازگشت SBI_EINVAL;

	اگر (!val) {
		/* support old binding */
		reset->مقدار = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} وگرنه اگر (!mask) {
		/* support value without mask */
		reset->مقدار = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} وگرنه {
		reset->مقدار = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	اگر (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	وگرنه
		sbi_system_reset_add_device(&syscon_poweroff);

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (ثابت تهی *)false },
	{ },
};

ثابت ساختار fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

ایستا ثابت ساختار fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (ثابت تهی *)true },
	{ },
};

ثابت ساختار fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
