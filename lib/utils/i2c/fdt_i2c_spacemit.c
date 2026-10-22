/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/i2c/fdt_i2c.h>

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

கட்டமைப்பு spacemit_i2c_adapter {
	unsigned long base;
	கட்டமைப்பு i2c_adapter adapter;
};

நிலையான உள்வரி வெறுமை spacemit_i2c_set_reg(கட்டமைப்பு spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (வெறுமை *)adap->base + reg);
}

நிலையான உள்வரி uint32_t spacemit_i2c_get_reg(கட்டமைப்பு spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	திரும்பு readl((வெறுமை *)adap->base + reg);
}

நிலையான வெறுமை spacemit_i2c_reset(கட்டமைப்பு spacemit_i2c_adapter *adap)
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

நிலையான முழு spacemit_i2c_wait_xfer_done(கட்டமைப்பு spacemit_i2c_adapter *adap)
{
	க்கு (முழு i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		எனில் (!(val & ICR_TB))
			திரும்பு 0;

		sbi_timer_udelay(1);
	};

	திரும்பு SBI_ETIMEDOUT;
}

நிலையான வெறுமை spacemit_i2c_start_xfer(கட்டமைப்பு spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	மாறிலி uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

நிலையான முழு spacemit_i2c_xfer_write(கட்டமைப்பு spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	திரும்பு spacemit_i2c_wait_xfer_done(adap);
}

நிலையான முழு spacemit_i2c_xfer_read(கட்டமைப்பு spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	முழு rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	எனில் (rc)
		திரும்பு rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	திரும்பு 0;
}

நிலையான முழு spacemit_i2c_adapter_write(கட்டமைப்பு i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, முழு len)
{
	கட்டமைப்பு spacemit_i2c_adapter *adap =
		container_of(ia, கட்டமைப்பு spacemit_i2c_adapter, adapter);
	முழு rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	எனில் (rc)
		திரும்பு rc;

	/* send register + data bytes */
	க்கு (முழு i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		எனில் (rc)
			திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான முழு spacemit_i2c_adapter_read(கட்டமைப்பு i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, முழு len)
{
	கட்டமைப்பு spacemit_i2c_adapter *adap =
		container_of(ia, கட்டமைப்பு spacemit_i2c_adapter, adapter);
	முழு rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	எனில் (rc)
		திரும்பு rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	எனில் (rc)
		திரும்பு rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	எனில் (rc)
		திரும்பு rc;

	/* read data bytes */
	க்கு (முழு i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		எனில் (rc)
			திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான முழு spacemit_i2c_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			     மாறிலி கட்டமைப்பு fdt_match *match)
{
	கட்டமைப்பு spacemit_i2c_adapter *adapter;
	uint64_t base;
	முழு rc;

	adapter = sbi_zalloc(அளவு(*adapter));
	எனில் (!adapter)
		திரும்பு SBI_ENOMEM;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &base, வெற்று);
	எனில் (rc) {
		sbi_free(adapter);
		திரும்பு rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.லிக = spacemit_i2c_adapter_write;
	adapter->adapter.பட = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	எனில் (rc) {
		sbi_free(adapter);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
