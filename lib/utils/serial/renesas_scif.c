// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/serial/renesas-scif.h>

/* clang-format off */

#تعریف_کړه SCIF_REG_SMR		0x0
#تعریف_کړه SCIF_REG_BRR		0x2
#تعریف_کړه SCIF_REG_SCR		0x4
#تعریف_کړه SCIF_REG_FTDR		0x6
#تعریف_کړه SCIF_REG_FSR		0x8
#تعریف_کړه SCIF_REG_FCR		0xc
#تعریف_کړه SCIF_REG_LSR		0x12
#تعریف_کړه SCIF_REG_SEMR		0x14

#تعریف_کړه SCIF_FCR_RFRST		0x2 /* Reset assert receive-FIFO (bit[1]) */
#تعریف_کړه SCIF_FCR_TFRST		0x4 /* Reset assert transmit-FIFO(bit[2]) */

#تعریف_کړه SCIF_FCR_RST_ASSRT_RFTF	(SCIF_FCR_RFRST | SCIF_FCR_TFRST) /* Reset assert tx-FIFO & rx-FIFO */
#تعریف_کړه SCIF_FCR_RST_NGATE_RFTF	0x0 /* Reset negate tx-FIFO & rx-FIFO */

#تعریف_کړه SCIF_SCR_RE		0x10 /* Enable receive (bit[4]) */
#تعریف_کړه SCIF_SCR_TE		0x20 /* Enable transmit(bit[5]) */
#تعریف_کړه SCIF_SCR_RCV_TRN_EN	(SCIF_SCR_RE | SCIF_SCR_TE) /* Enable receive & transmit */
#تعریف_کړه SCIF_SCR_RCV_TRN_DIS	0x0 /* Disable receive & transmit */

#تعریف_کړه SCIF_FSR_ER		0x80 /* Receive error flag */
#تعریف_کړه SCIF_FSR_TEND		0x40 /* Transmit End Flag */
#تعریف_کړه SCIF_FSR_TDFE		0x20 /* Transmit FIFO Data Empty Flag */
#تعریف_کړه SCIF_FSR_BRK		0x10 /* Detect break flag */
#تعریف_کړه SCIF_FSR_DR		0x1  /* Receive data ready flag */

#تعریف_کړه SCIF_FSR_TXD_CHK	(SCIF_FSR_TEND | SCIF_FSR_TDFE)

#تعریف_کړه SCIF_SEMR_MDDRS		0x10 /* MDDR access enable */

#تعریف_کړه SCIF_REG_8_بټ(reg)	((reg == SCIF_REG_BRR) || \
				 (reg == SCIF_REG_FTDR) || \
				 (reg == SCIF_REG_SEMR))

#تعریف_کړه SCBRR_ارزښت(clk, baudrate) ((clk) / (32 * (baudrate)) - 1)

/* clang-format on */

ساكن متطاير حرف *scif_بنسټ;

ساكن u32 get_reg(u32 دننه_واټن)
{
	إذا (SCIF_REG_8_بټ(دننه_واټن))
		أرجع readb(scif_بنسټ + دننه_واټن);

	أرجع readw(scif_بنسټ + دننه_واټن);
}

ساكن فراغ سېټ_reg(u32 دننه_واټن, u32 val)
{
	إذا (SCIF_REG_8_بټ(دننه_واټن))
		writeb(val, scif_بنسټ + دننه_واټن);
	وإلا
		writew(val, scif_بنسټ + دننه_واټن);
}

ساكن فراغ renesas_scif_putc(حرف ch)
{
	uint16_t reg;

	طالما (!(SCIF_FSR_TXD_CHK & get_reg(SCIF_REG_FSR)))
		;

	سېټ_reg(SCIF_REG_FTDR, ch);
	reg = get_reg(SCIF_REG_FSR);
	reg &= ~SCIF_FSR_TXD_CHK;
	سېټ_reg(SCIF_REG_FSR, reg);
}

ساكن بنية sbi_console_device renesas_scif_console = {
	.name		= "renesas_scif",
	.console_putc	= renesas_scif_putc,
};

صحيح renesas_scif_init(بینښه اوږد بنسټ_2, u32 دننه_freq, u32 baudrate)
{
	uint16_t data16;

	scif_بنسټ = (متطاير حرف *)بنسټ_2;

	سېټ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_DIS);
	سېټ_reg(SCIF_REG_FCR, SCIF_FCR_RST_ASSRT_RFTF);

	data16 = get_reg(SCIF_REG_FSR); /* Dummy read */
	سېټ_reg(SCIF_REG_FSR, 0x0); /* Clear all error bit */

	data16 = get_reg(SCIF_REG_LSR); /* Dummy read */
	سېټ_reg(SCIF_REG_LSR, 0x0); /* Clear ORER bit */

	سېټ_reg(SCIF_REG_SCR, 0x0);

	سېټ_reg(SCIF_REG_SMR, 0x0);

	data16 = get_reg(SCIF_REG_SEMR);
	سېټ_reg(SCIF_REG_SEMR, data16 & (~SCIF_SEMR_MDDRS));
	سېټ_reg(SCIF_REG_BRR, SCBRR_ارزښت(دننه_freq, baudrate));

	سېټ_reg(SCIF_REG_FCR, SCIF_FCR_RST_NGATE_RFTF);
	سېټ_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_EN);

	sbi_console_سېټ_device(&renesas_scif_console);

	أرجع 0;
}
