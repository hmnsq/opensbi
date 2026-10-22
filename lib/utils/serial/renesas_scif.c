// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/serial/renesas-scif.h>

/* clang-format off */

#నిర్వచించు SCIF_REG_SMR		0x0
#నిర్వచించు SCIF_REG_BRR		0x2
#నిర్వచించు SCIF_REG_SCR		0x4
#నిర్వచించు SCIF_REG_FTDR		0x6
#నిర్వచించు SCIF_REG_FSR		0x8
#నిర్వచించు SCIF_REG_FCR		0xc
#నిర్వచించు SCIF_REG_LSR		0x12
#నిర్వచించు SCIF_REG_SEMR		0x14

#నిర్వచించు SCIF_FCR_RFRST		0x2 /* Reset assert receive-FIFO (bit[1]) */
#నిర్వచించు SCIF_FCR_TFRST		0x4 /* Reset assert transmit-FIFO(bit[2]) */

#నిర్వచించు SCIF_FCR_RST_ASSRT_RFTF	(SCIF_FCR_RFRST | SCIF_FCR_TFRST) /* Reset assert tx-FIFO & rx-FIFO */
#నిర్వచించు SCIF_FCR_RST_NGATE_RFTF	0x0 /* Reset negate tx-FIFO & rx-FIFO */

#నిర్వచించు SCIF_SCR_RE		0x10 /* Enable receive (bit[4]) */
#నిర్వచించు SCIF_SCR_TE		0x20 /* Enable transmit(bit[5]) */
#నిర్వచించు SCIF_SCR_RCV_TRN_EN	(SCIF_SCR_RE | SCIF_SCR_TE) /* Enable receive & transmit */
#నిర్వచించు SCIF_SCR_RCV_TRN_DIS	0x0 /* Disable receive & transmit */

#నిర్వచించు SCIF_FSR_ER		0x80 /* Receive error flag */
#నిర్వచించు SCIF_FSR_TEND		0x40 /* Transmit End Flag */
#నిర్వచించు SCIF_FSR_TDFE		0x20 /* Transmit FIFO Data Empty Flag */
#నిర్వచించు SCIF_FSR_BRK		0x10 /* Detect break flag */
#నిర్వచించు SCIF_FSR_DR		0x1  /* Receive data ready flag */

#నిర్వచించు SCIF_FSR_TXD_CHK	(SCIF_FSR_TEND | SCIF_FSR_TDFE)

#నిర్వచించు SCIF_SEMR_MDDRS		0x10 /* MDDR access enable */

#నిర్వచించు SCIF_REG_8_బిట్టు(reg)	((reg == SCIF_REG_BRR) || \
				 (reg == SCIF_REG_FTDR) || \
				 (reg == SCIF_REG_SEMR))

#నిర్వచించు SCBRR_విలువ(clk, baudrate) ((clk) / (32 * (baudrate)) - 1)

/* clang-format on */

స్థిర అస్థిర అక్షరం *scif_పునాది;

స్థిర u32 get_reg(u32 స్థానభ్రంశం)
{
	ఒకవేళ (SCIF_REG_8_బిట్టు(స్థానభ్రంశం))
		తిరిగి readb(scif_పునాది + స్థానభ్రంశం);

	తిరిగి readw(scif_పునాది + స్థానభ్రంశం);
}

స్థిర శూన్య సమితి_reg(u32 స్థానభ్రంశం, u32 val)
{
	ఒకవేళ (SCIF_REG_8_బిట్టు(స్థానభ్రంశం))
		writeb(val, scif_పునాది + స్థానభ్రంశం);
	లేకపోతే
		writew(val, scif_పునాది + స్థానభ్రంశం);
}

స్థిర శూన్య renesas_scif_putc(అక్షరం ch)
{
	uint16_t reg;

	అయితే (!(SCIF_FSR_TXD_CHK & get_reg(SCIF_REG_FSR)))
		;

	సమితి_reg(SCIF_REG_FTDR, ch);
	reg = get_reg(SCIF_REG_FSR);
	reg &= ~SCIF_FSR_TXD_CHK;
	సమితి_reg(SCIF_REG_FSR, reg);
}

స్థిర నిర్మాణం sbi_console_device renesas_scif_console = {
	.name		= "renesas_scif",
	.console_putc	= renesas_scif_putc,
};

పూర్ణాంకం renesas_scif_init(గుర్తులేని దీర్ఘ పునాది_2, u32 లోపల_freq, u32 baudrate)
{
	uint16_t data16;

	scif_పునాది = (అస్థిర అక్షరం *)పునాది_2;

	సమితి_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_DIS);
	సమితి_reg(SCIF_REG_FCR, SCIF_FCR_RST_ASSRT_RFTF);

	data16 = get_reg(SCIF_REG_FSR); /* Dummy read */
	సమితి_reg(SCIF_REG_FSR, 0x0); /* Clear all error bit */

	data16 = get_reg(SCIF_REG_LSR); /* Dummy read */
	సమితి_reg(SCIF_REG_LSR, 0x0); /* Clear ORER bit */

	సమితి_reg(SCIF_REG_SCR, 0x0);

	సమితి_reg(SCIF_REG_SMR, 0x0);

	data16 = get_reg(SCIF_REG_SEMR);
	సమితి_reg(SCIF_REG_SEMR, data16 & (~SCIF_SEMR_MDDRS));
	సమితి_reg(SCIF_REG_BRR, SCBRR_విలువ(లోపల_freq, baudrate));

	సమితి_reg(SCIF_REG_FCR, SCIF_FCR_RST_NGATE_RFTF);
	సమితి_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_EN);

	sbi_console_సమితి_device(&renesas_scif_console);

	తిరిగి 0;
}
