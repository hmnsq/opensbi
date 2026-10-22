/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#define UART_REG_RXTX		0
#define UART_REG_TXFULL		1
#define UART_REG_RXEMPTY	2
#define UART_REG_EV_STATUS	3
#define UART_REG_EV_PENDING	4
#define UART_REG_EV_ENABLE	5

#define UART_EV_RX		0x2

/* clang-format on */

ସ୍ଥିର ଅସ୍ଥିର u32 *uart_base;

ସ୍ଥିର u8 get_reg(u8 reg)
{
	ଫେରନ୍ତୁ readb(uart_base + reg);
}

ସ୍ଥିର ଶୂନ୍ୟ set_reg(u8 reg, u8 val)
{
	writeb(val, uart_base + reg);
}

ସ୍ଥିର ଶୂନ୍ୟ litex_uart_putc(ଅକ୍ଷର ch)
{
	ଯେତେବେଳେ (get_reg(UART_REG_TXFULL));
	set_reg(UART_REG_RXTX, ch);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା litex_uart_getc(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି (get_reg(UART_REG_RXEMPTY))
		ଫେରନ୍ତୁ -1;

	ret = get_reg(UART_REG_RXTX);
	set_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ସଂରଚନା sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା litex_uart_init(unsigned long base)
{
	uart_base = (ଅସ୍ଥିର u32 *)base;
	set_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_set_device(&litex_console);
	ଫେରନ୍ତୁ 0;
}
