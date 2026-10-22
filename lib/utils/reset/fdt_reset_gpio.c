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

ರಚನೆ gpio_reset {
	ರಚನೆ gpio_pin pin;
	u32 ಸಕ್ರಿಯ_delay;
	u32 inactive_delay;
};

ಸ್ಥಿರ ರಚನೆ gpio_reset poweroff = {
	.ಸಕ್ರಿಯ_delay = 100,
	.inactive_delay = 100
};

ಸ್ಥಿರ ರಚನೆ gpio_reset restart = {
	.ಸಕ್ರಿಯ_delay = 100,
	.inactive_delay = 100
};

ಸ್ಥಿರ ರಚನೆ gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	ರಚನೆ gpio_reset *reset = ಖಾಲಿ;

	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಆದರೆ (!is_restart)
			reset = &poweroff;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಆದರೆ (is_restart)
			reset = &restart;
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (reset && !reset->pin.chip)
		reset = ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ reset;
}

ಸ್ಥಿರ ಶೂನ್ಯ gpio_reset_exec(ರಚನೆ gpio_reset *reset)
{
	ಆದರೆ (reset) {
		/* drive it active, also inactive->active edge */
		gpio_direction_output(&reset->pin, 1);
		sbi_timer_mdelay(reset->ಸಕ್ರಿಯ_delay);

		/* drive inactive, also active->inactive edge */
		gpio_ಗಣ(&reset->pin, 0);
		sbi_timer_mdelay(reset->inactive_delay);

		/* drive it active, also inactive->active edge */
		gpio_ಗಣ(&reset->pin, 1);
	}
	/* hang !!! */
	sbi_hart_hang();
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gpio_system_poweroff_check(u32 type, u32 reason)
{
	ಆದರೆ (gpio_reset_get(false, type))
		ಹಿಂತಿರುಗಿ 128;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gpio_system_restart_check(u32 type, u32 reason)
{
	ಆದರೆ (gpio_reset_get(true, type))
		ಹಿಂತಿರುಗಿ 128;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gpio_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			   ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc, len;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	bool is_restart = (ulong)ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir_prop = (is_restart) ? "open-source" : "input";
	ರಚನೆ gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಆದರೆ (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_ಒಳಹರಿವು(&reset->pin);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	ಆದರೆ (len > 0)
		reset->ಸಕ್ರಿಯ_delay = fdt32_ಗಮ್ಯ_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	ಆದರೆ (len > 0)
		reset->inactive_delay = fdt32_ಗಮ್ಯ_cpu(*val);

	ಆದರೆ (is_restart)
		sbi_system_reset_ಕೂಡಿಸು_device(&gpio_restart);
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_system_reset_ಕೂಡಿಸು_device(&gpio_poweroff);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ gpio_poweroff_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "gpio-poweroff", .ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)false },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_poweroff_gpio = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = gpio_poweroff_ಹೊಂದಿಕೆ,
	.init = gpio_reset_init,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ gpio_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "gpio-restart", .ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)true },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_gpio = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = gpio_reset_ಹೊಂದಿಕೆ,
	.init = gpio_reset_init,
};
