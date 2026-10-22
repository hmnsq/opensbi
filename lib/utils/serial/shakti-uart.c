/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/shakti-uart.h>

#تعریف_کن REG_BAUD	0x00
#تعریف_کن REG_TX		0x04
#تعریف_کن REG_RX		0x08
#تعریف_کن REG_وضعیت	0x0C
#تعریف_کن REG_DELAY	0x10
#تعریف_کن REG_CONTROL	0x14
#تعریف_کن REG_INT_EN	0x18
#تعریف_کن REG_IQ_چرخهها	0x1C
#تعریف_کن REG_RX_THRES	0x20

#تعریف_کن UART_TX_پر  0x2
#تعریف_کن UART_RX_NOT_خالی 0x4
#تعریف_کن UART_RX_پر  0x8

ساكن متطاير حرف *uart_پایه;

ساكن فراغ shakti_uart_putc(حرف ch)
{
	طالما ((readb(uart_پایه + REG_وضعیت) & UART_TX_پر))
		;
	writeb(ch, uart_پایه + REG_TX);
}

ساكن صحيح shakti_uart_getc(فراغ)
{
	إذا (readb(uart_پایه + REG_وضعیت) & UART_RX_NOT_خالی)
		أرجع readb(uart_پایه + REG_RX);
	أرجع -1;
}

ساكن بنية sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

صحيح shakti_uart_init(بیعلامت دراز پایه_2, u32 درون_freq, u32 baudrate)
{
	uart_پایه = (متطاير حرف *)پایه_2;
	u16 baud;

	إذا (baudrate) {
		baud = (u16)(درون_freq / (16 * baudrate));
		writew(baud, uart_پایه + REG_BAUD);
	}

	sbi_console_مجموعه_device(&shakti_console);

	أرجع 0;
}
