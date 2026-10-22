/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/i2c/fdt_i2c.h>

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

بنية spacemit_i2c_adapter {
	unsigned long base;
	بنية i2c_adapter adapter;
};

ساكن مضمن فراغ spacemit_i2c_set_reg(بنية spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (فراغ *)adap->base + reg);
}

ساكن مضمن uint32_t spacemit_i2c_get_reg(بنية spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	أرجع readl((فراغ *)adap->base + reg);
}

ساكن فراغ spacemit_i2c_reset(بنية spacemit_i2c_adapter *adap)
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

ساكن صحيح spacemit_i2c_wait_xfer_done(بنية spacemit_i2c_adapter *adap)
{
	لكل (صحيح i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		إذا (!(val & ICR_TB))
			أرجع 0;

		sbi_timer_udelay(1);
	};

	أرجع SBI_ETIMEDOUT;
}

ساكن فراغ spacemit_i2c_start_xfer(بنية spacemit_i2c_adapter *adap,
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

ساكن صحيح spacemit_i2c_xfer_write(بنية spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	أرجع spacemit_i2c_wait_xfer_done(adap);
}

ساكن صحيح spacemit_i2c_xfer_read(بنية spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	صحيح rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	إذا (rc)
		أرجع rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	أرجع 0;
}

ساكن صحيح spacemit_i2c_adapter_write(بنية i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, صحيح len)
{
	بنية spacemit_i2c_adapter *adap =
		container_of(ia, بنية spacemit_i2c_adapter, adapter);
	صحيح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	إذا (rc)
		أرجع rc;

	/* send register + data bytes */
	لكل (صحيح i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح spacemit_i2c_adapter_read(بنية i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, صحيح len)
{
	بنية spacemit_i2c_adapter *adap =
		container_of(ia, بنية spacemit_i2c_adapter, adapter);
	صحيح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	إذا (rc)
		أرجع rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	إذا (rc)
		أرجع rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	إذا (rc)
		أرجع rc;

	/* read data bytes */
	لكل (صحيح i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح spacemit_i2c_init(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_match *match)
{
	بنية spacemit_i2c_adapter *adapter;
	uint64_t base;
	صحيح rc;

	adapter = sbi_zalloc(حجم(*adapter));
	إذا (!adapter)
		أرجع SBI_ENOMEM;

	rc = fdt_get_نود_addr_size(fdt, nodeoff, 0, &base, عدم);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = spacemit_i2c_adapter_write;
	adapter->adapter.اقرأ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ثابت بنية fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
