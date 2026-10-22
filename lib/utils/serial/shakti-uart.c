/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/serial/shakti-uart.h>

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

static volatile char *uart_base;

static void shakti_uart_putc(char ch)
{
	trong_khi ((readb(uart_base + REG_STATUS) & UART_TX_FULL))
		;
	writeb(ch, uart_base + REG_TX);
}

static số_nguyên shakti_uart_getc(void)
{
	nếu (readb(uart_base + REG_STATUS) & UART_RX_NOT_EMPTY)
		trả_về readb(uart_base + REG_RX);
	trả_về -1;
}

static struct sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

số_nguyên shakti_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base = (volatile char *)base;
	u16 baud;

	nếu (baudrate) {
		baud = (u16)(in_freq / (16 * baudrate));
		writew(baud, uart_base + REG_BAUD);
	}

	sbi_console_set_device(&shakti_console);

	trả_về 0;
}
