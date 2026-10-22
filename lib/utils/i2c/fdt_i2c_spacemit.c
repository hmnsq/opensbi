/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/i2c/fdt_i2c.h>

/* Controller registers */
#घोषणा ICR_OFFSET	0x00		/* I2C control register */
#घोषणा IDBR_OFFSET	0x0c		/* I2C data buffer register */

/* Control register bits */
#घोषणा ICR_START	BIT(0)		/* start */
#घोषणा ICR_STOP	BIT(1)		/* stop */
#घोषणा ICR_ACKNAK	BIT(2)		/* ACK(0) or NAK(1) */
#घोषणा ICR_TB		BIT(3)		/* transfer byte */
#घोषणा ICR_UR		BIT(10)		/* unit reset */
#घोषणा ICR_SCLE	BIT(13)		/* SCL enable  */
#घोषणा ICR_IUE		BIT(14)		/* unit enable */

/* Timing */
#घोषणा I2C_RESET_US	10
#घोषणा I2C_TIMEOUT_US	1000

काष्ठा spacemit_i2c_adapter {
	अचिन्हित दीर्घ base;
	काष्ठा i2c_adapter adapter;
};

अटल अंतरभूत व्योम spacemit_i2c_set_reg(काष्ठा spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (व्योम *)adap->base + reg);
}

अटल अंतरभूत uint32_t spacemit_i2c_get_reg(काष्ठा spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	वापस readl((व्योम *)adap->base + reg);
}

अटल व्योम spacemit_i2c_reset(काष्ठा spacemit_i2c_adapter *adap)
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

अटल पूर्णांक spacemit_i2c_wait_xfer_done(काष्ठा spacemit_i2c_adapter *adap)
{
	क्रम (पूर्णांक i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_OFFSET);

		अगर (!(val & ICR_TB))
			वापस 0;

		sbi_timer_udelay(1);
	};

	वापस SBI_ETIMEDOUT;
}

अटल व्योम spacemit_i2c_start_xfer(काष्ठा spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	स्थिर uint32_t ctrl_mask = ICR_START | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_OFFSET);
	val &= ~ctrl_mask;
	val |= (ctrl & ctrl_mask);
	val |= ICR_TB;

	spacemit_i2c_set_reg(adap, ICR_OFFSET, val);
}

अटल पूर्णांक spacemit_i2c_xfer_write(काष्ठा spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_set_reg(adap, IDBR_OFFSET, byte);
	spacemit_i2c_start_xfer(adap, ctrl);

	वापस spacemit_i2c_wait_xfer_done(adap);
}

अटल पूर्णांक spacemit_i2c_xfer_read(काष्ठा spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	पूर्णांक rc;

	spacemit_i2c_start_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_done(adap);
	अगर (rc)
		वापस rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_OFFSET);
	वापस 0;
}

अटल पूर्णांक spacemit_i2c_adapter_write(काष्ठा i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *बफर, पूर्णांक len)
{
	काष्ठा spacemit_i2c_adapter *adap =
		container_of(ia, काष्ठा spacemit_i2c_adapter, adapter);
	पूर्णांक rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	अगर (rc)
		वापस rc;

	/* send register + data bytes */
	क्रम (पूर्णांक i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : बफर[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		अगर (rc)
			वापस rc;
	}

	वापस 0;
}

अटल पूर्णांक spacemit_i2c_adapter_read(काष्ठा i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *बफर, पूर्णांक len)
{
	काष्ठा spacemit_i2c_adapter *adap =
		container_of(ia, काष्ठा spacemit_i2c_adapter, adapter);
	पूर्णांक rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_START);
	अगर (rc)
		वापस rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	अगर (rc)
		वापस rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_START);
	अगर (rc)
		वापस rc;

	/* read data bytes */
	क्रम (पूर्णांक i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &बफर[i], ctrl);
		अगर (rc)
			वापस rc;
	}

	वापस 0;
}

अटल पूर्णांक spacemit_i2c_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			     स्थिर काष्ठा fdt_match *match)
{
	काष्ठा spacemit_i2c_adapter *adapter;
	uint64_t base;
	पूर्णांक rc;

	adapter = sbi_zalloc(माप(*adapter));
	अगर (!adapter)
		वापस SBI_ENOMEM;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &base, शून्य);
	अगर (rc) {
		sbi_free(adapter);
		वापस rc;
	}

	adapter->base = base;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ग_लिखो = spacemit_i2c_adapter_write;
	adapter->adapter.पढ़ो = spacemit_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	अगर (rc) {
		sbi_free(adapter);
		वापस rc;
	}

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match spacemit_i2c_match[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_i2c_adapter_spacemit = {
	.match_table = spacemit_i2c_match,
	.init = spacemit_i2c_init,
};
