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

#وصف_ڪريو SIFIVE_I2C_PRELO	0x00
#وصف_ڪريو SIFIVE_I2C_PREHI	0x04
#وصف_ڪريو SIFIVE_I2C_CTR		0x08
#وصف_ڪريو SIFIVE_I2C_TXR		0x00c
#وصف_ڪريو SIFIVE_I2C_RXR		SIFIVE_I2C_TXR
#وصف_ڪريو SIFIVE_I2C_CR		0x010
#وصف_ڪريو SIFIVE_I2C_SR		SIFIVE_I2C_CR

#وصف_ڪريو SIFIVE_I2C_CTR_IEN	(1 << 6)
#وصف_ڪريو SIFIVE_I2C_CTR_EN	(1 << 7)

#وصف_ڪريو SIFIVE_I2C_CMD_IACK	(1 << 0)
#وصف_ڪريو SIFIVE_I2C_CMD_ACK	(1 << 3)
#وصف_ڪريو SIFIVE_I2C_CMD_WR	(1 << 4)
#وصف_ڪريو SIFIVE_I2C_CMD_RD	(1 << 5)
#وصف_ڪريو SIFIVE_I2C_CMD_STO	(1 << 6)
#وصف_ڪريو SIFIVE_I2C_CMD_STA	(1 << 7)

#وصف_ڪريو SIFIVE_I2_C_حالت_IF	(1 << 0)
#وصف_ڪريو SIFIVE_I2_C_حالت_TIP	(1 << 1)
#وصف_ڪريو SIFIVE_I2_C_حالت_AL	(1 << 5)
#وصف_ڪريو SIFIVE_I2_C_حالت_BUSY	(1 << 6)
#وصف_ڪريو SIFIVE_I2_C_حالت_RXACK	(1 << 7)

#وصف_ڪريو SIFIVE_I2_C_WRITE_ٻٽو_انگ	(0 << 0)
#وصف_ڪريو SIFIVE_I2_C_READ_ٻٽو_انگ	(1 << 0)

بنية sifive_i2c_adapter {
	بنانشان ڊگهو addr;
	بنية i2c_adapter adapter;
};

ساكن مضمن فراغ sifive_i2c_setreg(بنية sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t قدر)
{
	writel(قدر, (متطاير حرف *)adap->addr + reg);
}

ساكن مضمن uint8_t sifive_i2c_getreg(بنية sifive_i2c_adapter *adap,
					uint8_t reg)
{
	أرجع readl((متطاير حرف *)adap->addr + reg);
}

ساكن صحيح sifive_i2c_adapter_rxack(بنية sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	إذا (val & SIFIVE_I2_C_حالت_RXACK)
		أرجع SBI_EIO;

	أرجع 0;
}

ساكن صحيح sifive_i2c_adapter_poll(بنية sifive_i2c_adapter *adap,
				   uint32_t نقاب)
{
	بنانشان صحيح timeout = 1; // [msec]
	صحيح ڳڻپ = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	افعل {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		إذا (!(val & نقاب))
			أرجع 0;

		sbi_timer_udelay(1);
		ڳڻپ += 1;
		إذا (ڳڻپ == (timeout * 1000))
			أرجع SBI_ETIMEDOUT;
	} طالما (1);
}

#وصف_ڪريو sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_حالت_TIP)
#وصف_ڪريو sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_حالت_BUSY)

ساكن صحيح sifive_i2c_adapter_شروعات(بنية sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t ٻٽو_انگ_2)
{
	uint8_t val = (addr << 1) | ٻٽو_انگ_2;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	أرجع sifive_i2c_adapter_poll_tip(adap);
}

ساكن صحيح sifive_i2c_adapter_write(بنية i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *بفر, صحيح len)
{
	بنية sifive_i2c_adapter *adap =
		container_جو(ia, بنية sifive_i2c_adapter, adapter);
	صحيح rc = sifive_i2c_adapter_شروعات(adap, addr, SIFIVE_I2_C_WRITE_ٻٽو_انگ);

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
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *بفر);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		إذا (rc)
			أرجع rc;

		rc = sifive_i2c_adapter_rxack(adap);
		إذا (rc)
			أرجع rc;

		بفر++;
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
				   uint8_t reg, uint8_t *بفر, صحيح len)
{
	بنية sifive_i2c_adapter *adap =
		container_جو(ia, بنية sifive_i2c_adapter, adapter);
	صحيح rc;

	rc = sifive_i2c_adapter_شروعات(adap, addr, SIFIVE_I2_C_WRITE_ٻٽو_انگ);
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
	rc = sifive_i2c_adapter_شروعات(adap, addr, SIFIVE_I2_C_READ_ٻٽو_انگ);
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

		*بفر = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		بفر++;
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
			   ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنية sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(حجم(*adapter));
	إذا (!adapter)
		أرجع SBI_ENOMEM;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = sifive_i2c_adapter_write;
	adapter->adapter.اقرأ = sifive_i2c_adapter_read;
	rc = i2c_adapter_جوڙيو(&adapter->adapter);
	إذا (rc) {
		sbi_free(adapter);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ sifive_i2c_ميلاپ[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ثابت بنية fdt_driver fdt_i2c_adapter_sifive = {
	.ميلاپ_جدول = sifive_i2c_ميلاپ,
	.init = sifive_i2c_init,
};
