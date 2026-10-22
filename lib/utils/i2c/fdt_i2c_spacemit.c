/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/i2c/fdt_i2c.h>

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

ഘടന spacemit_i2c_adapter {
	unsigned long base;
	ഘടന i2c_adapter adapter;
};

സ്ഥിതം വരിയിൽ ശൂന്യം spacemit_i2c_set_reg(ഘടന spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (ശൂന്യം *)adap->base + reg);
}

സ്ഥിതം വരിയിൽ uint32_t spacemit_i2c_get_reg(ഘടന spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	തിരികെ readl((ശൂന്യം *)adap->base + reg);
}

സ്ഥിതം ശൂന്യം spacemit_i2c_reset(ഘടന spacemit_i2c_adapter *adap)
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

സ്ഥിതം പൂർണ്ണസംഖ്യ spacemit_i2c_wait_xfer_done(ഘടന spacemit_i2c_adapter *adap)
{
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		എങ്കിൽ (!(val & ICR_TB))
			തിരികെ 0;

		sbi_timer_udelay(1);
	};

	തിരികെ SBI_ETIMEDOUT;
}

സ്ഥിതം ശൂന്യം spacemit_i2c_start_xfer(ഘടന spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	സ്ഥിരം uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ spacemit_i2c_xfer_write(ഘടന spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	തിരികെ spacemit_i2c_wait_xfer_done(adap);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ spacemit_i2c_xfer_read(ഘടന spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	പൂർണ്ണസംഖ്യ rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ spacemit_i2c_adapter_write(ഘടന i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, പൂർണ്ണസംഖ്യ len)
{
	ഘടന spacemit_i2c_adapter *adap =
		container_of(ia, ഘടന spacemit_i2c_adapter, adapter);
	പൂർണ്ണസംഖ്യ rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* send register + data bytes */
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ spacemit_i2c_adapter_read(ഘടന i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, പൂർണ്ണസംഖ്യ len)
{
	ഘടന spacemit_i2c_adapter *adap =
		container_of(ia, ഘടന spacemit_i2c_adapter, adapter);
	പൂർണ്ണസംഖ്യ rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* read data bytes */
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ spacemit_i2c_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			     സ്ഥിരം ഘടന fdt_match *match)
{
	ഘടന spacemit_i2c_adapter *adapter;
	uint64_t base;
	പൂർണ്ണസംഖ്യ rc;

	adapter = sbi_zalloc(വലുപ്പം(*adapter));
	എങ്കിൽ (!adapter)
		തിരികെ SBI_ENOMEM;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &base, ശൂന്യ);
	എങ്കിൽ (rc) {
		sbi_free(adapter);
		തിരികെ rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ലിഖ = spacemit_i2c_adapter_write;
	adapter->adapter.പഠ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	എങ്കിൽ (rc) {
		sbi_free(adapter);
		തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
