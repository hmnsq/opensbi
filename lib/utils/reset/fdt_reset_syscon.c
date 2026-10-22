/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/regmap/fdt_regmap.h>

কাঠামো syscon_reset {
	কাঠামো regmap *rmap;
	u32 priority;
	u32 offset;
	u32 মান;
	u32 mask;
};

স্থির কাঠামো syscon_reset poweroff;
স্থির কাঠামো syscon_reset reboot;

স্থির কাঠামো syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	কাঠামো syscon_reset *reset = ফাঁকা;

	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_SHUTDOWN:
		যদি (!is_reboot)
			reset = &poweroff;
		বিরতি;
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
		যদি (is_reboot)
			reset = &reboot;
		বিরতি;
	}

	যদি (reset && !reset->rmap)
		reset = ফাঁকা;

	ফেরত reset;
}

স্থির শূন্য syscon_reset_exec(কাঠামো syscon_reset *reset)
{
	/* Issue the reset through regmap */
	যদি (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->মান);

	/* hang !!! */
	sbi_hart_hang();
}

স্থির পূর্ণসংখ্যা syscon_poweroff_check(u32 type, u32 reason)
{
	কাঠামো syscon_reset *reset = syscon_reset_get(false, type);

	ফেরত (reset) ? reset->priority : 0;
}

স্থির শূন্য syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

স্থির কাঠামো sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

স্থির পূর্ণসংখ্যা syscon_reboot_check(u32 type, u32 reason)
{
	কাঠামো syscon_reset *reset = syscon_reset_get(true, type);

	ফেরত (reset) ? reset->priority : 0;
}

স্থির শূন্য syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

স্থির কাঠামো sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

স্থির পূর্ণসংখ্যা syscon_reset_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			     ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc, len;
	ধ্রুবক fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->তথ্য;
	কাঠামো syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	যদি (reset->rmap)
		ফেরত SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	যদি (rc)
		ফেরত rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	যদি (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	অন্যথা
		ফেরত SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	যদি (!val && !mask)
		ফেরত SBI_EINVAL;

	যদি (!val) {
		/* support old binding */
		reset->মান = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} অন্যথা যদি (!mask) {
		/* support value without mask */
		reset->মান = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} অন্যথা {
		reset->মান = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	যদি (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	অন্যথা
		sbi_system_reset_add_device(&syscon_poweroff);

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .তথ্য = (ধ্রুবক শূন্য *)false },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

স্থির ধ্রুবক কাঠামো fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .তথ্য = (ধ্রুবক শূন্য *)true },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
