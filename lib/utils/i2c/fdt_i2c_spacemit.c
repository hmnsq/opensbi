/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/i2c/fdt_i2c.h>

/* Controller registers */
#define ICR_OFFSET	0x00		/* I2C control register */
#define IDBR_OFFSET	0x0c		/* I2C data buffer register */

/* Control register bits */
#define ICR_START	BIT(0)		/* start */
#define ICR_STOP	BIT(1)		/* stop */
#define ICR_ACKNAK	BIT(2)		/* ACK(0) or NAK(1) */
#define ICR_TB		BIT(3)		/* transfer byte */
#define ICR_UR		BIT(10)		/* unit reset */
#define ICR_SCLE	BIT(13)		/* SCL enable  */
#define ICR_IUE		BIT(14)		/* unit enable */

/* Timing */
#define I2C_RESET_US	10
#define I2C_TIMEOUT_US	1000

struct spacemit_i2c_adapter {
	unsigned long base;
	struct i2c_adapter adapter;
};

static inline void spacemit_i2c_set_reg(struct spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (void *)adap->base + reg);
}

static inline uint32_t spacemit_i2c_get_reg(struct spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	вернуть readl((void *)adap->base + reg);
}

static void spacemit_i2c_reset(struct spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, ICR_IUE | ICR_SCLE);
}

static целое spacemit_i2c_wait_xfer_done(struct spacemit_i2c_adapter *adap)
{
	для (целое i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		если (!(val & ICR_TB))
			вернуть 0;

		sbi_timer_udelay(1);
	};

	вернуть SBI_ETIMEDOUT;
}

static void spacemit_i2c_start_xfer(struct spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	const uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

static целое spacemit_i2c_xfer_write(struct spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	вернуть spacemit_i2c_wait_xfer_done(adap);
}

static целое spacemit_i2c_xfer_read(struct spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	целое rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	если (rc)
		вернуть rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	вернуть 0;
}

static целое spacemit_i2c_adapter_write(struct i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, целое len)
{
	struct spacemit_i2c_adapter *adap =
		container_of(ia, struct spacemit_i2c_adapter, adapter);
	целое rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	если (rc)
		вернуть rc;

	/* send register + data bytes */
	для (целое i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		если (rc)
			вернуть rc;
	}

	вернуть 0;
}

static целое spacemit_i2c_adapter_read(struct i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, целое len)
{
	struct spacemit_i2c_adapter *adap =
		container_of(ia, struct spacemit_i2c_adapter, adapter);
	целое rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	если (rc)
		вернуть rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	если (rc)
		вернуть rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	если (rc)
		вернуть rc;

	/* read data bytes */
	для (целое i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		если (rc)
			вернуть rc;
	}

	вернуть 0;
}

static целое spacemit_i2c_init(const void *fdt, целое nodeoff,
			     const struct fdt_match *match)
{
	struct spacemit_i2c_adapter *adapter;
	uint64_t base;
	целое rc;

	adapter = sbi_zalloc(sizeof(*adapter));
	если (!adapter)
		вернуть SBI_ENOMEM;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &base, NULL);
	если (rc) {
		sbi_free(adapter);
		вернуть rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.write = spacemit_i2c_adapter_write;
	adapter->adapter.read = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	если (rc) {
		sbi_free(adapter);
		вернуть rc;
	}

	вернуть 0;
}

static const struct fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

const struct fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
