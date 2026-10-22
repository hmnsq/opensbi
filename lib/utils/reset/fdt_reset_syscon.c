/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/regmap/fdt_regmap.h>

काष्ठा syscon_reset {
	काष्ठा regmap *rmap;
	u32 priority;
	u32 offset;
	u32 मूल्य;
	u32 mask;
};

अटल काष्ठा syscon_reset poweroff;
अटल काष्ठा syscon_reset reboot;

अटल काष्ठा syscon_reset *syscon_reset_get(bool is_reboot, u32 type)
{
	काष्ठा syscon_reset *reset = शून्य;

	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
		अगर (!is_reboot)
			reset = &poweroff;
		अवरोध;
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		अगर (is_reboot)
			reset = &reboot;
		अवरोध;
	}

	अगर (reset && !reset->rmap)
		reset = शून्य;

	वापस reset;
}

अटल व्योम syscon_reset_exec(काष्ठा syscon_reset *reset)
{
	/* Issue the reset through regmap */
	अगर (reset)
		regmap_update_bits(reset->rmap, reset->offset,
				   reset->mask, reset->मूल्य);

	/* hang !!! */
	sbi_hart_hang();
}

अटल पूर्णांक syscon_poweroff_check(u32 type, u32 reason)
{
	काष्ठा syscon_reset *reset = syscon_reset_get(false, type);

	वापस (reset) ? reset->priority : 0;
}

अटल व्योम syscon_do_poweroff(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(false, type));
}

अटल काष्ठा sbi_system_reset_device syscon_poweroff = {
	.name = "syscon-poweroff",
	.system_reset_check = syscon_poweroff_check,
	.system_reset = syscon_do_poweroff
};

अटल पूर्णांक syscon_reboot_check(u32 type, u32 reason)
{
	काष्ठा syscon_reset *reset = syscon_reset_get(true, type);

	वापस (reset) ? reset->priority : 0;
}

अटल व्योम syscon_do_reboot(u32 type, u32 reason)
{
	syscon_reset_exec(syscon_reset_get(true, type));
}

अटल काष्ठा sbi_system_reset_device syscon_reboot = {
	.name = "syscon-reboot",
	.system_reset_check = syscon_reboot_check,
	.system_reset = syscon_do_reboot
};

अटल पूर्णांक syscon_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			     स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc, len;
	स्थिर fdt32_t *val, *mask;
	bool is_reboot = (ulong)match->तथ्य;
	काष्ठा syscon_reset *reset = (is_reboot) ? &reboot : &poweroff;

	अगर (reset->rmap)
		वापस SBI_EALREADY;

	rc = fdt_regmap_get(fdt, nodeoff, &reset->rmap);
	अगर (rc)
		वापस rc;

	val = fdt_getprop(fdt, nodeoff, "priority", &len);
	reset->priority = (val && len > 0) ? fdt32_to_cpu(*val) : 192;

	val = fdt_getprop(fdt, nodeoff, "offset", &len);
	अगर (val && len > 0)
		reset->offset = fdt32_to_cpu(*val);
	अन्यथा
		वापस SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "value", &len);
	mask = fdt_getprop(fdt, nodeoff, "mask", &len);
	अगर (!val && !mask)
		वापस SBI_EINVAL;

	अगर (!val) {
		/* support old binding */
		reset->मूल्य = fdt32_to_cpu(*mask);
		reset->mask = 0xFFFFFFFF;
	} अन्यथा अगर (!mask) {
		/* support value without mask */
		reset->मूल्य = fdt32_to_cpu(*val);
		reset->mask = 0xFFFFFFFF;
	} अन्यथा {
		reset->मूल्य = fdt32_to_cpu(*val);
		reset->mask = fdt32_to_cpu(*mask);
	}

	अगर (is_reboot)
		sbi_system_reset_add_device(&syscon_reboot);
	अन्यथा
		sbi_system_reset_add_device(&syscon_poweroff);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match syscon_poweroff_match[] = {
	{ .compatible = "syscon-poweroff", .तथ्य = (स्थिर व्योम *)false },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_syscon_poweroff = {
	.match_table = syscon_poweroff_match,
	.init = syscon_reset_init,
};

अटल स्थिर काष्ठा fdt_match syscon_reboot_match[] = {
	{ .compatible = "syscon-reboot", .तथ्य = (स्थिर व्योम *)true },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_syscon_reboot = {
	.match_table = syscon_reboot_match,
	.init = syscon_reset_init,
};
