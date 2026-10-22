/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi_utils/serial/shakti-uart.h>

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

സ്ഥിതം അസ്ഥിര അക്ഷരം *uart_base;

സ്ഥിതം ശൂന്യം shakti_uart_putc(അക്ഷരം ch)
{
	ആകുമ്പോൾ ((readb(uart_base + REG_STATUS) & UART_TX_FULL))
		;
	writeb(ch, uart_base + REG_TX);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ shakti_uart_getc(ശൂന്യം)
{
	എങ്കിൽ (readb(uart_base + REG_STATUS) & UART_RX_NOT_EMPTY)
		തിരികെ readb(uart_base + REG_RX);
	തിരികെ -1;
}

സ്ഥിതം ഘടന sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

പൂർണ്ണസംഖ്യ shakti_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base = (അസ്ഥിര അക്ഷരം *)base;
	u16 baud;

	എങ്കിൽ (baudrate) {
		baud = (u16)(in_freq / (16 * baudrate));
		writew(baud, uart_base + REG_BAUD);
	}

	sbi_console_set_device(&shakti_console);

	തിരികെ 0;
}
