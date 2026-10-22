/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi_utils/serial/gaisler-uart.h>

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

স্থির চঞ্চল বর্ণ *uart_base;

স্থির u32 get_reg(u32 অঙ্ক)
{
	প্রত্যাগম readl(uart_base + (অঙ্ক * 0x4));
}

স্থির শূন্য set_reg(u32 অঙ্ক, u32 val)
{
	writel(val, uart_base + (অঙ্ক * 0x4));
}

স্থির শূন্য gaisler_uart_putc(বর্ণ ch)
{
	যাবত্ (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_DATA, ch);
}

স্থির পূর্ণাঙ্ক gaisler_uart_getc(শূন্য)
{
	u32 ret = get_reg(UART_REG_STATUS);
	যদি (!(ret & UART_STATUS_DATAREADY))
		প্রত্যাগম -1;
	প্রত্যাগম get_reg(UART_REG_DATA) & UART_DATA_DATA;
}

স্থির সংরচনা sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

পূর্ণাঙ্ক gaisler_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (চঞ্চল বর্ণ *)base;

	/* Configure baudrate */
	যদি (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	প্রত্যাগম 0;
}
