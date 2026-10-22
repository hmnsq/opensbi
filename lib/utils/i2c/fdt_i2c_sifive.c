/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_timer.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/i2c/fdt_i2c.h>

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

সংরচনা sifive_i2c_adapter {
	unsigned long addr;
	সংরচনা i2c_adapter adapter;
};

স্থির অন্তঃস্থ শূন্য sifive_i2c_setreg(সংরচনা sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t মান)
{
	writel(মান, (চঞ্চল ৱর্ণ *)adap->addr + reg);
}

স্থির অন্তঃস্থ uint8_t sifive_i2c_getreg(সংরচনা sifive_i2c_adapter *adap,
					uint8_t reg)
{
	ঘূৰাই readl((চঞ্চল ৱর্ণ *)adap->addr + reg);
}

স্থির পূর্ণাঙ্ক sifive_i2c_adapter_rxack(সংরচনা sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	যদি (val & SIFIVE_I2C_STATUS_RXACK)
		ঘূৰাই SBI_EIO;

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক sifive_i2c_adapter_poll(সংরচনা sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned পূর্ণাঙ্ক timeout = 1; // [msec]
	পূর্ণাঙ্ক গুনতি = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	কৰা {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		যদি (!(val & mask))
			ঘূৰাই 0;

		sbi_timer_udelay(1);
		গুনতি += 1;
		যদি (গুনতি == (timeout * 1000))
			ঘূৰাই SBI_ETIMEDOUT;
	} যেতিয়ালৈ (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

স্থির পূর্ণাঙ্ক sifive_i2c_adapter_start(সংরচনা sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	ঘূৰাই sifive_i2c_adapter_poll_tip(adap);
}

স্থির পূর্ণাঙ্ক sifive_i2c_adapter_write(সংরচনা i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, পূর্ণাঙ্ক len)
{
	সংরচনা sifive_i2c_adapter *adap =
		container_of(ia, সংরচনা sifive_i2c_adapter, adapter);
	পূর্ণাঙ্ক rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	যদি (rc)
		ঘূৰাই rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ঘূৰাই rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	যদি (rc)
		ঘূৰাই rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ঘূৰাই rc;

	/* set value */
	যেতিয়ালৈ (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		যদি (rc)
			ঘূৰাই rc;

		rc = sifive_i2c_adapter_rxack(adap);
		যদি (rc)
			ঘূৰাই rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	যদি (rc)
		ঘূৰাই rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক sifive_i2c_adapter_read(সংরচনা i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, পূর্ণাঙ্ক len)
{
	সংরচনা sifive_i2c_adapter *adap =
		container_of(ia, সংরচনা sifive_i2c_adapter, adapter);
	পূর্ণাঙ্ক rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	যদি (rc)
		ঘূৰাই rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ঘূৰাই rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	যদি (rc)
		ঘূৰাই rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ঘূৰাই rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	যদি (rc)
		ঘূৰাই rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ঘূৰাই rc;

	যেতিয়ালৈ (len) {
		যদি (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		অন্যথা
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		যদি (rc)
			ঘূৰাই rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	যদি (rc)
		ঘূৰাই rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক sifive_i2c_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			   ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(পরিমাণ(*adapter));
	যদি (!adapter)
		ঘূৰাই SBI_ENOMEM;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (rc) {
		sbi_free(adapter);
		ঘূৰাই rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.লিখ = sifive_i2c_adapter_write;
	adapter->adapter.পঠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	যদি (rc) {
		sbi_free(adapter);
		ঘূৰাই rc;
	}

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
