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

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/gpio/fdt_gpio.h>

সংরচনা gpio_reset {
	সংরচনা gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

স্থির সংরচনা gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

স্থির সংরচনা gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

স্থির সংরচনা gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	সংরচনা gpio_reset *reset = রিক্ত;

	বিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
		যদি (!is_restart)
			reset = &poweroff;
		ভঙ্গ;
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		যদি (is_restart)
			reset = &restart;
		ভঙ্গ;
	}

	যদি (reset && !reset->pin.chip)
		reset = রিক্ত;

	প্রত্যাগম reset;
}

স্থির শূন্য gpio_reset_exec(সংরচনা gpio_reset *reset)
{
	যদি (reset) {
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

স্থির পূর্ণাঙ্ক gpio_system_poweroff_check(u32 type, u32 reason)
{
	যদি (gpio_reset_get(false, type))
		প্রত্যাগম 128;

	প্রত্যাগম 0;
}

স্থির শূন্য gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

স্থির সংরচনা sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

স্থির পূর্ণাঙ্ক gpio_system_restart_check(u32 type, u32 reason)
{
	যদি (gpio_reset_get(true, type))
		প্রত্যাগম 128;

	প্রত্যাগম 0;
}

স্থির শূন্য gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

স্থির সংরচনা sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

স্থির পূর্ণাঙ্ক gpio_reset_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			   ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc, len;
	ধ্রুব fdt32_t *val;
	bool is_restart = (ulong)match->data;
	ধ্রুব বর্ণ *dir_prop = (is_restart) ? "open-source" : "input";
	সংরচনা gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	যদি (rc)
		প্রত্যাগম rc;

	যদি (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_প্রবেশ(&reset->pin);
		যদি (rc)
			প্রত্যাগম rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	যদি (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	যদি (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	যদি (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	অন্যথা
		sbi_system_reset_add_device(&gpio_poweroff);

	প্রত্যাগম 0;
}

স্থির ধ্রুব সংরচনা fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (ধ্রুব শূন্য *)false },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

স্থির ধ্রুব সংরচনা fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (ধ্রুব শূন্য *)true },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
