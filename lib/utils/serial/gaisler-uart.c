/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/serial/gaisler-uart.h>

/* clang-format off */

#घोषणा UART_REG_तथ्य		0
#घोषणा UART_REG_STATUS		1
#घोषणा UART_REG_CTRL		2
#घोषणा UART_REG_SCALER		3

#घोषणा UART_तथ्य_तथ्य		0x000000ff
#घोषणा UART_STATUS_FIFOFULL	0x00000200
#घोषणा UART_STATUS_DATAREADY	0x00000001

#घोषणा UART_CTRL_DB		(1<<11)
#घोषणा UART_CTRL_FL		(1<<6)
#घोषणा UART_CTRL_TE		(1<<1)
#घोषणा UART_CTRL_RE		(1<<0)

/* clang-format on */

अटल अस्थिर अक्षर *uart_base;

अटल u32 get_reg(u32 अंक)
{
	वापस readl(uart_base + (अंक * 0x4));
}

अटल व्योम set_reg(u32 अंक, u32 val)
{
	writel(val, uart_base + (अंक * 0x4));
}

अटल व्योम gaisler_uart_putc(अक्षर ch)
{
	जबतक (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_तथ्य, ch);
}

अटल पूर्णांक gaisler_uart_getc(व्योम)
{
	u32 ret = get_reg(UART_REG_STATUS);
	अगर (!(ret & UART_STATUS_DATAREADY))
		वापस -1;
	वापस get_reg(UART_REG_तथ्य) & UART_तथ्य_तथ्य;
}

अटल काष्ठा sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

पूर्णांक gaisler_uart_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (अस्थिर अक्षर *)base;

	/* Configure baudrate */
	अगर (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	वापस 0;
}
