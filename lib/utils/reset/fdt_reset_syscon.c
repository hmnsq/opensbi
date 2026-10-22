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
	u32 प्राथमिकता;
	u32 विस्थापनम्;
	u32 मानम्;
	u32 आवरणम्;
};

स्थिर संरचना syscon_reset poweroff;
स्थिर संरचना syscon_reset reboot;

स्थिर संरचना syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	संरचना syscon_reset *reset = रिक्त;

	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		यदि (!is_reboot)
			reset = &poweroff;
		भङ्ग;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		यदि (is_reboot)
			reset = &reboot;
		भङ्ग;
	}

	यदि (reset && !reset->rmap)
		reset = रिक्त;

	प्रत्यागम reset;
}

स्थिर शून्य syscon_reset_exec(संरचना syscon_reset *reset)
{
	/* Issue the reset through regmap */
	यदि (reset)
		regmap_update_द्वयङ्काः(reset->rmap, reset->विस्थापनम्,
				   reset->आवरणम्, reset->मानम्);

	/* hang !!! */
	sbi_hart_hang();
}

स्थिर पूर्णाङ्क syscon_poweroff_check(u32 type, u32 reason)
{
	संरचना syscon_reset *reset = syscon_reset_get(false, type);

	प्रत्यागम (reset) ? reset->प्राथमिकता : 0;
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

	प्रत्यागम (reset) ? reset->प्राथमिकता : 0;
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
			     ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc, len;
	ध्रुव fdt32_t *val, *आवरणम्;
	bool is_reboot = (ulong)मेलनम्->दत्त;
	संरचना syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	यदि (reset->rmap)
		प्रत्यागम SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	यदि (rc)
		प्रत्यागम rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->प्राथमिकता = (val && len > 0) ? fdt32_गन्तव्यम्_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	यदि (val && len > 0)
		reset->विस्थापनम् = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		प्रत्यागम SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	आवरणम् = fdt_getprop(fdt, nodeoff, "mask", &len);
	यदि (!val && !आवरणम्)
		प्रत्यागम SBI_EINVAL;

	यदि (!val) {
		/* support old binding */
		reset->मानम् = fdt32_गन्तव्यम्_cpu(*आवरणम्);
		reset->आवरणम् = 0xFFFFFFFF;
	} अन्यथा यदि (!आवरणम्) {
		/* support value without mask */
		reset->मानम् = fdt32_गन्तव्यम्_cpu(*val);
		reset->आवरणम् = 0xFFFFFFFF;
	} अन्यथा {
		reset->मानम् = fdt32_गन्तव्यम्_cpu(*val);
		reset->आवरणम् = fdt32_गन्तव्यम्_cpu(*आवरणम्);
	}

	यदि (is_reboot)
		sbi_system_reset_सङ्कलय_device(&syscon_reboot);
	अन्यथा
		sbi_system_reset_सङ्कलय_device(&syscon_poweroff);

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् syscon_poweroff_मेलनम्[] = {
	{ .compatible = "syscon-poweroff", .दत्त = (ध्रुव शून्य *)false },
	{ },
};

ध्रुव संरचना fdt_driver fdt_syscon_poweroff = {
	.मेलनम्_सारणी = syscon_poweroff_मेलनम्,
	.init = syscon_reset_init,
};

स्थिर ध्रुव संरचना fdt_मेलनम् syscon_reboot_मेलनम्[] = {
	{ .compatible = "syscon-reboot", .दत्त = (ध्रुव शून्य *)true },
	{ },
};

ध्रुव संरचना fdt_driver fdt_syscon_reboot = {
	.मेलनम्_सारणी = syscon_reboot_मेलनम्,
	.init = syscon_reset_init,
};
