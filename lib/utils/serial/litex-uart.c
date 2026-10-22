/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_RXTX		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_TXFULL		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_RXEMPTY	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_EV_ਸਥਿਤੀ	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_EV_PENDING	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_EV_ENABLE	5

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_EV_RX		0x2

/* clang-format on */

ਸਥਿਰ ਅਸਥਿਰ u32 *uart_ਅਧਾਰ;

ਸਥਿਰ u8 get_reg(u8 reg)
{
	ਵਾਪਸ readb(uart_ਅਧਾਰ + reg);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_reg(u8 reg, u8 val)
{
	writeb(val, uart_ਅਧਾਰ + reg);
}

ਸਥਿਰ ਖਾਲੀ litex_uart_putc(ਅੱਖਰ ch)
{
	ਜਦੋਂ (get_reg(UART_REG_TXFULL));
	ਸੈੱਟ_reg(UART_REG_RXTX, ch);
}

ਸਥਿਰ ਪੂਰਨ litex_uart_getc(ਖਾਲੀ)
{
	ਪੂਰਨ ret;

	ਜੇ (get_reg(UART_REG_RXEMPTY))
		ਵਾਪਸ -1;

	ret = get_reg(UART_REG_RXTX);
	ਸੈੱਟ_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	ਵਾਪਸ ret;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

ਪੂਰਨ litex_uart_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2)
{
	uart_ਅਧਾਰ = (ਅਸਥਿਰ u32 *)ਅਧਾਰ_2;
	ਸੈੱਟ_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_ਸੈੱਟ_device(&litex_console);
	ਵਾਪਸ 0;
}
