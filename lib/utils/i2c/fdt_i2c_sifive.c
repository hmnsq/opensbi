/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/fdt_i2c.h>

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

ಸಂರಚನಾ sifive_i2c_adapter {
	unsigned long addr;
	ಸಂರಚನಾ i2c_adapter adapter;
};

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ sifive_i2c_setreg(ಸಂರಚನಾ sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t ಮೌಲ್ಯ)
{
	writel(ಮೌಲ್ಯ, (ಚಞ್ಚಲ ವರ್ಣ *)adap->addr + reg);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint8_t sifive_i2c_getreg(ಸಂರಚನಾ sifive_i2c_adapter *adap,
					uint8_t reg)
{
	ಪ್ರತ್ಯಾಗಮ readl((ಚಞ್ಚಲ ವರ್ಣ *)adap->addr + reg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_i2c_adapter_rxack(ಸಂರಚನಾ sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	ಯದಿ (val & SIFIVE_I2C_STATUS_RXACK)
		ಪ್ರತ್ಯಾಗಮ SBI_EIO;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_i2c_adapter_poll(ಸಂರಚನಾ sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ timeout = 1; // [msec]
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	ಕುರು {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		ಯದಿ (!(val & mask))
			ಪ್ರತ್ಯಾಗಮ 0;

		sbi_timer_udelay(1);
		ಎಣಿಕೆ += 1;
		ಯದಿ (ಎಣಿಕೆ == (timeout * 1000))
			ಪ್ರತ್ಯಾಗಮ SBI_ETIMEDOUT;
	} ಯಾವತ್ (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_i2c_adapter_start(ಸಂರಚನಾ sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	ಪ್ರತ್ಯಾಗಮ sifive_i2c_adapter_poll_tip(adap);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_i2c_adapter_write(ಸಂರಚನಾ i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಸಂರಚನಾ sifive_i2c_adapter *adap =
		container_of(ia, ಸಂರಚನಾ sifive_i2c_adapter, adapter);
	ಪೂರ್ಣಾಙ್ಕ rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* set value */
	ಯಾವತ್ (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		rc = sifive_i2c_adapter_rxack(adap);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_i2c_adapter_read(ಸಂರಚನಾ i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಸಂರಚನಾ sifive_i2c_adapter *adap =
		container_of(ia, ಸಂರಚನಾ sifive_i2c_adapter, adapter);
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	ಯಾವತ್ (len) {
		ಯದಿ (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		ಅನ್ಯಥಾ
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_i2c_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			   ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(ಪರಿಮಾಣ(*adapter));
	ಯದಿ (!adapter)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (rc) {
		sbi_free(adapter);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ಲಿಖ = sifive_i2c_adapter_write;
	adapter->adapter.ಪಠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	ಯದಿ (rc) {
		sbi_free(adapter);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
