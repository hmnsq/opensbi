/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/shakti-uart.h>

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

ಸ್ಥಿರ ಚಞ್ಚಲ ವರ್ಣ *uart_base;

ಸ್ಥಿರ ಶೂನ್ಯ shakti_uart_putc(ವರ್ಣ ch)
{
	ಯಾವತ್ ((readb(uart_base + REG_STATUS) & UART_TX_FULL))
		;
	writeb(ch, uart_base + REG_TX);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ shakti_uart_getc(ಶೂನ್ಯ)
{
	ಯದಿ (readb(uart_base + REG_STATUS) & UART_RX_NOT_EMPTY)
		ಪ್ರತ್ಯಾಗಮ readb(uart_base + REG_RX);
	ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

ಪೂರ್ಣಾಙ್ಕ shakti_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base = (ಚಞ್ಚಲ ವರ್ಣ *)base;
	u16 baud;

	ಯದಿ (baudrate) {
		baud = (u16)(in_freq / (16 * baudrate));
		writew(baud, uart_base + REG_BAUD);
	}

	sbi_console_set_device(&shakti_console);

	ಪ್ರತ್ಯಾಗಮ 0;
}
