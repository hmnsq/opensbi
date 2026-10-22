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

ساخت gpio_reset {
	ساخت gpio_pin pin;
	u32 فعال_delay;
	u32 inactive_delay;
};

ساکن ساخت gpio_reset poweroff = {
	.فعال_delay = 100,
	.inactive_delay = 100
};

ساکن ساخت gpio_reset restart = {
	.فعال_delay = 100,
	.inactive_delay = 100
};

ساکن ساخت gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	ساخت gpio_reset *reset = تہی;

	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		اگر (!is_restart)
			reset = &poweroff;
		توڑو;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		اگر (is_restart)
			reset = &restart;
		توڑو;
	}

	اگر (reset && !reset->pin.chip)
		reset = تہی;

	واپس reset;
}

ساکن خالی gpio_reset_exec(ساخت gpio_reset *reset)
{
	اگر (reset) {
		/* drive it active, also inactive->active edge */
		gpio_direction_output(&reset->pin, 1);
		sbi_timer_mdelay(reset->فعال_delay);

		/* drive inactive, also active->inactive edge */
		gpio_سیٹ(&reset->pin, 0);
		sbi_timer_mdelay(reset->inactive_delay);

		/* drive it active, also inactive->active edge */
		gpio_سیٹ(&reset->pin, 1);
	}
	/* hang !!! */
	sbi_hart_hang();
}

ساکن صحیح gpio_system_poweroff_check(u32 type, u32 reason)
{
	اگر (gpio_reset_get(false, type))
		واپس 128;

	واپس 0;
}

ساکن خالی gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

ساکن ساخت sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

ساکن صحیح gpio_system_restart_check(u32 type, u32 reason)
{
	اگر (gpio_reset_get(true, type))
		واپس 128;

	واپس 0;
}

ساکن خالی gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

ساکن ساخت sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

ساکن صحیح gpio_reset_init(ثابت خالی *fdt, صحیح nodeoff,
			   ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc, len;
	ثابت fdt32_t *val;
	bool is_restart = (ulong)مطابقت->ڈیٹا;
	ثابت حرف *dir_prop = (is_restart) ? "open-source" : "input";
	ساخت gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	اگر (rc)
		واپس rc;

	اگر (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_داخلہ(&reset->pin);
		اگر (rc)
			واپس rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	اگر (len > 0)
		reset->فعال_delay = fdt32_مقصد_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	اگر (len > 0)
		reset->inactive_delay = fdt32_مقصد_cpu(*val);

	اگر (is_restart)
		sbi_system_reset_جوڑو_device(&gpio_restart);
	ورنہ
		sbi_system_reset_جوڑو_device(&gpio_poweroff);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت gpio_poweroff_مطابقت[] = {
	{ .compatible = "gpio-poweroff", .ڈیٹا = (ثابت خالی *)false },
	{ },
};

ثابت ساخت fdt_driver fdt_poweroff_gpio = {
	.مطابقت_جدول = gpio_poweroff_مطابقت,
	.init = gpio_reset_init,
};

ساکن ثابت ساخت fdt_مطابقت gpio_reset_مطابقت[] = {
	{ .compatible = "gpio-restart", .ڈیٹا = (ثابت خالی *)true },
	{ },
};

ثابت ساخت fdt_driver fdt_reset_gpio = {
	.مطابقت_جدول = gpio_reset_مطابقت,
	.init = gpio_reset_init,
};
