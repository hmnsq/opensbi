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

#ضمن <libfdt.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/gpio/fdt_gpio.h>

بنية gpio_reset {
	بنية gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

ساكن بنية gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

ساكن بنية gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

ساكن بنية gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	بنية gpio_reset *reset = عدم;

	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
		ܐܢ (!is_restart)
			reset = &poweroff;
		اكسر;
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ܐܢ (is_restart)
			reset = &restart;
		اكسر;
	}

	ܐܢ (reset && !reset->pin.chip)
		reset = عدم;

	ܦܢܐ reset;
}

ساكن فراغ gpio_reset_exec(بنية gpio_reset *reset)
{
	ܐܢ (reset) {
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

ساكن صحيح gpio_system_poweroff_check(u32 type, u32 reason)
{
	ܐܢ (gpio_reset_get(false, type))
		ܦܢܐ 128;

	ܦܢܐ 0;
}

ساكن فراغ gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

ساكن بنية sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

ساكن صحيح gpio_system_restart_check(u32 type, u32 reason)
{
	ܐܢ (gpio_reset_get(true, type))
		ܦܢܐ 128;

	ܦܢܐ 0;
}

ساكن فراغ gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

ساكن بنية sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

ساكن صحيح gpio_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
			   ثابت بنية fdt_match *match)
{
	صحيح rc, len;
	ثابت fdt32_t *val;
	bool is_restart = (ulong)match->data;
	ثابت حرف *dir_prop = (is_restart) ? "open-source" : "input";
	بنية gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	ܐܢ (rc)
		ܦܢܐ rc;

	ܐܢ (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_input(&reset->pin);
		ܐܢ (rc)
			ܦܢܐ rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	ܐܢ (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	ܐܢ (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	ܐܢ (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	ܐܚܪܢܐ
		sbi_system_reset_add_device(&gpio_poweroff);

	ܦܢܐ 0;
}

ساكن ثابت بنية fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (ثابت فراغ *)false },
	{ },
};

ثابت بنية fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

ساكن ثابت بنية fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (ثابت فراغ *)true },
	{ },
};

ثابت بنية fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
