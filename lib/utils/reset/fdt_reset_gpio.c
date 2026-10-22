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

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/gpio/fdt_gpio.h>

संरचना gpio_reset {
	संरचना gpio_pin pin;
	u32 सक्रिय_delay;
	u32 inactive_delay;
};

स्थिर संरचना gpio_reset poweroff = {
	.सक्रिय_delay = 100,
	.inactive_delay = 100
};

स्थिर संरचना gpio_reset restart = {
	.सक्रिय_delay = 100,
	.inactive_delay = 100
};

स्थिर संरचना gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	संरचना gpio_reset *reset = रिक्त;

	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		यदि (!is_restart)
			reset = &poweroff;
		भङ्ग;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		यदि (is_restart)
			reset = &restart;
		भङ्ग;
	}

	यदि (reset && !reset->pin.chip)
		reset = रिक्त;

	प्रत्यागम reset;
}

स्थिर शून्य gpio_reset_exec(संरचना gpio_reset *reset)
{
	यदि (reset) {
		/* drive it active, also inactive->active edge */
		gpio_direction_output(&reset->pin, 1);
		sbi_timer_mdelay(reset->सक्रिय_delay);

		/* drive inactive, also active->inactive edge */
		gpio_समुच्चयः(&reset->pin, 0);
		sbi_timer_mdelay(reset->inactive_delay);

		/* drive it active, also inactive->active edge */
		gpio_समुच्चयः(&reset->pin, 1);
	}
	/* hang !!! */
	sbi_hart_hang();
}

स्थिर पूर्णाङ्क gpio_system_poweroff_check(u32 type, u32 reason)
{
	यदि (gpio_reset_get(false, type))
		प्रत्यागम 128;

	प्रत्यागम 0;
}

स्थिर शून्य gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

स्थिर संरचना sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

स्थिर पूर्णाङ्क gpio_system_restart_check(u32 type, u32 reason)
{
	यदि (gpio_reset_get(true, type))
		प्रत्यागम 128;

	प्रत्यागम 0;
}

स्थिर शून्य gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

स्थिर संरचना sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

स्थिर पूर्णाङ्क gpio_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			   ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc, len;
	ध्रुव fdt32_t *val;
	bool is_restart = (ulong)मेलनम्->दत्त;
	ध्रुव वर्ण *dir_prop = (is_restart) ? "open-source" : "input";
	संरचना gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	यदि (rc)
		प्रत्यागम rc;

	यदि (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_निवेशः(&reset->pin);
		यदि (rc)
			प्रत्यागम rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	यदि (len > 0)
		reset->सक्रिय_delay = fdt32_गन्तव्यम्_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	यदि (len > 0)
		reset->inactive_delay = fdt32_गन्तव्यम्_cpu(*val);

	यदि (is_restart)
		sbi_system_reset_सङ्कलय_device(&gpio_restart);
	अन्यथा
		sbi_system_reset_सङ्कलय_device(&gpio_poweroff);

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् gpio_poweroff_मेलनम्[] = {
	{ .compatible = "gpio-poweroff", .दत्त = (ध्रुव शून्य *)false },
	{ },
};

ध्रुव संरचना fdt_driver fdt_poweroff_gpio = {
	.मेलनम्_सारणी = gpio_poweroff_मेलनम्,
	.init = gpio_reset_init,
};

स्थिर ध्रुव संरचना fdt_मेलनम् gpio_reset_मेलनम्[] = {
	{ .compatible = "gpio-restart", .दत्त = (ध्रुव शून्य *)true },
	{ },
};

ध्रुव संरचना fdt_driver fdt_reset_gpio = {
	.मेलनम्_सारणी = gpio_reset_मेलनम्,
	.init = gpio_reset_init,
};
