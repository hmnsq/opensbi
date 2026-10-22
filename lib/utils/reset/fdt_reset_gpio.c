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

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/gpio/fdt_gpio.h>

ସଂରଚନା gpio_reset {
	ସଂରଚନା gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

ସ୍ଥିର ସଂରଚନା gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

ସ୍ଥିର ସଂରଚନା gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

ସ୍ଥିର ସଂରଚନା gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	ସଂରଚନା gpio_reset *reset = ଖାଲି;

	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
		ଯଦି (!is_restart)
			reset = &poweroff;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ଯଦି (is_restart)
			reset = &restart;
		ଭାଙ୍ଗ;
	}

	ଯଦି (reset && !reset->pin.chip)
		reset = ଖାଲି;

	ଫେରନ୍ତୁ reset;
}

ସ୍ଥିର ଶୂନ୍ୟ gpio_reset_exec(ସଂରଚନା gpio_reset *reset)
{
	ଯଦି (reset) {
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_system_poweroff_check(u32 type, u32 reason)
{
	ଯଦି (gpio_reset_get(false, type))
		ଫେରନ୍ତୁ 128;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_system_restart_check(u32 type, u32 reason)
{
	ଯଦି (gpio_reset_get(true, type))
		ଫେରନ୍ତୁ 128;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			   ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, len;
	ଅଚଳ fdt32_t *val;
	bool is_restart = (ulong)match->data;
	ଅଚଳ ଅକ୍ଷର *dir_prop = (is_restart) ? "open-source" : "input";
	ସଂରଚନା gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଯଦି (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_ନିବେଶ(&reset->pin);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	ଯଦି (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	ଯଦି (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	ଯଦି (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	ଅନ୍ୟଥା
		sbi_system_reset_add_device(&gpio_poweroff);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (ଅଚଳ ଶୂନ୍ୟ *)false },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (ଅଚଳ ଶୂନ୍ୟ *)true },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
