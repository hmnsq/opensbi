// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/renesas-scif.h>

/* clang-format off */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_SMR		0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_BRR		0x2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_SCR		0x4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_FTDR		0x6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_FSR		0x8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_FCR		0xc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_LSR		0x12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_SEMR		0x14

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FCR_RFRST		0x2 /* Reset assert receive-FIFO (bit[1]) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FCR_TFRST		0x4 /* Reset assert transmit-FIFO(bit[2]) */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FCR_RST_ASSRT_RFTF	(SCIF_FCR_RFRST | SCIF_FCR_TFRST) /* Reset assert tx-FIFO & rx-FIFO */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FCR_RST_NGATE_RFTF	0x0 /* Reset negate tx-FIFO & rx-FIFO */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_SCR_RE		0x10 /* Enable receive (bit[4]) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_SCR_TE		0x20 /* Enable transmit(bit[5]) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_SCR_RCV_TRN_EN	(SCIF_SCR_RE | SCIF_SCR_TE) /* Enable receive & transmit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_SCR_RCV_TRN_DIS	0x0 /* Disable receive & transmit */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FSR_ER		0x80 /* Receive error flag */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FSR_TEND		0x40 /* Transmit End Flag */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FSR_TDFE		0x20 /* Transmit FIFO Data Empty Flag */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FSR_BRK		0x10 /* Detect break flag */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FSR_DR		0x1  /* Receive data ready flag */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_FSR_TXD_CHK	(SCIF_FSR_TEND | SCIF_FSR_TDFE)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_SEMR_MDDRS		0x10 /* MDDR access enable */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCIF_REG_8_ਦੋਆਧਾਰੀ_ਅੰਕ(reg)	((reg == SCIF_REG_BRR) || \
				 (reg == SCIF_REG_FTDR) || \
				 (reg == SCIF_REG_SEMR))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCBRR_ਮੁੱਲ(clk, baudrate) ((clk) / (32 * (baudrate)) - 1)

/* clang-format on */

ਸਥਿਰ ਅਸਥਿਰ ਅੱਖਰ *scif_ਅਧਾਰ;

ਸਥਿਰ u32 get_reg(u32 ਵਿੱਥ)
{
	ਜੇ (SCIF_REG_8_ਦੋਆਧਾਰੀ_ਅੰਕ(ਵਿੱਥ))
		ਵਾਪਸ readb(scif_ਅਧਾਰ + ਵਿੱਥ);

	ਵਾਪਸ readw(scif_ਅਧਾਰ + ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_reg(u32 ਵਿੱਥ, u32 val)
{
	ਜੇ (SCIF_REG_8_ਦੋਆਧਾਰੀ_ਅੰਕ(ਵਿੱਥ))
		writeb(val, scif_ਅਧਾਰ + ਵਿੱਥ);
	ਨਹੀਂਤਾਂ
		writew(val, scif_ਅਧਾਰ + ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ renesas_scif_putc(ਅੱਖਰ ch)
{
	uint16_t reg;

	ਜਦੋਂ (!(SCIF_FSR_TXD_CHK & get_reg(SCIF_REG_FSR)))
		;

	ਸੈੱਟ_reg(SCIF_REG_FTDR, ch);
	reg = get_reg(SCIF_REG_FSR);
	reg &= ~SCIF_FSR_TXD_CHK;
	ਸੈੱਟ_reg(SCIF_REG_FSR, reg);
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device renesas_scif_console = {
	.name		= "renesas_scif",
	.console_putc	= renesas_scif_putc,
};

ਪੂਰਨ renesas_scif_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, u32 ਵਿੱਚ_freq, u32 baudrate)
{
	uint16_t data16;

	scif_ਅਧਾਰ = (ਅਸਥਿਰ ਅੱਖਰ *)ਅਧਾਰ_2;

	ਸੈੱਟ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_DIS);
	ਸੈੱਟ_reg(SCIF_REG_FCR, SCIF_FCR_RST_ASSRT_RFTF);

	data16 = get_reg(SCIF_REG_FSR); /* Dummy read */
	ਸੈੱਟ_reg(SCIF_REG_FSR, 0x0); /* Clear all error bit */

	data16 = get_reg(SCIF_REG_LSR); /* Dummy read */
	ਸੈੱਟ_reg(SCIF_REG_LSR, 0x0); /* Clear ORER bit */

	ਸੈੱਟ_reg(SCIF_REG_SCR, 0x0);

	ਸੈੱਟ_reg(SCIF_REG_SMR, 0x0);

	data16 = get_reg(SCIF_REG_SEMR);
	ਸੈੱਟ_reg(SCIF_REG_SEMR, data16 & (~SCIF_SEMR_MDDRS));
	ਸੈੱਟ_reg(SCIF_REG_BRR, SCBRR_ਮੁੱਲ(ਵਿੱਚ_freq, baudrate));

	ਸੈੱਟ_reg(SCIF_REG_FCR, SCIF_FCR_RST_NGATE_RFTF);
	ਸੈੱਟ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_EN);

	sbi_console_ਸੈੱਟ_device(&renesas_scif_console);

	ਵਾਪਸ 0;
}
