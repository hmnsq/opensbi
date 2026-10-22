/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/i2c/fdt_i2c.h>

/* Controller registers */
#define ICR_OFFSET	0x00		/* I2C control register */
#define IDBR_OFFSET	0x0c		/* I2C data buffer register */

/* Control register bits */
#define ICR_START	BIT(0)		/* start */
#define ICR_STOP	BIT(1)		/* stop */
#define ICR_ACKNAK	BIT(2)		/* ACK(0) or NAK(1) */
#define ICR_TB		BIT(3)		/* transfer byte */
#define ICR_UR		BIT(10)		/* unit reset */
#define ICR_SCLE	BIT(13)		/* SCL enable  */
#define ICR_IUE		BIT(14)		/* unit enable */

/* Timing */
#define I2C_RESET_US	10
#define I2C_TIMEOUT_US	1000

સંરચના spacemit_i2c_adapter {
	unsigned long base;
	સંરચના i2c_adapter adapter;
};

સ્થિર અંતઃરેખ શૂન્ય spacemit_i2c_set_reg(સંરચના spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (શૂન્ય *)adap->base + reg);
}

સ્થિર અંતઃરેખ uint32_t spacemit_i2c_get_reg(સંરચના spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	પાછા readl((શૂન્ય *)adap->base + reg);
}

સ્થિર શૂન્ય spacemit_i2c_reset(સંરચના spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_set_reg(adap, ICR_OFFSET, ICR_IUE | ICR_SCLE);
}

સ્થિર પૂર્ણાંક spacemit_i2c_wait_xfer_done(સંરચના spacemit_i2c_adapter *adap)
{
	માટે (પૂર્ણાંક i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		જો (!(val & ICR_TB))
			પાછા 0;

		sbi_timer_udelay(1);
	};

	પાછા SBI_ETIMEDOUT;
}

સ્થિર શૂન્ય spacemit_i2c_start_xfer(સંરચના spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	અચલ uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

સ્થિર પૂર્ણાંક spacemit_i2c_xfer_write(સંરચના spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	પાછા spacemit_i2c_wait_xfer_done(adap);
}

સ્થિર પૂર્ણાંક spacemit_i2c_xfer_read(સંરચના spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	પૂર્ણાંક rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	જો (rc)
		પાછા rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	પાછા 0;
}

સ્થિર પૂર્ણાંક spacemit_i2c_adapter_write(સંરચના i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *buffer, પૂર્ણાંક len)
{
	સંરચના spacemit_i2c_adapter *adap =
		container_of(ia, સંરચના spacemit_i2c_adapter, adapter);
	પૂર્ણાંક rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	જો (rc)
		પાછા rc;

	/* send register + data bytes */
	માટે (પૂર્ણાંક i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : buffer[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		જો (rc)
			પાછા rc;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક spacemit_i2c_adapter_read(સંરચના i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *buffer, પૂર્ણાંક len)
{
	સંરચના spacemit_i2c_adapter *adap =
		container_of(ia, સંરચના spacemit_i2c_adapter, adapter);
	પૂર્ણાંક rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	જો (rc)
		પાછા rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	જો (rc)
		પાછા rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	જો (rc)
		પાછા rc;

	/* read data bytes */
	માટે (પૂર્ણાંક i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &buffer[i], ctrl);
		જો (rc)
			પાછા rc;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક spacemit_i2c_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			     અચલ સંરચના fdt_match *match)
{
	સંરચના spacemit_i2c_adapter *adapter;
	uint64_t base;
	પૂર્ણાંક rc;

	adapter = sbi_zalloc(કદ(*adapter));
	જો (!adapter)
		પાછા SBI_ENOMEM;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &base, ખાલી);
	જો (rc) {
		sbi_free(adapter);
		પાછા rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.લિખ = spacemit_i2c_adapter_write;
	adapter->adapter.પઠ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	જો (rc) {
		sbi_free(adapter);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

અચલ સંરચના fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
