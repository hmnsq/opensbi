/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/regmap/fdt_regmap.h>

నిర్మాణం syscon_reset {
	నిర్మాణం regmap *rmap;
	u32 ప్రాధాన్యత;
	u32 స్థానభ్రంశం;
	u32 విలువ;
	u32 ముసుగు;
};

స్థిర నిర్మాణం syscon_reset poweroff;
స్థిర నిర్మాణం syscon_reset reboot;

స్థిర నిర్మాణం syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	నిర్మాణం syscon_reset *reset = ఖాళీ;

	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
		ఒకవేళ (!is_reboot)
			reset = &poweroff;
		ఆపు;
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ఒకవేళ (is_reboot)
			reset = &reboot;
		ఆపు;
	}

	ఒకవేళ (reset && !reset->rmap)
		reset = ఖాళీ;

	తిరిగి reset;
}

స్థిర శూన్య syscon_reset_exec(నిర్మాణం syscon_reset *reset)
{
	/* Issue the reset through regmap */
	ఒకవేళ (reset)
		regmap_update_బిట్లు(reset->rmap, reset->స్థానభ్రంశం,
				   reset->ముసుగు, reset->విలువ);

	/* hang !!! */
	sbi_hart_hang();
}

స్థిర పూర్ణాంకం syscon_poweroff_check(u32 type, u32 reason)
{
	నిర్మాణం syscon_reset *reset = syscon_reset_get(false, type);

	తిరిగి (reset) ? reset->ప్రాధాన్యత : 0;
}

స్థిర శూన్య syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

స్థిర నిర్మాణం sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

స్థిర పూర్ణాంకం syscon_reboot_check(u32 type, u32 reason)
{
	నిర్మాణం syscon_reset *reset = syscon_reset_get(true, type);

	తిరిగి (reset) ? reset->ప్రాధాన్యత : 0;
}

స్థిర శూన్య syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

స్థిర నిర్మాణం sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

స్థిర పూర్ణాంకం syscon_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			     స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc, len;
	స్థిరరాశి fdt32_t *val, *ముసుగు;
	bool is_reboot = (ulong)సరిపోలిక->దత్తాంశం;
	నిర్మాణం syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	ఒకవేళ (reset->rmap)
		తిరిగి SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	ఒకవేళ (rc)
		తిరిగి rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->ప్రాధాన్యత = (val && len > 0) ? fdt32_గమ్యం_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	ఒకవేళ (val && len > 0)
		reset->స్థానభ్రంశం = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		తిరిగి SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	ముసుగు = fdt_getprop(fdt, nodeoff, "mask", &len);
	ఒకవేళ (!val && !ముసుగు)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!val) {
		/* support old binding */
		reset->విలువ = fdt32_గమ్యం_cpu(*ముసుగు);
		reset->ముసుగు = 0xFFFFFFFF;
	} లేకపోతే ఒకవేళ (!ముసుగు) {
		/* support value without mask */
		reset->విలువ = fdt32_గమ్యం_cpu(*val);
		reset->ముసుగు = 0xFFFFFFFF;
	} లేకపోతే {
		reset->విలువ = fdt32_గమ్యం_cpu(*val);
		reset->ముసుగు = fdt32_గమ్యం_cpu(*ముసుగు);
	}

	ఒకవేళ (is_reboot)
		sbi_system_reset_కూడు_device(&syscon_reboot);
	లేకపోతే
		sbi_system_reset_కూడు_device(&syscon_poweroff);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక syscon_poweroff_సరిపోలిక[] = {
	{ .compatible = "syscon-poweroff", .దత్తాంశం = (స్థిరరాశి శూన్య *)false },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_syscon_poweroff = {
	.సరిపోలిక_పట్టిక = syscon_poweroff_సరిపోలిక,
	.init = syscon_reset_init,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక syscon_reboot_సరిపోలిక[] = {
	{ .compatible = "syscon-reboot", .దత్తాంశం = (స్థిరరాశి శూన్య *)true },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_syscon_reboot = {
	.సరిపోలిక_పట్టిక = syscon_reboot_సరిపోలిక,
	.init = syscon_reset_init,
};
