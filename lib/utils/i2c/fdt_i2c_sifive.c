/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/i2c/fdt_i2c.h>

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

sa_nracanaa sifive_i2c_adapter {
	unsigned long addr;
	sa_nracanaa i2c_adapter adapter;
};

static anta_hstha void sifive_i2c_setreg(sa_nracanaa sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t value)
{
	writel(value, (ca_ncala var_na *)adap->addr + reg);
}

static anta_hstha uint8_t sifive_i2c_getreg(sa_nracanaa sifive_i2c_adapter *adap,
					uint8_t reg)
{
	ສົ່ງຄືນ readl((ca_ncala var_na *)adap->addr + reg);
}

static ຈຳນວນເຕັມ sifive_i2c_adapter_rxack(sa_nracanaa sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	ຖ້າ (val & SIFIVE_I2C_STATUS_RXACK)
		ສົ່ງຄືນ SBI_EIO;

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ sifive_i2c_adapter_poll(sa_nracanaa sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned ຈຳນວນເຕັມ timeout = 1; // [msec]
	ຈຳນວນເຕັມ count = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	do {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		ຖ້າ (!(val & mask))
			ສົ່ງຄືນ 0;

		sbi_timer_udelay(1);
		count += 1;
		ຖ້າ (count == (timeout * 1000))
			ສົ່ງຄືນ SBI_ETIMEDOUT;
	} ໃນຂະນະທີ່ (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

static ຈຳນວນເຕັມ sifive_i2c_adapter_start(sa_nracanaa sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	ສົ່ງຄືນ sifive_i2c_adapter_poll_tip(adap);
}

static ຈຳນວນເຕັມ sifive_i2c_adapter_write(sa_nracanaa i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, ຈຳນວນເຕັມ len)
{
	sa_nracanaa sifive_i2c_adapter *adap =
		container_of(ia, sa_nracanaa sifive_i2c_adapter, adapter);
	ຈຳນວນເຕັມ rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	/* set value */
	ໃນຂະນະທີ່ (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		rc = sifive_i2c_adapter_rxack(adap);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ sifive_i2c_adapter_read(sa_nracanaa i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, ຈຳນວນເຕັມ len)
{
	sa_nracanaa sifive_i2c_adapter *adap =
		container_of(ia, sa_nracanaa sifive_i2c_adapter, adapter);
	ຈຳນວນເຕັມ rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	ໃນຂະນະທີ່ (len) {
		ຖ້າ (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		ບໍ່ດັ່ງນັ້ນ
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ sifive_i2c_init(const void *fdt, ຈຳນວນເຕັມ nodeoff,
			   const sa_nracanaa fdt_match *match)
{
	ຈຳນວນເຕັມ rc;
	sa_nracanaa sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(parimaa_na(*adapter));
	ຖ້າ (!adapter)
		ສົ່ງຄືນ SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	ຖ້າ (rc) {
		sbi_free(adapter);
		ສົ່ງຄືນ rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.write = sifive_i2c_adapter_write;
	adapter->adapter.read = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	ຖ້າ (rc) {
		sbi_free(adapter);
		ສົ່ງຄືນ rc;
	}

	ສົ່ງຄືນ 0;
}

static const sa_nracanaa fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

const sa_nracanaa fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
