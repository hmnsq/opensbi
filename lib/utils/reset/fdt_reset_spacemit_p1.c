/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <platform_override.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

ସ୍ଥିର ସଂରଚନା i2c_adapter *p1_adapter = ଖାଲି;
ସ୍ଥିର uint32_t p1_reg = 0;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା p1_system_reset_check(uint32_t type, uint32_t reason)
{
	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
		ଫେରନ୍ତୁ 1;
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ଫେରନ୍ତୁ 255;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ p1_ops(uint32_t type)
{
	uint8_t byte;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	ଯଦି (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	ଅନ୍ୟଥା
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	ଯଦି (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

ସ୍ଥିର ଶୂନ୍ୟ p1_system_reset(uint32_t type, uint32_t reason)
{
	ବାଛ (type) {
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_SHUTDOWN:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ସ୍ଥିତି SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		ଭାଙ୍ଗ;
	}
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା p1_reset_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			 ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, ଖାଲି);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	ଯଦି (i2c_bus < 0)
		ଫେରନ୍ତୁ i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	sbi_system_reset_add_device(&p1_reset);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
