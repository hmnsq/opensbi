/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi_utils/i2c/dw_i2c.h>

#تعریف_کرو DW_IC_CON		0x00
#تعریف_کرو DW_IC_TAR		0x04
#تعریف_کرو DW_IC_SAR		0x08
#تعریف_کرو DW_IC_ڈیٹا_CMD		0x10
#تعریف_کرو DW_IC_SS_SCL_HCNT	0x14
#تعریف_کرو DW_IC_SS_SCL_LCNT	0x18
#تعریف_کرو DW_IC_FS_SCL_HCNT	0x1c
#تعریف_کرو DW_IC_FS_SCL_LCNT	0x20
#تعریف_کرو DW_IC_HS_SCL_HCNT	0x24
#تعریف_کرو DW_IC_HS_SCL_LCNT	0x28
#تعریف_کرو DW_IC_INTR_STAT		0x2c
#تعریف_کرو DW_IC_INTR_نقاب		0x30
#تعریف_کرو DW_IC_RAW_INTR_STAT	0x34
#تعریف_کرو DW_IC_RX_TL		0x38
#تعریف_کرو DW_IC_TX_TL		0x3c
#تعریف_کرو DW_IC_CLR_INTR		0x40
#تعریف_کرو DW_IC_CLR_RX_UNDER	0x44
#تعریف_کرو DW_IC_CLR_RX_OVER	0x48
#تعریف_کرو DW_IC_CLR_TX_OVER	0x4c
#تعریف_کرو DW_IC_CLR_RD_REQ	0x50
#تعریف_کرو DW_IC_CLR_TX_ABRT	0x54
#تعریف_کرو DW_IC_CLR_RX_مکمل	0x58
#تعریف_کرو DW_IC_CLR_ACTIVITY	0x5c
#تعریف_کرو DW_IC_CLR_STOP_DET	0x60
#تعریف_کرو DW_IC_CLR_آغاز_DET	0x64
#تعریف_کرو DW_IC_CLR_GEN_CALL	0x68
#تعریف_کرو DW_IC_ENABLE		0x6c
#تعریف_کرو DW_IC_حالت		0x70
#تعریف_کرو DW_IC_TXFLR		0x74
#تعریف_کرو DW_IC_RXFLR		0x78
#تعریف_کرو DW_IC_SDA_HOLD		0x7c
#تعریف_کرو DW_IC_TX_ABRT_ماخذ	0x80
#تعریف_کرو DW_IC_ENABLE_حالت	0x9c
#تعریف_کرو DW_IC_CLR_RESTART_DET	0xa8
#تعریف_کرو DW_IC_COMP_PARAM_1	0xf4
#تعریف_کرو DW_IC_COMP_نسخہ	0xf8

#تعریف_کرو DW_I2_C_حالت_TXFIFO_خالی_خانے	ثنائی_ہندسہ(2)
#تعریف_کرو DW_I2_C_حالت_RXFIFO_NOT_خالی_خانے	ثنائی_ہندسہ(3)

#تعریف_کرو IC_ڈیٹا_CMD_READ	ثنائی_ہندسہ(8)
#تعریف_کرو IC_ڈیٹا_CMD_STOP	ثنائی_ہندسہ(9)
#تعریف_کرو IC_ڈیٹا_CMD_RESTART	ثنائی_ہندسہ(10)
#تعریف_کرو IC_INT_حالت_STOPDET	ثنائی_ہندسہ(9)

ساکن اندرونی خالی dw_i2c_setreg(ساخت dw_i2c_adapter *adap,
				 u8 reg, u32 قدر)
{
	writel(قدر, (خالی *)adap->addr + reg);
}

ساکن اندرونی u32 dw_i2c_getreg(ساخت dw_i2c_adapter *adap,
				u32 reg)
{
	واپس readl((خالی *)adap->addr + reg);
}

ساکن صحیح dw_i2c_adapter_poll(ساخت dw_i2c_adapter *adap,
			       u32 نقاب, u32 addr,
			       bool inverted)
{
	غیرعلامتی صحیح timeout = 10; /* msec */
	صحیح گنتی = 0;
	u32 val;

	کرو {
		val = dw_i2c_getreg(adap, addr);
		اگر (inverted) {
			اگر (!(val & نقاب))
				واپس 0;
		} ورنہ {
			اگر (val & نقاب)
				واپس 0;
		}
		sbi_timer_udelay(2);
		گنتی += 1;
		اگر (گنتی == (timeout * 1000))
			واپس SBI_ETIMEDOUT;
	} جب (1);
}

#تعریف_کرو dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_حالت_RXFIFO_NOT_خالی_خانے, DW_IC_حالت, 0)
#تعریف_کرو dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_حالت_TXFIFO_خالی_خانے, DW_IC_حالت, 0)

ساکن صحیح dw_i2c_write_addr(ساخت dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	واپس 0;
}

ساکن صحیح dw_i2c_adapter_read(ساخت i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *بفر, صحیح len)
{
	ساخت dw_i2c_adapter *adap =
		container_کا(ia, ساخت dw_i2c_adapter, adapter);
	صحیح rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	اگر (rc)
		واپس rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_ڈیٹا_CMD, reg);

	/* set value */
	جب (len) {
		اگر (len == 1)
			dw_i2c_setreg(adap, DW_IC_ڈیٹا_CMD,
				      IC_ڈیٹا_CMD_READ | IC_ڈیٹا_CMD_STOP);
		ورنہ
			dw_i2c_setreg(adap, DW_IC_ڈیٹا_CMD, IC_ڈیٹا_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		اگر (rc)
			واپس rc;

		*بفر = dw_i2c_getreg(adap, DW_IC_ڈیٹا_CMD) & 0xff;
		بفر++;
		len--;
	}

	واپس 0;
}

ساکن صحیح dw_i2c_adapter_write(ساخت i2c_adapter *ia, u8 addr,
				u8 reg, u8 *بفر, صحیح len)
{
	ساخت dw_i2c_adapter *adap =
		container_کا(ia, ساخت dw_i2c_adapter, adapter);
	صحیح rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	اگر (rc)
		واپس rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_ڈیٹا_CMD, reg);

	جب (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		اگر (rc)
			واپس rc;

		اگر (len == 1)
			dw_i2c_setreg(adap, DW_IC_ڈیٹا_CMD, *بفر | IC_ڈیٹا_CMD_STOP);
		ورنہ
			dw_i2c_setreg(adap, DW_IC_ڈیٹا_CMD, *بفر);

		بفر++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	واپس rc;
}

صحیح dw_i2c_init(ساخت i2c_adapter *adapter, صحیح nodeoff)
{
	adapter->id = nodeoff;
	adapter->اكتب = dw_i2c_adapter_write;
	adapter->اقرأ = dw_i2c_adapter_read;

	واپس i2c_adapter_جوڑو(adapter);
}
