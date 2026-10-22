/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#define UART_REG_RXTX		0
#define UART_REG_TXFULL		1
#define UART_REG_RXEMPTY	2
#define UART_REG_EV_STATUS	3
#define UART_REG_EV_PENDING	4
#define UART_REG_EV_ENABLE	5

#define UART_EV_RX		0x2

/* clang-format on */

സ്ഥിതം അസ്ഥിര u32 *uart_base;

സ്ഥിതം u8 get_reg(u8 reg)
{
	തിരികെ readb(uart_base + reg);
}

സ്ഥിതം ശൂന്യം set_reg(u8 reg, u8 val)
{
	writeb(val, uart_base + reg);
}

സ്ഥിതം ശൂന്യം litex_uart_putc(അക്ഷരം ch)
{
	ആകുമ്പോൾ (get_reg(UART_REG_TXFULL));
	set_reg(UART_REG_RXTX, ch);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ litex_uart_getc(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (get_reg(UART_REG_RXEMPTY))
		തിരികെ -1;

	ret = get_reg(UART_REG_RXTX);
	set_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	തിരികെ ret;
}

സ്ഥിതം ഘടന sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

പൂർണ്ണസംഖ്യ litex_uart_init(unsigned long base)
{
	uart_base = (അസ്ഥിര u32 *)base;
	set_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_set_device(&litex_console);
	തിരികെ 0;
}
