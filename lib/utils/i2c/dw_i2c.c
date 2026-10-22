/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/dw_i2c.h>

#define DW_IC_CON		0x00
#define DW_IC_TAR		0x04
#define DW_IC_SAR		0x08
#define DW_IC_DATA_CMD		0x10
#define DW_IC_SS_SCL_HCNT	0x14
#define DW_IC_SS_SCL_LCNT	0x18
#define DW_IC_FS_SCL_HCNT	0x1c
#define DW_IC_FS_SCL_LCNT	0x20
#define DW_IC_HS_SCL_HCNT	0x24
#define DW_IC_HS_SCL_LCNT	0x28
#define DW_IC_INTR_STAT		0x2c
#define DW_IC_INTR_MASK		0x30
#define DW_IC_RAW_INTR_STAT	0x34
#define DW_IC_RX_TL		0x38
#define DW_IC_TX_TL		0x3c
#define DW_IC_CLR_INTR		0x40
#define DW_IC_CLR_RX_UNDER	0x44
#define DW_IC_CLR_RX_OVER	0x48
#define DW_IC_CLR_TX_OVER	0x4c
#define DW_IC_CLR_RD_REQ	0x50
#define DW_IC_CLR_TX_ABRT	0x54
#define DW_IC_CLR_RX_DONE	0x58
#define DW_IC_CLR_ACTIVITY	0x5c
#define DW_IC_CLR_STOP_DET	0x60
#define DW_IC_CLR_START_DET	0x64
#define DW_IC_CLR_GEN_CALL	0x68
#define DW_IC_ENABLE		0x6c
#define DW_IC_STATUS		0x70
#define DW_IC_TXFLR		0x74
#define DW_IC_RXFLR		0x78
#define DW_IC_SDA_HOLD		0x7c
#define DW_IC_TX_ABRT_SOURCE	0x80
#define DW_IC_ENABLE_STATUS	0x9c
#define DW_IC_CLR_RESTART_DET	0xa8
#define DW_IC_COMP_PARAM_1	0xf4
#define DW_IC_COMP_VERSION	0xf8

#define DW_I2C_STATUS_TXFIFO_EMPTY	BIT(2)
#define DW_I2C_STATUS_RXFIFO_NOT_EMPTY	BIT(3)

#define IC_DATA_CMD_READ	BIT(8)
#define IC_DATA_CMD_STOP	BIT(9)
#define IC_DATA_CMD_RESTART	BIT(10)
#define IC_INT_STATUS_STOPDET	BIT(9)

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ dw_i2c_setreg(ಸಂರಚನಾ dw_i2c_adapter *adap,
				 u8 reg, u32 ಮೌಲ್ಯ)
{
	writel(ಮೌಲ್ಯ, (ಶೂನ್ಯ *)adap->addr + reg);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32 dw_i2c_getreg(ಸಂರಚನಾ dw_i2c_adapter *adap,
				u32 reg)
{
	ಪ್ರತ್ಯಾಗಮ readl((ಶೂನ್ಯ *)adap->addr + reg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dw_i2c_adapter_poll(ಸಂರಚನಾ dw_i2c_adapter *adap,
			       u32 mask, u32 addr,
			       bool inverted)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ timeout = 10; /* msec */
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;
	u32 val;

	ಕುರು {
		val = dw_i2c_getreg(adap, addr);
		ಯದಿ (inverted) {
			ಯದಿ (!(val & mask))
				ಪ್ರತ್ಯಾಗಮ 0;
		} ಅನ್ಯಥಾ {
			ಯದಿ (val & mask)
				ಪ್ರತ್ಯಾಗಮ 0;
		}
		sbi_timer_udelay(2);
		ಎಣಿಕೆ += 1;
		ಯದಿ (ಎಣಿಕೆ == (timeout * 1000))
			ಪ್ರತ್ಯಾಗಮ SBI_ETIMEDOUT;
	} ಯಾವತ್ (1);
}

#define dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_RXFIFO_NOT_EMPTY, DW_IC_STATUS, 0)
#define dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_TXFIFO_EMPTY, DW_IC_STATUS, 0)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dw_i2c_write_addr(ಸಂರಚನಾ dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dw_i2c_adapter_read(ಸಂರಚನಾ i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *buffer, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಸಂರಚನಾ dw_i2c_adapter *adap =
		container_of(ia, ಸಂರಚನಾ dw_i2c_adapter, adapter);
	ಪೂರ್ಣಾಙ್ಕ rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_DATA_CMD, reg);

	/* set value */
	ಯಾವತ್ (len) {
		ಯದಿ (len == 1)
			dw_i2c_setreg(adap, DW_IC_DATA_CMD,
				      IC_DATA_CMD_READ | IC_DATA_CMD_STOP);
		ಅನ್ಯಥಾ
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, IC_DATA_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		*buffer = dw_i2c_getreg(adap, DW_IC_DATA_CMD) & 0xff;
		buffer++;
		len--;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dw_i2c_adapter_write(ಸಂರಚನಾ i2c_adapter *ia, u8 addr,
				u8 reg, u8 *buffer, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಸಂರಚನಾ dw_i2c_adapter *adap =
		container_of(ia, ಸಂರಚನಾ dw_i2c_adapter, adapter);
	ಪೂರ್ಣಾಙ್ಕ rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_DATA_CMD, reg);

	ಯಾವತ್ (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		ಯದಿ (len == 1)
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, *buffer | IC_DATA_CMD_STOP);
		ಅನ್ಯಥಾ
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, *buffer);

		buffer++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಪೂರ್ಣಾಙ್ಕ dw_i2c_init(ಸಂರಚನಾ i2c_adapter *adapter, ಪೂರ್ಣಾಙ್ಕ nodeoff)
{
	adapter->id = nodeoff;
	adapter->ಲಿಖ = dw_i2c_adapter_write;
	adapter->ಪಠ = dw_i2c_adapter_read;

	ಪ್ರತ್ಯಾಗಮ i2c_adapter_add(adapter);
}
