/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi_utils/i2c/dw_i2c.h>

#परिभाषय DW_IC_CON		0x00
#परिभाषय DW_IC_TAR		0x04
#परिभाषय DW_IC_SAR		0x08
#परिभाषय DW_IC_दत्त_CMD		0x10
#परिभाषय DW_IC_SS_SCL_HCNT	0x14
#परिभाषय DW_IC_SS_SCL_LCNT	0x18
#परिभाषय DW_IC_FS_SCL_HCNT	0x1c
#परिभाषय DW_IC_FS_SCL_LCNT	0x20
#परिभाषय DW_IC_HS_SCL_HCNT	0x24
#परिभाषय DW_IC_HS_SCL_LCNT	0x28
#परिभाषय DW_IC_INTR_STAT		0x2c
#परिभाषय DW_IC_INTR_आवरणम्		0x30
#परिभाषय DW_IC_RAW_INTR_STAT	0x34
#परिभाषय DW_IC_RX_TL		0x38
#परिभाषय DW_IC_TX_TL		0x3c
#परिभाषय DW_IC_CLR_INTR		0x40
#परिभाषय DW_IC_CLR_RX_UNDER	0x44
#परिभाषय DW_IC_CLR_RX_OVER	0x48
#परिभाषय DW_IC_CLR_TX_OVER	0x4c
#परिभाषय DW_IC_CLR_RD_REQ	0x50
#परिभाषय DW_IC_CLR_TX_ABRT	0x54
#परिभाषय DW_IC_CLR_RX_समाप्तम्	0x58
#परिभाषय DW_IC_CLR_ACTIVITY	0x5c
#परिभाषय DW_IC_CLR_STOP_DET	0x60
#परिभाषय DW_IC_CLR_START_DET	0x64
#परिभाषय DW_IC_CLR_GEN_CALL	0x68
#परिभाषय DW_IC_ENABLE		0x6c
#परिभाषय DW_IC_स्थितिः		0x70
#परिभाषय DW_IC_TXFLR		0x74
#परिभाषय DW_IC_RXFLR		0x78
#परिभाषय DW_IC_SDA_HOLD		0x7c
#परिभाषय DW_IC_TX_ABRT_SOURCE	0x80
#परिभाषय DW_IC_ENABLE_स्थितिः	0x9c
#परिभाषय DW_IC_CLR_RESTART_DET	0xa8
#परिभाषय DW_IC_COMP_PARAM_1	0xf4
#परिभाषय DW_IC_COMP_संस्करणम्	0xf8

#परिभाषय DW_I2_C_स्थितिः_TXFIFO_रिक्तानि	BIT(2)
#परिभाषय DW_I2_C_स्थितिः_RXFIFO_NOT_रिक्तानि	BIT(3)

#परिभाषय IC_दत्त_CMD_READ	BIT(8)
#परिभाषय IC_दत्त_CMD_STOP	BIT(9)
#परिभाषय IC_दत्त_CMD_RESTART	BIT(10)
#परिभाषय IC_INT_स्थितिः_STOPDET	BIT(9)

स्थिर अन्तःस्थ शून्य dw_i2c_setreg(संरचना dw_i2c_adapter *adap,
				 u8 reg, u32 मानम्)
{
	writel(मानम्, (शून्य *)adap->addr + reg);
}

स्थिर अन्तःस्थ u32 dw_i2c_getreg(संरचना dw_i2c_adapter *adap,
				u32 reg)
{
	प्रत्यागम readl((शून्य *)adap->addr + reg);
}

स्थिर पूर्णाङ्क dw_i2c_adapter_poll(संरचना dw_i2c_adapter *adap,
			       u32 आवरणम्, u32 addr,
			       bool inverted)
{
	अचिह्नित पूर्णाङ्क timeout = 10; /* msec */
	पूर्णाङ्क गणनम् = 0;
	u32 val;

	कुरु {
		val = dw_i2c_getreg(adap, addr);
		यदि (inverted) {
			यदि (!(val & आवरणम्))
				प्रत्यागम 0;
		} अन्यथा {
			यदि (val & आवरणम्)
				प्रत्यागम 0;
		}
		sbi_timer_udelay(2);
		गणनम् += 1;
		यदि (गणनम् == (timeout * 1000))
			प्रत्यागम SBI_ETIMEDOUT;
	} यावत् (1);
}

#परिभाषय dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_स्थितिः_RXFIFO_NOT_रिक्तानि, DW_IC_स्थितिः, 0)
#परिभाषय dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_स्थितिः_TXFIFO_रिक्तानि, DW_IC_स्थितिः, 0)

स्थिर पूर्णाङ्क dw_i2c_write_addr(संरचना dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क dw_i2c_adapter_read(संरचना i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *बफर, पूर्णाङ्क len)
{
	संरचना dw_i2c_adapter *adap =
		container_सम्बन्धि(ia, संरचना dw_i2c_adapter, adapter);
	पूर्णाङ्क rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	यदि (rc)
		प्रत्यागम rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_दत्त_CMD, reg);

	/* set value */
	यावत् (len) {
		यदि (len == 1)
			dw_i2c_setreg(adap, DW_IC_दत्त_CMD,
				      IC_दत्त_CMD_READ | IC_दत्त_CMD_STOP);
		अन्यथा
			dw_i2c_setreg(adap, DW_IC_दत्त_CMD, IC_दत्त_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		यदि (rc)
			प्रत्यागम rc;

		*बफर = dw_i2c_getreg(adap, DW_IC_दत्त_CMD) & 0xff;
		बफर++;
		len--;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क dw_i2c_adapter_write(संरचना i2c_adapter *ia, u8 addr,
				u8 reg, u8 *बफर, पूर्णाङ्क len)
{
	संरचना dw_i2c_adapter *adap =
		container_सम्बन्धि(ia, संरचना dw_i2c_adapter, adapter);
	पूर्णाङ्क rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	यदि (rc)
		प्रत्यागम rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_दत्त_CMD, reg);

	यावत् (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		यदि (rc)
			प्रत्यागम rc;

		यदि (len == 1)
			dw_i2c_setreg(adap, DW_IC_दत्त_CMD, *बफर | IC_दत्त_CMD_STOP);
		अन्यथा
			dw_i2c_setreg(adap, DW_IC_दत्त_CMD, *बफर);

		बफर++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	प्रत्यागम rc;
}

पूर्णाङ्क dw_i2c_init(संरचना i2c_adapter *adapter, पूर्णाङ्क nodeoff)
{
	adapter->id = nodeoff;
	adapter->लिख = dw_i2c_adapter_write;
	adapter->पठ = dw_i2c_adapter_read;

	प्रत्यागम i2c_adapter_सङ्कलय(adapter);
}
