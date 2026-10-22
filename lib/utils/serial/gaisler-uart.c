/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/gaisler-uart.h>

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

ಸ್ಥಿರ ಚಞ್ಚಲ ವರ್ಣ *uart_base;

ಸ್ಥಿರ u32 get_reg(u32 ಅಂಕೆ)
{
	ಪ್ರತ್ಯಾಗಮ readl(uart_base + (ಅಂಕೆ * 0x4));
}

ಸ್ಥಿರ ಶೂನ್ಯ set_reg(u32 ಅಂಕೆ, u32 val)
{
	writel(val, uart_base + (ಅಂಕೆ * 0x4));
}

ಸ್ಥಿರ ಶೂನ್ಯ gaisler_uart_putc(ವರ್ಣ ch)
{
	ಯಾವತ್ (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_DATA, ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gaisler_uart_getc(ಶೂನ್ಯ)
{
	u32 ret = get_reg(UART_REG_STATUS);
	ಯದಿ (!(ret & UART_STATUS_DATAREADY))
		ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ get_reg(UART_REG_DATA) & UART_DATA_DATA;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

ಪೂರ್ಣಾಙ್ಕ gaisler_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (ಚಞ್ಚಲ ವರ್ಣ *)base;

	/* Configure baudrate */
	ಯದಿ (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	ಪ್ರತ್ಯಾಗಮ 0;
}
