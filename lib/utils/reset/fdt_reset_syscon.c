/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/regmap/fdt_regmap.h>

संरचना syscon_reset {
	संरचना regmap *rmap;
	u32 priority;
	u32 offset;
	u32 मानम्;
	u32 mask;
};

स्थिर संरचना syscon_reset poweroff;
स्थिर संरचना syscon_reset reboot;

स्थिर संरचना syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	संरचना syscon_reset *reset = रिक्त;

	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		सचे (!is_reboot)
			reset = &poweroff;
		भङ्ग;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		सचे (is_reboot)
			reset = &reboot;
		भङ्ग;
	}

	सचे (reset && !reset->rmap)
		reset = रिक्त;

	पटिनिवत्त reset;
}

स्थिर शून्य syscon_reset_exec(संरचना syscon_reset *reset)
{
	/* Issue the reset through regmap */
	सचे (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->मानम्);

	/* hang !!! */
	sbi_hart_hang();
}

स्थिर पूर्णाङ्क syscon_poweroff_check(u32 type, u32 reason)
{
	संरचना syscon_reset *reset = syscon_reset_get(false, type);

	पटिनिवत्त (reset) ? reset->priority : 0;
}

स्थिर शून्य syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

स्थिर संरचना sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

स्थिर पूर्णाङ्क syscon_reboot_check(u32 type, u32 reason)
{
	संरचना syscon_reset *reset = syscon_reset_get(true, type);

	पटिनिवत्त (reset) ? reset->priority : 0;
}

स्थिर शून्य syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

स्थिर संरचना sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

स्थिर पूर्णाङ्क syscon_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			     ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc, len;
	ध्रुव fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->data;
	संरचना syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	सचे (reset->rmap)
		पटिनिवत्त SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	सचे (rc)
		पटिनिवत्त rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	सचे (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	अञ्ञथा
		पटिनिवत्त SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	सचे (!val && !mask)
		पटिनिवत्त SBI_EINVAL;

	सचे (!val) {
		/* support old binding */
		reset->मानम् = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} अञ्ञथा सचे (!mask) {
		/* support value without mask */
		reset->मानम् = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} अञ्ञथा {
		reset->मानम् = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	सचे (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	अञ्ञथा
		sbi_system_reset_add_device(&syscon_poweroff);

	पटिनिवत्त 0;
}

स्थिर ध्रुव संरचना fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .data = (ध्रुव शून्य *)false },
	{ },
};

ध्रुव संरचना fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

स्थिर ध्रुव संरचना fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .data = (ध्रुव शून्य *)true },
	{ },
};

ध्रुव संरचना fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
