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

struct gpio_reset {
	struct gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

static struct gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

static struct gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

static struct gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	struct gpio_reset *reset = NULL;

	switch (type) {
	case SBI_SRST_RESET_TYPE_SHUTDOWN:
		ако (!is_restart)
			reset = &poweroff;
		break;
	case SBI_SRST_RESET_TYPE_COLD_REBOOT:
	case SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ако (is_restart)
			reset = &restart;
		break;
	}

	ако (reset && !reset->pin.chip)
		reset = NULL;

	върни reset;
}

static void gpio_reset_exec(struct gpio_reset *reset)
{
	ако (reset) {
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

static цяло gpio_system_poweroff_check(u32 type, u32 reason)
{
	ако (gpio_reset_get(false, type))
		върни 128;

	върни 0;
}

static void gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

static struct sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

static цяло gpio_system_restart_check(u32 type, u32 reason)
{
	ако (gpio_reset_get(true, type))
		върни 128;

	върни 0;
}

static void gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

static struct sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

static цяло gpio_reset_init(const void *fdt, цяло nodeoff,
			   const struct fdt_match *match)
{
	цяло rc, len;
	const fdt32_t *val;
	bool is_restart = (ulong)match->data;
	const char *dir_prop = (is_restart) ? "open-source" : "input";
	struct gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	ако (rc)
		върни rc;

	ако (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_input(&reset->pin);
		ако (rc)
			върни rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	ако (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	ако (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	ако (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	иначе
		sbi_system_reset_add_device(&gpio_poweroff);

	върни 0;
}

static const struct fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (const void *)false },
	{ },
};

const struct fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

static const struct fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (const void *)true },
	{ },
};

const struct fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
