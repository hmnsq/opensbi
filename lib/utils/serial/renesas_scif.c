// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/serial/renesas-scif.h>

/* clang-format off */

#وصف_ڪريو SCIF_REG_SMR		0x0
#وصف_ڪريو SCIF_REG_BRR		0x2
#وصف_ڪريو SCIF_REG_SCR		0x4
#وصف_ڪريو SCIF_REG_FTDR		0x6
#وصف_ڪريو SCIF_REG_FSR		0x8
#وصف_ڪريو SCIF_REG_FCR		0xc
#وصف_ڪريو SCIF_REG_LSR		0x12
#وصف_ڪريو SCIF_REG_SEMR		0x14

#وصف_ڪريو SCIF_FCR_RFRST		0x2 /* Reset assert receive-FIFO (bit[1]) */
#وصف_ڪريو SCIF_FCR_TFRST		0x4 /* Reset assert transmit-FIFO(bit[2]) */

#وصف_ڪريو SCIF_FCR_RST_ASSRT_RFTF	(SCIF_FCR_RFRST | SCIF_FCR_TFRST) /* Reset assert tx-FIFO & rx-FIFO */
#وصف_ڪريو SCIF_FCR_RST_NGATE_RFTF	0x0 /* Reset negate tx-FIFO & rx-FIFO */

#وصف_ڪريو SCIF_SCR_RE		0x10 /* Enable receive (bit[4]) */
#وصف_ڪريو SCIF_SCR_TE		0x20 /* Enable transmit(bit[5]) */
#وصف_ڪريو SCIF_SCR_RCV_TRN_EN	(SCIF_SCR_RE | SCIF_SCR_TE) /* Enable receive & transmit */
#وصف_ڪريو SCIF_SCR_RCV_TRN_DIS	0x0 /* Disable receive & transmit */

#وصف_ڪريو SCIF_FSR_ER		0x80 /* Receive error flag */
#وصف_ڪريو SCIF_FSR_TEND		0x40 /* Transmit End Flag */
#وصف_ڪريو SCIF_FSR_TDFE		0x20 /* Transmit FIFO Data Empty Flag */
#وصف_ڪريو SCIF_FSR_BRK		0x10 /* Detect break flag */
#وصف_ڪريو SCIF_FSR_DR		0x1  /* Receive data ready flag */

#وصف_ڪريو SCIF_FSR_TXD_CHK	(SCIF_FSR_TEND | SCIF_FSR_TDFE)

#وصف_ڪريو SCIF_SEMR_MDDRS		0x10 /* MDDR access enable */

#وصف_ڪريو SCIF_REG_8_ٻٽو_انگ(reg)	((reg == SCIF_REG_BRR) || \
				 (reg == SCIF_REG_FTDR) || \
				 (reg == SCIF_REG_SEMR))

#وصف_ڪريو SCBRR_قدر(clk, baudrate) ((clk) / (32 * (baudrate)) - 1)

/* clang-format on */

ساكن متطاير حرف *scif_بنياد;

ساكن u32 get_reg(u32 وٿي)
{
	إذا (SCIF_REG_8_ٻٽو_انگ(وٿي))
		أرجع readb(scif_بنياد + وٿي);

	أرجع readw(scif_بنياد + وٿي);
}

ساكن فراغ سيٽ_reg(u32 وٿي, u32 val)
{
	إذا (SCIF_REG_8_ٻٽو_انگ(وٿي))
		writeb(val, scif_بنياد + وٿي);
	وإلا
		writew(val, scif_بنياد + وٿي);
}

ساكن فراغ renesas_scif_putc(حرف ch)
{
	uint16_t reg;

	طالما (!(SCIF_FSR_TXD_CHK & get_reg(SCIF_REG_FSR)))
		;

	سيٽ_reg(SCIF_REG_FTDR, ch);
	reg = get_reg(SCIF_REG_FSR);
	reg &= ~SCIF_FSR_TXD_CHK;
	سيٽ_reg(SCIF_REG_FSR, reg);
}

ساكن بنية sbi_console_device renesas_scif_console = {
	.name		= "renesas_scif",
	.console_putc	= renesas_scif_putc,
};

صحيح renesas_scif_init(بنانشان ڊگهو بنياد_2, u32 اندريون_freq, u32 baudrate)
{
	uint16_t data16;

	scif_بنياد = (متطاير حرف *)بنياد_2;

	سيٽ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_DIS);
	سيٽ_reg(SCIF_REG_FCR, SCIF_FCR_RST_ASSRT_RFTF);

	data16 = get_reg(SCIF_REG_FSR); /* Dummy read */
	سيٽ_reg(SCIF_REG_FSR, 0x0); /* Clear all error bit */

	data16 = get_reg(SCIF_REG_LSR); /* Dummy read */
	سيٽ_reg(SCIF_REG_LSR, 0x0); /* Clear ORER bit */

	سيٽ_reg(SCIF_REG_SCR, 0x0);

	سيٽ_reg(SCIF_REG_SMR, 0x0);

	data16 = get_reg(SCIF_REG_SEMR);
	سيٽ_reg(SCIF_REG_SEMR, data16 & (~SCIF_SEMR_MDDRS));
	سيٽ_reg(SCIF_REG_BRR, SCBRR_قدر(اندريون_freq, baudrate));

	سيٽ_reg(SCIF_REG_FCR, SCIF_FCR_RST_NGATE_RFTF);
	سيٽ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_EN);

	sbi_console_سيٽ_device(&renesas_scif_console);

	أرجع 0;
}
