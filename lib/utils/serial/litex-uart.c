/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#define UART_REG_RXTX		0
#define UART_REG_TXFULL		1
#define UART_REG_RXEMPTY	2
#define UART_REG_EV_STATUS	3
#define UART_REG_EV_PENDING	4
#define UART_REG_EV_ENABLE	5

#define UART_EV_RX		0x2

/* clang-format on */

ಸ್ಥಿರ ಚಞ್ಚಲ u32 *uart_base;

ಸ್ಥಿರ u8 get_reg(u8 reg)
{
	ಪ್ರತ್ಯಾಗಮ readb(uart_base + reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ set_reg(u8 reg, u8 val)
{
	writeb(val, uart_base + reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ litex_uart_putc(ವರ್ಣ ch)
{
	ಯಾವತ್ (get_reg(UART_REG_TXFULL));
	set_reg(UART_REG_RXTX, ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ litex_uart_getc(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (get_reg(UART_REG_RXEMPTY))
		ಪ್ರತ್ಯಾಗಮ -1;

	ret = get_reg(UART_REG_RXTX);
	set_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

ಪೂರ್ಣಾಙ್ಕ litex_uart_init(unsigned long base)
{
	uart_base = (ಚಞ್ಚಲ u32 *)base;
	set_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_set_device(&litex_console);
	ಪ್ರತ್ಯಾಗಮ 0;
}
