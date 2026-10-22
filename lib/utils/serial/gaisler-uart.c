/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/gaisler-uart.h>

/* clang-format off */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_ਅੰਕੜੇ		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_ਸਥਿਤੀ		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_CTRL		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_SCALER		3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_ਅੰਕੜੇ_ਅੰਕੜੇ		0x000000ff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_ਸਥਿਤੀ_FIFOFULL	0x00000200
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_ਸਥਿਤੀ_DATAREADY	0x00000001

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_DB		(1<<11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_FL		(1<<6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_TE		(1<<1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_RE		(1<<0)

/* clang-format on */

ਸਥਿਰ ਅਸਥਿਰ ਅੱਖਰ *uart_ਅਧਾਰ;

ਸਥਿਰ u32 get_reg(u32 ਅੰਕ)
{
	ਵਾਪਸ readl(uart_ਅਧਾਰ + (ਅੰਕ * 0x4));
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_reg(u32 ਅੰਕ, u32 val)
{
	writel(val, uart_ਅਧਾਰ + (ਅੰਕ * 0x4));
}

ਸਥਿਰ ਖਾਲੀ gaisler_uart_putc(ਅੱਖਰ ch)
{
	ਜਦੋਂ (get_reg(UART_REG_ਸਥਿਤੀ) & UART_ਸਥਿਤੀ_FIFOFULL)
		;

	ਸੈੱਟ_reg(UART_REG_ਅੰਕੜੇ, ch);
}

ਸਥਿਰ ਪੂਰਨ gaisler_uart_getc(ਖਾਲੀ)
{
	u32 ret = get_reg(UART_REG_ਸਥਿਤੀ);
	ਜੇ (!(ret & UART_ਸਥਿਤੀ_DATAREADY))
		ਵਾਪਸ -1;
	ਵਾਪਸ get_reg(UART_REG_ਅੰਕੜੇ) & UART_ਅੰਕੜੇ_ਅੰਕੜੇ;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

ਪੂਰਨ gaisler_uart_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, u32 ਵਿੱਚ_freq, u32 baudrate)
{
	u32 ctrl;

	uart_ਅਧਾਰ = (ਅਸਥਿਰ ਅੱਖਰ *)ਅਧਾਰ_2;

	/* Configure baudrate */
	ਜੇ (ਵਿੱਚ_freq && baudrate)
		ਸੈੱਟ_reg(UART_REG_SCALER, ਵਿੱਚ_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	ਸੈੱਟ_reg(UART_REG_CTRL, ctrl);

	sbi_console_ਸੈੱਟ_device(&gaisler_console);

	ਵਾਪਸ 0;
}
