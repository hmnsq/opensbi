/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/i2c/fdt_i2c.h>

/* Controller registers */
#define ICR_OFFSET	0x00		/* I2C control register */
#define IDBR_OFFSET	0x0c		/* I2C data buffer register */

/* Control register bits */
#define ICR_START	BIT(0)		/* start */
#define ICR_STOP	BIT(1)		/* stop */
#define ICR_ACKNAK	BIT(2)		/* ACK(0) or NAK(1) */
#define ICR_TB		BIT(3)		/* transfer byte */
#define ICR_UR		BIT(10)		/* unit reset */
#define ICR_SCLE	BIT(13)		/* SCL enable  */
#define ICR_IUE		BIT(14)		/* unit enable */

/* Timing */
#define I2C_RESET_US	10
#define I2C_TIMEOUT_US	1000

ସଂରଚନା spacemit_i2c_adapter {
	unsigned long base;
	ସଂରଚନା i2c_adapter adapter;
};

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ spacemit_i2c_set_reg(ସଂରଚନା spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (ଶୂନ୍ୟ *)adap->base + reg);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t spacemit_i2c_get_reg(ସଂରଚନା spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	ଫେରନ୍ତୁ readl((ଶୂନ୍ୟ *)adap->base + reg);
}

ସ୍ଥିର ଶୂନ୍ୟ spacemit_i2c_reset(ସଂରଚନା spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, ICR_IUE | ICR_SCLE);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_i2c_wait_xfer_done(ସଂରଚନା spacemit_i2c_adapter *adap)
{
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		ଯଦି (!(val & ICR_TB))
			ଫେରନ୍ତୁ 0;

		sbi_timer_udelay(1);
	};

	ଫେରନ୍ତୁ SBI_ETIMEDOUT;
}

ସ୍ଥିର ଶୂନ୍ୟ spacemit_i2c_start_xfer(ସଂରଚନା spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ଅଚଳ uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_i2c_xfer_write(ସଂରଚନା spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	ଫେରନ୍ତୁ spacemit_i2c_wait_xfer_done(adap);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_i2c_xfer_read(ସଂରଚନା spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_i2c_adapter_write(ସଂରଚନା i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ସଂରଚନା spacemit_i2c_adapter *adap =
		container_of(ia, ସଂରଚନା spacemit_i2c_adapter, adapter);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* send register + data bytes */
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_i2c_adapter_read(ସଂରଚନା i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ସଂରଚନା spacemit_i2c_adapter *adap =
		container_of(ia, ସଂରଚନା spacemit_i2c_adapter, adapter);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* read data bytes */
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_i2c_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			     ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ସଂରଚନା spacemit_i2c_adapter *adapter;
	uint64_t base;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	adapter = sbi_zalloc(ଆକାର(*adapter));
	ଯଦି (!adapter)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &base, ଖାଲି);
	ଯଦି (rc) {
		sbi_free(adapter);
		ଫେରନ୍ତୁ rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ଲିଖ = spacemit_i2c_adapter_write;
	adapter->adapter.ପଠ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	ଯଦି (rc) {
		sbi_free(adapter);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
