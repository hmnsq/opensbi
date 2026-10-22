/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi_utils/i2c/dw_i2c.h>

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

സ്ഥിതം വരിയിൽ ശൂന്യം dw_i2c_setreg(ഘടന dw_i2c_adapter *adap,
				 u8 reg, u32 മൂല്യം)
{
	writel(മൂല്യം, (ശൂന്യം *)adap->addr + reg);
}

സ്ഥിതം വരിയിൽ u32 dw_i2c_getreg(ഘടന dw_i2c_adapter *adap,
				u32 reg)
{
	തിരികെ readl((ശൂന്യം *)adap->addr + reg);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dw_i2c_adapter_poll(ഘടന dw_i2c_adapter *adap,
			       u32 mask, u32 addr,
			       bool inverted)
{
	unsigned പൂർണ്ണസംഖ്യ timeout = 10; /* msec */
	പൂർണ്ണസംഖ്യ എണ്ണം = 0;
	u32 val;

	ചെയ്യുക {
		val = dw_i2c_getreg(adap, addr);
		എങ്കിൽ (inverted) {
			എങ്കിൽ (!(val & mask))
				തിരികെ 0;
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (val & mask)
				തിരികെ 0;
		}
		sbi_timer_udelay(2);
		എണ്ണം += 1;
		എങ്കിൽ (എണ്ണം == (timeout * 1000))
			തിരികെ SBI_ETIMEDOUT;
	} ആകുമ്പോൾ (1);
}

#define dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_RXFIFO_NOT_EMPTY, DW_IC_STATUS, 0)
#define dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_TXFIFO_EMPTY, DW_IC_STATUS, 0)

സ്ഥിതം പൂർണ്ണസംഖ്യ dw_i2c_write_addr(ഘടന dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dw_i2c_adapter_read(ഘടന i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *buffer, പൂർണ്ണസംഖ്യ len)
{
	ഘടന dw_i2c_adapter *adap =
		container_of(ia, ഘടന dw_i2c_adapter, adapter);
	പൂർണ്ണസംഖ്യ rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_DATA_CMD, reg);

	/* set value */
	ആകുമ്പോൾ (len) {
		എങ്കിൽ (len == 1)
			dw_i2c_setreg(adap, DW_IC_DATA_CMD,
				      IC_DATA_CMD_READ | IC_DATA_CMD_STOP);
		അല്ലെങ്കിൽ
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, IC_DATA_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		എങ്കിൽ (rc)
			തിരികെ rc;

		*buffer = dw_i2c_getreg(adap, DW_IC_DATA_CMD) & 0xff;
		buffer++;
		len--;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dw_i2c_adapter_write(ഘടന i2c_adapter *ia, u8 addr,
				u8 reg, u8 *buffer, പൂർണ്ണസംഖ്യ len)
{
	ഘടന dw_i2c_adapter *adap =
		container_of(ia, ഘടന dw_i2c_adapter, adapter);
	പൂർണ്ണസംഖ്യ rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	എങ്കിൽ (rc)
		തിരികെ rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_DATA_CMD, reg);

	ആകുമ്പോൾ (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		എങ്കിൽ (rc)
			തിരികെ rc;

		എങ്കിൽ (len == 1)
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, *buffer | IC_DATA_CMD_STOP);
		അല്ലെങ്കിൽ
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, *buffer);

		buffer++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	തിരികെ rc;
}

പൂർണ്ണസംഖ്യ dw_i2c_init(ഘടന i2c_adapter *adapter, പൂർണ്ണസംഖ്യ nodeoff)
{
	adapter->id = nodeoff;
	adapter->ലിഖ = dw_i2c_adapter_write;
	adapter->പഠ = dw_i2c_adapter_read;

	തിരികെ i2c_adapter_add(adapter);
}
