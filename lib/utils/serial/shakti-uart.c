/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/shakti-uart.h>

#وصف_ڪريو REG_BAUD	0x00
#وصف_ڪريو REG_TX		0x04
#وصف_ڪريو REG_RX		0x08
#وصف_ڪريو REG_حالت	0x0C
#وصف_ڪريو REG_DELAY	0x10
#وصف_ڪريو REG_CONTROL	0x14
#وصف_ڪريو REG_INT_EN	0x18
#وصف_ڪريو REG_IQ_چڪرن	0x1C
#وصف_ڪريو REG_RX_THRES	0x20

#وصف_ڪريو UART_TX_ڀريل  0x2
#وصف_ڪريو UART_RX_NOT_خالي_جايون 0x4
#وصف_ڪريو UART_RX_ڀريل  0x8

ساكن متطاير حرف *uart_بنياد;

ساكن فراغ shakti_uart_putc(حرف ch)
{
	طالما ((readb(uart_بنياد + REG_حالت) & UART_TX_ڀريل))
		;
	writeb(ch, uart_بنياد + REG_TX);
}

ساكن صحيح shakti_uart_getc(فراغ)
{
	إذا (readb(uart_بنياد + REG_حالت) & UART_RX_NOT_خالي_جايون)
		أرجع readb(uart_بنياد + REG_RX);
	أرجع -1;
}

ساكن بنية sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

صحيح shakti_uart_init(بنانشان ڊگهو بنياد_2, u32 اندريون_freq, u32 baudrate)
{
	uart_بنياد = (متطاير حرف *)بنياد_2;
	u16 baud;

	إذا (baudrate) {
		baud = (u16)(اندريون_freq / (16 * baudrate));
		writew(baud, uart_بنياد + REG_BAUD);
	}

	sbi_console_سيٽ_device(&shakti_console);

	أرجع 0;
}
