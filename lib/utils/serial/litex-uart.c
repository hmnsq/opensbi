/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#define UART_REG_RXTX		0
#define UART_REG_TXFULL		1
#define UART_REG_RXEMPTY	2
#define UART_REG_EV_STATUS	3
#define UART_REG_EV_PENDING	4
#define UART_REG_EV_ENABLE	5

#define UART_EV_RX		0x2

/* clang-format on */

સ્થિર અસ્થિર u32 *uart_base;

સ્થિર u8 get_reg(u8 reg)
{
	પાછા readb(uart_base + reg);
}

સ્થિર શૂન્ય set_reg(u8 reg, u8 val)
{
	writeb(val, uart_base + reg);
}

સ્થિર શૂન્ય litex_uart_putc(અક્ષર ch)
{
	જ્યારે (get_reg(UART_REG_TXFULL));
	set_reg(UART_REG_RXTX, ch);
}

સ્થિર પૂર્ણાંક litex_uart_getc(શૂન્ય)
{
	પૂર્ણાંક ret;

	જો (get_reg(UART_REG_RXEMPTY))
		પાછા -1;

	ret = get_reg(UART_REG_RXTX);
	set_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	પાછા ret;
}

સ્થિર સંરચના sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

પૂર્ણાંક litex_uart_init(unsigned long base)
{
	uart_base = (અસ્થિર u32 *)base;
	set_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_set_device(&litex_console);
	પાછા 0;
}
