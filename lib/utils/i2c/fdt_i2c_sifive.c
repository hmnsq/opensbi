/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/i2c/fdt_i2c.h>

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

संरचना sifive_i2c_adapter {
	अचिन्हित दीर्घ addr;
	संरचना i2c_adapter adapter;
};

स्थिर अन्तःस्थ शून्य sifive_i2c_setreg(संरचना sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t मूल्य)
{
	writel(मूल्य, (चञ्चल वर्ण *)adap->addr + reg);
}

स्थिर अन्तःस्थ uint8_t sifive_i2c_getreg(संरचना sifive_i2c_adapter *adap,
					uint8_t reg)
{
	प्रत्यागम readl((चञ्चल वर्ण *)adap->addr + reg);
}

स्थिर पूर्णाङ्क sifive_i2c_adapter_rxack(संरचना sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	यदि (val & SIFIVE_I2C_STATUS_RXACK)
		प्रत्यागम SBI_EIO;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sifive_i2c_adapter_poll(संरचना sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	अचिन्हित पूर्णाङ्क timeout = 1; // [msec]
	पूर्णाङ्क गिनती = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	कुरु {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		यदि (!(val & mask))
			प्रत्यागम 0;

		sbi_timer_udelay(1);
		गिनती += 1;
		यदि (गिनती == (timeout * 1000))
			प्रत्यागम SBI_ETIMEDOUT;
	} यावत् (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

स्थिर पूर्णाङ्क sifive_i2c_adapter_start(संरचना sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	प्रत्यागम sifive_i2c_adapter_poll_tip(adap);
}

स्थिर पूर्णाङ्क sifive_i2c_adapter_write(संरचना i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, पूर्णाङ्क len)
{
	संरचना sifive_i2c_adapter *adap =
		container_of(ia, संरचना sifive_i2c_adapter, adapter);
	पूर्णाङ्क rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	यदि (rc)
		प्रत्यागम rc;

	rc = sifive_i2c_adapter_rxack(adap);
	यदि (rc)
		प्रत्यागम rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	यदि (rc)
		प्रत्यागम rc;

	rc = sifive_i2c_adapter_rxack(adap);
	यदि (rc)
		प्रत्यागम rc;

	/* set value */
	यावत् (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		यदि (rc)
			प्रत्यागम rc;

		rc = sifive_i2c_adapter_rxack(adap);
		यदि (rc)
			प्रत्यागम rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	यदि (rc)
		प्रत्यागम rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sifive_i2c_adapter_read(संरचना i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, पूर्णाङ्क len)
{
	संरचना sifive_i2c_adapter *adap =
		container_of(ia, संरचना sifive_i2c_adapter, adapter);
	पूर्णाङ्क rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	यदि (rc)
		प्रत्यागम rc;

	rc = sifive_i2c_adapter_rxack(adap);
	यदि (rc)
		प्रत्यागम rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	यदि (rc)
		प्रत्यागम rc;

	rc = sifive_i2c_adapter_rxack(adap);
	यदि (rc)
		प्रत्यागम rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	यदि (rc)
		प्रत्यागम rc;

	rc = sifive_i2c_adapter_rxack(adap);
	यदि (rc)
		प्रत्यागम rc;

	यावत् (len) {
		यदि (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		अन्यथा
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		यदि (rc)
			प्रत्यागम rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	यदि (rc)
		प्रत्यागम rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क sifive_i2c_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			   ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(परिमाण(*adapter));
	यदि (!adapter)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_get_गाँठ_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc) {
		sbi_free(adapter);
		प्रत्यागम rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.लिख = sifive_i2c_adapter_write;
	adapter->adapter.पठ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	यदि (rc) {
		sbi_free(adapter);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
