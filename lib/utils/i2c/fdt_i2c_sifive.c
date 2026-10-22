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

#تعریف_کرو SIFIVE_I2C_PRELO	0x00
#تعریف_کرو SIFIVE_I2C_PREHI	0x04
#تعریف_کرو SIFIVE_I2C_CTR		0x08
#تعریف_کرو SIFIVE_I2C_TXR		0x00c
#تعریف_کرو SIFIVE_I2C_RXR		SIFIVE_I2C_TXR
#تعریف_کرو SIFIVE_I2C_CR		0x010
#تعریف_کرو SIFIVE_I2C_SR		SIFIVE_I2C_CR

#تعریف_کرو SIFIVE_I2C_CTR_IEN	(1 << 6)
#تعریف_کرو SIFIVE_I2C_CTR_EN	(1 << 7)

#تعریف_کرو SIFIVE_I2C_CMD_IACK	(1 << 0)
#تعریف_کرو SIFIVE_I2C_CMD_ACK	(1 << 3)
#تعریف_کرو SIFIVE_I2C_CMD_WR	(1 << 4)
#تعریف_کرو SIFIVE_I2C_CMD_RD	(1 << 5)
#تعریف_کرو SIFIVE_I2C_CMD_STO	(1 << 6)
#تعریف_کرو SIFIVE_I2C_CMD_STA	(1 << 7)

#تعریف_کرو SIFIVE_I2_C_حالت_IF	(1 << 0)
#تعریف_کرو SIFIVE_I2_C_حالت_TIP	(1 << 1)
#تعریف_کرو SIFIVE_I2_C_حالت_AL	(1 << 5)
#تعریف_کرو SIFIVE_I2_C_حالت_BUSY	(1 << 6)
#تعریف_کرو SIFIVE_I2_C_حالت_RXACK	(1 << 7)

#تعریف_کرو SIFIVE_I2_C_WRITE_ثنائی_ہندسہ	(0 << 0)
#تعریف_کرو SIFIVE_I2_C_READ_ثنائی_ہندسہ	(1 << 0)

ساخت sifive_i2c_adapter {
	غیرعلامتی طویل addr;
	ساخت i2c_adapter adapter;
};

ساکن اندرونی خالی sifive_i2c_setreg(ساخت sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t قدر)
{
	writel(قدر, (متلون حرف *)adap->addr + reg);
}

ساکن اندرونی uint8_t sifive_i2c_getreg(ساخت sifive_i2c_adapter *adap,
					uint8_t reg)
{
	واپس readl((متلون حرف *)adap->addr + reg);
}

ساکن صحیح sifive_i2c_adapter_rxack(ساخت sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	اگر (val & SIFIVE_I2_C_حالت_RXACK)
		واپس SBI_EIO;

	واپس 0;
}

ساکن صحیح sifive_i2c_adapter_poll(ساخت sifive_i2c_adapter *adap,
				   uint32_t نقاب)
{
	غیرعلامتی صحیح timeout = 1; // [msec]
	صحیح گنتی = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	کرو {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		اگر (!(val & نقاب))
			واپس 0;

		sbi_timer_udelay(1);
		گنتی += 1;
		اگر (گنتی == (timeout * 1000))
			واپس SBI_ETIMEDOUT;
	} جب (1);
}

#تعریف_کرو sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_حالت_TIP)
#تعریف_کرو sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_حالت_BUSY)

ساکن صحیح sifive_i2c_adapter_آغاز(ساخت sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t ثنائی_ہندسہ_2)
{
	uint8_t val = (addr << 1) | ثنائی_ہندسہ_2;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	واپس sifive_i2c_adapter_poll_tip(adap);
}

ساکن صحیح sifive_i2c_adapter_write(ساخت i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *بفر, صحیح len)
{
	ساخت sifive_i2c_adapter *adap =
		container_کا(ia, ساخت sifive_i2c_adapter, adapter);
	صحیح rc = sifive_i2c_adapter_آغاز(adap, addr, SIFIVE_I2_C_WRITE_ثنائی_ہندسہ);

	اگر (rc)
		واپس rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		واپس rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	اگر (rc)
		واپس rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		واپس rc;

	/* set value */
	جب (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *بفر);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		اگر (rc)
			واپس rc;

		rc = sifive_i2c_adapter_rxack(adap);
		اگر (rc)
			واپس rc;

		بفر++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	اگر (rc)
		واپس rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	واپس 0;
}

ساکن صحیح sifive_i2c_adapter_read(ساخت i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *بفر, صحیح len)
{
	ساخت sifive_i2c_adapter *adap =
		container_کا(ia, ساخت sifive_i2c_adapter, adapter);
	صحیح rc;

	rc = sifive_i2c_adapter_آغاز(adap, addr, SIFIVE_I2_C_WRITE_ثنائی_ہندسہ);
	اگر (rc)
		واپس rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		واپس rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	اگر (rc)
		واپس rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		واپس rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_آغاز(adap, addr, SIFIVE_I2_C_READ_ثنائی_ہندسہ);
	اگر (rc)
		واپس rc;

	rc = sifive_i2c_adapter_rxack(adap);
	اگر (rc)
		واپس rc;

	جب (len) {
		اگر (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		ورنہ
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		اگر (rc)
			واپس rc;

		*بفر = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		بفر++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	اگر (rc)
		واپس rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	واپس 0;
}

ساکن صحیح sifive_i2c_init(ثابت خالی *fdt, صحیح nodeoff,
			   ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(حجم(*adapter));
	اگر (!adapter)
		واپس SBI_ENOMEM;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, تہی);
	اگر (rc) {
		sbi_free(adapter);
		واپس rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.اكتب = sifive_i2c_adapter_write;
	adapter->adapter.اقرأ = sifive_i2c_adapter_read;
	rc = i2c_adapter_جوڑو(&adapter->adapter);
	اگر (rc) {
		sbi_free(adapter);
		واپس rc;
	}

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت sifive_i2c_مطابقت[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ثابت ساخت fdt_driver fdt_i2c_adapter_sifive = {
	.مطابقت_جدول = sifive_i2c_مطابقت,
	.init = sifive_i2c_init,
};
