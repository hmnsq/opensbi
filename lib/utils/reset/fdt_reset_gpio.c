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

কাঠামো gpio_reset {
	কাঠামো gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

স্থির কাঠামো gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

স্থির কাঠামো gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

স্থির কাঠামো gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	কাঠামো gpio_reset *reset = ফাঁকা;

	নির্বাচন (type) {
	ক্ষেত্র SBI_SRST_RESET_TYPE_SHUTDOWN:
		যদি (!is_restart)
			reset = &poweroff;
		বিরতি;
	ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
		যদি (is_restart)
			reset = &restart;
		বিরতি;
	}

	যদি (reset && !reset->pin.chip)
		reset = ফাঁকা;

	ফেরত reset;
}

স্থির শূন্য gpio_reset_exec(কাঠামো gpio_reset *reset)
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

স্থির পূর্ণসংখ্যা gpio_system_poweroff_check(u32 type, u32 reason)
{
	যদি (gpio_reset_get(false, type))
		ফেরত 128;

	ফেরত 0;
}

স্থির শূন্য gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

স্থির কাঠামো sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

স্থির পূর্ণসংখ্যা gpio_system_restart_check(u32 type, u32 reason)
{
	যদি (gpio_reset_get(true, type))
		ফেরত 128;

	ফেরত 0;
}

স্থির শূন্য gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

স্থির কাঠামো sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

স্থির পূর্ণসংখ্যা gpio_reset_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			   ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc, len;
	ধ্রুবক fdt32_t *val;
	bool is_restart = (ulong)match->তথ্য;
	ধ্রুবক অক্ষর *dir_prop = (is_restart) ? "open-source" : "input";
	কাঠামো gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	যদি (rc)
		ফেরত rc;

	যদি (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_প্রবেশ(&reset->pin);
		যদি (rc)
			ফেরত rc;
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

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .তথ্য = (ধ্রুবক শূন্য *)false },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

স্থির ধ্রুবক কাঠামো fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .তথ্য = (ধ্রুবক শূন্য *)true },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
