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

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/gpio/fdt_gpio.h>

ਬਣਤਰ gpio_reset {
	ਬਣਤਰ gpio_pin pin;
	u32 ਸਰਗਰਮ_delay;
	u32 inactive_delay;
};

ਸਥਿਰ ਬਣਤਰ gpio_reset poweroff = {
	.ਸਰਗਰਮ_delay = 100,
	.inactive_delay = 100
};

ਸਥਿਰ ਬਣਤਰ gpio_reset restart = {
	.ਸਰਗਰਮ_delay = 100,
	.inactive_delay = 100
};

ਸਥਿਰ ਬਣਤਰ gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	ਬਣਤਰ gpio_reset *reset = ਨਿਰਾ;

	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ਜੇ (!is_restart)
			reset = &poweroff;
		ਰੋਕੋ;
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ਜੇ (is_restart)
			reset = &restart;
		ਰੋਕੋ;
	}

	ਜੇ (reset && !reset->pin.chip)
		reset = ਨਿਰਾ;

	ਵਾਪਸ reset;
}

ਸਥਿਰ ਖਾਲੀ gpio_reset_exec(ਬਣਤਰ gpio_reset *reset)
{
	ਜੇ (reset) {
		/* drive it active, also inactive->active edge */
		gpio_direction_output(&reset->pin, 1);
		sbi_timer_mdelay(reset->ਸਰਗਰਮ_delay);

		/* drive inactive, also active->inactive edge */
		gpio_ਸੈੱਟ(&reset->pin, 0);
		sbi_timer_mdelay(reset->inactive_delay);

		/* drive it active, also inactive->active edge */
		gpio_ਸੈੱਟ(&reset->pin, 1);
	}
	/* hang !!! */
	sbi_hart_hang();
}

ਸਥਿਰ ਪੂਰਨ gpio_system_poweroff_check(u32 type, u32 reason)
{
	ਜੇ (gpio_reset_get(false, type))
		ਵਾਪਸ 128;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

ਸਥਿਰ ਪੂਰਨ gpio_system_restart_check(u32 type, u32 reason)
{
	ਜੇ (gpio_reset_get(true, type))
		ਵਾਪਸ 128;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

ਸਥਿਰ ਪੂਰਨ gpio_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			   ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc, len;
	ਅਚਲ fdt32_t *val;
	bool is_restart = (ulong)ਮੇਲ->ਅੰਕੜੇ;
	ਅਚਲ ਅੱਖਰ *dir_prop = (is_restart) ? "open-source" : "input";
	ਬਣਤਰ gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਜੇ (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_ਨਿਵੇਸ਼(&reset->pin);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	ਜੇ (len > 0)
		reset->ਸਰਗਰਮ_delay = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	ਜੇ (len > 0)
		reset->inactive_delay = fdt32_ਮੰਜ਼ਿਲ_cpu(*val);

	ਜੇ (is_restart)
		sbi_system_reset_ਜੋੜੋ_device(&gpio_restart);
	ਨਹੀਂਤਾਂ
		sbi_system_reset_ਜੋੜੋ_device(&gpio_poweroff);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ gpio_poweroff_ਮੇਲ[] = {
	{ .compatible = "gpio-poweroff", .ਅੰਕੜੇ = (ਅਚਲ ਖਾਲੀ *)false },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_poweroff_gpio = {
	.ਮੇਲ_ਸਾਰਣੀ = gpio_poweroff_ਮੇਲ,
	.init = gpio_reset_init,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ gpio_reset_ਮੇਲ[] = {
	{ .compatible = "gpio-restart", .ਅੰਕੜੇ = (ਅਚਲ ਖਾਲੀ *)true },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_gpio = {
	.ਮੇਲ_ਸਾਰਣੀ = gpio_reset_ਮੇਲ,
	.init = gpio_reset_init,
};
