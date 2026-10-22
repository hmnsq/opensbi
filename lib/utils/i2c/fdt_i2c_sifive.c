/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/i2c/fdt_i2c.h>

#घोषणा SIFIVE_I2C_PRELO	0x00
#घोषणा SIFIVE_I2C_PREHI	0x04
#घोषणा SIFIVE_I2C_CTR		0x08
#घोषणा SIFIVE_I2C_TXR		0x00c
#घोषणा SIFIVE_I2C_RXR		SIFIVE_I2C_TXR
#घोषणा SIFIVE_I2C_CR		0x010
#घोषणा SIFIVE_I2C_SR		SIFIVE_I2C_CR

#घोषणा SIFIVE_I2C_CTR_IEN	(1 << 6)
#घोषणा SIFIVE_I2C_CTR_EN	(1 << 7)

#घोषणा SIFIVE_I2C_CMD_IACK	(1 << 0)
#घोषणा SIFIVE_I2C_CMD_ACK	(1 << 3)
#घोषणा SIFIVE_I2C_CMD_WR	(1 << 4)
#घोषणा SIFIVE_I2C_CMD_RD	(1 << 5)
#घोषणा SIFIVE_I2C_CMD_STO	(1 << 6)
#घोषणा SIFIVE_I2C_CMD_STA	(1 << 7)

#घोषणा SIFIVE_I2C_STATUS_IF	(1 << 0)
#घोषणा SIFIVE_I2C_STATUS_TIP	(1 << 1)
#घोषणा SIFIVE_I2C_STATUS_AL	(1 << 5)
#घोषणा SIFIVE_I2C_STATUS_BUSY	(1 << 6)
#घोषणा SIFIVE_I2C_STATUS_RXACK	(1 << 7)

#घोषणा SIFIVE_I2C_WRITE_BIT	(0 << 0)
#घोषणा SIFIVE_I2C_READ_BIT	(1 << 0)

काष्ठा sifive_i2c_adapter {
	अचिन्हित दीर्घ addr;
	काष्ठा i2c_adapter adapter;
};

अटल अंतरभूत व्योम sifive_i2c_setreg(काष्ठा sifive_i2c_adapter *adap,
				     uint8_t reg, uint8_t मूल्य)
{
	writel(मूल्य, (अस्थिर अक्षर *)adap->addr + reg);
}

अटल अंतरभूत uint8_t sifive_i2c_getreg(काष्ठा sifive_i2c_adapter *adap,
					uint8_t reg)
{
	वापस readl((अस्थिर अक्षर *)adap->addr + reg);
}

अटल पूर्णांक sifive_i2c_adapter_rxack(काष्ठा sifive_i2c_adapter *adap)
{
	uint8_t val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);

	अगर (val & SIFIVE_I2C_STATUS_RXACK)
		वापस SBI_EIO;

	वापस 0;
}

अटल पूर्णांक sifive_i2c_adapter_poll(काष्ठा sifive_i2c_adapter *adap,
				   uint32_t mask)
{
	अचिन्हित पूर्णांक timeout = 1; // [msec]
	पूर्णांक गिनती = 0;
	uint8_t val;

	sbi_timer_udelay(80); // worst case if bus speed is 100 kHz

	करो {
		val = sifive_i2c_getreg(adap, SIFIVE_I2C_SR);
		अगर (!(val & mask))
			वापस 0;

		sbi_timer_udelay(1);
		गिनती += 1;
		अगर (गिनती == (timeout * 1000))
			वापस SBI_ETIMEDOUT;
	} जबतक (1);
}

#घोषणा sifive_i2c_adapter_poll_tip(adap)	\
	sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_TIP)
#घोषणा sifive_i2c_adapter_poll_busy(adap)	\
sifive_i2c_adapter_poll(adap, SIFIVE_I2C_STATUS_BUSY)

अटल पूर्णांक sifive_i2c_adapter_start(काष्ठा sifive_i2c_adapter *adap,
				    uint8_t addr, uint8_t bit)
{
	uint8_t val = (addr << 1) | bit;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, val);
	val = SIFIVE_I2C_CMD_STA | SIFIVE_I2C_CMD_WR | SIFIVE_I2C_CMD_IACK;
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, val);

	वापस sifive_i2c_adapter_poll_tip(adap);
}

