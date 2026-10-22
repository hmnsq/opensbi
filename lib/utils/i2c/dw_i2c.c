/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi_utils/i2c/dw_i2c.h>

#घोषणा DW_IC_CON		0x00
#घोषणा DW_IC_TAR		0x04
#घोषणा DW_IC_SAR		0x08
#घोषणा DW_IC_तथ्य_CMD		0x10
#घोषणा DW_IC_SS_SCL_HCNT	0x14
#घोषणा DW_IC_SS_SCL_LCNT	0x18
#घोषणा DW_IC_FS_SCL_HCNT	0x1c
#घोषणा DW_IC_FS_SCL_LCNT	0x20
#घोषणा DW_IC_HS_SCL_HCNT	0x24
#घोषणा DW_IC_HS_SCL_LCNT	0x28
#घोषणा DW_IC_INTR_STAT		0x2c
#घोषणा DW_IC_INTR_MASK		0x30
#घोषणा DW_IC_RAW_INTR_STAT	0x34
#घोषणा DW_IC_RX_TL		0x38
#घोषणा DW_IC_TX_TL		0x3c
#घोषणा DW_IC_CLR_INTR		0x40
#घोषणा DW_IC_CLR_RX_UNDER	0x44
#घोषणा DW_IC_CLR_RX_OVER	0x48
#घोषणा DW_IC_CLR_TX_OVER	0x4c
#घोषणा DW_IC_CLR_RD_REQ	0x50
#घोषणा DW_IC_CLR_TX_ABRT	0x54
#घोषणा DW_IC_CLR_RX_DONE	0x58
#घोषणा DW_IC_CLR_ACTIVITY	0x5c
#घोषणा DW_IC_CLR_STOP_DET	0x60
#घोषणा DW_IC_CLR_START_DET	0x64
#घोषणा DW_IC_CLR_GEN_CALL	0x68
#घोषणा DW_IC_ENABLE		0x6c
#घोषणा DW_IC_STATUS		0x70
#घोषणा DW_IC_TXFLR		0x74
#घोषणा DW_IC_RXFLR		0x78
#घोषणा DW_IC_SDA_HOLD		0x7c
#घोषणा DW_IC_TX_ABRT_SOURCE	0x80
#घोषणा DW_IC_ENABLE_STATUS	0x9c
#घोषणा DW_IC_CLR_RESTART_DET	0xa8
#घोषणा DW_IC_COMP_PARAM_1	0xf4
#घोषणा DW_IC_COMP_VERSION	0xf8

#घोषणा DW_I2C_STATUS_TXFIFO_EMPTY	BIT(2)
#घोषणा DW_I2C_STATUS_RXFIFO_NOT_EMPTY	BIT(3)

#घोषणा IC_तथ्य_CMD_READ	BIT(8)
#घोषणा IC_तथ्य_CMD_STOP	BIT(9)
#घोषणा IC_तथ्य_CMD_RESTART	BIT(10)
#घोषणा IC_INT_STATUS_STOPDET	BIT(9)

अटल अंतरभूत व्योम dw_i2c_setreg(काष्ठा dw_i2c_adapter *adap,
				 u8 reg, u32 मूल्य)
{
	writel(मूल्य, (व्योम *)adap->addr + reg);
}

अटल अंतरभूत u32 dw_i2c_getreg(काष्ठा dw_i2c_adapter *adap,
				u32 reg)
{
	वापस readl((व्योम *)adap->addr + reg);
}

अटल पूर्णांक dw_i2c_adapter_poll(काष्ठा dw_i2c_adapter *adap,
			       u32 mask, u32 addr,
			       bool inverted)
{
	अचिन्हित पूर्णांक timeout = 10; /* msec */
	पूर्णांक गिनती = 0;
	u32 val;

	करो {
		val = dw_i2c_getreg(adap, addr);
		अगर (inverted) {
			अगर (!(val & mask))
				वापस 0;
		} अन्यथा {
			अगर (val & mask)
				वापस 0;
		}
		sbi_timer_udelay(2);
		गिनती += 1;
		अगर (गिनती == (timeout * 1000))
			वापस SBI_ETIMEDOUT;
	} जबतक (1);
}

#घोषणा dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_RXFIFO_NOT_EMPTY, DW_IC_STATUS, 0)
#घोषणा dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2C_STATUS_TXFIFO_EMPTY, DW_IC_STATUS, 0)

अटल पूर्णांक dw_i2c_write_addr(काष्ठा dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	वापस 0;
}

अटल पूर्णांक dw_i2c_adapter_read(काष्ठा i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *बफर, पूर्णांक len)
{
	काष्ठा dw_i2c_adapter *adap =
		container_of(ia, काष्ठा dw_i2c_adapter, adapter);
	पूर्णांक rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	अगर (rc)
		वापस rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_तथ्य_CMD, reg);

	/* set value */
	जबतक (len) {
		अगर (len == 1)
			dw_i2c_setreg(adap, DW_IC_तथ्य_CMD,
				      IC_तथ्य_CMD_READ | IC_तथ्य_CMD_STOP);
		अन्यथा
			dw_i2c_setreg(adap, DW_IC_तथ्य_CMD, IC_तथ्य_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		अगर (rc)
			वापस rc;

		*बफर = dw_i2c_getreg(adap, DW_IC_तथ्य_CMD) & 0xff;
		बफर++;
		len--;
	}

	वापस 0;
}

अटल पूर्णांक dw_i2c_adapter_write(काष्ठा i2c_adapter *ia, u8 addr,
				u8 reg, u8 *बफर, पूर्णांक len)
{
	काष्ठा dw_i2c_adapter *adap =
		container_of(ia, काष्ठा dw_i2c_adapter, adapter);
	पूर्णांक rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	अगर (rc)
		वापस rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_तथ्य_CMD, reg);

	जबतक (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		अगर (rc)
			वापस rc;

		अगर (len == 1)
			dw_i2c_setreg(adap, DW_IC_तथ्य_CMD, *बफर | IC_तथ्य_CMD_STOP);
		अन्यथा
			dw_i2c_setreg(adap, DW_IC_तथ्य_CMD, *बफर);

		बफर++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	वापस rc;
}

पूर्णांक dw_i2c_init(काष्ठा i2c_adapter *adapter, पूर्णांक nodeoff)
{
	adapter->id = nodeoff;
	adapter->ग_लिखो = dw_i2c_adapter_write;
	adapter->पढ़ो = dw_i2c_adapter_read;

	वापस i2c_adapter_add(adapter);
}
