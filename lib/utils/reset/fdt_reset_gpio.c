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

#समावेश <libfdt.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/gpio/fdt_gpio.h>

काष्ठा gpio_reset {
	काष्ठा gpio_pin pin;
	u32 active_delay;
	u32 inactive_delay;
};

अटल काष्ठा gpio_reset poweroff = {
	.active_delay = 100,
	.inactive_delay = 100
};

अटल काष्ठा gpio_reset restart = {
	.active_delay = 100,
	.inactive_delay = 100
};

अटल काष्ठा gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	काष्ठा gpio_reset *reset = शून्य;

	चयन (type) {
	हाल SBI_SRST_RESET_TYPE_SHUTDOWN:
		अगर (!is_restart)
			reset = &poweroff;
		अवरोध;
	हाल SBI_SRST_RESET_TYPE_COLD_REBOOT:
	हाल SBI_SRST_RESET_TYPE_WARM_REBOOT:
		अगर (is_restart)
			reset = &restart;
		अवरोध;
	}

	अगर (reset && !reset->pin.chip)
		reset = शून्य;

	वापस reset;
}

अटल व्योम gpio_reset_exec(काष्ठा gpio_reset *reset)
{
	अगर (reset) {
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

अटल पूर्णांक gpio_system_poweroff_check(u32 type, u32 reason)
{
	अगर (gpio_reset_get(false, type))
		वापस 128;

	वापस 0;
}

अटल व्योम gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

अटल काष्ठा sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

अटल पूर्णांक gpio_system_restart_check(u32 type, u32 reason)
{
	अगर (gpio_reset_get(true, type))
		वापस 128;

	वापस 0;
}

अटल व्योम gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

अटल काष्ठा sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

अटल पूर्णांक gpio_reset_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			   स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc, len;
	स्थिर fdt32_t *val;
	bool is_restart = (ulong)match->तथ्य;
	स्थिर अक्षर *dir_prop = (is_restart) ? "open-source" : "input";
	काष्ठा gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	अगर (rc)
		वापस rc;

	अगर (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_निवेश(&reset->pin);
		अगर (rc)
			वापस rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	अगर (len > 0)
		reset->active_delay = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	अगर (len > 0)
		reset->inactive_delay = fdt32_to_cpu(*val);

	अगर (is_restart)
		sbi_system_reset_add_device(&gpio_restart);
	अन्यथा
		sbi_system_reset_add_device(&gpio_poweroff);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match gpio_poweroff_match[] = {
	{ .compatible = "gpio-poweroff", .तथ्य = (स्थिर व्योम *)false },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_poweroff_gpio = {
	.match_table = gpio_poweroff_match,
	.init = gpio_reset_init,
};

अटल स्थिर काष्ठा fdt_match gpio_reset_match[] = {
	{ .compatible = "gpio-restart", .तथ्य = (स्थिर व्योम *)true },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_reset_gpio = {
	.match_table = gpio_reset_match,
	.init = gpio_reset_init,
};
