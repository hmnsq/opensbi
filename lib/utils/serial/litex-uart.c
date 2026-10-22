/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#define UART_REG_RXTX		0
#define UART_REG_TXFULL		1
#define UART_REG_RXEMPTY	2
#define UART_REG_EV_STATUS	3
#define UART_REG_EV_PENDING	4
#define UART_REG_EV_ENABLE	5

#define UART_EV_RX		0x2

/* clang-format on */

স্থির চঞ্চল u32 *uart_base;

স্থির u8 get_reg(u8 reg)
{
	প্রত্যাগম readb(uart_base + reg);
}

স্থির শূন্য set_reg(u8 reg, u8 val)
{
	writeb(val, uart_base + reg);
}

স্থির শূন্য litex_uart_putc(বর্ণ ch)
{
	যাবত্ (get_reg(UART_REG_TXFULL));
	set_reg(UART_REG_RXTX, ch);
}

স্থির পূর্ণাঙ্ক litex_uart_getc(শূন্য)
{
	পূর্ণাঙ্ক ret;

	যদি (get_reg(UART_REG_RXEMPTY))
		প্রত্যাগম -1;

	ret = get_reg(UART_REG_RXTX);
	set_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	প্রত্যাগম ret;
}

স্থির সংরচনা sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

পূর্ণাঙ্ক litex_uart_init(unsigned long base)
{
	uart_base = (চঞ্চল u32 *)base;
	set_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_set_device(&litex_console);
	প্রত্যাগম 0;
}
