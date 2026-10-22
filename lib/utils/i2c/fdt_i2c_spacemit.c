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
#تعریف_کن ICR_جابه_جایی	0x00		/* I2C control register */
#تعریف_کن IDBR_جابه_جایی	0x0c		/* I2C data buffer register */

/* Control register bits */
#تعریف_کن ICR_شروع	بیت(0)		/*شروع*/
#تعریف_کن ICR_STOP	بیت(1)		/* stop */
#تعریف_کن ICR_ACKNAK	بیت(2)		/* ACK(0) or NAK(1) */
#تعریف_کن ICR_TB		بیت(3)		/* transfer byte */
#تعریف_کن ICR_UR		بیت(10)		/* unit reset */
#تعریف_کن ICR_SCLE	بیت(13)		/* SCL enable  */
#تعریف_کن ICR_IUE		بیت(14)		/* unit enable */

/* Timing */
#تعریف_کن I2C_RESET_US	10
#تعریف_کن I2C_TIMEOUT_US	1000

بنية spacemit_i2c_adapter {
	بیعلامت دراز پایه_2;
	بنية i2c_adapter adapter;
};

ساكن مضمن فراغ spacemit_i2c_مجموعه_reg(بنية spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (فراغ *)adap->پایه_2 + reg);
}

ساكن مضمن uint32_t spacemit_i2c_get_reg(بنية spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	أرجع readl((فراغ *)adap->پایه_2 + reg);
}

ساكن فراغ spacemit_i2c_reset(بنية spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_مجموعه_reg(adap, ICR_جابه_جایی, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_مجموعه_reg(adap, ICR_جابه_جایی, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_مجموعه_reg(adap, ICR_جابه_جایی, ICR_IUE | ICR_SCLE);
}

ساكن صحيح spacemit_i2c_wait_xfer_انجامشده(بنية spacemit_i2c_adapter *adap)
{
	لكل (صحيح i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_جابه_جایی);

		إذا (!(val & ICR_TB))
			أرجع 0;

		sbi_timer_udelay(1);
	};

	أرجع SBI_ETIMEDOUT;
}

ساكن فراغ spacemit_i2c_شروع_xfer(بنية spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ثابت uint32_t ctrl_ماسک = ICR_شروع | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_جابه_جایی);
	val &= ~ctrl_ماسک;
	val |= (ctrl & ctrl_ماسک);
	val |= ICR_TB;

	spacemit_i2c_مجموعه_reg(adap, ICR_جابه_جایی, val);
}

ساكن صحيح spacemit_i2c_xfer_write(بنية spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_مجموعه_reg(adap, IDBR_جابه_جایی, byte);
	spacemit_i2c_شروع_xfer(adap, ctrl);

	أرجع spacemit_i2c_wait_xfer_انجامشده(adap);
}

ساكن صحيح spacemit_i2c_xfer_read(بنية spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	صحيح rc;

	spacemit_i2c_شروع_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_انجامشده(adap);
	إذا (rc)
		أرجع rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_جابه_جایی);
	أرجع 0;
}

ساكن صحيح spacemit_i2c_adapter_write(بنية i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *میانگیر, صحيح len)
{
	بنية spacemit_i2c_adapter *adap =
		container_از(ia, بنية spacemit_i2c_adapter, adapter);
	صحيح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_شروع);
	إذا (rc)
		أرجع rc;

	/* send register + data bytes */
	لكل (صحيح i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : میانگیر[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح spacemit_i2c_adapter_read(بنية i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *میانگیر, صحيح len)
{
	بنية spacemit_i2c_adapter *adap =
		container_از(ia, بنية spacemit_i2c_adapter, adapter);
	صحيح rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_شروع);
	إذا (rc)
		أرجع rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	إذا (rc)
		أرجع rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_شروع);
	إذا (rc)
		أرجع rc;

	/* read data bytes */
	لكل (صحيح i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &میانگیر[i], ctrl);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح spacemit_i2c_init(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_تطابق *تطابق)
{
	بنية spacemit_i2c_adapter *adapter;
	uint64_t پایه_2;
	صحيح rc;

	adapter = sbi_zalloc(حجم(*adapter));
	إذا (!adapter)
		أرجع SBI_ENOMEM;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &پایه_2, عدم);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	adapter->پایه_2 = پایه_2;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = spacemit_i2c_adapter_write;
	adapter->adapter.اقرأ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_جمع_کن(&adapter->adapter);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_تطابق spacemit_i2c_تطابق[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ثابت بنية fdt_driver fdt_i2c_adapter_spacemit = {
	.تطابق_جدول = spacemit_i2c_تطابق,
	.init = spacemit_i2c_init,
};
