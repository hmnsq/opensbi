/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#define UART_REG_RXTX		0
#define UART_REG_TXFULL		1
#define UART_REG_RXEMPTY	2
#define UART_REG_EV_STATUS	3
#define UART_REG_EV_PENDING	4
#define UART_REG_EV_ENABLE	5

#define UART_EV_RX		0x2

/* clang-format on */

நிலையான நிலையற்ற u32 *uart_base;

நிலையான u8 get_reg(u8 reg)
{
	திரும்பு readb(uart_base + reg);
}

நிலையான வெறுமை set_reg(u8 reg, u8 val)
{
	writeb(val, uart_base + reg);
}

நிலையான வெறுமை litex_uart_putc(எழுத்து ch)
{
	போது (get_reg(UART_REG_TXFULL));
	set_reg(UART_REG_RXTX, ch);
}

நிலையான முழு litex_uart_getc(வெறுமை)
{
	முழு ret;

	எனில் (get_reg(UART_REG_RXEMPTY))
		திரும்பு -1;

	ret = get_reg(UART_REG_RXTX);
	set_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	திரும்பு ret;
}

நிலையான கட்டமைப்பு sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

முழு litex_uart_init(unsigned long base)
{
	uart_base = (நிலையற்ற u32 *)base;
	set_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_set_device(&litex_console);
	திரும்பு 0;
}
