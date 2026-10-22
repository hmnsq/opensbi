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

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/gpio/fdt_gpio.h>

నిర్మాణం gpio_reset {
	నిర్మాణం gpio_pin pin;
	u32 చురుకైన_delay;
	u32 inactive_delay;
};

స్థిర నిర్మాణం gpio_reset poweroff = {
	.చురుకైన_delay = 100,
	.inactive_delay = 100
};

స్థిర నిర్మాణం gpio_reset restart = {
	.చురుకైన_delay = 100,
	.inactive_delay = 100
};

స్థిర నిర్మాణం gpio_reset *gpio_reset_get(bool is_restart, u32 type)
{
	నిర్మాణం gpio_reset *reset = ఖాళీ;

	ఎంపిక (type) {
	స్థితి SBI_SRST_RESET_TYPE_SHUTDOWN:
		ఒకవేళ (!is_restart)
			reset = &poweroff;
		ఆపు;
	స్థితి SBI_SRST_RESET_TYPE_COLD_REBOOT:
	స్థితి SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ఒకవేళ (is_restart)
			reset = &restart;
		ఆపు;
	}

	ఒకవేళ (reset && !reset->pin.chip)
		reset = ఖాళీ;

	తిరిగి reset;
}

స్థిర శూన్య gpio_reset_exec(నిర్మాణం gpio_reset *reset)
{
	ఒకవేళ (reset) {
		/* drive it active, also inactive->active edge */
		gpio_direction_output(&reset->pin, 1);
		sbi_timer_mdelay(reset->చురుకైన_delay);

		/* drive inactive, also active->inactive edge */
		gpio_సమితి(&reset->pin, 0);
		sbi_timer_mdelay(reset->inactive_delay);

		/* drive it active, also inactive->active edge */
		gpio_సమితి(&reset->pin, 1);
	}
	/* hang !!! */
	sbi_hart_hang();
}

స్థిర పూర్ణాంకం gpio_system_poweroff_check(u32 type, u32 reason)
{
	ఒకవేళ (gpio_reset_get(false, type))
		తిరిగి 128;

	తిరిగి 0;
}

స్థిర శూన్య gpio_system_poweroff(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(false, type));
}

స్థిర నిర్మాణం sbi_system_reset_device gpio_poweroff = {
	.name = "gpio-poweroff",
	.system_reset_check = gpio_system_poweroff_check,
	.system_reset = gpio_system_poweroff
};

స్థిర పూర్ణాంకం gpio_system_restart_check(u32 type, u32 reason)
{
	ఒకవేళ (gpio_reset_get(true, type))
		తిరిగి 128;

	తిరిగి 0;
}

స్థిర శూన్య gpio_system_restart(u32 type, u32 reason)
{
	gpio_reset_exec(gpio_reset_get(true, type));
}

స్థిర నిర్మాణం sbi_system_reset_device gpio_restart = {
	.name = "gpio-restart",
	.system_reset_check = gpio_system_restart_check,
	.system_reset = gpio_system_restart
};

స్థిర పూర్ణాంకం gpio_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			   స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc, len;
	స్థిరరాశి fdt32_t *val;
	bool is_restart = (ulong)సరిపోలిక->దత్తాంశం;
	స్థిరరాశి అక్షరం *dir_prop = (is_restart) ? "open-source" : "input";
	నిర్మాణం gpio_reset *reset = (is_restart) ? &restart : &poweroff;

	rc = fdt_gpio_pin_get(fdt, nodeoff, 0, &reset->pin);
	ఒకవేళ (rc)
		తిరిగి rc;

	ఒకవేళ (fdt_getprop(fdt, nodeoff, dir_prop, &len)) {
		rc = gpio_direction_ఆదానం(&reset->pin);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	val = fdt_getprop(fdt, nodeoff, "active-delay-ms", &len);
	ఒకవేళ (len > 0)
		reset->చురుకైన_delay = fdt32_గమ్యం_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "inactive-delay-ms", &len);
	ఒకవేళ (len > 0)
		reset->inactive_delay = fdt32_గమ్యం_cpu(*val);

	ఒకవేళ (is_restart)
		sbi_system_reset_కూడు_device(&gpio_restart);
	లేకపోతే
		sbi_system_reset_కూడు_device(&gpio_poweroff);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక gpio_poweroff_సరిపోలిక[] = {
	{ .compatible = "gpio-poweroff", .దత్తాంశం = (స్థిరరాశి శూన్య *)false },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_poweroff_gpio = {
	.సరిపోలిక_పట్టిక = gpio_poweroff_సరిపోలిక,
	.init = gpio_reset_init,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక gpio_reset_సరిపోలిక[] = {
	{ .compatible = "gpio-restart", .దత్తాంశం = (స్థిరరాశి శూన్య *)true },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_gpio = {
	.సరిపోలిక_పట్టిక = gpio_reset_సరిపోలిక,
	.init = gpio_reset_init,
};
