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

ساخت syscon_reset {
	ساخت regmap *rmap;
	u32 ترجیح;
	u32 انحراف;
	u32 قدر;
	u32 نقاب;
};

ساکن ساخت syscon_reset poweroff;
ساکن ساخت syscon_reset reboot;

ساکن ساخت syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	ساخت syscon_reset *reset = تہی;

	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		اگر (!is_reboot)
			reset = &poweroff;
		توڑو;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		اگر (is_reboot)
			reset = &reboot;
		توڑو;
	}

	اگر (reset && !reset->rmap)
		reset = تہی;

	واپس reset;
}

ساکن خالی syscon_reset_exec(ساخت syscon_reset *reset)
{
	/* Issue the reset through regmap */
	اگر (reset)
		regmap_update_بٹ(reset->rmap, reset->انحراف,
				   reset->نقاب, reset->قدر);

	/* hang !!! */
	sbi_hart_hang();
}

ساکن صحیح syscon_poweroff_check(u32 type, u32 reason)
{
	ساخت syscon_reset *reset = syscon_reset_get(false, type);

	واپس (reset) ? reset->ترجیح : 0;
}

ساکن خالی syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

ساکن ساخت sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

ساکن صحیح syscon_reboot_check(u32 type, u32 reason)
{
	ساخت syscon_reset *reset = syscon_reset_get(true, type);

	واپس (reset) ? reset->ترجیح : 0;
}

ساکن خالی syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

ساکن ساخت sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

ساکن صحیح syscon_reset_init(ثابت خالی *fdt, صحیح nodeoff,
			     ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc, len;
	ثابت fdt32_t *val, *نقاب;
	bool is_reboot = (ulong)مطابقت->ڈیٹا;
	ساخت syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	اگر (reset->rmap)
		واپس SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	اگر (rc)
		واپس rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->ترجیح = (val && len > 0) ? fdt32_مقصد_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	اگر (val && len > 0)
		reset->انحراف = fdt32_مقصد_cpu(*val);
	ورنہ
		واپس SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	نقاب = fdt_getprop(fdt, nodeoff, "mask", &len);
	اگر (!val && !نقاب)
		واپس SBI_EINVAL;

	اگر (!val) {
		/* support old binding */
		reset->قدر = fdt32_مقصد_cpu(*نقاب);
		reset->نقاب = 0xFFFFFFFF;
	} ورنہ اگر (!نقاب) {
		/* support value without mask */
		reset->قدر = fdt32_مقصد_cpu(*val);
		reset->نقاب = 0xFFFFFFFF;
	} ورنہ {
		reset->قدر = fdt32_مقصد_cpu(*val);
		reset->نقاب = fdt32_مقصد_cpu(*نقاب);
	}

	اگر (is_reboot)
		sbi_system_reset_جوڑو_device(&syscon_reboot);
	ورنہ
		sbi_system_reset_جوڑو_device(&syscon_poweroff);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت syscon_poweroff_مطابقت[] = {
	{ .compatible = "syscon-poweroff", .ڈیٹا = (ثابت خالی *)false },
	{ },
};

ثابت ساخت fdt_driver fdt_syscon_poweroff = {
	.مطابقت_جدول = syscon_poweroff_مطابقت,
	.init = syscon_reset_init,
};

ساکن ثابت ساخت fdt_مطابقت syscon_reboot_مطابقت[] = {
	{ .compatible = "syscon-reboot", .ڈیٹا = (ثابت خالی *)true },
	{ },
};

ثابت ساخت fdt_driver fdt_syscon_reboot = {
	.مطابقت_جدول = syscon_reboot_مطابقت,
	.init = syscon_reset_init,
};
