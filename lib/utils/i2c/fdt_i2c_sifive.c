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

struct sifive_i2c_adapter {
	unsigned long addr;
	struct i2c_adapter adapter;
};

static inline void sifive_i2c_setreg(struct sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t value)
{
	writel(value, (volatile char *)adap->addr + reg);
}

static inline uint8_t sifive_i2c_getreg(struct sifive_i2c_adapter *adap,
					uint8_t reg)
{
	повернути readl((volatile char *)adap->addr + reg);
}

static ціле sifive_i2c_adapter_rxack(struct sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	якщо (val & SIFIVE_I2C_STATUS_RXACK)
		повернути SBI_EIO;

	повернути 0;
}

static ціле sifive_i2c_adapter_poll(struct sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	unsigned ціле timeout = 1; // [msec]
	ціле count = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	do {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		якщо (!(val & mask))
			повернути 0;

		sbi_timer_udelay(1);
		count += 1;
		якщо (count == (timeout * 1000))
			повернути SBI_ETIMEDOUT;
	} поки (1);
}

#define sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#define sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

static ціле sifive_i2c_adapter_start(struct sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	повернути sifive_i2c_adapter_poll_tip(adap);
}

static ціле sifive_i2c_adapter_write(struct i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *buffer, ціле len)
{
	struct sifive_i2c_adapter *adap =
		container_of(ia, struct sifive_i2c_adapter, adapter);
	ціле rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	якщо (rc)
		повернути rc;

	rc = sifive_i2c_adapter_rxack(adap);
	якщо (rc)
		повернути rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	якщо (rc)
		повернути rc;

	rc = sifive_i2c_adapter_rxack(adap);
	якщо (rc)
		повернути rc;

	/* set value */
	поки (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *buffer);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		якщо (rc)
			повернути rc;

		rc = sifive_i2c_adapter_rxack(adap);
		якщо (rc)
			повернути rc;

		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	якщо (rc)
		повернути rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	повернути 0;
}

static ціле sifive_i2c_adapter_read(struct i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *buffer, ціле len)
{
	struct sifive_i2c_adapter *adap =
		container_of(ia, struct sifive_i2c_adapter, adapter);
	ціле rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	якщо (rc)
		повернути rc;

	rc = sifive_i2c_adapter_rxack(adap);
	якщо (rc)
		повернути rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	якщо (rc)
		повернути rc;

	rc = sifive_i2c_adapter_rxack(adap);
	якщо (rc)
		повернути rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	якщо (rc)
		повернути rc;

	rc = sifive_i2c_adapter_rxack(adap);
	якщо (rc)
		повернути rc;

	поки (len) {
		якщо (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		інакше
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		якщо (rc)
			повернути rc;

		*buffer = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		buffer++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	якщо (rc)
		повернути rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	повернути 0;
}

static ціле sifive_i2c_init(const void *fdt, ціле nodeoff,
			   const struct fdt_match *match)
{
	ціле rc;
	struct sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(sizeof(*adapter));
	якщо (!adapter)
		повернути SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, NULL);
	якщо (rc) {
		sbi_free(adapter);
		повернути rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.write = sifive_i2c_adapter_write;
	adapter->adapter.read = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	якщо (rc) {
		sbi_free(adapter);
		повернути rc;
	}

	повернути 0;
}

static const struct fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

const struct fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
