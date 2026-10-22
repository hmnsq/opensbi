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

#تعریف_کن UART_REG_RXTX		0
#تعریف_کن UART_REG_TXFULL		1
#تعریف_کن UART_REG_RXEMPTY	2
#تعریف_کن UART_REG_EV_وضعیت	3
#تعریف_کن UART_REG_EV_PENDING	4
#تعریف_کن UART_REG_EV_ENABLE	5

#تعریف_کن UART_EV_RX		0x2

/* clang-format on */

ساكن متطاير u32 *uart_پایه;

ساكن u8 get_reg(u8 reg)
{
	أرجع readb(uart_پایه + reg);
}

ساكن فراغ مجموعه_reg(u8 reg, u8 val)
{
	writeb(val, uart_پایه + reg);
}

ساكن فراغ litex_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_TXFULL));
	مجموعه_reg(UART_REG_RXTX, ch);
}

ساكن صحيح litex_uart_getc(فراغ)
{
	صحيح ret;

	إذا (get_reg(UART_REG_RXEMPTY))
		أرجع -1;

	ret = get_reg(UART_REG_RXTX);
	مجموعه_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	أرجع ret;
}

ساكن بنية sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

صحيح litex_uart_init(بیعلامت دراز پایه_2)
{
	uart_پایه = (متطاير u32 *)پایه_2;
	مجموعه_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_مجموعه_device(&litex_console);
	أرجع 0;
}
