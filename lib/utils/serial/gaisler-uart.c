/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/gaisler-uart.h>

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

ସ୍ଥିର ଅସ୍ଥିର ଅକ୍ଷର *uart_base;

ସ୍ଥିର u32 get_reg(u32 ଅଙ୍କ)
{
	ଫେରନ୍ତୁ readl(uart_base + (ଅଙ୍କ * 0x4));
}

ସ୍ଥିର ଶୂନ୍ୟ set_reg(u32 ଅଙ୍କ, u32 val)
{
	writel(val, uart_base + (ଅଙ୍କ * 0x4));
}

ସ୍ଥିର ଶୂନ୍ୟ gaisler_uart_putc(ଅକ୍ଷର ch)
{
	ଯେତେବେଳେ (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_DATA, ch);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gaisler_uart_getc(ଶୂନ୍ୟ)
{
	u32 ret = get_reg(UART_REG_STATUS);
	ଯଦି (!(ret & UART_STATUS_DATAREADY))
		ଫେରନ୍ତୁ -1;
	ଫେରନ୍ତୁ get_reg(UART_REG_DATA) & UART_DATA_DATA;
}

ସ୍ଥିର ସଂରଚନା sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା gaisler_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (ଅସ୍ଥିର ଅକ୍ଷର *)base;

	/* Configure baudrate */
	ଯଦି (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	ଫେରନ୍ତୁ 0;
}
