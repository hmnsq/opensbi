/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 SiFive
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Green Wan <green.wan@sifive.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/gpio/fdt_gpio.h>

संरचना gpio_reset {
	संरचना gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

स्थिर संरचना gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

स्थिर संरचना gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

स्थिर संरचना gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	संरचना gpio_reset *reset = रिक्त;

	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		यदि (!is_restart)
			reset = &poweroff;
		रोक;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		यदि (is_restart)
			reset = &restart;
		रोक;
	}

	यदि (reset && !reset->pin.chip)
		reset = रिक्त;

	फर्काऊ reset;
}

स्थिर शून्य gpio_reset_exec(संरचना gpio_reset *reset)
{
	यदि (reset) {
		/* drive it active, also inactive->active edge */
		gpio_direction_output(&reset->pin, 1);
		sbi_timer_mdelay(reset->active_delay);

		/* drive inactive, also active->inactive edge */
		gpio_set(&reset->pin, 0);
		sbi_timer_mdelay(reset->inactive_delay);

		/* drive it active, also inactive->active edge */
		gpio_set(&reset->pin, 1);
	}
	/* hang !!! */
	sbi_hart_hang();
}

स्थिर पूर्णाङ्क gpio_system_poweroff_check(u32 type, u32 reason)
{
	यदि (gpio_reset_get(false, type))
		फर्काऊ 128;

	फर्काऊ 0;
}

स्थिर शून्य gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

स्थिर संरचना sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

स्थिर पूर्णाङ्क gpio_system_restart_check(u32 type, u32 reason)
{
	यदि (gpio_reset_get(true, type))
		फर्काऊ 128;

	फर्काऊ 0;
}

स्थिर शून्य gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

स्थिर संरचना sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

स्थिर पूर्णाङ्क gpio_reset_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			   अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc, len;
	अचर fdt32_t *val;
	bool is_restart = (ulong)match->data;
	अचर वर्ण *dir_prop = (is_restart) ? "open-source" : "input";
	संरचना gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	यदि (rc)
		फर्काऊ rc;

	यदि (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_आगत(&reset->pin);
		यदि (rc)
			फर्काऊ rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	यदि (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	यदि (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	यदि (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	अन्यथा
		sbi_system_reset_add_device(&gpio_poweroff);

	फर्काऊ 0;
}

स्थिर अचर संरचना fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (अचर शून्य *)false },
	{ },
};

अचर संरचना fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

स्थिर अचर संरचना fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (अचर शून्य *)true },
	{ },
};

अचर संरचना fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
