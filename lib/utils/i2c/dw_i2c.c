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

#وصف_ڪريو DW_IC_CON		0x00
#وصف_ڪريو DW_IC_TAR		0x04
#وصف_ڪريو DW_IC_SAR		0x08
#وصف_ڪريو DW_IC_معلومات_CMD		0x10
#وصف_ڪريو DW_IC_SS_SCL_HCNT	0x14
#وصف_ڪريو DW_IC_SS_SCL_LCNT	0x18
#وصف_ڪريو DW_IC_FS_SCL_HCNT	0x1c
#وصف_ڪريو DW_IC_FS_SCL_LCNT	0x20
#وصف_ڪريو DW_IC_HS_SCL_HCNT	0x24
#وصف_ڪريو DW_IC_HS_SCL_LCNT	0x28
#وصف_ڪريو DW_IC_INTR_STAT		0x2c
#وصف_ڪريو DW_IC_INTR_نقاب		0x30
#وصف_ڪريو DW_IC_RAW_INTR_STAT	0x34
#وصف_ڪريو DW_IC_RX_TL		0x38
#وصف_ڪريو DW_IC_TX_TL		0x3c
#وصف_ڪريو DW_IC_CLR_INTR		0x40
#وصف_ڪريو DW_IC_CLR_RX_UNDER	0x44
#وصف_ڪريو DW_IC_CLR_RX_OVER	0x48
#وصف_ڪريو DW_IC_CLR_TX_OVER	0x4c
#وصف_ڪريو DW_IC_CLR_RD_REQ	0x50
#وصف_ڪريو DW_IC_CLR_TX_ABRT	0x54
#وصف_ڪريو DW_IC_CLR_RX_پورو	0x58
#وصف_ڪريو DW_IC_CLR_ACTIVITY	0x5c
#وصف_ڪريو DW_IC_CLR_STOP_DET	0x60
#وصف_ڪريو DW_IC_CLR_شروعات_DET	0x64
#وصف_ڪريو DW_IC_CLR_GEN_CALL	0x68
#وصف_ڪريو DW_IC_ENABLE		0x6c
#وصف_ڪريو DW_IC_حالت		0x70
#وصف_ڪريو DW_IC_TXFLR		0x74
#وصف_ڪريو DW_IC_RXFLR		0x78
#وصف_ڪريو DW_IC_SDA_HOLD		0x7c
#وصف_ڪريو DW_IC_TX_ABRT_ذريعو	0x80
#وصف_ڪريو DW_IC_ENABLE_حالت	0x9c
#وصف_ڪريو DW_IC_CLR_RESTART_DET	0xa8
#وصف_ڪريو DW_IC_COMP_PARAM_1	0xf4
#وصف_ڪريو DW_IC_COMP_نسخو	0xf8

#وصف_ڪريو DW_I2_C_حالت_TXFIFO_خالي_جايون	ٻٽو_انگ(2)
#وصف_ڪريو DW_I2_C_حالت_RXFIFO_NOT_خالي_جايون	ٻٽو_انگ(3)

#وصف_ڪريو IC_معلومات_CMD_READ	ٻٽو_انگ(8)
#وصف_ڪريو IC_معلومات_CMD_STOP	ٻٽو_انگ(9)
#وصف_ڪريو IC_معلومات_CMD_RESTART	ٻٽو_انگ(10)
#وصف_ڪريو IC_INT_حالت_STOPDET	ٻٽو_انگ(9)

ساكن مضمن فراغ dw_i2c_setreg(بنية dw_i2c_adapter *adap,
				 u8 reg, u32 قدر)
{
	writel(قدر, (فراغ *)adap->addr + reg);
}

ساكن مضمن u32 dw_i2c_getreg(بنية dw_i2c_adapter *adap,
				u32 reg)
{
	أرجع readl((فراغ *)adap->addr + reg);
}

ساكن صحيح dw_i2c_adapter_poll(بنية dw_i2c_adapter *adap,
			       u32 نقاب, u32 addr,
			       bool inverted)
{
	بنانشان صحيح timeout = 10; /* msec */
	صحيح ڳڻپ = 0;
	u32 val;

	افعل {
		val = dw_i2c_getreg(adap, addr);
		إذا (inverted) {
			إذا (!(val & نقاب))
				أرجع 0;
		} وإلا {
			إذا (val & نقاب)
				أرجع 0;
		}
		sbi_timer_udelay(2);
		ڳڻپ += 1;
		إذا (ڳڻپ == (timeout * 1000))
			أرجع SBI_ETIMEDOUT;
	} طالما (1);
}

#وصف_ڪريو dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_حالت_RXFIFO_NOT_خالي_جايون, DW_IC_حالت, 0)
#وصف_ڪريو dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_حالت_TXFIFO_خالي_جايون, DW_IC_حالت, 0)

ساكن صحيح dw_i2c_write_addr(بنية dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	أرجع 0;
}

ساكن صحيح dw_i2c_adapter_read(بنية i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *بفر, صحيح len)
{
	بنية dw_i2c_adapter *adap =
		container_جو(ia, بنية dw_i2c_adapter, adapter);
	صحيح rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	إذا (rc)
		أرجع rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_معلومات_CMD, reg);

	/* set value */
	طالما (len) {
		إذا (len == 1)
			dw_i2c_setreg(adap, DW_IC_معلومات_CMD,
				      IC_معلومات_CMD_READ | IC_معلومات_CMD_STOP);
		وإلا
			dw_i2c_setreg(adap, DW_IC_معلومات_CMD, IC_معلومات_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		إذا (rc)
			أرجع rc;

		*بفر = dw_i2c_getreg(adap, DW_IC_معلومات_CMD) & 0xff;
		بفر++;
		len--;
	}

	أرجع 0;
}

ساكن صحيح dw_i2c_adapter_write(بنية i2c_adapter *ia, u8 addr,
				u8 reg, u8 *بفر, صحيح len)
{
	بنية dw_i2c_adapter *adap =
		container_جو(ia, بنية dw_i2c_adapter, adapter);
	صحيح rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	إذا (rc)
		أرجع rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_معلومات_CMD, reg);

	طالما (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		إذا (rc)
			أرجع rc;

		إذا (len == 1)
			dw_i2c_setreg(adap, DW_IC_معلومات_CMD, *بفر | IC_معلومات_CMD_STOP);
		وإلا
			dw_i2c_setreg(adap, DW_IC_معلومات_CMD, *بفر);

		بفر++;
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

	أرجع i2c_adapter_جوڙيو(adapter);
}
