/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi_utils/serial/shakti-uart.h>

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

স্থির চঞ্চল বর্ণ *uart_base;

স্থির শূন্য shakti_uart_putc(বর্ণ ch)
{
	যাবত্ ((readb(uart_base + REG_STATUS) & UART_TX_FULL))
		;
	writeb(ch, uart_base + REG_TX);
}

স্থির পূর্ণাঙ্ক shakti_uart_getc(শূন্য)
{
	যদি (readb(uart_base + REG_STATUS) & UART_RX_NOT_EMPTY)
		প্রত্যাগম readb(uart_base + REG_RX);
	প্রত্যাগম -1;
}

স্থির সংরচনা sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

পূর্ণাঙ্ক shakti_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base = (চঞ্চল বর্ণ *)base;
	u16 baud;

	যদি (baudrate) {
		baud = (u16)(in_freq / (16 * baudrate));
		writew(baud, uart_base + REG_BAUD);
	}

	sbi_console_set_device(&shakti_console);

	প্রত্যাগম 0;
}
