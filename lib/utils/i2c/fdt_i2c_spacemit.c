/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/i2c/fdt_i2c.h>

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

संरचना spacemit_i2c_adapter {
	unsigned long base;
	संरचना i2c_adapter adapter;
};

स्थिर अन्तःस्थ शून्य spacemit_i2c_set_reg(संरचना spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (शून्य *)adap->base + reg);
}

स्थिर अन्तःस्थ uint32_t spacemit_i2c_get_reg(संरचना spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	प्रत्यागम readl((शून्य *)adap->base + reg);
}

स्थिर शून्य spacemit_i2c_reset(संरचना spacemit_i2c_adapter *adap)
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

स्थिर पूर्णाङ्क spacemit_i2c_wait_xfer_done(संरचना spacemit_i2c_adapter *adap)
{
	प्रति (पूर्णाङ्क i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		यदि (!(val & ICR_TB))
			प्रत्यागम 0;

		sbi_timer_udelay(1);
	};

	प्रत्यागम SBI_ETIMEDOUT;
}

स्थिर शून्य spacemit_i2c_start_xfer(संरचना spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ध्रुव uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

स्थिर पूर्णाङ्क spacemit_i2c_xfer_write(संरचना spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	प्रत्यागम spacemit_i2c_wait_xfer_done(adap);
}

स्थिर पूर्णाङ्क spacemit_i2c_xfer_read(संरचना spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	पूर्णाङ्क rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	यदि (rc)
		प्रत्यागम rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क spacemit_i2c_adapter_write(संरचना i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, पूर्णाङ्क len)
{
	संरचना spacemit_i2c_adapter *adap =
		container_of(ia, संरचना spacemit_i2c_adapter, adapter);
	पूर्णाङ्क rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	यदि (rc)
		प्रत्यागम rc;

	/* send register + data bytes */
	प्रति (पूर्णाङ्क i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		यदि (rc)
			प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क spacemit_i2c_adapter_read(संरचना i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, पूर्णाङ्क len)
{
	संरचना spacemit_i2c_adapter *adap =
		container_of(ia, संरचना spacemit_i2c_adapter, adapter);
	पूर्णाङ्क rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	यदि (rc)
		प्रत्यागम rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	यदि (rc)
		प्रत्यागम rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	यदि (rc)
		प्रत्यागम rc;

	/* read data bytes */
	प्रति (पूर्णाङ्क i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		यदि (rc)
			प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क spacemit_i2c_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			     ध्रुव संरचना fdt_match *match)
{
	संरचना spacemit_i2c_adapter *adapter;
	uint64_t base;
	पूर्णाङ्क rc;

	adapter = sbi_zalloc(परिमाण(*adapter));
	यदि (!adapter)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &base, रिक्त);
	यदि (rc) {
		sbi_free(adapter);
		प्रत्यागम rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.लिख = spacemit_i2c_adapter_write;
	adapter->adapter.पठ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	यदि (rc) {
		sbi_free(adapter);
		प्रत्यागम rc;
	}

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
