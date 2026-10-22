/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/dw_i2c.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CON		0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_TAR		0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_SAR		0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_ਅੰਕੜੇ_CMD		0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_SS_SCL_HCNT	0x14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_SS_SCL_LCNT	0x18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_FS_SCL_HCNT	0x1c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_FS_SCL_LCNT	0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_HS_SCL_HCNT	0x24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_HS_SCL_LCNT	0x28
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_INTR_STAT		0x2c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_INTR_ਨਕਾਬ		0x30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_RAW_INTR_STAT	0x34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_RX_TL		0x38
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_TX_TL		0x3c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_INTR		0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_RX_UNDER	0x44
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_RX_OVER	0x48
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_TX_OVER	0x4c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_RD_REQ	0x50
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_TX_ABRT	0x54
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_RX_ਪੂਰਾ	0x58
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_ACTIVITY	0x5c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_STOP_DET	0x60
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_ਸ਼ੁਰੂ_DET	0x64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_GEN_CALL	0x68
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_ENABLE		0x6c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_ਸਥਿਤੀ		0x70
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_TXFLR		0x74
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_RXFLR		0x78
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_SDA_HOLD		0x7c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_TX_ABRT_ਸਰੋਤ	0x80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_ENABLE_ਸਥਿਤੀ	0x9c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_CLR_RESTART_DET	0xa8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_COMP_PARAM_1	0xf4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_IC_COMP_ਸੰਸਕਰਣ	0xf8

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_I2_C_ਸਥਿਤੀ_TXFIFO_ਖਾਲੀ_ਥਾਵਾਂ	ਦੋਆਧਾਰੀ_ਅੰਕ(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DW_I2_C_ਸਥਿਤੀ_RXFIFO_NOT_ਖਾਲੀ_ਥਾਵਾਂ	ਦੋਆਧਾਰੀ_ਅੰਕ(3)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IC_ਅੰਕੜੇ_CMD_READ	ਦੋਆਧਾਰੀ_ਅੰਕ(8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IC_ਅੰਕੜੇ_CMD_STOP	ਦੋਆਧਾਰੀ_ਅੰਕ(9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IC_ਅੰਕੜੇ_CMD_RESTART	ਦੋਆਧਾਰੀ_ਅੰਕ(10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IC_INT_ਸਥਿਤੀ_STOPDET	ਦੋਆਧਾਰੀ_ਅੰਕ(9)

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ dw_i2c_setreg(ਬਣਤਰ dw_i2c_adapter *adap,
				 u8 reg, u32 ਮੁੱਲ)
{
	writel(ਮੁੱਲ, (ਖਾਲੀ *)adap->addr + reg);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ u32 dw_i2c_getreg(ਬਣਤਰ dw_i2c_adapter *adap,
				u32 reg)
{
	ਵਾਪਸ readl((ਖਾਲੀ *)adap->addr + reg);
}

ਸਥਿਰ ਪੂਰਨ dw_i2c_adapter_poll(ਬਣਤਰ dw_i2c_adapter *adap,
			       u32 ਨਕਾਬ, u32 addr,
			       bool inverted)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ timeout = 10; /* msec */
	ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
	u32 val;

	ਕਰੋ {
		val = dw_i2c_getreg(adap, addr);
		ਜੇ (inverted) {
			ਜੇ (!(val & ਨਕਾਬ))
				ਵਾਪਸ 0;
		} ਨਹੀਂਤਾਂ {
			ਜੇ (val & ਨਕਾਬ)
				ਵਾਪਸ 0;
		}
		sbi_timer_udelay(2);
		ਗਿਣਤੀ_2 += 1;
		ਜੇ (ਗਿਣਤੀ_2 == (timeout * 1000))
			ਵਾਪਸ SBI_ETIMEDOUT;
	} ਜਦੋਂ (1);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_ਸਥਿਤੀ_RXFIFO_NOT_ਖਾਲੀ_ਥਾਵਾਂ, DW_IC_ਸਥਿਤੀ, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_ਸਥਿਤੀ_TXFIFO_ਖਾਲੀ_ਥਾਵਾਂ, DW_IC_ਸਥਿਤੀ, 0)

ਸਥਿਰ ਪੂਰਨ dw_i2c_write_addr(ਬਣਤਰ dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ dw_i2c_adapter_read(ਬਣਤਰ i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *ਬਫਰ, ਪੂਰਨ len)
{
	ਬਣਤਰ dw_i2c_adapter *adap =
		container_ਦਾ(ia, ਬਣਤਰ dw_i2c_adapter, adapter);
	ਪੂਰਨ rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_ਅੰਕੜੇ_CMD, reg);

	/* set value */
	ਜਦੋਂ (len) {
		ਜੇ (len == 1)
			dw_i2c_setreg(adap, DW_IC_ਅੰਕੜੇ_CMD,
				      IC_ਅੰਕੜੇ_CMD_READ | IC_ਅੰਕੜੇ_CMD_STOP);
		ਨਹੀਂਤਾਂ
			dw_i2c_setreg(adap, DW_IC_ਅੰਕੜੇ_CMD, IC_ਅੰਕੜੇ_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		*ਬਫਰ = dw_i2c_getreg(adap, DW_IC_ਅੰਕੜੇ_CMD) & 0xff;
		ਬਫਰ++;
		len--;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ dw_i2c_adapter_write(ਬਣਤਰ i2c_adapter *ia, u8 addr,
				u8 reg, u8 *ਬਫਰ, ਪੂਰਨ len)
{
	ਬਣਤਰ dw_i2c_adapter *adap =
		container_ਦਾ(ia, ਬਣਤਰ dw_i2c_adapter, adapter);
	ਪੂਰਨ rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_ਅੰਕੜੇ_CMD, reg);

	ਜਦੋਂ (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		ਜੇ (len == 1)
			dw_i2c_setreg(adap, DW_IC_ਅੰਕੜੇ_CMD, *ਬਫਰ | IC_ਅੰਕੜੇ_CMD_STOP);
		ਨਹੀਂਤਾਂ
			dw_i2c_setreg(adap, DW_IC_ਅੰਕੜੇ_CMD, *ਬਫਰ);

		ਬਫਰ++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	ਵਾਪਸ rc;
}

ਪੂਰਨ dw_i2c_init(ਬਣਤਰ i2c_adapter *adapter, ਪੂਰਨ nodeoff)
{
	adapter->id = nodeoff;
	adapter->ਲਿਖ = dw_i2c_adapter_write;
	adapter->ਪਠ = dw_i2c_adapter_read;

	ਵਾਪਸ i2c_adapter_ਜੋੜੋ(adapter);
}
