/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 starfivetech.com
 *
 * Authors:
 *   Minda Chen <minda.chen@starfivetech.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/dw_i2c.h>

#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CON		0x00
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_TAR		0x04
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_SAR		0x08
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_ದತ್ತಾಂಶ_CMD		0x10
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_SS_SCL_HCNT	0x14
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_SS_SCL_LCNT	0x18
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_FS_SCL_HCNT	0x1c
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_FS_SCL_LCNT	0x20
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_HS_SCL_HCNT	0x24
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_HS_SCL_LCNT	0x28
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_INTR_STAT		0x2c
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_INTR_ಮುಖವಾಡ		0x30
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_RAW_INTR_STAT	0x34
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_RX_TL		0x38
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_TX_TL		0x3c
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_INTR		0x40
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_RX_UNDER	0x44
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_RX_OVER	0x48
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_TX_OVER	0x4c
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_RD_REQ	0x50
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_TX_ABRT	0x54
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_RX_ಮುಗಿದ	0x58
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_ACTIVITY	0x5c
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_STOP_DET	0x60
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_ಆರಂಭ_DET	0x64
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_GEN_CALL	0x68
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_ENABLE		0x6c
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_ಸ್ಥಿತಿ		0x70
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_TXFLR		0x74
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_RXFLR		0x78
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_SDA_HOLD		0x7c
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_TX_ABRT_ಮೂಲ	0x80
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_ENABLE_ಸ್ಥಿತಿ	0x9c
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_CLR_RESTART_DET	0xa8
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_COMP_PARAM_1	0xf4
#ವ್ಯಾಖ್ಯಾನಿಸು DW_IC_COMP_ಆವೃತ್ತಿ	0xf8

#ವ್ಯಾಖ್ಯಾನಿಸು DW_I2_C_ಸ್ಥಿತಿ_TXFIFO_ಖಾಲಿಗಳು	ಬಿಟ್(2)
#ವ್ಯಾಖ್ಯಾನಿಸು DW_I2_C_ಸ್ಥಿತಿ_RXFIFO_NOT_ಖಾಲಿಗಳು	ಬಿಟ್(3)

