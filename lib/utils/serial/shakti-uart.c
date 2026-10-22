/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/serial/shakti-uart.h>

#تعریف_کرو REG_BAUD	0x00
#تعریف_کرو REG_TX		0x04
#تعریف_کرو REG_RX		0x08
#تعریف_کرو REG_حالت	0x0C
#تعریف_کرو REG_DELAY	0x10
#تعریف_کرو REG_CONTROL	0x14
#تعریف_کرو REG_INT_EN	0x18
#تعریف_کرو REG_IQ_ادوار	0x1C
#تعریف_کرو REG_RX_THRES	0x20

#تعریف_کرو UART_TX_بھرا  0x2
#تعریف_کرو UART_RX_NOT_خالی_خانے 0x4
#تعریف_کرو UART_RX_بھرا  0x8

ساکن متلون حرف *uart_بنیاد;

ساکن خالی shakti_uart_putc(حرف ch)
{
	جب ((readb(uart_بنیاد + REG_حالت) & UART_TX_بھرا))
		;
	writeb(ch, uart_بنیاد + REG_TX);
}

ساکن صحیح shakti_uart_getc(خالی)
{
	اگر (readb(uart_بنیاد + REG_حالت) & UART_RX_NOT_خالی_خانے)
		واپس readb(uart_بنیاد + REG_RX);
	واپس -1;
}

ساکن ساخت sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

صحیح shakti_uart_init(غیرعلامتی طویل بنیاد_2, u32 اندر_freq, u32 baudrate)
{
	uart_بنیاد = (متلون حرف *)بنیاد_2;
	u16 baud;

	اگر (baudrate) {
		baud = (u16)(اندر_freq / (16 * baudrate));
		writew(baud, uart_بنیاد + REG_BAUD);
	}

	sbi_console_سیٹ_device(&shakti_console);

	واپس 0;
}
