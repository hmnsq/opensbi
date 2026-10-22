/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/regmap/fdt_regmap.h>

ସଂରଚନା syscon_reset {
	ସଂରଚନା regmap *rmap;
	u32 priority;
	u32 offset;
	u32 ମୂଲ୍ୟ;
	u32 mask;
};

ସ୍ଥିର ସଂରଚନା syscon_reset poweroff;
ସ୍ଥିର ସଂରଚନା syscon_reset reboot;

ସ୍ଥିର ସଂରଚନା syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	ସଂରଚନା syscon_reset *reset = ଖାଲି;

	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
		ଯଦି (!is_reboot)
			reset = &poweroff;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ଯଦି (is_reboot)
			reset = &reboot;
		ଭାଙ୍ଗ;
	}

	ଯଦି (reset && !reset->rmap)
		reset = ଖାଲି;

	ଫେରନ୍ତୁ reset;
}

ସ୍ଥିର ଶୂନ୍ୟ syscon_reset_exec(ସଂରଚନା syscon_reset *reset)
{
	/* Issue the reset through regmap */
	ଯଦି (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->ମୂଲ୍ୟ);

	/* hang !!! */
	sbi_hart_hang();
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syscon_poweroff_check(u32 type, u32 reason)
{
	ସଂରଚନା syscon_reset *reset = syscon_reset_get(false, type);

	ଫେରନ୍ତୁ (reset) ? reset->priority : 0;
}

ସ୍ଥିର ଶୂନ୍ୟ syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syscon_reboot_check(u32 type, u32 reason)
{
	ସଂରଚନା syscon_reset *reset = syscon_reset_get(true, type);

	ଫେରନ୍ତୁ (reset) ? reset->priority : 0;
}

ସ୍ଥିର ଶୂନ୍ୟ syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syscon_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			     ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, len;
	ଅଚଳ fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	ସଂରଚନା syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	ଯଦି (reset->rmap)
		ଫେରନ୍ତୁ SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	ଯଦି (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	ଯଦି (!val && !mask)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (!val) {
		/* support old binding */
		reset->ମୂଲ୍ୟ = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} ଅନ୍ୟଥା ଯଦି (!mask) {
		/* support value without mask */
		reset->ମୂଲ୍ୟ = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} ଅନ୍ୟଥା {
		reset->ମୂଲ୍ୟ = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	ଯଦି (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	ଅନ୍ୟଥା
		sbi_system_reset_add_device(&syscon_poweroff);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (ଅଚଳ ଶୂନ୍ୟ *)false },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (ଅଚଳ ଶୂନ୍ୟ *)true },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
