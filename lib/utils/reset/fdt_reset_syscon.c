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
	u32 لومړیتوب;
	u32 دننه_واټن;
	u32 ارزښت;
	u32 ماسک;
};

ساكن بنية syscon_reset poweroff;
ساكن بنية syscon_reset reboot;

ساكن بنية syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	بنية syscon_reset *reset = عدم;

	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
		إذا (!is_reboot)
			reset = &poweroff;
		اكسر;
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		إذا (is_reboot)
			reset = &reboot;
		اكسر;
	}

	إذا (reset && !reset->rmap)
		reset = عدم;

	أرجع reset;
}

ساكن فراغ syscon_reset_exec(بنية syscon_reset *reset)
{
	/* Issue the reset through regmap */
	إذا (reset)
		regmap_update_بټونه(reset->rmap, reset->دننه_واټن,
				   reset->ماسک, reset->ارزښت);

	/* hang !!! */
	sbi_hart_hang();
}

ساكن صحيح syscon_poweroff_check(u32 type, u32 reason)
{
	بنية syscon_reset *reset = syscon_reset_get(false, type);

	أرجع (reset) ? reset->لومړیتوب : 0;
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

	أرجع (reset) ? reset->لومړیتوب : 0;
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
			     ثابت بنية fdt_سمون *سمون)
{
	صحيح rc, len;
	ثابت fdt32_t *val, *ماسک;
	bool is_reboot = (ulong)سمون->معلومات;
	بنية syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	إذا (reset->rmap)
		أرجع SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	إذا (rc)
		أرجع rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->لومړیتوب = (val && len > 0) ? fdt32_منزل_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	إذا (val && len > 0)
		reset->دننه_واټن = fdt32_منزل_cpu(*val);
	وإلا
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	ماسک = fdt_getprop(fdt, nodeoff, "mask", &len);
	إذا (!val && !ماسک)
		أرجع SBI_EINVAL;

	إذا (!val) {
		/* support old binding */
		reset->ارزښت = fdt32_منزل_cpu(*ماسک);
		reset->ماسک = 0xFFFFFFFF;
	} وإلا إذا (!ماسک) {
		/* support value without mask */
		reset->ارزښت = fdt32_منزل_cpu(*val);
		reset->ماسک = 0xFFFFFFFF;
	} وإلا {
		reset->ارزښت = fdt32_منزل_cpu(*val);
		reset->ماسک = fdt32_منزل_cpu(*ماسک);
	}

	إذا (is_reboot)
		sbi_system_reset_جمع_کړه_device(&syscon_reboot);
	وإلا
		sbi_system_reset_جمع_کړه_device(&syscon_poweroff);

	أرجع 0;
}

ساكن ثابت بنية fdt_سمون syscon_poweroff_سمون[] = {
	{ .compatible = "syscon-poweroff", .معلومات = (ثابت فراغ *)false },
	{ },
};

ثابت بنية fdt_driver fdt_syscon_poweroff = {
	.سمون_جدول = syscon_poweroff_سمون,
	.init = syscon_reset_init,
};

ساكن ثابت بنية fdt_سمون syscon_reboot_سمون[] = {
	{ .compatible = "syscon-reboot", .معلومات = (ثابت فراغ *)true },
	{ },
};

ثابت بنية fdt_driver fdt_syscon_reboot = {
	.سمون_جدول = syscon_reboot_سمون,
	.init = syscon_reset_init,
};
