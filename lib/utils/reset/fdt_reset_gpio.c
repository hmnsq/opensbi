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

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/gpio/fdt_gpio.h>

கட்டமைப்பு gpio_reset {
	கட்டமைப்பு gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

நிலையான கட்டமைப்பு gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

நிலையான கட்டமைப்பு gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

நிலையான கட்டமைப்பு gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	கட்டமைப்பு gpio_reset *reset = வெற்று;

	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
		எனில் (!is_restart)
			reset = &poweroff;
		நிறுத்து;
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		எனில் (is_restart)
			reset = &restart;
		நிறுத்து;
	}

	எனில் (reset && !reset->pin.chip)
		reset = வெற்று;

	திரும்பு reset;
}

நிலையான வெறுமை gpio_reset_exec(கட்டமைப்பு gpio_reset *reset)
{
	எனில் (reset) {
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

நிலையான முழு gpio_system_poweroff_check(u32 type, u32 reason)
{
	எனில் (gpio_reset_get(false, type))
		திரும்பு 128;

	திரும்பு 0;
}

நிலையான வெறுமை gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

நிலையான கட்டமைப்பு sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

நிலையான முழு gpio_system_restart_check(u32 type, u32 reason)
{
	எனில் (gpio_reset_get(true, type))
		திரும்பு 128;

	திரும்பு 0;
}

நிலையான வெறுமை gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

நிலையான கட்டமைப்பு sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

நிலையான முழு gpio_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			   மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc, len;
	மாறிலி fdt32_t *val;
	bool is_restart = (ulong)match->data;
	மாறிலி எழுத்து *dir_prop = (is_restart) ? "open-source" : "input";
	கட்டமைப்பு gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	எனில் (rc)
		திரும்பு rc;

	எனில் (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_உள்ளீடு(&reset->pin);
		எனில் (rc)
			திரும்பு rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	எனில் (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	எனில் (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	எனில் (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	இல்லையெனில்
		sbi_system_reset_add_device(&gpio_poweroff);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .data = (மாறிலி வெறுமை *)false },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

நிலையான மாறிலி கட்டமைப்பு fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .data = (மாறிலி வெறுமை *)true },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
