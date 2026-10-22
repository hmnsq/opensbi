/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/i2c/fdt_i2c.h>

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

ساختار spacemit_i2c_adapter {
	unsigned long base;
	ساختار i2c_adapter adapter;
};

ایستا درونخطی تهی spacemit_i2c_set_reg(ساختار spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (تهی *)adap->base + reg);
}

ایستا درونخطی uint32_t spacemit_i2c_get_reg(ساختار spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	بازگشت readl((تهی *)adap->base + reg);
}

ایستا تهی spacemit_i2c_reset(ساختار spacemit_i2c_adapter *adap)
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

ایستا صحیح spacemit_i2c_wait_xfer_done(ساختار spacemit_i2c_adapter *adap)
{
	برای (صحیح i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		اگر (!(val & ICR_TB))
			بازگشت 0;

		sbi_timer_udelay(1);
	};

	بازگشت SBI_ETIMEDOUT;
}

ایستا تهی spacemit_i2c_start_xfer(ساختار spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ثابت uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

ایستا صحیح spacemit_i2c_xfer_write(ساختار spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	بازگشت spacemit_i2c_wait_xfer_done(adap);
}

ایستا صحیح spacemit_i2c_xfer_read(ساختار spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	صحیح rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	اگر (rc)
		بازگشت rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	بازگشت 0;
}

ایستا صحیح spacemit_i2c_adapter_write(ساختار i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, صحیح len)
{
	ساختار spacemit_i2c_adapter *adap =
		container_of(ia, ساختار spacemit_i2c_adapter, adapter);
	صحیح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	اگر (rc)
		بازگشت rc;

	/* send register + data bytes */
	برای (صحیح i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		اگر (rc)
			بازگشت rc;
	}

	بازگشت 0;
}

ایستا صحیح spacemit_i2c_adapter_read(ساختار i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, صحیح len)
{
	ساختار spacemit_i2c_adapter *adap =
		container_of(ia, ساختار spacemit_i2c_adapter, adapter);
	صحیح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	اگر (rc)
		بازگشت rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	اگر (rc)
		بازگشت rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	اگر (rc)
		بازگشت rc;

	/* read data bytes */
	برای (صحیح i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		اگر (rc)
			بازگشت rc;
	}

	بازگشت 0;
}

ایستا صحیح spacemit_i2c_init(ثابت تهی *fdt, صحیح nodeoff,
			     ثابت ساختار fdt_match *match)
{
	ساختار spacemit_i2c_adapter *adapter;
	uint64_t base;
	صحیح rc;

	adapter = sbi_zalloc(اندازه(*adapter));
	اگر (!adapter)
		بازگشت SBI_ENOMEM;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &base, پوچ);
	اگر (rc) {
		sbi_free(adapter);
		بازگشت rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = spacemit_i2c_adapter_write;
	adapter->adapter.اقرأ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	اگر (rc) {
		sbi_free(adapter);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ثابت ساختار fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
