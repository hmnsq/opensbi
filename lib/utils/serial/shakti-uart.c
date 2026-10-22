/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/shakti-uart.h>

#تعریف_کرو REG_BAUD	0x00
#تعریف_کرو REG_TX		0x04
#تعریف_کرو REG_RX		0x08
#تعریف_کرو REG_حالت	0x0C
#تعریف_کرو REG_DELAY	0x10
#تعریف_کرو REG_CONTROL	0x14
#تعریف_کرو REG_INT_EN	0x18
#تعریف_کرو REG_IQ_چکراں	0x1C
#تعریف_کرو REG_RX_THRES	0x20

#تعریف_کرو UART_TX_بھریا  0x2
#تعریف_کرو UART_RX_NOT_خالی_تھاواں 0x4
#تعریف_کرو UART_RX_بھریا  0x8

ساكن متطاير حرف *uart_ادھار;

ساكن فراغ shakti_uart_putc(حرف ch)
{
	طالما ((readb(uart_ادھار + REG_حالت) & UART_TX_بھریا))
		;
	writeb(ch, uart_ادھار + REG_TX);
}

ساكن صحيح shakti_uart_getc(فراغ)
{
	إذا (readb(uart_ادھار + REG_حالت) & UART_RX_NOT_خالی_تھاواں)
		أرجع readb(uart_ادھار + REG_RX);
	أرجع -1;
}

ساكن بنية sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

صحيح shakti_uart_init(بنانشانی لما ادھار_2, u32 وچ_freq, u32 baudrate)
{
	uart_ادھار = (متطاير حرف *)ادھار_2;
	u16 baud;

	إذا (baudrate) {
		baud = (u16)(وچ_freq / (16 * baudrate));
		writew(baud, uart_ادھار + REG_BAUD);
	}

	sbi_console_سیٹ_device(&shakti_console);

	أرجع 0;
}
