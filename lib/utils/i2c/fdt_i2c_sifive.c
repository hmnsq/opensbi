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

#define SIFIVE_I2C_PRELO	0x00
#define SIFIVE_I2C_PREHI	0x04
#define SIFIVE_I2C_CTR		0x08
#define SIFIVE_I2C_TXR		0x00c
#define SIFIVE_I2C_RXR		SIFIVE_I2C_TXR
#define SIFIVE_I2C_CR		0x010
#define SIFIVE_I2C_SR		SIFIVE_I2C_CR

#define SIFIVE_I2C_CTR_IEN	(1 << 6)
#define SIFIVE_I2C_CTR_EN	(1 << 7)

#define SIFIVE_I2C_CMD_IACK	(1 << 0)
#define SIFIVE_I2C_CMD_ACK	(1 << 3)
#define SIFIVE_I2C_CMD_WR	(1 << 4)
#define SIFIVE_I2C_CMD_RD	(1 << 5)
#define SIFIVE_I2C_CMD_STO	(1 << 6)
#define SIFIVE_I2C_CMD_STA	(1 << 7)

#define SIFIVE_I2C_STATUS_IF	(1 << 0)
#define SIFIVE_I2C_STATUS_TIP	(1 << 1)
#define SIFIVE_I2C_STATUS_AL	(1 << 5)
#define SIFIVE_I2C_STATUS_BUSY	(1 << 6)
#define SIFIVE_I2C_STATUS_RXACK	(1 << 7)

#define SIFIVE_I2C_WRITE_BIT	(0 << 0)
#define SIFIVE_I2C_READ_BIT	(1 << 0)

بنية sifive_i2c_adapter {
	unsigned long addr;
	بنية i2c_adapter adapter;
};

ساكن مضمن فراغ sifive_i2c_setreg(بنية sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t قيمة)
{
	writel(قيمة, (متطاير حرف *)adap->addr + reg);
}

ساكن مضمن uint8_t sifive_i2c_getreg(بنية sifive_i2c_adapter *adap,
					uint8_t reg)
{
	إرجاع readl((متطاير حرف *)adap->addr + reg);
}

ساكن صحيح sifive_i2c_adapter_rxack(بنية sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	إذا (val & SIFIVE_I2C_STATUS_RXACK)
		إرجاع SBI_EIO;

	إرجاع 0;
}

ساكن صحيح sifive_i2c_adapter_poll(بنية sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned صحيح timeout = 1; // [msec]
	صحيح تعداد_2 = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	افعل {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		إذا (!(val & mask))
			إرجاع 0;

		sbi_timer_udelay(1);
		تعداد_2 += 1;
		إذا (تعداد_2 == (timeout * 1000))
			إرجاع SBI_ETIMEDOUT;
	} بينما (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

ساكن صحيح sifive_i2c_adapter_start(بنية sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	إرجاع sifive_i2c_adapter_poll_tip(adap);
}

ساكن صحيح sifive_i2c_adapter_write(بنية i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, صحيح len)
{
	بنية sifive_i2c_adapter *adap =
		container_of(ia, بنية sifive_i2c_adapter, adapter);
	صحيح rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	إذا (rc)
		إرجاع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		إرجاع rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	إذا (rc)
		إرجاع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		إرجاع rc;

	/* set value */
	بينما (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		إذا (rc)
			إرجاع rc;

		rc = sifive_i2c_adapter_rxack(adap);
		إذا (rc)
			إرجاع rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	إذا (rc)
		إرجاع rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	إرجاع 0;
}

ساكن صحيح sifive_i2c_adapter_read(بنية i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, صحيح len)
{
	بنية sifive_i2c_adapter *adap =
		container_of(ia, بنية sifive_i2c_adapter, adapter);
	صحيح rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	إذا (rc)
		إرجاع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		إرجاع rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	إذا (rc)
		إرجاع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		إرجاع rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	إذا (rc)
		إرجاع rc;

	rc = sifive_i2c_adapter_rxack(adap);
	إذا (rc)
		إرجاع rc;

	بينما (len) {
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
			إرجاع rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	إذا (rc)
		إرجاع rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	إرجاع 0;
}

ساكن صحيح sifive_i2c_init(ثابت فراغ *fdt, صحيح nodeoff,
			   ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(حجم(*adapter));
	إذا (!adapter)
		إرجاع SBI_ENOMEM;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc) {
		sbi_free(adapter);
		إرجاع rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = sifive_i2c_adapter_write;
	adapter->adapter.اقرأ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	إذا (rc) {
		sbi_free(adapter);
		إرجاع rc;
	}

	إرجاع 0;
}

ساكن ثابت بنية fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ثابت بنية fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
