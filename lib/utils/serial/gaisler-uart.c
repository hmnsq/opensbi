/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi_utils/serial/gaisler-uart.h>

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

நிலையான நிலையற்ற எழுத்து *uart_base;

நிலையான u32 get_reg(u32 இலக்கம்)
{
	திரும்பு readl(uart_base + (இலக்கம் * 0x4));
}

நிலையான வெறுமை set_reg(u32 இலக்கம், u32 val)
{
	writel(val, uart_base + (இலக்கம் * 0x4));
}

நிலையான வெறுமை gaisler_uart_putc(எழுத்து ch)
{
	போது (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_DATA, ch);
}

நிலையான முழு gaisler_uart_getc(வெறுமை)
{
	u32 ret = get_reg(UART_REG_STATUS);
	எனில் (!(ret & UART_STATUS_DATAREADY))
		திரும்பு -1;
	திரும்பு get_reg(UART_REG_DATA) & UART_DATA_DATA;
}

நிலையான கட்டமைப்பு sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

முழு gaisler_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (நிலையற்ற எழுத்து *)base;

	/* Configure baudrate */
	எனில் (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	திரும்பு 0;
}
