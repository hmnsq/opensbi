/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/fdt_i2c.h>

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_PRELO	0x00
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_PREHI	0x04
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CTR		0x08
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_TXR		0x00c
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_RXR		SIFIVE_I2C_TXR
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CR		0x010
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_SR		SIFIVE_I2C_CR

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CTR_IEN	(1 << 6)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CTR_EN	(1 << 7)

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CMD_IACK	(1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CMD_ACK	(1 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CMD_WR	(1 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CMD_RD	(1 << 5)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CMD_STO	(1 << 6)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2C_CMD_STA	(1 << 7)

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2_C_ಸ್ಥಿತಿ_IF	(1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2_C_ಸ್ಥಿತಿ_TIP	(1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2_C_ಸ್ಥಿತಿ_AL	(1 << 5)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2_C_ಸ್ಥಿತಿ_BUSY	(1 << 6)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2_C_ಸ್ಥಿತಿ_RXACK	(1 << 7)

#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2_C_WRITE_ಬಿಟ್	(0 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು SIFIVE_I2_C_READ_ಬಿಟ್	(1 << 0)

ರಚನೆ sifive_i2c_adapter {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr;
	ರಚನೆ i2c_adapter adapter;
};

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ sifive_i2c_setreg(ರಚನೆ sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t ಮೌಲ್ಯ)
{
	writel(ಮೌಲ್ಯ, (ಅಸ್ಥಿರ ಅಕ್ಷರ *)adap->addr + reg);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint8_t sifive_i2c_getreg(ರಚನೆ sifive_i2c_adapter *adap,
					uint8_t reg)
{
	ಹಿಂತಿರುಗಿ readl((ಅಸ್ಥಿರ ಅಕ್ಷರ *)adap->addr + reg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_i2c_adapter_rxack(ರಚನೆ sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	ಆದರೆ (val & SIFIVE_I2_C_ಸ್ಥಿತಿ_RXACK)
		ಹಿಂತಿರುಗಿ SBI_EIO;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_i2c_adapter_poll(ರಚನೆ sifive_i2c_adapter *adap,
				   uint32_t ಮುಖವಾಡ)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ timeout = 1; // [msec]
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	ಮಾಡು {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		ಆದರೆ (!(val & ಮುಖವಾಡ))
			ಹಿಂತಿರುಗಿ 0;

		sbi_timer_udelay(1);
		ಎಣಿಕೆ_2 += 1;
		ಆದರೆ (ಎಣಿಕೆ_2 == (timeout * 1000))
			ಹಿಂತಿರುಗಿ SBI_ETIMEDOUT;
	} ಆಗ (1);
}

#ವ್ಯಾಖ್ಯಾನಿಸು sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_ಸ್ಥಿತಿ_TIP)
#ವ್ಯಾಖ್ಯಾನಿಸು sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_ಸ್ಥಿತಿ_BUSY)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_i2c_adapter_ಆರಂಭ(ರಚನೆ sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t ಬಿಟ್_2)
{
	uint8_t val = (addr << 1) | ಬಿಟ್_2;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	ಹಿಂತಿರುಗಿ sifive_i2c_adapter_poll_tip(adap);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_i2c_adapter_write(ರಚನೆ i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *ಬಫರ್, ಪೂರ್ಣಾಂಕ len)
{
	ರಚನೆ sifive_i2c_adapter *adap =
		container_ದ(ia, ರಚನೆ sifive_i2c_adapter, adapter);
	ಪೂರ್ಣಾಂಕ rc = sifive_i2c_adapter_ಆರಂಭ(adap, addr, SIFIVE_I2_C_WRITE_ಬಿಟ್);

	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* set value */
	ಆಗ (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *ಬಫರ್);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		rc = sifive_i2c_adapter_rxack(adap);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		ಬಫರ್++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_i2c_adapter_read(ರಚನೆ i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *ಬಫರ್, ಪೂರ್ಣಾಂಕ len)
{
	ರಚನೆ sifive_i2c_adapter *adap =
		container_ದ(ia, ರಚನೆ sifive_i2c_adapter, adapter);
	ಪೂರ್ಣಾಂಕ rc;

	rc = sifive_i2c_adapter_ಆರಂಭ(adap, addr, SIFIVE_I2_C_WRITE_ಬಿಟ್);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_ಆರಂಭ(adap, addr, SIFIVE_I2_C_READ_ಬಿಟ್);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಆಗ (len) {
		ಆದರೆ (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		ಇಲ್ಲದಿದ್ದರೆ
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		*ಬಫರ್ = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		ಬಫರ್++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_i2c_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			   ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(ಗಾತ್ರ(*adapter));
	ಆದರೆ (!adapter)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &addr, ಖಾಲಿ);
	ಆದರೆ (rc) {
		sbi_free(adapter);
		ಹಿಂತಿರುಗಿ rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ಲಿಖ = sifive_i2c_adapter_write;
	adapter->adapter.ಪಠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_ಕೂಡಿಸು(&adapter->adapter);
	ಆದರೆ (rc) {
		sbi_free(adapter);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sifive_i2c_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_i2c_adapter_sifive = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sifive_i2c_ಹೊಂದಿಕೆ,
	.init = sifive_i2c_init,
};
