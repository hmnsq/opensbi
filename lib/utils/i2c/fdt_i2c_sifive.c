/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/i2c/fdt_i2c.h>

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

কাঠামো sifive_i2c_adapter {
	unsigned long addr;
	কাঠামো i2c_adapter adapter;
};

স্থির অন্তঃরেখ শূন্য sifive_i2c_setreg(কাঠামো sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t মান)
{
	writel(মান, (অস্থির অক্ষর *)adap->addr + reg);
}

স্থির অন্তঃরেখ uint8_t sifive_i2c_getreg(কাঠামো sifive_i2c_adapter *adap,
					uint8_t reg)
{
	ফেরত readl((অস্থির অক্ষর *)adap->addr + reg);
}

স্থির পূর্ণসংখ্যা sifive_i2c_adapter_rxack(কাঠামো sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	যদি (val & SIFIVE_I2C_STATUS_RXACK)
		ফেরত SBI_EIO;

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা sifive_i2c_adapter_poll(কাঠামো sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned পূর্ণসংখ্যা timeout = 1; // [msec]
	পূর্ণসংখ্যা গুনতি = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	করো {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		যদি (!(val & mask))
			ফেরত 0;

		sbi_timer_udelay(1);
		গুনতি += 1;
		যদি (গুনতি == (timeout * 1000))
			ফেরত SBI_ETIMEDOUT;
	} যখন (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

স্থির পূর্ণসংখ্যা sifive_i2c_adapter_start(কাঠামো sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	ফেরত sifive_i2c_adapter_poll_tip(adap);
}

স্থির পূর্ণসংখ্যা sifive_i2c_adapter_write(কাঠামো i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *বাফার, পূর্ণসংখ্যা len)
{
	কাঠামো sifive_i2c_adapter *adap =
		container_of(ia, কাঠামো sifive_i2c_adapter, adapter);
	পূর্ণসংখ্যা rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	যদি (rc)
		ফেরত rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ফেরত rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	যদি (rc)
		ফেরত rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ফেরত rc;

	/* set value */
	যখন (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *বাফার);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		যদি (rc)
			ফেরত rc;

		rc = sifive_i2c_adapter_rxack(adap);
		যদি (rc)
			ফেরত rc;

		বাফার++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	যদি (rc)
		ফেরত rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা sifive_i2c_adapter_read(কাঠামো i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *বাফার, পূর্ণসংখ্যা len)
{
	কাঠামো sifive_i2c_adapter *adap =
		container_of(ia, কাঠামো sifive_i2c_adapter, adapter);
	পূর্ণসংখ্যা rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	যদি (rc)
		ফেরত rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ফেরত rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	যদি (rc)
		ফেরত rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ফেরত rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	যদি (rc)
		ফেরত rc;

	rc = sifive_i2c_adapter_rxack(adap);
	যদি (rc)
		ফেরত rc;

	যখন (len) {
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
			ফেরত rc;

		*বাফার = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		বাফার++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	যদি (rc)
		ফেরত rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা sifive_i2c_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			   ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(আকার(*adapter));
	যদি (!adapter)
		ফেরত SBI_ENOMEM;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &addr, ফাঁকা);
	যদি (rc) {
		sbi_free(adapter);
		ফেরত rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.লিখ = sifive_i2c_adapter_write;
	adapter->adapter.পঠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	যদি (rc) {
		sbi_free(adapter);
		ফেরত rc;
	}

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
