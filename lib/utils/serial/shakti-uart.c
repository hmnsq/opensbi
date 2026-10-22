/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/shakti-uart.h>

#تعریف_کړه REG_BAUD	0x00
#تعریف_کړه REG_TX		0x04
#تعریف_کړه REG_RX		0x08
#تعریف_کړه REG_حالت	0x0C
#تعریف_کړه REG_DELAY	0x10
#تعریف_کړه REG_CONTROL	0x14
#تعریف_کړه REG_INT_EN	0x18
#تعریف_کړه REG_IQ_دورې	0x1C
#تعریف_کړه REG_RX_THRES	0x20

#تعریف_کړه UART_TX_ډک  0x2
#تعریف_کړه UART_RX_NOT_تشه 0x4
#تعریف_کړه UART_RX_ډک  0x8

ساكن متطاير حرف *uart_بنسټ;

ساكن فراغ shakti_uart_putc(حرف ch)
{
	طالما ((readb(uart_بنسټ + REG_حالت) & UART_TX_ډک))
		;
	writeb(ch, uart_بنسټ + REG_TX);
}

ساكن صحيح shakti_uart_getc(فراغ)
{
	إذا (readb(uart_بنسټ + REG_حالت) & UART_RX_NOT_تشه)
		أرجع readb(uart_بنسټ + REG_RX);
	أرجع -1;
}

ساكن بنية sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

صحيح shakti_uart_init(بینښه اوږد بنسټ_2, u32 دننه_freq, u32 baudrate)
{
	uart_بنسټ = (متطاير حرف *)بنسټ_2;
	u16 baud;

	إذا (baudrate) {
		baud = (u16)(دننه_freq / (16 * baudrate));
		writew(baud, uart_بنسټ + REG_BAUD);
	}

	sbi_console_سېټ_device(&shakti_console);

	أرجع 0;
}
