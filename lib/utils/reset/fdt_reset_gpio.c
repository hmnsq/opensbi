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

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/gpio/fdt_gpio.h>

ಸಂರಚನಾ gpio_reset {
	ಸಂರಚನಾ gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

ಸ್ಥಿರ ಸಂರಚನಾ gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

ಸ್ಥಿರ ಸಂರಚನಾ gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

ಸ್ಥಿರ ಸಂರಚನಾ gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	ಸಂರಚನಾ gpio_reset *reset = ರಿಕ್ತ;

	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಯದಿ (!is_restart)
			reset = &poweroff;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಯದಿ (is_restart)
			reset = &restart;
		ಭಙ್ಗ;
	}

	ಯದಿ (reset && !reset->pin.chip)
		reset = ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ reset;
}

ಸ್ಥಿರ ಶೂನ್ಯ gpio_reset_exec(ಸಂರಚನಾ gpio_reset *reset)
{
	ಯದಿ (reset) {
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gpio_system_poweroff_check(u32 type, u32 reason)
{
	ಯದಿ (gpio_reset_get(false, type))
		ಪ್ರತ್ಯಾಗಮ 128;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gpio_system_restart_check(u32 type, u32 reason)
{
	ಯದಿ (gpio_reset_get(true, type))
		ಪ್ರತ್ಯಾಗಮ 128;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gpio_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			   ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc, len;
	ಧ್ರುವ fdt32_t *val;
	bool is_restart = (ulong)match->data;
	ಧ್ರುವ ವರ್ಣ *dir_prop = (is_restart) ? "open-source" : "input";
	ಸಂರಚನಾ gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಯದಿ (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_ಒಳಹರಿವು(&reset->pin);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	ಯದಿ (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	ಯದಿ (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	ಯದಿ (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	ಅನ್ಯಥಾ
		sbi_system_reset_add_device(&gpio_poweroff);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (ಧ್ರುವ ಶೂನ್ಯ *)false },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (ಧ್ರುವ ಶೂನ್ಯ *)true },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
