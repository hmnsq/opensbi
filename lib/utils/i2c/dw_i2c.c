/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi_utils/i2c/dw_i2c.h>

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

நிலையான உள்வரி வெறுமை dw_i2c_setreg(கட்டமைப்பு dw_i2c_adapter *adap,
				 u8 reg, u32 மதிப்பு)
{
	writel(மதிப்பு, (வெறுமை *)adap->addr + reg);
}

நிலையான உள்வரி u32 dw_i2c_getreg(கட்டமைப்பு dw_i2c_adapter *adap,
				u32 reg)
{
	திரும்பு readl((வெறுமை *)adap->addr + reg);
}

நிலையான முழு dw_i2c_adapter_poll(கட்டமைப்பு dw_i2c_adapter *adap,
			       u32 mask, u32 addr,
			       bool inverted)
{
	unsigned முழு timeout = 10; /* msec */
	முழு மொத்தம் = 0;
	u32 val;

	செய் {
		val = dw_i2c_getreg(adap, addr);
		எனில் (inverted) {
			எனில் (!(val & mask))
				திரும்பு 0;
		} இல்லையெனில் {
			எனில் (val & mask)
				திரும்பு 0;
		}
		sbi_timer_udelay(2);
		மொத்தம் += 1;
		எனில் (மொத்தம் == (timeout * 1000))
			திரும்பு SBI_ETIMEDOUT;
	} போது (1);
}

#define dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_RXFIFO_NOT_EMPTY, DW_IC_STATUS, 0)
#define dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_TXFIFO_EMPTY, DW_IC_STATUS, 0)

நிலையான முழு dw_i2c_write_addr(கட்டமைப்பு dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	திரும்பு 0;
}

நிலையான முழு dw_i2c_adapter_read(கட்டமைப்பு i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *buffer, முழு len)
{
	கட்டமைப்பு dw_i2c_adapter *adap =
		container_of(ia, கட்டமைப்பு dw_i2c_adapter, adapter);
	முழு rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	எனில் (rc)
		திரும்பு rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_DATA_CMD, reg);

	/* set value */
	போது (len) {
		எனில் (len == 1)
			dw_i2c_setreg(adap, DW_IC_DATA_CMD,
				      IC_DATA_CMD_READ | IC_DATA_CMD_STOP);
		இல்லையெனில்
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, IC_DATA_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		எனில் (rc)
			திரும்பு rc;

		*buffer = dw_i2c_getreg(adap, DW_IC_DATA_CMD) & 0xff;
		buffer++;
		len--;
	}

	திரும்பு 0;
}

நிலையான முழு dw_i2c_adapter_write(கட்டமைப்பு i2c_adapter *ia, u8 addr,
				u8 reg, u8 *buffer, முழு len)
{
	கட்டமைப்பு dw_i2c_adapter *adap =
		container_of(ia, கட்டமைப்பு dw_i2c_adapter, adapter);
	முழு rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	எனில் (rc)
		திரும்பு rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_DATA_CMD, reg);

	போது (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		எனில் (rc)
			திரும்பு rc;

		எனில் (len == 1)
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, *buffer | IC_DATA_CMD_STOP);
		இல்லையெனில்
			dw_i2c_setreg(adap, DW_IC_DATA_CMD, *buffer);

		buffer++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	திரும்பு rc;
}

முழு dw_i2c_init(கட்டமைப்பு i2c_adapter *adapter, முழு nodeoff)
{
	adapter->id = nodeoff;
	adapter->லிக = dw_i2c_adapter_write;
	adapter->பட = dw_i2c_adapter_read;

	திரும்பு i2c_adapter_add(adapter);
}
