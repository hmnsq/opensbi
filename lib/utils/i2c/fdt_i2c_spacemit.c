/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 *
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/i2c/fdt_i2c.h>

/* Controller registers */
#నిర్వచించు ICR_స్థానభ్రంశం	0x00		/* I2C control register */
#నిర్వచించు IDBR_స్థానభ్రంశం	0x0c		/* I2C data buffer register */

/* Control register bits */
#నిర్వచించు ICR_ప్రారంభం	బిట్టు(0)		/*ప్రారంభం*/
#నిర్వచించు ICR_STOP	బిట్టు(1)		/* stop */
#నిర్వచించు ICR_ACKNAK	బిట్టు(2)		/* ACK(0) or NAK(1) */
#నిర్వచించు ICR_TB		బిట్టు(3)		/* transfer byte */
#నిర్వచించు ICR_UR		బిట్టు(10)		/* unit reset */
#నిర్వచించు ICR_SCLE	బిట్టు(13)		/* SCL enable  */
#నిర్వచించు ICR_IUE		బిట్టు(14)		/* unit enable */

/* Timing */
#నిర్వచించు I2C_RESET_US	10
#నిర్వచించు I2C_TIMEOUT_US	1000

నిర్మాణం spacemit_i2c_adapter {
	గుర్తులేని దీర్ఘ పునాది_2;
	నిర్మాణం i2c_adapter adapter;
};

స్థిర అంతఃవరుస శూన్య spacemit_i2c_సమితి_reg(నిర్మాణం spacemit_i2c_adapter *adap,
					uint8_t reg, uint32_t val)
{
	writel(val, (శూన్య *)adap->పునాది_2 + reg);
}

స్థిర అంతఃవరుస uint32_t spacemit_i2c_get_reg(నిర్మాణం spacemit_i2c_adapter *adap,
					    uint32_t reg)
{
	తిరిగి readl((శూన్య *)adap->పునాది_2 + reg);
}

స్థిర శూన్య spacemit_i2c_reset(నిర్మాణం spacemit_i2c_adapter *adap)
{
	/* disable unit */
	spacemit_i2c_సమితి_reg(adap, ICR_స్థానభ్రంశం, 0);
	sbi_timer_udelay(I2C_RESET_US);

	/* reset unit */
	spacemit_i2c_సమితి_reg(adap, ICR_స్థానభ్రంశం, ICR_UR);
	sbi_timer_udelay(I2C_RESET_US);

	/* clear reset and enable unit and SCL */
	spacemit_i2c_సమితి_reg(adap, ICR_స్థానభ్రంశం, ICR_IUE | ICR_SCLE);
}

స్థిర పూర్ణాంకం spacemit_i2c_wait_xfer_పూర్తి(నిర్మాణం spacemit_i2c_adapter *adap)
{
	కొరకు (పూర్ణాంకం i = 0; i < I2C_TIMEOUT_US; i++) {
		uint32_t val = spacemit_i2c_get_reg(adap, ICR_స్థానభ్రంశం);

		ఒకవేళ (!(val & ICR_TB))
			తిరిగి 0;

		sbi_timer_udelay(1);
	};

	తిరిగి SBI_ETIMEDOUT;
}

స్థిర శూన్య spacemit_i2c_ప్రారంభం_xfer(నిర్మాణం spacemit_i2c_adapter *adap,
				    uint32_t ctrl)
{
	స్థిరరాశి uint32_t ctrl_ముసుగు = ICR_ప్రారంభం | ICR_STOP | ICR_ACKNAK;
	uint32_t val;

	val = spacemit_i2c_get_reg(adap, ICR_స్థానభ్రంశం);
	val &= ~ctrl_ముసుగు;
	val |= (ctrl & ctrl_ముసుగు);
	val |= ICR_TB;

	spacemit_i2c_సమితి_reg(adap, ICR_స్థానభ్రంశం, val);
}

