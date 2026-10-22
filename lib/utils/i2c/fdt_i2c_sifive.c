/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/i2c/fdt_i2c.h>

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

ساختار sifive_i2c_adapter {
	unsigned long addr;
	ساختار i2c_adapter adapter;
};

ایستا درونخطی تهی sifive_i2c_setreg(ساختار sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t مقدار)
{
	writel(مقدار, (فرار نویسه *)adap->addr + reg);
}

ایستا درونخطی uint8_t sifive_i2c_getreg(ساختار sifive_i2c_adapter *adap,
					uint8_t reg)
{
	بازگشت readl((فرار نویسه *)adap->addr + reg);
}

ایستا صحیح sifive_i2c_adapter_rxack(ساختار sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	اگر (val & SIFIVE_I2C_STATUS_RXACK)
		بازگشت SBI_EIO;

	بازگشت 0;
}

ایستا صحیح sifive_i2c_adapter_poll(ساختار sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned صحیح timeout = 1; // [msec]
	صحیح شمار = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	انجامبده {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		اگر (!(val & mask))
			بازگشت 0;

		sbi_timer_udelay(1);
		شمار += 1;
		اگر (شمار == (timeout * 1000))
			بازگشت SBI_ETIMEDOUT;
	} وقتی (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

ایستا صحیح sifive_i2c_adapter_start(ساختار sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	بازگشت sifive_i2c_adapter_poll_tip(adap);
}

ایستا صحیح sifive_i2c_adapter_write(ساختار i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, صحیح len)
{
	ساختار sifive_i2c_adapter *adap =
		container_of(ia, ساختار sifive_i2c_adapter, adapter);
	صحیح rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	اگر (rc)
		بازگشت rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		بازگشت rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	اگر (rc)
		بازگشت rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		بازگشت rc;

	/* set value */
	وقتی (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		اگر (rc)
			بازگشت rc;

		rc = sifive_i2c_adapter_rxack(adap);
		اگر (rc)
			بازگشت rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	اگر (rc)
		بازگشت rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	بازگشت 0;
}

ایستا صحیح sifive_i2c_adapter_read(ساختار i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, صحیح len)
{
	ساختار sifive_i2c_adapter *adap =
		container_of(ia, ساختار sifive_i2c_adapter, adapter);
	صحیح rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	اگر (rc)
		بازگشت rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		بازگشت rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	اگر (rc)
		بازگشت rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		بازگشت rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	اگر (rc)
		بازگشت rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		بازگشت rc;

	وقتی (len) {
		اگر (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		وگرنه
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		اگر (rc)
			بازگشت rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	اگر (rc)
		بازگشت rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	بازگشت 0;
}

ایستا صحیح sifive_i2c_init(ثابت تهی *fdt, صحیح nodeoff,
			   ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(اندازه(*adapter));
	اگر (!adapter)
		بازگشت SBI_ENOMEM;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &addr, پوچ);
	اگر (rc) {
		sbi_free(adapter);
		بازگشت rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = sifive_i2c_adapter_write;
	adapter->adapter.اقرأ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	اگر (rc) {
		sbi_free(adapter);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ثابت ساختار fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
