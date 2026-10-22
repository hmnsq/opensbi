// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/serial/renesas-scif.h>

/* clang-format off */

#परिभाषय SCIF_REG_SMR		0x0
#परिभाषय SCIF_REG_BRR		0x2
#परिभाषय SCIF_REG_SCR		0x4
#परिभाषय SCIF_REG_FTDR		0x6
#परिभाषय SCIF_REG_FSR		0x8
#परिभाषय SCIF_REG_FCR		0xc
#परिभाषय SCIF_REG_LSR		0x12
#परिभाषय SCIF_REG_SEMR		0x14

#परिभाषय SCIF_FCR_RFRST		0x2 /* Reset assert receive-FIFO (bit[1]) */
#परिभाषय SCIF_FCR_TFRST		0x4 /* Reset assert transmit-FIFO(bit[2]) */

#परिभाषय SCIF_FCR_RST_ASSRT_RFTF	(SCIF_FCR_RFRST | SCIF_FCR_TFRST) /* Reset assert tx-FIFO & rx-FIFO */
#परिभाषय SCIF_FCR_RST_NGATE_RFTF	0x0 /* Reset negate tx-FIFO & rx-FIFO */

#परिभाषय SCIF_SCR_RE		0x10 /* Enable receive (bit[4]) */
#परिभाषय SCIF_SCR_TE		0x20 /* Enable transmit(bit[5]) */
#परिभाषय SCIF_SCR_RCV_TRN_EN	(SCIF_SCR_RE | SCIF_SCR_TE) /* Enable receive & transmit */
#परिभाषय SCIF_SCR_RCV_TRN_DIS	0x0 /* Disable receive & transmit */

#परिभाषय SCIF_FSR_ER		0x80 /* Receive error flag */
#परिभाषय SCIF_FSR_TEND		0x40 /* Transmit End Flag */
#परिभाषय SCIF_FSR_TDFE		0x20 /* Transmit FIFO Data Empty Flag */
#परिभाषय SCIF_FSR_BRK		0x10 /* Detect break flag */
#परिभाषय SCIF_FSR_DR		0x1  /* Receive data ready flag */

#परिभाषय SCIF_FSR_TXD_CHK	(SCIF_FSR_TEND | SCIF_FSR_TDFE)

#परिभाषय SCIF_SEMR_MDDRS		0x10 /* MDDR access enable */

#परिभाषय SCIF_REG_8BIT(reg)	((reg == SCIF_REG_BRR) || \
				 (reg == SCIF_REG_FTDR) || \
				 (reg == SCIF_REG_SEMR))

#परिभाषय SCBRR_मानम्(clk, baudrate) ((clk) / (32 * (baudrate)) - 1)

/* clang-format on */

स्थिर चञ्चल वर्ण *scif_आधारः;

स्थिर u32 get_reg(u32 विस्थापनम्)
{
	यदि (SCIF_REG_8BIT(विस्थापनम्))
		प्रत्यागम readb(scif_आधारः + विस्थापनम्);

	प्रत्यागम readw(scif_आधारः + विस्थापनम्);
}

स्थिर शून्य समुच्चयः_reg(u32 विस्थापनम्, u32 val)
{
	यदि (SCIF_REG_8BIT(विस्थापनम्))
		writeb(val, scif_आधारः + विस्थापनम्);
	अन्यथा
		writew(val, scif_आधारः + विस्थापनम्);
}

स्थिर शून्य renesas_scif_putc(वर्ण ch)
{
	uint16_t reg;

	यावत् (!(SCIF_FSR_TXD_CHK & get_reg(SCIF_REG_FSR)))
		;

	समुच्चयः_reg(SCIF_REG_FTDR, ch);
	reg = get_reg(SCIF_REG_FSR);
	reg &= ~SCIF_FSR_TXD_CHK;
	समुच्चयः_reg(SCIF_REG_FSR, reg);
}

स्थिर संरचना sbi_console_device renesas_scif_console = {
	.name		= "renesas_scif",
	.console_putc	= renesas_scif_putc,
};

पूर्णाङ्क renesas_scif_init(अचिह्नित दीर्घ आधारः_2, u32 अन्तर्_freq, u32 baudrate)
{
	uint16_t data16;

	scif_आधारः = (चञ्चल वर्ण *)आधारः_2;

	समुच्चयः_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_DIS);
	समुच्चयः_reg(SCIF_REG_FCR, SCIF_FCR_RST_ASSRT_RFTF);

	data16 = get_reg(SCIF_REG_FSR); /* Dummy read */
	समुच्चयः_reg(SCIF_REG_FSR, 0x0); /* Clear all error bit */

	data16 = get_reg(SCIF_REG_LSR); /* Dummy read */
	समुच्चयः_reg(SCIF_REG_LSR, 0x0); /* Clear ORER bit */

	समुच्चयः_reg(SCIF_REG_SCR, 0x0);

	समुच्चयः_reg(SCIF_REG_SMR, 0x0);

	data16 = get_reg(SCIF_REG_SEMR);
	समुच्चयः_reg(SCIF_REG_SEMR, data16 & (~SCIF_SEMR_MDDRS));
	समुच्चयः_reg(SCIF_REG_BRR, SCBRR_मानम्(अन्तर्_freq, baudrate));

	समुच्चयः_reg(SCIF_REG_FCR, SCIF_FCR_RST_NGATE_RFTF);
	समुच्चयः_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_EN);

	sbi_console_समुच्चयः_device(&renesas_scif_console);

	प्रत्यागम 0;
}
