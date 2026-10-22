/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/regmap/fdt_regmap.h>

સંરચના syscon_reset {
	સંરચના regmap *rmap;
	u32 priority;
	u32 offset;
	u32 કિંમત;
	u32 mask;
};

સ્થિર સંરચના syscon_reset poweroff;
સ્થિર સંરચના syscon_reset reboot;

સ્થિર સંરચના syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	સંરચના syscon_reset *reset = ખાલી;

	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
		જો (!is_reboot)
			reset = &poweroff;
		તોડો;
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		જો (is_reboot)
			reset = &reboot;
		તોડો;
	}

	જો (reset && !reset->rmap)
		reset = ખાલી;

	પાછા reset;
}

સ્થિર શૂન્ય syscon_reset_exec(સંરચના syscon_reset *reset)
{
	/* Issue the reset through regmap */
	જો (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->કિંમત);

	/* hang !!! */
	sbi_hart_hang();
}

સ્થિર પૂર્ણાંક syscon_poweroff_check(u32 type, u32 reason)
{
	સંરચના syscon_reset *reset = syscon_reset_get(false, type);

	પાછા (reset) ? reset->priority : 0;
}

સ્થિર શૂન્ય syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

સ્થિર સંરચના sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

સ્થિર પૂર્ણાંક syscon_reboot_check(u32 type, u32 reason)
{
	સંરચના syscon_reset *reset = syscon_reset_get(true, type);

	પાછા (reset) ? reset->priority : 0;
}

સ્થિર શૂન્ય syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

સ્થિર સંરચના sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

સ્થિર પૂર્ણાંક syscon_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			     અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc, len;
	અચલ fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	સંરચના syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	જો (reset->rmap)
		પાછા SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	જો (rc)
		પાછા rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	જો (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	નહીંતર
		પાછા SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	જો (!val && !mask)
		પાછા SBI_EINVAL;

	જો (!val) {
		/* support old binding */
		reset->કિંમત = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} નહીંતર જો (!mask) {
		/* support value without mask */
		reset->કિંમત = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} નહીંતર {
		reset->કિંમત = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	જો (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	નહીંતર
		sbi_system_reset_add_device(&syscon_poweroff);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (અચલ શૂન્ય *)false },
	{ },
};

અચલ સંરચના fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

સ્થિર અચલ સંરચના fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (અચલ શૂન્ય *)true },
	{ },
};

અચલ સંરચના fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
