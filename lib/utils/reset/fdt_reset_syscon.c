/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/regmap/fdt_regmap.h>

கட்டமைப்பு syscon_reset {
	கட்டமைப்பு regmap *rmap;
	u32 priority;
	u32 offset;
	u32 மதிப்பு;
	u32 mask;
};

நிலையான கட்டமைப்பு syscon_reset poweroff;
நிலையான கட்டமைப்பு syscon_reset reboot;

நிலையான கட்டமைப்பு syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	கட்டமைப்பு syscon_reset *reset = வெற்று;

	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
		எனில் (!is_reboot)
			reset = &poweroff;
		நிறுத்து;
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		எனில் (is_reboot)
			reset = &reboot;
		நிறுத்து;
	}

	எனில் (reset && !reset->rmap)
		reset = வெற்று;

	திரும்பு reset;
}

நிலையான வெறுமை syscon_reset_exec(கட்டமைப்பு syscon_reset *reset)
{
	/* Issue the reset through regmap */
	எனில் (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->மதிப்பு);

	/* hang !!! */
	sbi_hart_hang();
}

நிலையான முழு syscon_poweroff_check(u32 type, u32 reason)
{
	கட்டமைப்பு syscon_reset *reset = syscon_reset_get(false, type);

	திரும்பு (reset) ? reset->priority : 0;
}

நிலையான வெறுமை syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

நிலையான கட்டமைப்பு sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

நிலையான முழு syscon_reboot_check(u32 type, u32 reason)
{
	கட்டமைப்பு syscon_reset *reset = syscon_reset_get(true, type);

	திரும்பு (reset) ? reset->priority : 0;
}

நிலையான வெறுமை syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

நிலையான கட்டமைப்பு sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

நிலையான முழு syscon_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			     மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc, len;
	மாறிலி fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	கட்டமைப்பு syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	எனில் (reset->rmap)
		திரும்பு SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	எனில் (rc)
		திரும்பு rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	எனில் (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	இல்லையெனில்
		திரும்பு SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	எனில் (!val && !mask)
		திரும்பு SBI_EINVAL;

	எனில் (!val) {
		/* support old binding */
		reset->மதிப்பு = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} இல்லையெனில் எனில் (!mask) {
		/* support value without mask */
		reset->மதிப்பு = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} இல்லையெனில் {
		reset->மதிப்பு = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	எனில் (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	இல்லையெனில்
		sbi_system_reset_add_device(&syscon_poweroff);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (மாறிலி வெறுமை *)false },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

நிலையான மாறிலி கட்டமைப்பு fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (மாறிலி வெறுமை *)true },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
