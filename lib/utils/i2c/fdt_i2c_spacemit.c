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
#تعریف_کرو ICR_انحراف	0x00		/* I2C control register */
#تعریف_کرو IDBR_انحراف	0x0c		/* I2C data buffer register */

/* Control register bits */
#تعریف_کرو ICR_آغاز	ثنائی_ہندسہ(0)		/*آغاز*/
#تعریف_کرو ICR_STOP	ثنائی_ہندسہ(1)		/* stop */
#تعریف_کرو ICR_ACKNAK	ثنائی_ہندسہ(2)		/* ACK(0) or NAK(1) */
#تعریف_کرو ICR_TB		ثنائی_ہندسہ(3)		/* transfer byte */
#تعریف_کرو ICR_UR		ثنائی_ہندسہ(10)		/* unit reset */
#تعریف_کرو ICR_SCLE	ثنائی_ہندسہ(13)		/* SCL enable  */
#تعریف_کرو ICR_IUE		ثنائی_ہندسہ(14)		/* unit enable */

/* Timing */
#تعریف_کرو I2C_RESET_US	10
#تعریف_کرو I2C_TIMEOUT_US	1000

ساخت spacemit_i2c_adapter {
	غیرعلامتی طویل بنیاد_2;
	ساخت i2c_adapter adapter;
};

ساکن اندرونی خالی spacemit_i2c_سیٹ_reg(ساخت spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (خالی *)adap->بنیاد_2 + reg);
}

ساکن اندرونی uint32_t spacemit_i2c_get_reg(ساخت spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	واپس readl((خالی *)adap->بنیاد_2 + reg);
}

ساکن خالی spacemit_i2c_reset(ساخت spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_سیٹ_reg(adap, ICR_انحراف, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_سیٹ_reg(adap, ICR_انحراف, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_سیٹ_reg(adap, ICR_انحراف, ICR_IUE | ICR_SCLE);
}

ساکن صحیح spacemit_i2c_wait_xfer_مکمل(ساخت spacemit_i2c_adapter *adap)
{
	برائے (صحیح i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_انحراف);

		اگر (!(val & ICR_TB))
			واپس 0;

		sbi_timer_udelay(1);
	};

	واپس SBI_ETIMEDOUT;
}

ساکن خالی spacemit_i2c_آغاز_xfer(ساخت spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ثابت uint32_t ctrl_نقاب = ICR_آغاز | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_انحراف);
	val &= ~ctrl_نقاب;
	val |= (ctrl & ctrl_نقاب);
	val |= ICR_TB;

	spacemit_i2c_سیٹ_reg(adap, ICR_انحراف, val);
}

ساکن صحیح spacemit_i2c_xfer_write(ساخت spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_سیٹ_reg(adap, IDBR_انحراف, byte);
	spacemit_i2c_آغاز_xfer(adap, ctrl);

	واپس spacemit_i2c_wait_xfer_مکمل(adap);
}

ساکن صحیح spacemit_i2c_xfer_read(ساخت spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	صحیح rc;

	spacemit_i2c_آغاز_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_مکمل(adap);
	اگر (rc)
		واپس rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_انحراف);
	واپس 0;
}

ساکن صحیح spacemit_i2c_adapter_write(ساخت i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *بفر, صحیح len)
{
	ساخت spacemit_i2c_adapter *adap =
		container_کا(ia, ساخت spacemit_i2c_adapter, adapter);
	صحیح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_آغاز);
	اگر (rc)
		واپس rc;

	/* send register + data bytes */
	برائے (صحیح i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : بفر[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		اگر (rc)
			واپس rc;
	}

	واپس 0;
}

ساکن صحیح spacemit_i2c_adapter_read(ساخت i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *بفر, صحیح len)
{
	ساخت spacemit_i2c_adapter *adap =
		container_کا(ia, ساخت spacemit_i2c_adapter, adapter);
	صحیح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_آغاز);
	اگر (rc)
		واپس rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	اگر (rc)
		واپس rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_آغاز);
	اگر (rc)
		واپس rc;

	/* read data bytes */
	برائے (صحیح i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &بفر[i], ctrl);
		اگر (rc)
			واپس rc;
	}

	واپس 0;
}

ساکن صحیح spacemit_i2c_init(ثابت خالی *fdt, صحیح nodeoff,
			     ثابت ساخت fdt_مطابقت *مطابقت)
{
	ساخت spacemit_i2c_adapter *adapter;
	uint64_t بنیاد_2;
	صحیح rc;

	adapter = sbi_zalloc(حجم(*adapter));
	اگر (!adapter)
		واپس SBI_ENOMEM;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &بنیاد_2, تہی);
	اگر (rc) {
		sbi_free(adapter);
		واپس rc;
	}

	adapter->بنیاد_2 = بنیاد_2;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = spacemit_i2c_adapter_write;
	adapter->adapter.اقرأ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_جوڑو(&adapter->adapter);
	اگر (rc) {
		sbi_free(adapter);
		واپس rc;
	}

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت spacemit_i2c_مطابقت[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ثابت ساخت fdt_driver fdt_i2c_adapter_spacemit = {
	.مطابقت_جدول = spacemit_i2c_مطابقت,
	.init = spacemit_i2c_init,
};
