// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/serial/renesas-scif.h>

/* clang-format off */

#تعریف_کرو SCIF_REG_SMR		0x0
#تعریف_کرو SCIF_REG_BRR		0x2
#تعریف_کرو SCIF_REG_SCR		0x4
#تعریف_کرو SCIF_REG_FTDR		0x6
#تعریف_کرو SCIF_REG_FSR		0x8
#تعریف_کرو SCIF_REG_FCR		0xc
#تعریف_کرو SCIF_REG_LSR		0x12
#تعریف_کرو SCIF_REG_SEMR		0x14

#تعریف_کرو SCIF_FCR_RFRST		0x2 /* Reset assert receive-FIFO (bit[1]) */
#تعریف_کرو SCIF_FCR_TFRST		0x4 /* Reset assert transmit-FIFO(bit[2]) */

#تعریف_کرو SCIF_FCR_RST_ASSRT_RFTF	(SCIF_FCR_RFRST | SCIF_FCR_TFRST) /* Reset assert tx-FIFO & rx-FIFO */
#تعریف_کرو SCIF_FCR_RST_NGATE_RFTF	0x0 /* Reset negate tx-FIFO & rx-FIFO */

#تعریف_کرو SCIF_SCR_RE		0x10 /* Enable receive (bit[4]) */
#تعریف_کرو SCIF_SCR_TE		0x20 /* Enable transmit(bit[5]) */
#تعریف_کرو SCIF_SCR_RCV_TRN_EN	(SCIF_SCR_RE | SCIF_SCR_TE) /* Enable receive & transmit */
#تعریف_کرو SCIF_SCR_RCV_TRN_DIS	0x0 /* Disable receive & transmit */

#تعریف_کرو SCIF_FSR_ER		0x80 /* Receive error flag */
#تعریف_کرو SCIF_FSR_TEND		0x40 /* Transmit End Flag */
#تعریف_کرو SCIF_FSR_TDFE		0x20 /* Transmit FIFO Data Empty Flag */
#تعریف_کرو SCIF_FSR_BRK		0x10 /* Detect break flag */
#تعریف_کرو SCIF_FSR_DR		0x1  /* Receive data ready flag */

#تعریف_کرو SCIF_FSR_TXD_CHK	(SCIF_FSR_TEND | SCIF_FSR_TDFE)

#تعریف_کرو SCIF_SEMR_MDDRS		0x10 /* MDDR access enable */

#تعریف_کرو SCIF_REG_8_ثنائی_ہندسہ(reg)	((reg == SCIF_REG_BRR) || \
				 (reg == SCIF_REG_FTDR) || \
				 (reg == SCIF_REG_SEMR))

#تعریف_کرو SCBRR_قدر(clk, baudrate) ((clk) / (32 * (baudrate)) - 1)

/* clang-format on */

ساكن متطاير حرف *scif_ادھار;

ساكن u32 get_reg(u32 وتھ)
{
	إذا (SCIF_REG_8_ثنائی_ہندسہ(وتھ))
		أرجع readb(scif_ادھار + وتھ);

	أرجع readw(scif_ادھار + وتھ);
}

ساكن فراغ سیٹ_reg(u32 وتھ, u32 val)
{
	إذا (SCIF_REG_8_ثنائی_ہندسہ(وتھ))
		writeb(val, scif_ادھار + وتھ);
	وإلا
		writew(val, scif_ادھار + وتھ);
}

ساكن فراغ renesas_scif_putc(حرف ch)
{
	uint16_t reg;

	طالما (!(SCIF_FSR_TXD_CHK & get_reg(SCIF_REG_FSR)))
		;

	سیٹ_reg(SCIF_REG_FTDR, ch);
	reg = get_reg(SCIF_REG_FSR);
	reg &= ~SCIF_FSR_TXD_CHK;
	سیٹ_reg(SCIF_REG_FSR, reg);
}

ساكن بنية sbi_console_device renesas_scif_console = {
	.name		= "renesas_scif",
	.console_putc	= renesas_scif_putc,
};

صحيح renesas_scif_init(بنانشانی لما ادھار_2, u32 وچ_freq, u32 baudrate)
{
	uint16_t data16;

	scif_ادھار = (متطاير حرف *)ادھار_2;

	سیٹ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_DIS);
	سیٹ_reg(SCIF_REG_FCR, SCIF_FCR_RST_ASSRT_RFTF);

	data16 = get_reg(SCIF_REG_FSR); /* Dummy read */
	سیٹ_reg(SCIF_REG_FSR, 0x0); /* Clear all error bit */

	data16 = get_reg(SCIF_REG_LSR); /* Dummy read */
	سیٹ_reg(SCIF_REG_LSR, 0x0); /* Clear ORER bit */

	سیٹ_reg(SCIF_REG_SCR, 0x0);

	سیٹ_reg(SCIF_REG_SMR, 0x0);

	data16 = get_reg(SCIF_REG_SEMR);
	سیٹ_reg(SCIF_REG_SEMR, data16 & (~SCIF_SEMR_MDDRS));
	سیٹ_reg(SCIF_REG_BRR, SCBRR_قدر(وچ_freq, baudrate));

	سیٹ_reg(SCIF_REG_FCR, SCIF_FCR_RST_NGATE_RFTF);
	سیٹ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_EN);

	sbi_console_سیٹ_device(&renesas_scif_console);

	أرجع 0;
}
