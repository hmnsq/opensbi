// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/renesas-scif.h>

/* clang-format off */

#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_SMR		0x0
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_BRR		0x2
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_SCR		0x4
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_FTDR		0x6
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_FSR		0x8
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_FCR		0xc
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_LSR		0x12
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_SEMR		0x14

#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FCR_RFRST		0x2 /* Reset assert receive-FIFO (bit[1]) */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FCR_TFRST		0x4 /* Reset assert transmit-FIFO(bit[2]) */

#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FCR_RST_ASSRT_RFTF	(SCIF_FCR_RFRST | SCIF_FCR_TFRST) /* Reset assert tx-FIFO & rx-FIFO */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FCR_RST_NGATE_RFTF	0x0 /* Reset negate tx-FIFO & rx-FIFO */

#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_SCR_RE		0x10 /* Enable receive (bit[4]) */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_SCR_TE		0x20 /* Enable transmit(bit[5]) */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_SCR_RCV_TRN_EN	(SCIF_SCR_RE | SCIF_SCR_TE) /* Enable receive & transmit */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_SCR_RCV_TRN_DIS	0x0 /* Disable receive & transmit */

#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FSR_ER		0x80 /* Receive error flag */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FSR_TEND		0x40 /* Transmit End Flag */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FSR_TDFE		0x20 /* Transmit FIFO Data Empty Flag */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FSR_BRK		0x10 /* Detect break flag */
#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FSR_DR		0x1  /* Receive data ready flag */

#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_FSR_TXD_CHK	(SCIF_FSR_TEND | SCIF_FSR_TDFE)

#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_SEMR_MDDRS		0x10 /* MDDR access enable */

#ವ್ಯಾಖ್ಯಾನಿಸು SCIF_REG_8_ಬಿಟ್(reg)	((reg == SCIF_REG_BRR) || \
				 (reg == SCIF_REG_FTDR) || \
				 (reg == SCIF_REG_SEMR))

#ವ್ಯಾಖ್ಯಾನಿಸು SCBRR_ಮೌಲ್ಯ(clk, baudrate) ((clk) / (32 * (baudrate)) - 1)

/* clang-format on */

ಸ್ಥಿರ ಅಸ್ಥಿರ ಅಕ್ಷರ *scif_ತಳ;

ಸ್ಥಿರ u32 get_reg(u32 ಸ್ಥಾನಾಂತರ)
{
	ಆದರೆ (SCIF_REG_8_ಬಿಟ್(ಸ್ಥಾನಾಂತರ))
		ಹಿಂತಿರುಗಿ readb(scif_ತಳ + ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ readw(scif_ತಳ + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_reg(u32 ಸ್ಥಾನಾಂತರ, u32 val)
{
	ಆದರೆ (SCIF_REG_8_ಬಿಟ್(ಸ್ಥಾನಾಂತರ))
		writeb(val, scif_ತಳ + ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ
		writew(val, scif_ತಳ + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ renesas_scif_putc(ಅಕ್ಷರ ch)
{
	uint16_t reg;

	ಆಗ (!(SCIF_FSR_TXD_CHK & get_reg(SCIF_REG_FSR)))
		;

	ಗಣ_reg(SCIF_REG_FTDR, ch);
	reg = get_reg(SCIF_REG_FSR);
	reg &= ~SCIF_FSR_TXD_CHK;
	ಗಣ_reg(SCIF_REG_FSR, reg);
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device renesas_scif_console = {
	.name		= "renesas_scif",
	.console_putc	= renesas_scif_putc,
};

ಪೂರ್ಣಾಂಕ renesas_scif_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, u32 ಒಳ_freq, u32 baudrate)
{
	uint16_t data16;

	scif_ತಳ = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)ತಳ_2;

	ಗಣ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_DIS);
	ಗಣ_reg(SCIF_REG_FCR, SCIF_FCR_RST_ASSRT_RFTF);

	data16 = get_reg(SCIF_REG_FSR); /* Dummy read */
	ಗಣ_reg(SCIF_REG_FSR, 0x0); /* Clear all error bit */

	data16 = get_reg(SCIF_REG_LSR); /* Dummy read */
	ಗಣ_reg(SCIF_REG_LSR, 0x0); /* Clear ORER bit */

	ಗಣ_reg(SCIF_REG_SCR, 0x0);

	ಗಣ_reg(SCIF_REG_SMR, 0x0);

	data16 = get_reg(SCIF_REG_SEMR);
	ಗಣ_reg(SCIF_REG_SEMR, data16 & (~SCIF_SEMR_MDDRS));
	ಗಣ_reg(SCIF_REG_BRR, SCBRR_ಮೌಲ್ಯ(ಒಳ_freq, baudrate));

	ಗಣ_reg(SCIF_REG_FCR, SCIF_FCR_RST_NGATE_RFTF);
	ಗಣ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_EN);

	sbi_console_ಗಣ_device(&renesas_scif_console);

	ಹಿಂತಿರುಗಿ 0;
}
