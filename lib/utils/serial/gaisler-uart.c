/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi_utils/serial/gaisler-uart.h>

/* clang-format off */

#define UART_REG_DATA		0
#define UART_REG_STATUS		1
#define UART_REG_CTRL		2
#define UART_REG_SCALER		3

#define UART_DATA_DATA		0x000000ff
#define UART_STATUS_FIFOFULL	0x00000200
#define UART_STATUS_DATAREADY	0x00000001

#define UART_CTRL_DB		(1<<11)
#define UART_CTRL_FL		(1<<6)
#define UART_CTRL_TE		(1<<1)
#define UART_CTRL_RE		(1<<0)

/* clang-format on */

സ്ഥിതം അസ്ഥിര അക്ഷരം *uart_base;

സ്ഥിതം u32 get_reg(u32 അക്കം)
{
	തിരികെ readl(uart_base + (അക്കം * 0x4));
}

സ്ഥിതം ശൂന്യം set_reg(u32 അക്കം, u32 val)
{
	writel(val, uart_base + (അക്കം * 0x4));
}

സ്ഥിതം ശൂന്യം gaisler_uart_putc(അക്ഷരം ch)
{
	ആകുമ്പോൾ (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_DATA, ch);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ gaisler_uart_getc(ശൂന്യം)
{
	u32 ret = get_reg(UART_REG_STATUS);
	എങ്കിൽ (!(ret & UART_STATUS_DATAREADY))
		തിരികെ -1;
	തിരികെ get_reg(UART_REG_DATA) & UART_DATA_DATA;
}

സ്ഥിതം ഘടന sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

പൂർണ്ണസംഖ്യ gaisler_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (അസ്ഥിര അക്ഷരം *)base;

	/* Configure baudrate */
	എങ്കിൽ (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	തിരികെ 0;
}
