/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/i2c/fdt_i2c.h>

#నిర్వచించు SIFIVE_I2C_PRELO	0x00
#నిర్వచించు SIFIVE_I2C_PREHI	0x04
#నిర్వచించు SIFIVE_I2C_CTR		0x08
#నిర్వచించు SIFIVE_I2C_TXR		0x00c
#నిర్వచించు SIFIVE_I2C_RXR		SIFIVE_I2C_TXR
#నిర్వచించు SIFIVE_I2C_CR		0x010
#నిర్వచించు SIFIVE_I2C_SR		SIFIVE_I2C_CR

#నిర్వచించు SIFIVE_I2C_CTR_IEN	(1 << 6)
#నిర్వచించు SIFIVE_I2C_CTR_EN	(1 << 7)

#నిర్వచించు SIFIVE_I2C_CMD_IACK	(1 << 0)
#నిర్వచించు SIFIVE_I2C_CMD_ACK	(1 << 3)
#నిర్వచించు SIFIVE_I2C_CMD_WR	(1 << 4)
#నిర్వచించు SIFIVE_I2C_CMD_RD	(1 << 5)
#నిర్వచించు SIFIVE_I2C_CMD_STO	(1 << 6)
#నిర్వచించు SIFIVE_I2C_CMD_STA	(1 << 7)

#నిర్వచించు SIFIVE_I2_C_స్థితి_IF	(1 << 0)
#నిర్వచించు SIFIVE_I2_C_స్థితి_TIP	(1 << 1)
#నిర్వచించు SIFIVE_I2_C_స్థితి_AL	(1 << 5)
#నిర్వచించు SIFIVE_I2_C_స్థితి_BUSY	(1 << 6)
#నిర్వచించు SIFIVE_I2_C_స్థితి_RXACK	(1 << 7)

#నిర్వచించు SIFIVE_I2_C_WRITE_బిట్టు	(0 << 0)
#నిర్వచించు SIFIVE_I2_C_READ_బిట్టు	(1 << 0)

నిర్మాణం sifive_i2c_adapter {
	గుర్తులేని దీర్ఘ addr;
	నిర్మాణం i2c_adapter adapter;
};

స్థిర అంతఃవరుస శూన్య sifive_i2c_setreg(నిర్మాణం sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t విలువ)
{
	writel(విలువ, (అస్థిర అక్షరం *)adap->addr + reg);
}

స్థిర అంతఃవరుస uint8_t sifive_i2c_getreg(నిర్మాణం sifive_i2c_adapter *adap,
					uint8_t reg)
{
	తిరిగి readl((అస్థిర అక్షరం *)adap->addr + reg);
}

స్థిర పూర్ణాంకం sifive_i2c_adapter_rxack(నిర్మాణం sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	ఒకవేళ (val & SIFIVE_I2_C_స్థితి_RXACK)
		తిరిగి SBI_EIO;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sifive_i2c_adapter_poll(నిర్మాణం sifive_i2c_adapter *adap,
				   uint32_t ముసుగు)
{
	గుర్తులేని పూర్ణాంకం timeout = 1; // [msec]
	పూర్ణాంకం లెక్క = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	చేయి {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		ఒకవేళ (!(val & ముసుగు))
			తిరిగి 0;

		sbi_timer_udelay(1);
		లెక్క += 1;
		ఒకవేళ (లెక్క == (timeout * 1000))
			తిరిగి SBI_ETIMEDOUT;
	} అయితే (1);
}

#నిర్వచించు sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_స్థితి_TIP)
#నిర్వచించు sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_స్థితి_BUSY)

స్థిర పూర్ణాంకం sifive_i2c_adapter_ప్రారంభం(నిర్మాణం sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t బిట్టు_2)
{
	uint8_t val = (addr << 1) | బిట్టు_2;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	తిరిగి sifive_i2c_adapter_poll_tip(adap);
}

స్థిర పూర్ణాంకం sifive_i2c_adapter_write(నిర్మాణం i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *బఫరు, పూర్ణాంకం len)
{
	నిర్మాణం sifive_i2c_adapter *adap =
		container_యొక్క(ia, నిర్మాణం sifive_i2c_adapter, adapter);
	పూర్ణాంకం rc = sifive_i2c_adapter_ప్రారంభం(adap, addr, SIFIVE_I2_C_WRITE_బిట్టు);

	ఒకవేళ (rc)
		తిరిగి rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* set value */
	అయితే (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *బఫరు);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ఒకవేళ (rc)
			తిరిగి rc;

		rc = sifive_i2c_adapter_rxack(adap);
		ఒకవేళ (rc)
			తిరిగి rc;

		బఫరు++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sifive_i2c_adapter_read(నిర్మాణం i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *బఫరు, పూర్ణాంకం len)
{
	నిర్మాణం sifive_i2c_adapter *adap =
		container_యొక్క(ia, నిర్మాణం sifive_i2c_adapter, adapter);
	పూర్ణాంకం rc;

	rc = sifive_i2c_adapter_ప్రారంభం(adap, addr, SIFIVE_I2_C_WRITE_బిట్టు);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_ప్రారంభం(adap, addr, SIFIVE_I2_C_READ_బిట్టు);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	అయితే (len) {
		ఒకవేళ (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		లేకపోతే
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ఒకవేళ (rc)
			తిరిగి rc;

		*బఫరు = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		బఫరు++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sifive_i2c_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			   స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(పరిమాణం(*adapter));
	ఒకవేళ (!adapter)
		తిరిగి SBI_ENOMEM;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, ఖాళీ);
	ఒకవేళ (rc) {
		sbi_free(adapter);
		తిరిగి rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.లిఖ = sifive_i2c_adapter_write;
	adapter->adapter.పఠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_కూడు(&adapter->adapter);
	ఒకవేళ (rc) {
		sbi_free(adapter);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sifive_i2c_సరిపోలిక[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_i2c_adapter_sifive = {
	.సరిపోలిక_పట్టిక = sifive_i2c_సరిపోలిక,
	.init = sifive_i2c_init,
};
