/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi_utils/serial/gaisler-uart.h>

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

સ્થિર અસ્થિર અક્ષર *uart_base;

સ્થિર u32 get_reg(u32 અંક)
{
	પાછા readl(uart_base + (અંક * 0x4));
}

સ્થિર શૂન્ય set_reg(u32 અંક, u32 val)
{
	writel(val, uart_base + (અંક * 0x4));
}

સ્થિર શૂન્ય gaisler_uart_putc(અક્ષર ch)
{
	જ્યારે (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_DATA, ch);
}

સ્થિર પૂર્ણાંક gaisler_uart_getc(શૂન્ય)
{
	u32 ret = get_reg(UART_REG_STATUS);
	જો (!(ret & UART_STATUS_DATAREADY))
		પાછા -1;
	પાછા get_reg(UART_REG_DATA) & UART_DATA_DATA;
}

સ્થિર સંરચના sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

પૂર્ણાંક gaisler_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (અસ્થિર અક્ષર *)base;

	/* Configure baudrate */
	જો (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	પાછા 0;
}
