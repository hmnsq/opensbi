/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi_utils/i2c/dw_i2c.h>

#నిర్వచించు DW_IC_CON		0x00
#నిర్వచించు DW_IC_TAR		0x04
#నిర్వచించు DW_IC_SAR		0x08
#నిర్వచించు DW_IC_దత్తాంశం_CMD		0x10
#నిర్వచించు DW_IC_SS_SCL_HCNT	0x14
#నిర్వచించు DW_IC_SS_SCL_LCNT	0x18
#నిర్వచించు DW_IC_FS_SCL_HCNT	0x1c
#నిర్వచించు DW_IC_FS_SCL_LCNT	0x20
#నిర్వచించు DW_IC_HS_SCL_HCNT	0x24
#నిర్వచించు DW_IC_HS_SCL_LCNT	0x28
#నిర్వచించు DW_IC_INTR_STAT		0x2c
#నిర్వచించు DW_IC_INTR_ముసుగు		0x30
#నిర్వచించు DW_IC_RAW_INTR_STAT	0x34
#నిర్వచించు DW_IC_RX_TL		0x38
#నిర్వచించు DW_IC_TX_TL		0x3c
#నిర్వచించు DW_IC_CLR_INTR		0x40
#నిర్వచించు DW_IC_CLR_RX_UNDER	0x44
#నిర్వచించు DW_IC_CLR_RX_OVER	0x48
#నిర్వచించు DW_IC_CLR_TX_OVER	0x4c
#నిర్వచించు DW_IC_CLR_RD_REQ	0x50
#నిర్వచించు DW_IC_CLR_TX_ABRT	0x54
#నిర్వచించు DW_IC_CLR_RX_పూర్తి	0x58
#నిర్వచించు DW_IC_CLR_ACTIVITY	0x5c
#నిర్వచించు DW_IC_CLR_STOP_DET	0x60
#నిర్వచించు DW_IC_CLR_ప్రారంభం_DET	0x64
#నిర్వచించు DW_IC_CLR_GEN_CALL	0x68
#నిర్వచించు DW_IC_ENABLE		0x6c
#నిర్వచించు DW_IC_స్థితి		0x70
#నిర్వచించు DW_IC_TXFLR		0x74
#నిర్వచించు DW_IC_RXFLR		0x78
#నిర్వచించు DW_IC_SDA_HOLD		0x7c
#నిర్వచించు DW_IC_TX_ABRT_ఆధారం	0x80
#నిర్వచించు DW_IC_ENABLE_స్థితి	0x9c
#నిర్వచించు DW_IC_CLR_RESTART_DET	0xa8
#నిర్వచించు DW_IC_COMP_PARAM_1	0xf4
#నిర్వచించు DW_IC_COMP_సంస్కరణ	0xf8

#నిర్వచించు DW_I2_C_స్థితి_TXFIFO_ఖాళీలు	బిట్టు(2)
#నిర్వచించు DW_I2_C_స్థితి_RXFIFO_NOT_ఖాళీలు	బిట్టు(3)

#నిర్వచించు IC_దత్తాంశం_CMD_READ	బిట్టు(8)
#నిర్వచించు IC_దత్తాంశం_CMD_STOP	బిట్టు(9)
#నిర్వచించు IC_దత్తాంశం_CMD_RESTART	బిట్టు(10)
#నిర్వచించు IC_INT_స్థితి_STOPDET	బిట్టు(9)

స్థిర అంతఃవరుస శూన్య dw_i2c_setreg(నిర్మాణం dw_i2c_adapter *adap,
				 u8 reg, u32 విలువ)
{
	writel(విలువ, (శూన్య *)adap->addr + reg);
}

స్థిర అంతఃవరుస u32 dw_i2c_getreg(నిర్మాణం dw_i2c_adapter *adap,
				u32 reg)
{
	తిరిగి readl((శూన్య *)adap->addr + reg);
}

స్థిర పూర్ణాంకం dw_i2c_adapter_poll(నిర్మాణం dw_i2c_adapter *adap,
			       u32 ముసుగు, u32 addr,
			       bool inverted)
{
	గుర్తులేని పూర్ణాంకం timeout = 10; /* msec */
	పూర్ణాంకం లెక్క = 0;
	u32 val;

	చేయి {
		val = dw_i2c_getreg(adap, addr);
		ఒకవేళ (inverted) {
			ఒకవేళ (!(val & ముసుగు))
				తిరిగి 0;
		} లేకపోతే {
			ఒకవేళ (val & ముసుగు)
				తిరిగి 0;
		}
		sbi_timer_udelay(2);
		లెక్క += 1;
		ఒకవేళ (లెక్క == (timeout * 1000))
			తిరిగి SBI_ETIMEDOUT;
	} అయితే (1);
}

#నిర్వచించు dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_స్థితి_RXFIFO_NOT_ఖాళీలు, DW_IC_స్థితి, 0)
#నిర్వచించు dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_స్థితి_TXFIFO_ఖాళీలు, DW_IC_స్థితి, 0)

స్థిర పూర్ణాంకం dw_i2c_write_addr(నిర్మాణం dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం dw_i2c_adapter_read(నిర్మాణం i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *బఫరు, పూర్ణాంకం len)
{
	నిర్మాణం dw_i2c_adapter *adap =
		container_యొక్క(ia, నిర్మాణం dw_i2c_adapter, adapter);
	పూర్ణాంకం rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_దత్తాంశం_CMD, reg);

	/* set value */
	అయితే (len) {
		ఒకవేళ (len == 1)
			dw_i2c_setreg(adap, DW_IC_దత్తాంశం_CMD,
				      IC_దత్తాంశం_CMD_READ | IC_దత్తాంశం_CMD_STOP);
		లేకపోతే
			dw_i2c_setreg(adap, DW_IC_దత్తాంశం_CMD, IC_దత్తాంశం_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		ఒకవేళ (rc)
			తిరిగి rc;

		*బఫరు = dw_i2c_getreg(adap, DW_IC_దత్తాంశం_CMD) & 0xff;
		బఫరు++;
		len--;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం dw_i2c_adapter_write(నిర్మాణం i2c_adapter *ia, u8 addr,
				u8 reg, u8 *బఫరు, పూర్ణాంకం len)
{
	నిర్మాణం dw_i2c_adapter *adap =
		container_యొక్క(ia, నిర్మాణం dw_i2c_adapter, adapter);
	పూర్ణాంకం rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_దత్తాంశం_CMD, reg);

	అయితే (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		ఒకవేళ (rc)
			తిరిగి rc;

		ఒకవేళ (len == 1)
			dw_i2c_setreg(adap, DW_IC_దత్తాంశం_CMD, *బఫరు | IC_దత్తాంశం_CMD_STOP);
		లేకపోతే
			dw_i2c_setreg(adap, DW_IC_దత్తాంశం_CMD, *బఫరు);

		బఫరు++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	తిరిగి rc;
}

పూర్ణాంకం dw_i2c_init(నిర్మాణం i2c_adapter *adapter, పూర్ణాంకం nodeoff)
{
	adapter->id = nodeoff;
	adapter->లిఖ = dw_i2c_adapter_write;
	adapter->పఠ = dw_i2c_adapter_read;

	తిరిగి i2c_adapter_కూడు(adapter);
}
