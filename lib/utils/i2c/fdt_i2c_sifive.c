/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/i2c/fdt_i2c.h>

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

ସଂରଚନା sifive_i2c_adapter {
	unsigned long addr;
	ସଂରଚନା i2c_adapter adapter;
};

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ sifive_i2c_setreg(ସଂରଚନା sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t ମୂଲ୍ୟ)
{
	writel(ମୂଲ୍ୟ, (ଅସ୍ଥିର ଅକ୍ଷର *)adap->addr + reg);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint8_t sifive_i2c_getreg(ସଂରଚନା sifive_i2c_adapter *adap,
					uint8_t reg)
{
	ଫେରନ୍ତୁ readl((ଅସ୍ଥିର ଅକ୍ଷର *)adap->addr + reg);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_i2c_adapter_rxack(ସଂରଚନା sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	ଯଦି (val & SIFIVE_I2C_STATUS_RXACK)
		ଫେରନ୍ତୁ SBI_EIO;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_i2c_adapter_poll(ସଂରଚନା sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout = 1; // [msec]
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	କର {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		ଯଦି (!(val & mask))
			ଫେରନ୍ତୁ 0;

		sbi_timer_udelay(1);
		ଗଣନା_2 += 1;
		ଯଦି (ଗଣନା_2 == (timeout * 1000))
			ଫେରନ୍ତୁ SBI_ETIMEDOUT;
	} ଯେତେବେଳେ (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_i2c_adapter_start(ସଂରଚନା sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	ଫେରନ୍ତୁ sifive_i2c_adapter_poll_tip(adap);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_i2c_adapter_write(ସଂରଚନା i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ସଂରଚନା sifive_i2c_adapter *adap =
		container_of(ia, ସଂରଚନା sifive_i2c_adapter, adapter);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* set value */
	ଯେତେବେଳେ (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		rc = sifive_i2c_adapter_rxack(adap);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_i2c_adapter_read(ସଂରଚନା i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ସଂରଚନା sifive_i2c_adapter *adap =
		container_of(ia, ସଂରଚନା sifive_i2c_adapter, adapter);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	ଯେତେବେଳେ (len) {
		ଯଦି (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		ଅନ୍ୟଥା
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_i2c_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			   ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(ଆକାର(*adapter));
	ଯଦି (!adapter)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, ଖାଲି);
	ଯଦି (rc) {
		sbi_free(adapter);
		ଫେରନ୍ତୁ rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ଲିଖ = sifive_i2c_adapter_write;
	adapter->adapter.ପଠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	ଯଦି (rc) {
		sbi_free(adapter);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
