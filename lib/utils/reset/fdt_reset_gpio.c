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

#شامل <libfdt.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/gpio/fdt_gpio.h>

ساختار gpio_reset {
	ساختار gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

ایستا ساختار gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

ایستا ساختار gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

ایستا ساختار gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	ساختار gpio_reset *reset = پوچ;

	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		اگر (!is_restart)
			reset = &poweroff;
		بشکن;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		اگر (is_restart)
			reset = &restart;
		بشکن;
	}

	اگر (reset && !reset->pin.chip)
		reset = پوچ;

	بازگشت reset;
}

ایستا تهی gpio_reset_exec(ساختار gpio_reset *reset)
{
	اگر (reset) {
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

ایستا صحیح gpio_system_poweroff_check(u32 type, u32 reason)
{
	اگر (gpio_reset_get(false, type))
		بازگشت 128;

	بازگشت 0;
}

ایستا تهی gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

ایستا ساختار sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

ایستا صحیح gpio_system_restart_check(u32 type, u32 reason)
{
	اگر (gpio_reset_get(true, type))
		بازگشت 128;

	بازگشت 0;
}

ایستا تهی gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

ایستا ساختار sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

ایستا صحیح gpio_reset_init(ثابت تهی *fdt, صحیح nodeoff,
			   ثابت ساختار fdt_match *match)
{
	صحیح rc, len;
	ثابت fdt32_t *val;
	bool is_restart = (ulong)match->data;
	ثابت نویسه *dir_prop = (is_restart) ? "open-source" : "input";
	ساختار gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	اگر (rc)
		بازگشت rc;

	اگر (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_درون_داد(&reset->pin);
		اگر (rc)
			بازگشت rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	اگر (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	اگر (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	اگر (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	وگرنه
		sbi_system_reset_add_device(&gpio_poweroff);

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (ثابت تهی *)false },
	{ },
};

ثابت ساختار fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

ایستا ثابت ساختار fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (ثابت تهی *)true },
	{ },
};

ثابت ساختار fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