#ವ್ಯಾಖ್ಯಾನಿಸು IC_ದತ್ತಾಂಶ_CMD_READ	ಬಿಟ್(8)
#ವ್ಯಾಖ್ಯಾನಿಸು IC_ದತ್ತಾಂಶ_CMD_STOP	ಬಿಟ್(9)
#ವ್ಯಾಖ್ಯಾನಿಸು IC_ದತ್ತಾಂಶ_CMD_RESTART	ಬಿಟ್(10)
#ವ್ಯಾಖ್ಯಾನಿಸು IC_INT_ಸ್ಥಿತಿ_STOPDET	ಬಿಟ್(9)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ dw_i2c_setreg(ರಚನೆ dw_i2c_adapter *adap,
				 u8 reg, u32 ಮೌಲ್ಯ)
{
	writel(ಮೌಲ್ಯ, (ಶೂನ್ಯ *)adap->addr + reg);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32 dw_i2c_getreg(ರಚನೆ dw_i2c_adapter *adap,
				u32 reg)
{
	ಹಿಂತಿರುಗಿ readl((ಶೂನ್ಯ *)adap->addr + reg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dw_i2c_adapter_poll(ರಚನೆ dw_i2c_adapter *adap,
			       u32 ಮುಖವಾಡ, u32 addr,
			       bool inverted)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ timeout = 10; /* msec */
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
	u32 val;

	ಮಾಡು {
		val = dw_i2c_getreg(adap, addr);
		ಆದರೆ (inverted) {
			ಆದರೆ (!(val & ಮುಖವಾಡ))
				ಹಿಂತಿರುಗಿ 0;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (val & ಮುಖವಾಡ)
				ಹಿಂತಿರುಗಿ 0;
		}
		sbi_timer_udelay(2);
		ಎಣಿಕೆ_2 += 1;
		ಆದರೆ (ಎಣಿಕೆ_2 == (timeout * 1000))
			ಹಿಂತಿರುಗಿ SBI_ETIMEDOUT;
	} ಆಗ (1);
}

#ವ್ಯಾಖ್ಯಾನಿಸು dw_i2c_adapter_poll_rxrdy(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_ಸ್ಥಿತಿ_RXFIFO_NOT_ಖಾಲಿಗಳು, DW_IC_ಸ್ಥಿತಿ, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು dw_i2c_adapter_poll_txfifo_ready(adap)	\
	dw_i2c_adapter_poll(adap, DW_I2_C_ಸ್ಥಿತಿ_TXFIFO_ಖಾಲಿಗಳು, DW_IC_ಸ್ಥಿತಿ, 0)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dw_i2c_write_addr(ರಚನೆ dw_i2c_adapter *adap, u8 addr)
{
	dw_i2c_setreg(adap, DW_IC_ENABLE, 0);
	dw_i2c_setreg(adap, DW_IC_TAR, addr);
	dw_i2c_setreg(adap, DW_IC_ENABLE, 1);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dw_i2c_adapter_read(ರಚನೆ i2c_adapter *ia, u8 addr,
			       u8 reg, u8 *ಬಫರ್, ಪೂರ್ಣಾಂಕ len)
{
	ರಚನೆ dw_i2c_adapter *adap =
		container_ದ(ia, ರಚನೆ dw_i2c_adapter, adapter);
	ಪೂರ್ಣಾಂಕ rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_ದತ್ತಾಂಶ_CMD, reg);

	/* set value */
	ಆಗ (len) {
		ಆದರೆ (len == 1)
			dw_i2c_setreg(adap, DW_IC_ದತ್ತಾಂಶ_CMD,
				      IC_ದತ್ತಾಂಶ_CMD_READ | IC_ದತ್ತಾಂಶ_CMD_STOP);
		ಇಲ್ಲದಿದ್ದರೆ
			dw_i2c_setreg(adap, DW_IC_ದತ್ತಾಂಶ_CMD, IC_ದತ್ತಾಂಶ_CMD_READ);

		rc = dw_i2c_adapter_poll_rxrdy(adap);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		*ಬಫರ್ = dw_i2c_getreg(adap, DW_IC_ದತ್ತಾಂಶ_CMD) & 0xff;
		ಬಫರ್++;
		len--;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dw_i2c_adapter_write(ರಚನೆ i2c_adapter *ia, u8 addr,
				u8 reg, u8 *ಬಫರ್, ಪೂರ್ಣಾಂಕ len)
{
	ರಚನೆ dw_i2c_adapter *adap =
		container_ದ(ia, ರಚನೆ dw_i2c_adapter, adapter);
	ಪೂರ್ಣಾಂಕ rc;

	dw_i2c_write_addr(adap, addr);

	rc = dw_i2c_adapter_poll_txfifo_ready(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* set register address */
	dw_i2c_setreg(adap, DW_IC_ದತ್ತಾಂಶ_CMD, reg);

	ಆಗ (len) {
		rc = dw_i2c_adapter_poll_txfifo_ready(adap);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		ಆದರೆ (len == 1)
			dw_i2c_setreg(adap, DW_IC_ದತ್ತಾಂಶ_CMD, *ಬಫರ್ | IC_ದತ್ತಾಂಶ_CMD_STOP);
		ಇಲ್ಲದಿದ್ದರೆ
			dw_i2c_setreg(adap, DW_IC_ದತ್ತಾಂಶ_CMD, *ಬಫರ್);

		ಬಫರ್++;
		len--;
	}
	rc = dw_i2c_adapter_poll_txfifo_ready(adap);

	ಹಿಂತಿರುಗಿ rc;
}

ಪೂರ್ಣಾಂಕ dw_i2c_init(ರಚನೆ i2c_adapter *adapter, ಪೂರ್ಣಾಂಕ nodeoff)
{
	adapter->id = nodeoff;
	adapter->ಲಿಖ = dw_i2c_adapter_write;
	adapter->ಪಠ = dw_i2c_adapter_read;

	ಹಿಂತಿರುಗಿ i2c_adapter_ಕೂಡಿಸು(adapter);
}
