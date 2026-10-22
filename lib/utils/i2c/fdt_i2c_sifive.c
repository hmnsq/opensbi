/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/fdt_i2c.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_PRELO	0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_PREHI	0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CTR		0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_TXR		0x00c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_RXR		SIFIVE_I2C_TXR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CR		0x010
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_SR		SIFIVE_I2C_CR

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CTR_IEN	(1 << 6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CTR_EN	(1 << 7)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CMD_IACK	(1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CMD_ACK	(1 << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CMD_WR	(1 << 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CMD_RD	(1 << 5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CMD_STO	(1 << 6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2C_CMD_STA	(1 << 7)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2_C_ਸਥਿਤੀ_IF	(1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2_C_ਸਥਿਤੀ_TIP	(1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2_C_ਸਥਿਤੀ_AL	(1 << 5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2_C_ਸਥਿਤੀ_BUSY	(1 << 6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2_C_ਸਥਿਤੀ_RXACK	(1 << 7)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2_C_WRITE_ਦੋਆਧਾਰੀ_ਅੰਕ	(0 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIFIVE_I2_C_READ_ਦੋਆਧਾਰੀ_ਅੰਕ	(1 << 0)

ਬਣਤਰ sifive_i2c_adapter {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr;
	ਬਣਤਰ i2c_adapter adapter;
};

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ sifive_i2c_setreg(ਬਣਤਰ sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t ਮੁੱਲ)
{
	writel(ਮੁੱਲ, (ਅਸਥਿਰ ਅੱਖਰ *)adap->addr + reg);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint8_t sifive_i2c_getreg(ਬਣਤਰ sifive_i2c_adapter *adap,
					uint8_t reg)
{
	ਵਾਪਸ readl((ਅਸਥਿਰ ਅੱਖਰ *)adap->addr + reg);
}

ਸਥਿਰ ਪੂਰਨ sifive_i2c_adapter_rxack(ਬਣਤਰ sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	ਜੇ (val & SIFIVE_I2_C_ਸਥਿਤੀ_RXACK)
		ਵਾਪਸ SBI_EIO;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sifive_i2c_adapter_poll(ਬਣਤਰ sifive_i2c_adapter *adap,
				   uint32_t ਨਕਾਬ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ timeout = 1; // [msec]
	ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	ਕਰੋ {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		ਜੇ (!(val & ਨਕਾਬ))
			ਵਾਪਸ 0;

		sbi_timer_udelay(1);
		ਗਿਣਤੀ_2 += 1;
		ਜੇ (ਗਿਣਤੀ_2 == (timeout * 1000))
			ਵਾਪਸ SBI_ETIMEDOUT;
	} ਜਦੋਂ (1);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_ਸਥਿਤੀ_TIP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2_C_ਸਥਿਤੀ_BUSY)

ਸਥਿਰ ਪੂਰਨ sifive_i2c_adapter_ਸ਼ੁਰੂ(ਬਣਤਰ sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t ਦੋਆਧਾਰੀ_ਅੰਕ_2)
{
	uint8_t val = (addr << 1) | ਦੋਆਧਾਰੀ_ਅੰਕ_2;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	ਵਾਪਸ sifive_i2c_adapter_poll_tip(adap);
}

ਸਥਿਰ ਪੂਰਨ sifive_i2c_adapter_write(ਬਣਤਰ i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *ਬਫਰ, ਪੂਰਨ len)
{
	ਬਣਤਰ sifive_i2c_adapter *adap =
		container_ਦਾ(ia, ਬਣਤਰ sifive_i2c_adapter, adapter);
	ਪੂਰਨ rc = sifive_i2c_adapter_ਸ਼ੁਰੂ(adap, addr, SIFIVE_I2_C_WRITE_ਦੋਆਧਾਰੀ_ਅੰਕ);

	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* set value */
	ਜਦੋਂ (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *ਬਫਰ);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		rc = sifive_i2c_adapter_rxack(adap);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		ਬਫਰ++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sifive_i2c_adapter_read(ਬਣਤਰ i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *ਬਫਰ, ਪੂਰਨ len)
{
	ਬਣਤਰ sifive_i2c_adapter *adap =
		container_ਦਾ(ia, ਬਣਤਰ sifive_i2c_adapter, adapter);
	ਪੂਰਨ rc;

	rc = sifive_i2c_adapter_ਸ਼ੁਰੂ(adap, addr, SIFIVE_I2_C_WRITE_ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_ਸ਼ੁਰੂ(adap, addr, SIFIVE_I2_C_READ_ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = sifive_i2c_adapter_rxack(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਜਦੋਂ (len) {
		ਜੇ (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		ਨਹੀਂਤਾਂ
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		*ਬਫਰ = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		ਬਫਰ++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ sifive_i2c_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			   ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(ਆਕਾਰ(*adapter));
	ਜੇ (!adapter)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, ਨਿਰਾ);
	ਜੇ (rc) {
		sbi_free(adapter);
		ਵਾਪਸ rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ਲਿਖ = sifive_i2c_adapter_write;
	adapter->adapter.ਪਠ = sifive_i2c_adapter_read;
	rc = i2c_adapter_ਜੋੜੋ(&adapter->adapter);
	ਜੇ (rc) {
		sbi_free(adapter);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sifive_i2c_ਮੇਲ[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_i2c_adapter_sifive = {
	.ਮੇਲ_ਸਾਰਣੀ = sifive_i2c_ਮੇਲ,
	.init = sifive_i2c_init,
};
