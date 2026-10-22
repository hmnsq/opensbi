/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi_utils/i2c/dw_i2c.h>

#تعریف_کن DW_IC_CON		0x00
#تعریف_کن DW_IC_TAR		0x04
#تعریف_کن DW_IC_SAR		0x08
#تعریف_کن DW_IC_داده_CMD		0x10
#تعریف_کن DW_IC_SS_SCL_HCNT	0x14
#تعریف_کن DW_IC_SS_SCL_LCNT	0x18
#تعریف_کن DW_IC_FS_SCL_HCNT	0x1c
#تعریف_کن DW_IC_FS_SCL_LCNT	0x20
#تعریف_کن DW_IC_HS_SCL_HCNT	0x24
#تعریف_کن DW_IC_HS_SCL_LCNT	0x28
#تعریف_کن DW_IC_INTR_STAT		0x2c
#تعریف_کن DW_IC_INTR_ماسک		0x30
#تعریف_کن DW_IC_RAW_INTR_STAT	0x34
#تعریف_کن DW_IC_RX_TL		0x38
#تعریف_کن DW_IC_TX_TL		0x3c
#تعریف_کن DW_IC_CLR_INTR		0x40
#تعریف_کن DW_IC_CLR_RX_UNDER	0x44
#تعریف_کن DW_IC_CLR_RX_OVER	0x48
#تعریف_کن DW_IC_CLR_TX_OVER	0x4c
#تعریف_کن DW_IC_CLR_RD_REQ	0x50
#تعریف_کن DW_IC_CLR_TX_ABRT	0x54
#تعریف_کن DW_IC_CLR_RX_انجامشده	0x58
#تعریف_کن DW_IC_CLR_ACTIVITY	0x5c
#تعریف_کن DW_IC_CLR_STOP_DET	0x60
#تعریف_کن DW_IC_CLR_شروع_DET	0x64
#تعریف_کن DW_IC_CLR_GEN_CALL	0x68
#تعریف_کن DW_IC_ENABLE		0x6c
#تعریف_کن DW_IC_وضعیت		0x70
#تعریف_کن DW_IC_TXFLR		0x74
#تعریف_کن DW_IC_RXFLR		0x78
#تعریف_کن DW_IC_SDA_HOLD		0x7c
#تعریف_کن DW_IC_TX_ABRT_منبع	0x80
#تعریف_کن DW_IC_ENABLE_وضعیت	0x9c
#تعریف_کن DW_IC_CLR_RESTART_DET	0xa8
#تعریف_کن DW_IC_COMP_PARAM_1	0xf4
#تعریف_کن DW_IC_COMP_نسخه	0xf8

#تعریف_کن DW_I2_C_وضعیت_TXFIFO_خالی	بیت(2)
#تعریف_کن DW_I2_C_وضعیت_RXFIFO_NOT_خالی	بیت(3)

#تعریف_کن IC_داده_CMD_READ	بیت(8)
#تعریف_کن IC_داده_CMD_STOP	بیت(9)
#تعریف_کن IC_داده_CMD_RESTART	بیت(10)
#تعریف_کن IC_INT_وضعیت_STOPDET	بیت(9)

ساكن مضمن فراغ dw_i2c_setreg(بنية dw_i2c_adapter *adap,
				 u8 reg, u32 مقدار)
{
	writel(مقدار, (فراغ *)adap->addr + reg);
}

ساكن مضمن u32 dw_i2c_getreg(بنية dw_i2c_adapter *adap,
				u32 reg)
{
	أرجع readl((فراغ *)adap->addr + reg);
}

ساكن صحيح dw_i2c_adapter_poll(بنية dw_i2c_adapter *adap,
			       u32 ماسک, u32 addr,
			       bool inverted)
{
	بیعلامت صحيح timeout = 10; /* msec */
	صحيح شمار = 0;
	u32 val;

	افعل {
		val = dw_i2c_getreg(adap, addr);
		إذا (inverted) {
			إذا (!(val & ماسک))
				أرجع 0;
		} وإلا {
			إذا (val & ماسک)
				أرجع 0;
		}
		sbi_timer_udelay(2);
		شمار += 1;
		إذا (شمار == (timeout * 1000))
			أرجع SBI_ETIMEDOUT;
	} طالما (1);
}

#تعریف_کن dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_وضعیت_RXFIFO_NOT_خالی, DW_IC_وضعیت, 0)
#تعریف_کن dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_وضعیت_TXFIFO_خالی, DW_IC_وضعیت, 0)

ساكن صحيح dw_i2c_write_addr(بنية dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	أرجع 0;
}

ساكن صحيح dw_i2c_adapter_read(بنية i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *میانگیر, صحيح len)
{
	بنية dw_i2c_adapter *adap =
		container_از(ia, بنية dw_i2c_adapter, adapter);
	صحيح rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	إذا (rc)
		أرجع rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_داده_CMD, reg);

	/* set value */
	طالما (len) {
		إذا (len == 1)
			dw_i2c_setreg(adap, DW_IC_داده_CMD,
				      IC_داده_CMD_READ | IC_داده_CMD_STOP);
		وإلا
			dw_i2c_setreg(adap, DW_IC_داده_CMD, IC_داده_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		إذا (rc)
			أرجع rc;

		*میانگیر = dw_i2c_getreg(adap, DW_IC_داده_CMD) & 0xff;
		میانگیر++;
		len--;
	}

	أرجع 0;
}

ساكن صحيح dw_i2c_adapter_write(بنية i2c_adapter *ia, u8 addr,
				u8 reg, u8 *میانگیر, صحيح len)
{
	بنية dw_i2c_adapter *adap =
		container_از(ia, بنية dw_i2c_adapter, adapter);
	صحيح rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	إذا (rc)
		أرجع rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_داده_CMD, reg);

	طالما (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		إذا (rc)
			أرجع rc;

		إذا (len == 1)
			dw_i2c_setreg(adap, DW_IC_داده_CMD, *میانگیر | IC_داده_CMD_STOP);
		وإلا
			dw_i2c_setreg(adap, DW_IC_داده_CMD, *میانگیر);

		میانگیر++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	أرجع rc;
}

صحيح dw_i2c_init(بنية i2c_adapter *adapter, صحيح nodeoff)
{
	adapter->id = nodeoff;
	adapter->اكتب = dw_i2c_adapter_write;
	adapter->اقرأ = dw_i2c_adapter_read;

	أرجع i2c_adapter_جمع_کن(adapter);
}