స్థిర పూర్ణాంకం spacemit_i2c_xfer_write(నిర్మాణం spacemit_i2c_adapter *adap,
				   uint8_t byte, uint32_t ctrl)
{
	spacemit_i2c_సమితి_reg(adap, IDBR_స్థానభ్రంశం, byte);
	spacemit_i2c_ప్రారంభం_xfer(adap, ctrl);

	తిరిగి spacemit_i2c_wait_xfer_పూర్తి(adap);
}

స్థిర పూర్ణాంకం spacemit_i2c_xfer_read(నిర్మాణం spacemit_i2c_adapter *adap,
				  uint8_t *byte, uint32_t ctrl)
{
	పూర్ణాంకం rc;

	spacemit_i2c_ప్రారంభం_xfer(adap, ctrl);

	rc = spacemit_i2c_wait_xfer_పూర్తి(adap);
	ఒకవేళ (rc)
		తిరిగి rc;

	*byte = spacemit_i2c_get_reg(adap, IDBR_స్థానభ్రంశం);
	తిరిగి 0;
}

స్థిర పూర్ణాంకం spacemit_i2c_adapter_write(నిర్మాణం i2c_adapter *ia, uint8_t addr,
				      uint8_t reg, uint8_t *బఫరు, పూర్ణాంకం len)
{
	నిర్మాణం spacemit_i2c_adapter *adap =
		container_యొక్క(ia, నిర్మాణం spacemit_i2c_adapter, adapter);
	పూర్ణాంకం rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_ప్రారంభం);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* send register + data bytes */
	కొరకు (పూర్ణాంకం i = 0; i < len + 1; i++) {
		uint32_t ctrl = (i == len) ? ICR_STOP : 0;
		uint8_t byte = (i == 0) ? reg : బఫరు[i - 1];

		rc = spacemit_i2c_xfer_write(adap, byte, ctrl);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం spacemit_i2c_adapter_read(నిర్మాణం i2c_adapter *ia, uint8_t addr,
				     uint8_t reg, uint8_t *బఫరు, పూర్ణాంకం len)
{
	నిర్మాణం spacemit_i2c_adapter *adap =
		container_యొక్క(ia, నిర్మాణం spacemit_i2c_adapter, adapter);
	పూర్ణాంకం rc;

	/* reset controller to a known state */
	spacemit_i2c_reset(adap);

	/* send device address (in write mode) */
	rc = spacemit_i2c_xfer_write(adap, addr << 1, ICR_ప్రారంభం);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* send register */
	rc = spacemit_i2c_xfer_write(adap, reg, 0);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* repeated start and send device address (in read mode) */
	rc = spacemit_i2c_xfer_write(adap, (addr << 1) | 1, ICR_ప్రారంభం);
	ఒకవేళ (rc)
		తిరిగి rc;

	/* read data bytes */
	కొరకు (పూర్ణాంకం i = 0; i < len; i++) {
		uint32_t ctrl = (i == len - 1) ? (ICR_ACKNAK | ICR_STOP) : 0;

		rc = spacemit_i2c_xfer_read(adap, &బఫరు[i], ctrl);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం spacemit_i2c_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			     స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	నిర్మాణం spacemit_i2c_adapter *adapter;
	uint64_t పునాది_2;
	పూర్ణాంకం rc;

	adapter = sbi_zalloc(పరిమాణం(*adapter));
	ఒకవేళ (!adapter)
		తిరిగి SBI_ENOMEM;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &పునాది_2, ఖాళీ);
	ఒకవేళ (rc) {
		sbi_free(adapter);
		తిరిగి rc;
	}

	adapter->పునాది_2 = పునాది_2;
	adapter->adapter.id = nodeoff;
	adapter->adapter.లిఖ = spacemit_i2c_adapter_write;
	adapter->adapter.పఠ = spacemit_i2c_adapter_read;
	rc = i2c_adapter_కూడు(&adapter->adapter);
	ఒకవేళ (rc) {
		sbi_free(adapter);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక spacemit_i2c_సరిపోలిక[] = {
	{ .compatible = "spacemit,k1-i2c" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_i2c_adapter_spacemit = {
	.సరిపోలిక_పట్టిక = spacemit_i2c_సరిపోలిక,
	.init = spacemit_i2c_init,
};
