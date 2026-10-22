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
	u32 اولویت;
	u32 جابه_جایی;
	u32 مقدار;
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
		regmap_update_بیتها(reset->rmap, reset->جابه_جایی,
				   reset->ماسک, reset->مقدار);

	/* hang !!! */
	sbi_hart_hang();
}

ساكن صحيح syscon_poweroff_check(u32 type, u32 reason)
{
	بنية syscon_reset *reset = syscon_reset_get(false, type);

	أرجع (reset) ? reset->اولویت : 0;
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

	أرجع (reset) ? reset->اولویت : 0;
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
			     ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc, len;
	ثابت fdt32_t *val, *ماسک;
	bool is_reboot = (ulong)تطابق->داده;
	بنية syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	إذا (reset->rmap)
		أرجع SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	إذا (rc)
		أرجع rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->اولویت = (val && len > 0) ? fdt32_مقصد_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	إذا (val && len > 0)
		reset->جابه_جایی = fdt32_مقصد_cpu(*val);
	وإلا
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	ماسک = fdt_getprop(fdt, nodeoff, "mask", &len);
	إذا (!val && !ماسک)
		أرجع SBI_EINVAL;

	إذا (!val) {
		/* support old binding */
		reset->مقدار = fdt32_مقصد_cpu(*ماسک);
		reset->ماسک = 0xFFFFFFFF;
	} وإلا إذا (!ماسک) {
		/* support value without mask */
		reset->مقدار = fdt32_مقصد_cpu(*val);
		reset->ماسک = 0xFFFFFFFF;
	} وإلا {
		reset->مقدار = fdt32_مقصد_cpu(*val);
		reset->ماسک = fdt32_مقصد_cpu(*ماسک);
	}

	إذا (is_reboot)
		sbi_system_reset_جمع_کن_device(&syscon_reboot);
	وإلا
		sbi_system_reset_جمع_کن_device(&syscon_poweroff);

	أرجع 0;
}

ساكن ثابت بنية fdt_تطابق syscon_poweroff_تطابق[] = {
	{ .compatible = "syscon-poweroff", .داده = (ثابت فراغ *)false },
	{ },
};

ثابت بنية fdt_driver fdt_syscon_poweroff = {
	.تطابق_جدول = syscon_poweroff_تطابق,
	.init = syscon_reset_init,
};

ساكن ثابت بنية fdt_تطابق syscon_reboot_تطابق[] = {
	{ .compatible = "syscon-reboot", .داده = (ثابت فراغ *)true },
	{ },
};

ثابت بنية fdt_driver fdt_syscon_reboot = {
	.تطابق_جدول = syscon_reboot_تطابق,
	.init = syscon_reset_init,
};
