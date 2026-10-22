/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/i2c/fdt_i2c.h>

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

સંરચના sifive_i2c_adapter {
	unsigned long addr;
	સંરચના i2c_adapter adapter;
};

સ્થિર અંતઃરેખ શૂન્ય sifive_i2c_setreg(સંરચના sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t કિંમત)
{
	writel(કિંમત, (અસ્થિર અક્ષર *)adap->addr + reg);
}

સ્થિર અંતઃરેખ uint8_t sifive_i2c_getreg(સંરચના sifive_i2c_adapter *adap,
					uint8_t reg)
{
	પાછા readl((અસ્થિર અક્ષર *)adap->addr + reg);
}

સ્થિર પૂર્ણાંક sifive_i2c_adapter_rxack(સંરચના sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	જો (val & SIFIVE_I2C_STATUS_RXACK)
		પાછા SBI_EIO;

	પાછા 0;
}

સ્થિર પૂર્ણાંક sifive_i2c_adapter_poll(સંરચના sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned પૂર્ણાંક timeout = 1; // [msec]
	પૂર્ણાંક ગણતરી = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	કરો {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		જો (!(val & mask))
			પાછા 0;

		sbi_timer_udelay(1);
		ગણતરી += 1;
		જો (ગણતરી == (timeout * 1000))
			પાછા SBI_ETIMEDOUT;
	} જ્યારે (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

સ્થિર પૂર્ણાંક sifive_i2c_adapter_start(સંરચના sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	પાછા sifive_i2c_adapter_poll_tip(adap);
}

સ્થિર પૂર્ણાંક sifive_i2c_adapter_write(સંરચના i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, પૂર્ણાંક len)
{
	સંરચના sifive_i2c_adapter *adap =
		container_of(ia, સંરચના sifive_i2c_adapter, adapter);
	પૂર્ણાંક rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	જો (rc)
		પાછા rc;

	rc = sifive_i2c_adapter_rxack(adap);
	જો (rc)
		પાછા rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	જો (rc)
		પાછા rc;

	rc = sifive_i2c_adapter_rxack(adap);
	જો (rc)
		પાછા rc;

	/* set value */
	જ્યારે (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		જો (rc)
			પાછા rc;

		rc = sifive_i2c_adapter_rxack(adap);
		જો (rc)
			પાછા rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	જો (rc)
		પાછા rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	પાછા 0;
}

સ્થિર પૂર્ણાંક sifive_i2c_adapter_read(સંરચના i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, પૂર્ણાંક len)
{
	સંરચના sifive_i2c_adapter *adap =
		container_of(ia, સંરચના sifive_i2c_adapter, adapter);
	પૂર્ણાંક rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	જો (rc)
		પાછા rc;

	rc = sifive_i2c_adapter_rxack(adap);
	જો (rc)
		પાછા rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	જો (rc)
		પાછા rc;

	rc = sifive_i2c_adapter_rxack(adap);
	જો (rc)
		પાછા rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	જો (rc)
		પાછા rc;

	rc = sifive_i2c_adapter_rxack(adap);
	જો (rc)
		પાછા rc;

	જ્યારે (len) {
		જો (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		નહીંતર
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		જો (rc)
			પાછા rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	જો (rc)
		પાછા rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	પાછા 0;
}

સ્થિર પૂર્ણાંક sifive_i2c_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			   અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(કદ(*adapter));
	જો (!adapter)
		પાછા SBI_ENOMEM;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, ખાલી);
	જો (rc) {
		sbi_free(adapter);
		પાછા rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.લિખ = sifive_i2c_adapter_write;
	adapter->adapter.પઠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	જો (rc) {
		sbi_free(adapter);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

અચલ સંરચના fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
