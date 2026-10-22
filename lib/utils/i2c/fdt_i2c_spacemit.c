/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/fdt_i2c.h>

/* Controller registers */
#ವ್ಯಾಖ್ಯಾನಿಸು ICR_ಸ್ಥಾನಾಂತರ	0x00		/* I2C control register */
#ವ್ಯಾಖ್ಯಾನಿಸು IDBR_ಸ್ಥಾನಾಂತರ	0x0c		/* I2C data buffer register */

/* Control register bits */
#ವ್ಯಾಖ್ಯಾನಿಸು ICR_ಆರಂಭ	ಬಿಟ್(0)		/*ಆರಂಭ*/
#ವ್ಯಾಖ್ಯಾನಿಸು ICR_STOP	ಬಿಟ್(1)		/* stop */
#ವ್ಯಾಖ್ಯಾನಿಸು ICR_ACKNAK	ಬಿಟ್(2)		/* ACK(0) or NAK(1) */
#ವ್ಯಾಖ್ಯಾನಿಸು ICR_TB		ಬಿಟ್(3)		/* transfer byte */
#ವ್ಯಾಖ್ಯಾನಿಸು ICR_UR		ಬಿಟ್(10)		/* unit reset */
#ವ್ಯಾಖ್ಯಾನಿಸು ICR_SCLE	ಬಿಟ್(13)		/* SCL enable  */
#ವ್ಯಾಖ್ಯಾನಿಸು ICR_IUE		ಬಿಟ್(14)		/* unit enable */

/* Timing */
#ವ್ಯಾಖ್ಯಾನಿಸು I2C_RESET_US	10
#ವ್ಯಾಖ್ಯಾನಿಸು I2C_TIMEOUT_US	1000

ರಚನೆ spacemit_i2c_adapter {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2;
	ರಚನೆ i2c_adapter adapter;
};

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ spacemit_i2c_ಗಣ_reg(ರಚನೆ spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (ಶೂನ್ಯ *)adap->ತಳ_2 + reg);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t spacemit_i2c_get_reg(ರಚನೆ spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	ಹಿಂತಿರುಗಿ readl((ಶೂನ್ಯ *)adap->ತಳ_2 + reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_i2c_reset(ರಚನೆ spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_ಗಣ_reg(adap, ICR_ಸ್ಥಾನಾಂತರ, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_ಗಣ_reg(adap, ICR_ಸ್ಥಾನಾಂತರ, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_ಗಣ_reg(adap, ICR_ಸ್ಥಾನಾಂತರ, ICR_IUE | ICR_SCLE);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_i2c_wait_xfer_ಮುಗಿದ(ರಚನೆ spacemit_i2c_adapter *adap)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_ಸ್ಥಾನಾಂತರ);

		ಆದರೆ (!(val & ICR_TB))
			ಹಿಂತಿರುಗಿ 0;

		sbi_timer_udelay(1);
	};

	ಹಿಂತಿರುಗಿ SBI_ETIMEDOUT;
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_i2c_ಆರಂಭ_xfer(ರಚನೆ spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ಸ್ಥಿರಾಂಕ uint32_t ctrl_ಮುಖವಾಡ = ICR_ಆರಂಭ | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_ಸ್ಥಾನಾಂತರ);
	val &= ~ctrl_ಮುಖವಾಡ;
	val |= (ctrl & ctrl_ಮುಖವಾಡ);
	val |= ICR_TB;

	spacemit_i2c_ಗಣ_reg(adap, ICR_ಸ್ಥಾನಾಂತರ, val);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_i2c_xfer_write(ರಚನೆ spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_ಗಣ_reg(adap, IDBR_ಸ್ಥಾನಾಂತರ, byte);
	spacemit_i2c_ಆರಂಭ_xfer(adap, ctrl);

	ಹಿಂತಿರುಗಿ spacemit_i2c_wait_xfer_ಮುಗಿದ(adap);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_i2c_xfer_read(ರಚನೆ spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	ಪೂರ್ಣಾಂಕ rc;

	spacemit_i2c_ಆರಂಭ_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_ಮುಗಿದ(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_i2c_adapter_write(ರಚನೆ i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *ಬಫರ್, ಪೂರ್ಣಾಂಕ len)
{
	ರಚನೆ spacemit_i2c_adapter *adap =
		container_ದ(ia, ರಚನೆ spacemit_i2c_adapter, adapter);
	ಪೂರ್ಣಾಂಕ rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_ಆರಂಭ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* send register + data bytes */
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : ಬಫರ್[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_i2c_adapter_read(ರಚನೆ i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *ಬಫರ್, ಪೂರ್ಣಾಂಕ len)
{
	ರಚನೆ spacemit_i2c_adapter *adap =
		container_ದ(ia, ರಚನೆ spacemit_i2c_adapter, adapter);
	ಪೂರ್ಣಾಂಕ rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_ಆರಂಭ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_ಆರಂಭ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* read data bytes */
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &ಬಫರ್[i], ctrl);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_i2c_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			     ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ರಚನೆ spacemit_i2c_adapter *adapter;
	uint64_t ತಳ_2;
	ಪೂರ್ಣಾಂಕ rc;

	adapter = sbi_zalloc(ಗಾತ್ರ(*adapter));
	ಆದರೆ (!adapter)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &ತಳ_2, ಖಾಲಿ);
	ಆದರೆ (rc) {
		sbi_free(adapter);
		ಹಿಂತಿರುಗಿ rc;
	}

	adapter->ತಳ_2 = ತಳ_2;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ಲಿಖ = spacemit_i2c_adapter_write;
	adapter->adapter.ಪಠ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_ಕೂಡಿಸು(&adapter->adapter);
	ಆದರೆ (rc) {
		sbi_free(adapter);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ spacemit_i2c_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_i2c_adapter_spacemit = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = spacemit_i2c_ಹೊಂದಿಕೆ,
	.init = spacemit_i2c_init,
};
