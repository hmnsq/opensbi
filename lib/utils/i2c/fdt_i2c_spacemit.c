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
#وصف_ڪريو ICR_وٿي	0x00		/* I2C control register */
#وصف_ڪريو IDBR_وٿي	0x0c		/* I2C data buffer register */

/* Control register bits */
#وصف_ڪريو ICR_شروعات	ٻٽو_انگ(0)		/*شروعات*/
#وصف_ڪريو ICR_STOP	ٻٽو_انگ(1)		/* stop */
#وصف_ڪريو ICR_ACKNAK	ٻٽو_انگ(2)		/* ACK(0) or NAK(1) */
#وصف_ڪريو ICR_TB		ٻٽو_انگ(3)		/* transfer byte */
#وصف_ڪريو ICR_UR		ٻٽو_انگ(10)		/* unit reset */
#وصف_ڪريو ICR_SCLE	ٻٽو_انگ(13)		/* SCL enable  */
#وصف_ڪريو ICR_IUE		ٻٽو_انگ(14)		/* unit enable */

/* Timing */
#وصف_ڪريو I2C_RESET_US	10
#وصف_ڪريو I2C_TIMEOUT_US	1000

بنية spacemit_i2c_adapter {
	بنانشان ڊگهو بنياد_2;
	بنية i2c_adapter adapter;
};

ساكن مضمن فراغ spacemit_i2c_سيٽ_reg(بنية spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (فراغ *)adap->بنياد_2 + reg);
}

ساكن مضمن uint32_t spacemit_i2c_get_reg(بنية spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	أرجع readl((فراغ *)adap->بنياد_2 + reg);
}

ساكن فراغ spacemit_i2c_reset(بنية spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_سيٽ_reg(adap, ICR_وٿي, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_سيٽ_reg(adap, ICR_وٿي, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_سيٽ_reg(adap, ICR_وٿي, ICR_IUE | ICR_SCLE);
}

ساكن صحيح spacemit_i2c_wait_xfer_پورو(بنية spacemit_i2c_adapter *adap)
{
	لكل (صحيح i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_وٿي);

		إذا (!(val & ICR_TB))
			أرجع 0;

		sbi_timer_udelay(1);
	};

	أرجع SBI_ETIMEDOUT;
}

ساكن فراغ spacemit_i2c_شروعات_xfer(بنية spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ثابت uint32_t ctrl_نقاب = ICR_شروعات | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_وٿي);
	val &= ~ctrl_نقاب;
	val |= (ctrl & ctrl_نقاب);
	val |= ICR_TB;

	spacemit_i2c_سيٽ_reg(adap, ICR_وٿي, val);
}

ساكن صحيح spacemit_i2c_xfer_write(بنية spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_سيٽ_reg(adap, IDBR_وٿي, byte);
	spacemit_i2c_شروعات_xfer(adap, ctrl);

	أرجع spacemit_i2c_wait_xfer_پورو(adap);
}

ساكن صحيح spacemit_i2c_xfer_read(بنية spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	صحيح rc;

	spacemit_i2c_شروعات_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_پورو(adap);
	إذا (rc)
		أرجع rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_وٿي);
	أرجع 0;
}

ساكن صحيح spacemit_i2c_adapter_write(بنية i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *بفر, صحيح len)
{
	بنية spacemit_i2c_adapter *adap =
		container_جو(ia, بنية spacemit_i2c_adapter, adapter);
	صحيح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_شروعات);
	إذا (rc)
		أرجع rc;

	/* send register + data bytes */
	لكل (صحيح i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : بفر[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح spacemit_i2c_adapter_read(بنية i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *بفر, صحيح len)
{
	بنية spacemit_i2c_adapter *adap =
		container_جو(ia, بنية spacemit_i2c_adapter, adapter);
	صحيح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_شروعات);
	إذا (rc)
		أرجع rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	إذا (rc)
		أرجع rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_شروعات);
	إذا (rc)
		أرجع rc;

	/* read data bytes */
	لكل (صحيح i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &بفر[i], ctrl);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح spacemit_i2c_init(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_ميلاپ *ميلاپ)
{
	بنية spacemit_i2c_adapter *adapter;
	uint64_t بنياد_2;
	صحيح rc;

	adapter = sbi_zalloc(حجم(*adapter));
	إذا (!adapter)
		أرجع SBI_ENOMEM;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, 0, &بنياد_2, عدم);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	adapter->بنياد_2 = بنياد_2;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = spacemit_i2c_adapter_write;
	adapter->adapter.اقرأ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_جوڙيو(&adapter->adapter);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ spacemit_i2c_ميلاپ[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ثابت بنية fdt_driver fdt_i2c_adapter_spacemit = {
	.ميلاپ_جدول = spacemit_i2c_ميلاپ,
	.init = spacemit_i2c_init,
};
