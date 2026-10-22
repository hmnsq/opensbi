/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/i2c/fdt_i2c.h>

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

கட்டமைப்பு sifive_i2c_adapter {
	unsigned long addr;
	கட்டமைப்பு i2c_adapter adapter;
};

நிலையான உள்வரி வெறுமை sifive_i2c_setreg(கட்டமைப்பு sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t மதிப்பு)
{
	writel(மதிப்பு, (நிலையற்ற எழுத்து *)adap->addr + reg);
}

நிலையான உள்வரி uint8_t sifive_i2c_getreg(கட்டமைப்பு sifive_i2c_adapter *adap,
					uint8_t reg)
{
	திரும்பு readl((நிலையற்ற எழுத்து *)adap->addr + reg);
}

நிலையான முழு sifive_i2c_adapter_rxack(கட்டமைப்பு sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	எனில் (val & SIFIVE_I2C_STATUS_RXACK)
		திரும்பு SBI_EIO;

	திரும்பு 0;
}

நிலையான முழு sifive_i2c_adapter_poll(கட்டமைப்பு sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned முழு timeout = 1; // [msec]
	முழு மொத்தம் = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	செய் {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		எனில் (!(val & mask))
			திரும்பு 0;

		sbi_timer_udelay(1);
		மொத்தம் += 1;
		எனில் (மொத்தம் == (timeout * 1000))
			திரும்பு SBI_ETIMEDOUT;
	} போது (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

நிலையான முழு sifive_i2c_adapter_start(கட்டமைப்பு sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	திரும்பு sifive_i2c_adapter_poll_tip(adap);
}

நிலையான முழு sifive_i2c_adapter_write(கட்டமைப்பு i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, முழு len)
{
	கட்டமைப்பு sifive_i2c_adapter *adap =
		container_of(ia, கட்டமைப்பு sifive_i2c_adapter, adapter);
	முழு rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	எனில் (rc)
		திரும்பு rc;

	rc = sifive_i2c_adapter_rxack(adap);
	எனில் (rc)
		திரும்பு rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	எனில் (rc)
		திரும்பு rc;

	rc = sifive_i2c_adapter_rxack(adap);
	எனில் (rc)
		திரும்பு rc;

	/* set value */
	போது (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		எனில் (rc)
			திரும்பு rc;

		rc = sifive_i2c_adapter_rxack(adap);
		எனில் (rc)
			திரும்பு rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	எனில் (rc)
		திரும்பு rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	திரும்பு 0;
}

நிலையான முழு sifive_i2c_adapter_read(கட்டமைப்பு i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, முழு len)
{
	கட்டமைப்பு sifive_i2c_adapter *adap =
		container_of(ia, கட்டமைப்பு sifive_i2c_adapter, adapter);
	முழு rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	எனில் (rc)
		திரும்பு rc;

	rc = sifive_i2c_adapter_rxack(adap);
	எனில் (rc)
		திரும்பு rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	எனில் (rc)
		திரும்பு rc;

	rc = sifive_i2c_adapter_rxack(adap);
	எனில் (rc)
		திரும்பு rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	எனில் (rc)
		திரும்பு rc;

	rc = sifive_i2c_adapter_rxack(adap);
	எனில் (rc)
		திரும்பு rc;

	போது (len) {
		எனில் (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		இல்லையெனில்
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		எனில் (rc)
			திரும்பு rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	எனில் (rc)
		திரும்பு rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	திரும்பு 0;
}

நிலையான முழு sifive_i2c_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			   மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(அளவு(*adapter));
	எனில் (!adapter)
		திரும்பு SBI_ENOMEM;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, வெற்று);
	எனில் (rc) {
		sbi_free(adapter);
		திரும்பு rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.லிக = sifive_i2c_adapter_write;
	adapter->adapter.பட = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	எனில் (rc) {
		sbi_free(adapter);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
