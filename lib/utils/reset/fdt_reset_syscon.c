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
	u32 ترجيح;
	u32 وٿي;
	u32 قدر;
	u32 نقاب;
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
		regmap_update_بٽ(reset->rmap, reset->وٿي,
				   reset->نقاب, reset->قدر);

	/* hang !!! */
	sbi_hart_hang();
}

ساكن صحيح syscon_poweroff_check(u32 type, u32 reason)
{
	بنية syscon_reset *reset = syscon_reset_get(false, type);

	أرجع (reset) ? reset->ترجيح : 0;
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

	أرجع (reset) ? reset->ترجيح : 0;
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
			     ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc, len;
	ثابت fdt32_t *val, *نقاب;
	bool is_reboot = (ulong)ميلاپ->معلومات;
	بنية syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	إذا (reset->rmap)
		أرجع SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	إذا (rc)
		أرجع rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->ترجيح = (val && len > 0) ? fdt32_منزل_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	إذا (val && len > 0)
		reset->وٿي = fdt32_منزل_cpu(*val);
	وإلا
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	نقاب = fdt_getprop(fdt, nodeoff, "mask", &len);
	إذا (!val && !نقاب)
		أرجع SBI_EINVAL;

	إذا (!val) {
		/* support old binding */
		reset->قدر = fdt32_منزل_cpu(*نقاب);
		reset->نقاب = 0xFFFFFFFF;
	} وإلا إذا (!نقاب) {
		/* support value without mask */
		reset->قدر = fdt32_منزل_cpu(*val);
		reset->نقاب = 0xFFFFFFFF;
	} وإلا {
		reset->قدر = fdt32_منزل_cpu(*val);
		reset->نقاب = fdt32_منزل_cpu(*نقاب);
	}

	إذا (is_reboot)
		sbi_system_reset_جوڙيو_device(&syscon_reboot);
	وإلا
		sbi_system_reset_جوڙيو_device(&syscon_poweroff);

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ syscon_poweroff_ميلاپ[] = {
	{ .compatible = "syscon-poweroff", .معلومات = (ثابت فراغ *)false },
	{ },
};

ثابت بنية fdt_driver fdt_syscon_poweroff = {
	.ميلاپ_جدول = syscon_poweroff_ميلاپ,
	.init = syscon_reset_init,
};

ساكن ثابت بنية fdt_ميلاپ syscon_reboot_ميلاپ[] = {
	{ .compatible = "syscon-reboot", .معلومات = (ثابت فراغ *)true },
	{ },
};

ثابت بنية fdt_driver fdt_syscon_reboot = {
	.ميلاپ_جدول = syscon_reboot_ميلاپ,
	.init = syscon_reset_init,
};
