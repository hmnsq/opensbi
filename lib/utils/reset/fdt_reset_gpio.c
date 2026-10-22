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

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/gpio/fdt_gpio.h>

સંરચના gpio_reset {
	સંરચના gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

સ્થિર સંરચના gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

સ્થિર સંરચના gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

સ્થિર સંરચના gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	સંરચના gpio_reset *reset = ખાલી;

	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
		જો (!is_restart)
			reset = &poweroff;
		તોડો;
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		જો (is_restart)
			reset = &restart;
		તોડો;
	}

	જો (reset && !reset->pin.chip)
		reset = ખાલી;

	પાછા reset;
}

સ્થિર શૂન્ય gpio_reset_exec(સંરચના gpio_reset *reset)
{
	જો (reset) {
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

સ્થિર પૂર્ણાંક gpio_system_poweroff_check(u32 type, u32 reason)
{
	જો (gpio_reset_get(false, type))
		પાછા 128;

	પાછા 0;
}

સ્થિર શૂન્ય gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

સ્થિર સંરચના sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

સ્થિર પૂર્ણાંક gpio_system_restart_check(u32 type, u32 reason)
{
	જો (gpio_reset_get(true, type))
		પાછા 128;

	પાછા 0;
}

સ્થિર શૂન્ય gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

સ્થિર સંરચના sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

સ્થિર પૂર્ણાંક gpio_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			   અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc, len;
	અચલ fdt32_t *val;
	bool is_restart = (ulong)match->data;
	અચલ અક્ષર *dir_prop = (is_restart) ? "open-source" : "input";
	સંરચના gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	જો (rc)
		પાછા rc;

	જો (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_આગત(&reset->pin);
		જો (rc)
			પાછા rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	જો (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	જો (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	જો (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	નહીંતર
		sbi_system_reset_add_device(&gpio_poweroff);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (અચલ શૂન્ય *)false },
	{ },
};

અચલ સંરચના fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

સ્થિર અચલ સંરચના fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (અચલ શૂન્ય *)true },
	{ },
};

અચલ સંરચના fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
