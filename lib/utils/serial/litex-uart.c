/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#تعریف_کړه UART_REG_RXTX		0
#تعریف_کړه UART_REG_TXFULL		1
#تعریف_کړه UART_REG_RXEMPTY	2
#تعریف_کړه UART_REG_EV_حالت	3
#تعریف_کړه UART_REG_EV_PENDING	4
#تعریف_کړه UART_REG_EV_ENABLE	5

#تعریف_کړه UART_EV_RX		0x2

/* clang-format on */

ساكن متطاير u32 *uart_بنسټ;

ساكن u8 get_reg(u8 reg)
{
	أرجع readb(uart_بنسټ + reg);
}

ساكن فراغ سېټ_reg(u8 reg, u8 val)
{
	writeb(val, uart_بنسټ + reg);
}

ساكن فراغ litex_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_TXFULL));
	سېټ_reg(UART_REG_RXTX, ch);
}

ساكن صحيح litex_uart_getc(فراغ)
{
	صحيح ret;

	إذا (get_reg(UART_REG_RXEMPTY))
		أرجع -1;

	ret = get_reg(UART_REG_RXTX);
	سېټ_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	أرجع ret;
}

ساكن بنية sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

صحيح litex_uart_init(بینښه اوږد بنسټ_2)
{
	uart_بنسټ = (متطاير u32 *)بنسټ_2;
	سېټ_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_سېټ_device(&litex_console);
	أرجع 0;
}
