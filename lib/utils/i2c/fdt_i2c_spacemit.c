/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/fdt_i2c.h>

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

ಸಂರಚನಾ spacemit_i2c_adapter {
	unsigned long base;
	ಸಂರಚನಾ i2c_adapter adapter;
};

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ spacemit_i2c_set_reg(ಸಂರಚನಾ spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (ಶೂನ್ಯ *)adap->base + reg);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint32_t spacemit_i2c_get_reg(ಸಂರಚನಾ spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	ಪ್ರತ್ಯಾಗಮ readl((ಶೂನ್ಯ *)adap->base + reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_i2c_reset(ಸಂರಚನಾ spacemit_i2c_adapter *adap)
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_i2c_wait_xfer_done(ಸಂರಚನಾ spacemit_i2c_adapter *adap)
{
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		ಯದಿ (!(val & ICR_TB))
			ಪ್ರತ್ಯಾಗಮ 0;

		sbi_timer_udelay(1);
	};

	ಪ್ರತ್ಯಾಗಮ SBI_ETIMEDOUT;
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_i2c_start_xfer(ಸಂರಚನಾ spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ಧ್ರುವ uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_i2c_xfer_write(ಸಂರಚನಾ spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	ಪ್ರತ್ಯಾಗಮ spacemit_i2c_wait_xfer_done(adap);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_i2c_xfer_read(ಸಂರಚನಾ spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_i2c_adapter_write(ಸಂರಚನಾ i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಸಂರಚನಾ spacemit_i2c_adapter *adap =
		container_of(ia, ಸಂರಚನಾ spacemit_i2c_adapter, adapter);
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* send register + data bytes */
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_i2c_adapter_read(ಸಂರಚನಾ i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಸಂರಚನಾ spacemit_i2c_adapter *adap =
		container_of(ia, ಸಂರಚನಾ spacemit_i2c_adapter, adapter);
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* read data bytes */
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_i2c_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			     ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಸಂರಚನಾ spacemit_i2c_adapter *adapter;
	uint64_t base;
	ಪೂರ್ಣಾಙ್ಕ rc;

	adapter = sbi_zalloc(ಪರಿಮಾಣ(*adapter));
	ಯದಿ (!adapter)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &base, ರಿಕ್ತ);
	ಯದಿ (rc) {
		sbi_free(adapter);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ಲಿಖ = spacemit_i2c_adapter_write;
	adapter->adapter.ಪಠ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	ಯದಿ (rc) {
		sbi_free(adapter);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
