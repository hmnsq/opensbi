/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/serial/shakti-uart.h>

#define REG_BAUD	0x00
#define REG_TX		0x04
#define REG_RX		0x08
#define REG_STATUS	0x0C
#define REG_DELAY	0x10
#define REG_CONTROL	0x14
#define REG_INT_EN	0x18
#define REG_IQ_CYCLES	0x1C
#define REG_RX_THRES	0x20

#define UART_TX_FULL  0x2
#define UART_RX_NOT_EMPTY 0x4
#define UART_RX_FULL  0x8

स्थिर चञ्चल वर्ण *uart_base;

स्थिर शून्य shakti_uart_putc(वर्ण ch)
{
	जबसम्म ((readb(uart_base + REG_STATUS) & UART_TX_FULL))
		;
	writeb(ch, uart_base + REG_TX);
}

स्थिर पूर्णाङ्क shakti_uart_getc(शून्य)
{
	यदि (readb(uart_base + REG_STATUS) & UART_RX_NOT_EMPTY)
		फर्काऊ readb(uart_base + REG_RX);
	फर्काऊ -1;
}

स्थिर संरचना sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

पूर्णाङ्क shakti_uart_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate)
{
	uart_base = (चञ्चल वर्ण *)base;
	u16 baud;

	यदि (baudrate) {
		baud = (u16)(in_freq / (16 * baudrate));
		writew(baud, uart_base + REG_BAUD);
	}

	sbi_console_set_device(&shakti_console);

	फर्काऊ 0;
}
