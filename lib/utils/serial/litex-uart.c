/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#تعریف_کرو UART_REG_RXTX		0
#تعریف_کرو UART_REG_TXFULL		1
#تعریف_کرو UART_REG_RXEMPTY	2
#تعریف_کرو UART_REG_EV_حالت	3
#تعریف_کرو UART_REG_EV_PENDING	4
#تعریف_کرو UART_REG_EV_ENABLE	5

#تعریف_کرو UART_EV_RX		0x2

/* clang-format on */

ساکن متلون u32 *uart_بنیاد;

ساکن u8 get_reg(u8 reg)
{
	واپس readb(uart_بنیاد + reg);
}

ساکن خالی سیٹ_reg(u8 reg, u8 val)
{
	writeb(val, uart_بنیاد + reg);
}

ساکن خالی litex_uart_putc(حرف ch)
{
	جب (get_reg(UART_REG_TXFULL));
	سیٹ_reg(UART_REG_RXTX, ch);
}

ساکن صحیح litex_uart_getc(خالی)
{
	صحیح ret;

	اگر (get_reg(UART_REG_RXEMPTY))
		واپس -1;

	ret = get_reg(UART_REG_RXTX);
	سیٹ_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	واپس ret;
}

ساکن ساخت sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

صحیح litex_uart_init(غیرعلامتی طویل بنیاد_2)
{
	uart_بنیاد = (متلون u32 *)بنیاد_2;
	سیٹ_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_سیٹ_device(&litex_console);
	واپس 0;
}
