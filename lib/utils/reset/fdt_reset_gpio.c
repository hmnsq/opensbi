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

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/gpio/fdt_gpio.h>

ഘടന gpio_reset {
	ഘടന gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

സ്ഥിതം ഘടന gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

സ്ഥിതം ഘടന gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

സ്ഥിതം ഘടന gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	ഘടന gpio_reset *reset = ശൂന്യ;

	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
		എങ്കിൽ (!is_restart)
			reset = &poweroff;
		നിർത്തുക;
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		എങ്കിൽ (is_restart)
			reset = &restart;
		നിർത്തുക;
	}

	എങ്കിൽ (reset && !reset->pin.chip)
		reset = ശൂന്യ;

	തിരികെ reset;
}

സ്ഥിതം ശൂന്യം gpio_reset_exec(ഘടന gpio_reset *reset)
{
	എങ്കിൽ (reset) {
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

സ്ഥിതം പൂർണ്ണസംഖ്യ gpio_system_poweroff_check(u32 type, u32 reason)
{
	എങ്കിൽ (gpio_reset_get(false, type))
		തിരികെ 128;

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

സ്ഥിതം ഘടന sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

സ്ഥിതം പൂർണ്ണസംഖ്യ gpio_system_restart_check(u32 type, u32 reason)
{
	എങ്കിൽ (gpio_reset_get(true, type))
		തിരികെ 128;

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

സ്ഥിതം ഘടന sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

സ്ഥിതം പൂർണ്ണസംഖ്യ gpio_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			   സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc, len;
	സ്ഥിരം fdt32_t *val;
	bool is_restart = (ulong)match->data;
	സ്ഥിരം അക്ഷരം *dir_prop = (is_restart) ? "open-source" : "input";
	ഘടന gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	എങ്കിൽ (rc)
		തിരികെ rc;

	എങ്കിൽ (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_നിവേശം(&reset->pin);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	എങ്കിൽ (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	എങ്കിൽ (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	എങ്കിൽ (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	അല്ലെങ്കിൽ
		sbi_system_reset_add_device(&gpio_poweroff);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (സ്ഥിരം ശൂന്യം *)false },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

സ്ഥിതം സ്ഥിരം ഘടന fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (സ്ഥിരം ശൂന്യം *)true },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
