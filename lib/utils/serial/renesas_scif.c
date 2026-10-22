// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/serial/renesas-scif.h>

/* clang-format off */

#घोषणा SCIF_REG_SMR		0x0
#घोषणा SCIF_REG_BRR		0x2
#घोषणा SCIF_REG_SCR		0x4
#घोषणा SCIF_REG_FTDR		0x6
#घोषणा SCIF_REG_FSR		0x8
#घोषणा SCIF_REG_FCR		0xc
#घोषणा SCIF_REG_LSR		0x12
#घोषणा SCIF_REG_SEMR		0x14

#घोषणा SCIF_FCR_RFRST		0x2 /* Reset assert receive-FIFO (bit[1]) */
#घोषणा SCIF_FCR_TFRST		0x4 /* Reset assert transmit-FIFO(bit[2]) */

#घोषणा SCIF_FCR_RST_ASSRT_RFTF	(SCIF_FCR_RFRST | SCIF_FCR_TFRST) /* Reset assert tx-FIFO & rx-FIFO */
#घोषणा SCIF_FCR_RST_NGATE_RFTF	0x0 /* Reset negate tx-FIFO & rx-FIFO */

#घोषणा SCIF_SCR_RE		0x10 /* Enable receive (bit[4]) */
#घोषणा SCIF_SCR_TE		0x20 /* Enable transmit(bit[5]) */
#घोषणा SCIF_SCR_RCV_TRN_EN	(SCIF_SCR_RE | SCIF_SCR_TE) /* Enable receive & transmit */
#घोषणा SCIF_SCR_RCV_TRN_DIS	0x0 /* Disable receive & transmit */

#घोषणा SCIF_FSR_ER		0x80 /* Receive error flag */
#घोषणा SCIF_FSR_TEND		0x40 /* Transmit End Flag */
#घोषणा SCIF_FSR_TDFE		0x20 /* Transmit FIFO Data Empty Flag */
#घोषणा SCIF_FSR_BRK		0x10 /* Detect break flag */
#घोषणा SCIF_FSR_DR		0x1  /* Receive data ready flag */

#घोषणा SCIF_FSR_TXD_CHK	(SCIF_FSR_TEND | SCIF_FSR_TDFE)

#घोषणा SCIF_SEMR_MDDRS		0x10 /* MDDR access enable */

#घोषणा SCIF_REG_8BIT(reg)	((reg == SCIF_REG_BRR) || \
				 (reg == SCIF_REG_FTDR) || \
				 (reg == SCIF_REG_SEMR))

#घोषणा SCBRR_मूल्य(clk, baudrate) ((clk) / (32 * (baudrate)) - 1)

/* clang-format on */

अटल अस्थिर अक्षर *scif_base;

अटल u32 get_reg(u32 offset)
{
	अगर (SCIF_REG_8BIT(offset))
		वापस readb(scif_base + offset);

	वापस readw(scif_base + offset);
}

अटल व्योम set_reg(u32 offset, u32 val)
{
	अगर (SCIF_REG_8BIT(offset))
		writeb(val, scif_base + offset);
	अन्यथा
		writew(val, scif_base + offset);
}

अटल व्योम renesas_scif_putc(अक्षर ch)
{
	uint16_t reg;

	जबतक (!(SCIF_FSR_TXD_CHK & get_reg(SCIF_REG_FSR)))
		;

	set_reg(SCIF_REG_FTDR, ch);
	reg = get_reg(SCIF_REG_FSR);
	reg &= ~SCIF_FSR_TXD_CHK;
	set_reg(SCIF_REG_FSR, reg);
}

अटल काष्ठा sbi_console_device renesas_scif_console = {
	.name		= "renesas_scif",
	.console_putc	= renesas_scif_putc,
};

पूर्णांक renesas_scif_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate)
{
	uint16_t data16;

	scif_base = (अस्थिर अक्षर *)base;

	set_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_DIS);
	set_reg(SCIF_REG_FCR, SCIF_FCR_RST_ASSRT_RFTF);

	data16 = get_reg(SCIF_REG_FSR); /* Dummy read */
	set_reg(SCIF_REG_FSR, 0x0); /* Clear all error bit */

	data16 = get_reg(SCIF_REG_LSR); /* Dummy read */
	set_reg(SCIF_REG_LSR, 0x0); /* Clear ORER bit */

	set_reg(SCIF_REG_SCR, 0x0);

	set_reg(SCIF_REG_SMR, 0x0);

	data16 = get_reg(SCIF_REG_SEMR);
	set_reg(SCIF_REG_SEMR, data16 & (~SCIF_SEMR_MDDRS));
	set_reg(SCIF_REG_BRR, SCBRR_मूल्य(in_freq, baudrate));

	set_reg(SCIF_REG_FCR, SCIF_FCR_RST_NGATE_RFTF);
	set_reg(SCIF_REG_SCR, SCIF_SCR_RCV_TRN_EN);

	sbi_console_set_device(&renesas_scif_console);

	वापस 0;
}
