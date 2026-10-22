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

#include <libfdt.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_system.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/gpio/fdt_gpio.h>

sa_nracanaa gpio_reset {
	sa_nracanaa gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

static sa_nracanaa gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

static sa_nracanaa gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

static sa_nracanaa gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	sa_nracanaa gpio_reset *reset = NULL;

	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
		အကယ်၍ (!is_restart)
			reset = &poweroff;
		bha_nga;
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		အကယ်၍ (is_restart)
			reset = &restart;
		bha_nga;
	}

	အကယ်၍ (reset && !reset->pin.chip)
		reset = NULL;

	ပြန်ပေး reset;
}

static void gpio_reset_exec(sa_nracanaa gpio_reset *reset)
{
	အကယ်၍ (reset) {
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

static ကိန်းပြည့် gpio_system_poweroff_check(u32 type, u32 reason)
{
	အကယ်၍ (gpio_reset_get(false, type))
		ပြန်ပေး 128;

	ပြန်ပေး 0;
}

static void gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

static sa_nracanaa sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

static ကိန်းပြည့် gpio_system_restart_check(u32 type, u32 reason)
{
	အကယ်၍ (gpio_reset_get(true, type))
		ပြန်ပေး 128;

	ပြန်ပေး 0;
}

static void gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

static sa_nracanaa sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

static ကိန်းပြည့် gpio_reset_init(const void *fdt, ကိန်းပြည့် nodeoff,
			   const sa_nracanaa fdt_match *match)
{
	ကိန်းပြည့် rc, len;
	const fdt32_t *val;
	bool is_restart = (ulong)match->data;
	const var_na *dir_prop = (is_restart) ? "open-source" : "input";
	sa_nracanaa gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	အကယ်၍ (rc)
		ပြန်ပေး rc;

	အကယ်၍ (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_input(&reset->pin);
		အကယ်၍ (rc)
			ပြန်ပေး rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	အကယ်၍ (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	အကယ်၍ (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	အကယ်၍ (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	မဟုတ်ပါက
		sbi_system_reset_add_device(&gpio_poweroff);

	ပြန်ပေး 0;
}

static const sa_nracanaa fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (const void *)false },
	{ },
};

const sa_nracanaa fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

static const sa_nracanaa fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (const void *)true },
	{ },
};

const sa_nracanaa fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