अटल पूर्णांक sifive_i2c_adapter_write(काष्ठा i2c_adapter *ia, uint8_t addr,
				    uint8_t reg, uint8_t *बफर, पूर्णांक len)
{
	काष्ठा sifive_i2c_adapter *adap =
		container_of(ia, काष्ठा sifive_i2c_adapter, adapter);
	पूर्णांक rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);

	अगर (rc)
		वापस rc;

	rc = sifive_i2c_adapter_rxack(adap);
	अगर (rc)
		वापस rc;

	/* set register address */
	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
				SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_tip(adap);
	अगर (rc)
		वापस rc;

	rc = sifive_i2c_adapter_rxack(adap);
	अगर (rc)
		वापस rc;

	/* set value */
	जबतक (len) {
		sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, *बफर);
		sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
						       SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		अगर (rc)
			वापस rc;

		rc = sifive_i2c_adapter_rxack(adap);
		अगर (rc)
			वापस rc;

		बफर++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);

	/* poll BUSY instead of ACK*/
	rc = sifive_i2c_adapter_poll_busy(adap);
	अगर (rc)
		वापस rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	वापस 0;
}

अटल पूर्णांक sifive_i2c_adapter_read(काष्ठा i2c_adapter *ia, uint8_t addr,
				   uint8_t reg, uint8_t *बफर, पूर्णांक len)
{
	काष्ठा sifive_i2c_adapter *adap =
		container_of(ia, काष्ठा sifive_i2c_adapter, adapter);
	पूर्णांक rc;

	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_WRITE_BIT);
	अगर (rc)
		वापस rc;

	rc = sifive_i2c_adapter_rxack(adap);
	अगर (rc)
		वापस rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_TXR, reg);
	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_WR |
					       SIFIVE_I2C_CMD_IACK);

	rc = sifive_i2c_adapter_poll_tip(adap);
	अगर (rc)
		वापस rc;

	rc = sifive_i2c_adapter_rxack(adap);
	अगर (rc)
		वापस rc;

	/* setting addr with high 0 bit */
	rc = sifive_i2c_adapter_start(adap, addr, SIFIVE_I2C_READ_BIT);
	अगर (rc)
		वापस rc;

	rc = sifive_i2c_adapter_rxack(adap);
	अगर (rc)
		वापस rc;

	जबतक (len) {
		अगर (len == 1)
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_ACK |
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);
		अन्यथा
			sifive_i2c_setreg(adap, SIFIVE_I2C_CR,
					  SIFIVE_I2C_CMD_RD |
					  SIFIVE_I2C_CMD_IACK);

		rc = sifive_i2c_adapter_poll_tip(adap);
		अगर (rc)
			वापस rc;

		*बफर = sifive_i2c_getreg(adap, SIFIVE_I2C_RXR);
		बफर++;
		len--;
	}

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_STO |
					       SIFIVE_I2C_CMD_IACK);
	rc = sifive_i2c_adapter_poll_busy(adap);
	अगर (rc)
		वापस rc;

	sifive_i2c_setreg(adap, SIFIVE_I2C_CR, SIFIVE_I2C_CMD_IACK);

	वापस 0;
}

अटल पूर्णांक sifive_i2c_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			   स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा sifive_i2c_adapter *adapter;
	uint64_t addr;

	adapter = sbi_zalloc(माप(*adapter));
	अगर (!adapter)
		वापस SBI_ENOMEM;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, शून्य);
	अगर (rc) {
		sbi_free(adapter);
		वापस rc;
	}

	adapter->addr = addr;
	adapter->adapter.id = nodeoff;
	adapter->adapter.ग_लिखो = sifive_i2c_adapter_write;
	adapter->adapter.पढ़ो = sifive_i2c_adapter_read;
	rc = i2c_adapter_add(&adapter->adapter);
	अगर (rc) {
		sbi_free(adapter);
		वापस rc;
	}

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match sifive_i2c_match[] = {
	{ .compatible = "sifive,i2c0" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_i2c_adapter_sifive = {
	.match_table = sifive_i2c_match,
	.init = sifive_i2c_init,
};
