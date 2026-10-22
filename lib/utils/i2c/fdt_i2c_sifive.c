/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/i2c/fdt_i2c.h>

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

ഘടന sifive_i2c_adapter {
	unsigned long addr;
	ഘടന i2c_adapter adapter;
};

സ്ഥിതം വരിയിൽ ശൂന്യം sifive_i2c_setreg(ഘടന sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t മൂല്യം)
{
	writel(മൂല്യം, (അസ്ഥിര അക്ഷരം *)adap->addr + reg);
}

സ്ഥിതം വരിയിൽ uint8_t sifive_i2c_getreg(ഘടന sifive_i2c_adapter *adap,
					uint8_t reg)
{
	തിരികെ readl((അസ്ഥിര അക്ഷരം *)adap->addr + reg);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_i2c_adapter_rxack(ഘടന sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	എങ്കിൽ (val & SIFIVE_I2C_STATUS_RXACK)
		തിരികെ SBI_EIO;

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_i2c_adapter_poll(ഘടന sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned പൂർണ്ണസംഖ്യ timeout = 1; // [msec]
	പൂർണ്ണസംഖ്യ എണ്ണം = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	ചെയ്യുക {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		എങ്കിൽ (!(val & mask))
			തിരികെ 0;

		sbi_timer_udelay(1);
		എണ്ണം += 1;
		എങ്കിൽ (എണ്ണം == (timeout * 1000))
			തിരികെ SBI_ETIMEDOUT;
	} ആകുമ്പോൾ (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_i2c_adapter_start(ഘടന sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	തിരികെ sifive_i2c_adapter_poll_tip(adap);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_i2c_adapter_write(ഘടന i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, പൂർണ്ണസംഖ്യ len)
{
	ഘടന sifive_i2c_adapter *adap =
		container_of(ia, ഘടന sifive_i2c_adapter, adapter);
	പൂർണ്ണസംഖ്യ rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* set value */
	ആകുമ്പോൾ (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		എങ്കിൽ (rc)
			തിരികെ rc;

		rc = sifive_i2c_adapter_rxack(adap);
		എങ്കിൽ (rc)
			തിരികെ rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_i2c_adapter_read(ഘടന i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, പൂർണ്ണസംഖ്യ len)
{
	ഘടന sifive_i2c_adapter *adap =
		container_of(ia, ഘടന sifive_i2c_adapter, adapter);
	പൂർണ്ണസംഖ്യ rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	ആകുമ്പോൾ (len) {
		എങ്കിൽ (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		അല്ലെങ്കിൽ
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		എങ്കിൽ (rc)
			തിരികെ rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_i2c_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			   സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(വലുപ്പം(*adapter));
	എങ്കിൽ (!adapter)
		തിരികെ SBI_ENOMEM;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, ശൂന്യ);
	എങ്കിൽ (rc) {
		sbi_free(adapter);
		തിരികെ rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ലിഖ = sifive_i2c_adapter_write;
	adapter->adapter.പഠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	എങ്കിൽ (rc) {
		sbi_free(adapter);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
