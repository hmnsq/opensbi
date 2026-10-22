/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/fdt_i2c.h>

/* Controller registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICR_ਵਿੱਥ	0x00		/* I2C control register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDBR_ਵਿੱਥ	0x0c		/* I2C data buffer register */

/* Control register bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICR_ਸ਼ੁਰੂ	ਦੋਆਧਾਰੀ_ਅੰਕ(0)		/*ਸ਼ੁਰੂ*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICR_STOP	ਦੋਆਧਾਰੀ_ਅੰਕ(1)		/* stop */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICR_ACKNAK	ਦੋਆਧਾਰੀ_ਅੰਕ(2)		/* ACK(0) or NAK(1) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICR_TB		ਦੋਆਧਾਰੀ_ਅੰਕ(3)		/* transfer byte */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICR_UR		ਦੋਆਧਾਰੀ_ਅੰਕ(10)		/* unit reset */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICR_SCLE	ਦੋਆਧਾਰੀ_ਅੰਕ(13)		/* SCL enable  */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICR_IUE		ਦੋਆਧਾਰੀ_ਅੰਕ(14)		/* unit enable */

/* Timing */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ I2C_RESET_US	10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ I2C_TIMEOUT_US	1000

ਬਣਤਰ spacemit_i2c_adapter {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2;
	ਬਣਤਰ i2c_adapter adapter;
};

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ spacemit_i2c_ਸੈੱਟ_reg(ਬਣਤਰ spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (ਖਾਲੀ *)adap->ਅਧਾਰ_2 + reg);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t spacemit_i2c_get_reg(ਬਣਤਰ spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	ਵਾਪਸ readl((ਖਾਲੀ *)adap->ਅਧਾਰ_2 + reg);
}

ਸਥਿਰ ਖਾਲੀ spacemit_i2c_reset(ਬਣਤਰ spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_ਸੈੱਟ_reg(adap, ICR_ਵਿੱਥ, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_ਸੈੱਟ_reg(adap, ICR_ਵਿੱਥ, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_ਸੈੱਟ_reg(adap, ICR_ਵਿੱਥ, ICR_IUE | ICR_SCLE);
}

ਸਥਿਰ ਪੂਰਨ spacemit_i2c_wait_xfer_ਪੂਰਾ(ਬਣਤਰ spacemit_i2c_adapter *adap)
{
	ਲਈ (ਪੂਰਨ i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_ਵਿੱਥ);

		ਜੇ (!(val & ICR_TB))
			ਵਾਪਸ 0;

		sbi_timer_udelay(1);
	};

	ਵਾਪਸ SBI_ETIMEDOUT;
}

ਸਥਿਰ ਖਾਲੀ spacemit_i2c_ਸ਼ੁਰੂ_xfer(ਬਣਤਰ spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	ਅਚਲ uint32_t ctrl_ਨਕਾਬ = ICR_ਸ਼ੁਰੂ | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_ਵਿੱਥ);
	val &= ~ctrl_ਨਕਾਬ;
	val |= (ctrl & ctrl_ਨਕਾਬ);
	val |= ICR_TB;

	spacemit_i2c_ਸੈੱਟ_reg(adap, ICR_ਵਿੱਥ, val);
}

ਸਥਿਰ ਪੂਰਨ spacemit_i2c_xfer_write(ਬਣਤਰ spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_ਸੈੱਟ_reg(adap, IDBR_ਵਿੱਥ, byte);
	spacemit_i2c_ਸ਼ੁਰੂ_xfer(adap, ctrl);

	ਵਾਪਸ spacemit_i2c_wait_xfer_ਪੂਰਾ(adap);
}

ਸਥਿਰ ਪੂਰਨ spacemit_i2c_xfer_read(ਬਣਤਰ spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	ਪੂਰਨ rc;

	spacemit_i2c_ਸ਼ੁਰੂ_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_ਪੂਰਾ(adap);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_ਵਿੱਥ);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ spacemit_i2c_adapter_write(ਬਣਤਰ i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *ਬਫਰ, ਪੂਰਨ len)
{
	ਬਣਤਰ spacemit_i2c_adapter *adap =
		container_ਦਾ(ia, ਬਣਤਰ spacemit_i2c_adapter, adapter);
	ਪੂਰਨ rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_ਸ਼ੁਰੂ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* send register + data bytes */
	ਲਈ (ਪੂਰਨ i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : ਬਫਰ[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ spacemit_i2c_adapter_read(ਬਣਤਰ i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *ਬਫਰ, ਪੂਰਨ len)
{
	ਬਣਤਰ spacemit_i2c_adapter *adap =
		container_ਦਾ(ia, ਬਣਤਰ spacemit_i2c_adapter, adapter);
	ਪੂਰਨ rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_ਸ਼ੁਰੂ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_ਸ਼ੁਰੂ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	/* read data bytes */
	ਲਈ (ਪੂਰਨ i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &ਬਫਰ[i], ctrl);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ spacemit_i2c_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			     ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਬਣਤਰ spacemit_i2c_adapter *adapter;
	uint64_t ਅਧਾਰ_2;
	ਪੂਰਨ rc;

	adapter = sbi_zalloc(ਆਕਾਰ(*adapter));
	ਜੇ (!adapter)
		ਵਾਪਸ SBI_ENOMEM;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &ਅਧਾਰ_2, ਨਿਰਾ);
	ਜੇ (rc) {
		sbi_free(adapter);
		ਵਾਪਸ rc;
	}

	adapter->ਅਧਾਰ_2 = ਅਧਾਰ_2;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ਲਿਖ = spacemit_i2c_adapter_write;
	adapter->adapter.ਪਠ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_ਜੋੜੋ(&adapter->adapter);
	ਜੇ (rc) {
		sbi_free(adapter);
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ spacemit_i2c_ਮੇਲ[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_i2c_adapter_spacemit = {
	.ਮੇਲ_ਸਾਰਣੀ = spacemit_i2c_ਮੇਲ,
	.init = spacemit_i2c_init,
};
