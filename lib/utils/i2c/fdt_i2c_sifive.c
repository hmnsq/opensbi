/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/i2c/fdt_i2c.h>

#تعریف_کن SIFIVE_I2C_PRELO	0x00
#تعریف_کن SIFIVE_I2C_PREHI	0x04
#تعریف_کن SIFIVE_I2C_CTR		0x08
#تعریف_کن SIFIVE_I2C_TXR		0x00c
#تعریف_کن SIFIVE_I2C_RXR		SIFIVE_I2C_TXR
#تعریف_کن SIFIVE_I2C_CR		0x010
#تعریف_کن SIFIVE_I2C_SR		SIFIVE_I2C_CR

#تعریف_کن SIFIVE_I2C_CTR_IEN	(1 << 6)
#تعریف_کن SIFIVE_I2C_CTR_EN	(1 << 7)

#تعریف_کن SIFIVE_I2C_CMD_IACK	(1 << 0)
#تعریف_کن SIFIVE_I2C_CMD_ACK	(1 << 3)
#تعریف_کن SIFIVE_I2C_CMD_WR	(1 << 4)
#تعریف_کن SIFIVE_I2C_CMD_RD	(1 << 5)
#تعریف_کن SIFIVE_I2C_CMD_STO	(1 << 6)
#تعریف_کن SIFIVE_I2C_CMD_STA	(1 << 7)

#تعریف_کن SIFIVE_I2_C_وضعیت_IF	(1 << 0)
#تعریف_کن SIFIVE_I2_C_وضعیت_TIP	(1 << 1)
#تعریف_کن SIFIVE_I2_C_وضعیت_AL	(1 << 5)
#تعریف_کن SIFIVE_I2_C_وضعیت_BUSY	(1 << 6)
#تعریف_کن SIFIVE_I2_C_وضعیت_RXACK	(1 << 7)

#تعریف_کن SIFIVE_I2_C_WRITE_بیت	(0 << 0)
#تعریف_کن SIFIVE_I2_C_READ_بیت	(1 << 0)

بنية sifive_i2c_adapter {
	بیعلامت دراز addr;
	بنية i2c_adapter adapter;
};

ساكن مضمن فراغ sifive_i2c_setreg(بنية sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t مقدار)
{
	writel(مقدار, (متطاير حرف *)adap->addr + reg);
}

ساكن مضمن uint8_t sifive_i2c_getreg(بنية sifive_i2c_adapter *adap,
					uint8_t reg)
{
	أرجع readl((متطاير حرف *)adap->addr + reg);
}

ساكن صحيح sifive_i2c_adapter_rxack(بنية sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	إذا (val & SIFIVE_I2_C_وضعیت_RXACK)
		أرجع SBI_EIO;

	أرجع 0;
}

ساكن صحيح sifive_i2c_adapter_poll(بنية sifive_i2c_adapter *adap,
				   uint32_t ماسک)
{
	بیعلامت صحيح timeout = 1; // [msec]
	صحيح شمار = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	افعل {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		إذا (!(val & ماسک))
			أرجع 0;

		sbi_timer_udelay(1);
		شمار += 1;
		إذا (شمار == (timeout * 1000))
			أرجع SBI_ETIMEDOUT;
	} طالما (1);
}

#تعریف_کن sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_وضعیت_TIP)
#تعریف_کن sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_وضعیت_BUSY)

ساكن صحيح sifive_i2c_adapter_شروع(بنية sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t بیت_2)
{
	uint8_t val = (addr << 1) | بیت_2;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	أرجع sifive_i2c_adapter_poll_tip(adap);
}

ساكن صحيح sifive_i2c_adapter_write(بنية i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *میانگیر, صحيح len)
{
	بنية sifive_i2c_adapter *adap =
		container_از(ia, بنية sifive_i2c_adapter, adapter);
	صحيح rc = sifive_i2c_adapter_شروع(adap, addr, SIFIVE_I2_C_WRITE_بیت);

	إذا (rc)
		أرجع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		أرجع rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	إذا (rc)
		أرجع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		أرجع rc;

	/* set value */
	طالما (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *میانگیر);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		إذا (rc)
			أرجع rc;

		rc = sifive_i2c_adapter_rxack(adap);
		إذا (rc)
			أرجع rc;

		میانگیر++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	إذا (rc)
		أرجع rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	أرجع 0;
}

ساكن صحيح sifive_i2c_adapter_read(بنية i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *میانگیر, صحيح len)
{
	بنية sifive_i2c_adapter *adap =
		container_از(ia, بنية sifive_i2c_adapter, adapter);
	صحيح rc;

	rc = sifive_i2c_adapter_شروع(adap, addr, SIFIVE_I2_C_WRITE_بیت);
	إذا (rc)
		أرجع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		أرجع rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	إذا (rc)
		أرجع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		أرجع rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_شروع(adap, addr, SIFIVE_I2_C_READ_بیت);
	إذا (rc)
		أرجع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		أرجع rc;

	طالما (len) {
		إذا (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		وإلا
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		إذا (rc)
			أرجع rc;

		*میانگیر = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		میانگیر++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	إذا (rc)
		أرجع rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	أرجع 0;
}

ساكن صحيح sifive_i2c_init(ثابت فراغ *fdt, صحيح nodeoff,
			   ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بنية sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(حجم(*adapter));
	إذا (!adapter)
		أرجع SBI_ENOMEM;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = sifive_i2c_adapter_write;
	adapter->adapter.اقرأ = sifive_i2c_adapter_read;
	rc = i2c_adapter_جمع_کن(&adapter->adapter);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_تطابق sifive_i2c_تطابق[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ثابت بنية fdt_driver fdt_i2c_adapter_sifive = {
	.تطابق_جدول = sifive_i2c_تطابق,
	.init = sifive_i2c_init,
};
