/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/serial/shakti-uart.h>

#परिभाषय REG_BAUD	0x00
#परिभाषय REG_TX		0x04
#परिभाषय REG_RX		0x08
#परिभाषय REG_स्थितिः	0x0C
#परिभाषय REG_DELAY	0x10
#परिभाषय REG_CONTROL	0x14
#परिभाषय REG_INT_EN	0x18
#परिभाषय REG_IQ_आवृत्तयः	0x1C
#परिभाषय REG_RX_THRES	0x20

#परिभाषय UART_TX_पूर्णम्  0x2
#परिभाषय UART_RX_NOT_रिक्तानि 0x4
#परिभाषय UART_RX_पूर्णम्  0x8

स्थिर चञ्चल वर्ण *uart_आधारः;

स्थिर शून्य shakti_uart_putc(वर्ण ch)
{
	यावत् ((readb(uart_आधारः + REG_स्थितिः) & UART_TX_पूर्णम्))
		;
	writeb(ch, uart_आधारः + REG_TX);
}

स्थिर पूर्णाङ्क shakti_uart_getc(शून्य)
{
	यदि (readb(uart_आधारः + REG_स्थितिः) & UART_RX_NOT_रिक्तानि)
		प्रत्यागम readb(uart_आधारः + REG_RX);
	प्रत्यागम -1;
}

स्थिर संरचना sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

पूर्णाङ्क shakti_uart_init(अचिह्नित दीर्घ आधारः_2, u32 अन्तर्_freq, u32 baudrate)
{
	uart_आधारः = (चञ्चल वर्ण *)आधारः_2;
	u16 baud;

	यदि (baudrate) {
		baud = (u16)(अन्तर्_freq / (16 * baudrate));
		writew(baud, uart_आधारः + REG_BAUD);
	}

	sbi_console_समुच्चयः_device(&shakti_console);

	प्रत्यागम 0;
}
