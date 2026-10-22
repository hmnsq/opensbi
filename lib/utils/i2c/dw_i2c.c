/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_bitops.h>
#অন্তর্ভাব <sbi_utils/i2c/dw_i2c.h>

#define DW_IC_CON		0x00
#define DW_IC_TAR		0x04
#define DW_IC_SAR		0x08
#define DW_IC_তথ্য_CMD		0x10
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

#define IC_তথ্য_CMD_READ	BIT(8)
#define IC_তথ্য_CMD_STOP	BIT(9)
#define IC_তথ্য_CMD_RESTART	BIT(10)
#define IC_INT_STATUS_STOPDET	BIT(9)

স্থির অন্তঃরেখ শূন্য dw_i2c_setreg(কাঠামো dw_i2c_adapter *adap,
				 u8 reg, u32 মান)
{
	writel(মান, (শূন্য *)adap->addr + reg);
}

স্থির অন্তঃরেখ u32 dw_i2c_getreg(কাঠামো dw_i2c_adapter *adap,
				u32 reg)
{
	ফেরত readl((শূন্য *)adap->addr + reg);
}

স্থির পূর্ণসংখ্যা dw_i2c_adapter_poll(কাঠামো dw_i2c_adapter *adap,
			       u32 mask, u32 addr,
			       bool inverted)
{
	unsigned পূর্ণসংখ্যা timeout = 10; /* msec */
	পূর্ণসংখ্যা গুনতি = 0;
	u32 val;

	করো {
		val = dw_i2c_getreg(adap, addr);
		যদি (inverted) {
			যদি (!(val & mask))
				ফেরত 0;
		} অন্যথা {
			যদি (val & mask)
				ফেরত 0;
		}
		sbi_timer_udelay(2);
		গুনতি += 1;
		যদি (গুনতি == (timeout * 1000))
			ফেরত SBI_ETIMEDOUT;
	} যখন (1);
}

#define dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_RXFIFO_NOT_EMPTY, DW_IC_STATUS, 0)
#define dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_TXFIFO_EMPTY, DW_IC_STATUS, 0)

স্থির পূর্ণসংখ্যা dw_i2c_write_addr(কাঠামো dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা dw_i2c_adapter_read(কাঠামো i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *বাফার, পূর্ণসংখ্যা len)
{
	কাঠামো dw_i2c_adapter *adap =
		container_of(ia, কাঠামো dw_i2c_adapter, adapter);
	পূর্ণসংখ্যা rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	যদি (rc)
		ফেরত rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_তথ্য_CMD, reg);

	/* set value */
	যখন (len) {
		যদি (len == 1)
			dw_i2c_setreg(adap, DW_IC_তথ্য_CMD,
				      IC_তথ্য_CMD_READ | IC_তথ্য_CMD_STOP);
		অন্যথা
			dw_i2c_setreg(adap, DW_IC_তথ্য_CMD, IC_তথ্য_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		যদি (rc)
			ফেরত rc;

		*বাফার = dw_i2c_getreg(adap, DW_IC_তথ্য_CMD) & 0xff;
		বাফার++;
		len--;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা dw_i2c_adapter_write(কাঠামো i2c_adapter *ia, u8 addr,
				u8 reg, u8 *বাফার, পূর্ণসংখ্যা len)
{
	কাঠামো dw_i2c_adapter *adap =
		container_of(ia, কাঠামো dw_i2c_adapter, adapter);
	পূর্ণসংখ্যা rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	যদি (rc)
		ফেরত rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_তথ্য_CMD, reg);

	যখন (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		যদি (rc)
			ফেরত rc;

		যদি (len == 1)
			dw_i2c_setreg(adap, DW_IC_তথ্য_CMD, *বাফার | IC_তথ্য_CMD_STOP);
		অন্যথা
			dw_i2c_setreg(adap, DW_IC_তথ্য_CMD, *বাফার);

		বাফার++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	ফেরত rc;
}

পূর্ণসংখ্যা dw_i2c_init(কাঠামো i2c_adapter *adapter, পূর্ণসংখ্যা nodeoff)
{
	adapter->id = nodeoff;
	adapter->লিখ = dw_i2c_adapter_write;
	adapter->পঠ = dw_i2c_adapter_read;

	ফেরত i2c_adapter_add(adapter);
}
