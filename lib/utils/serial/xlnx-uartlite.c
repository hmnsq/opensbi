/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi_utils/serial/xlnx_uartlite.h>

/* clang-format off */

#define UART_RX_OFFSET		0x00
#define UART_TX_OFFSET		0x04
#define UART_STATUS_OFFSET	0x08
# define UART_STATUS_RXVALID	0x01
# define UART_STATUS_RXFULL	0x02
# define UART_STATUS_TXEMPTY	0x04
# define UART_STATUS_TXFULL	0x08
# define UART_STATUS_IE	0x10
# define UART_STATUS_OVERRUN	0x20
# define UART_STATUS_FRAME	0x40
# define UART_STATUS_PARITY	0x80
#define UART_CTRL_OFFSET	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

স্থির চঞ্চল বর্ণ *xlnx_uartlite_base;

স্থির শূন্য xlnx_uartlite_putc(বর্ণ ch)
{
	যাবত্((readb(xlnx_uartlite_base + UART_STATUS_OFFSET) & UART_STATUS_TXFULL))
		;

	writeb(ch, xlnx_uartlite_base + UART_TX_OFFSET);
}

স্থির পূর্ণাঙ্ক xlnx_uartlite_getc(শূন্য)
{
	u16 status = readb(xlnx_uartlite_base + UART_STATUS_OFFSET);

	যদি (status & UART_STATUS_RXVALID)
		প্রত্যাগম readb(xlnx_uartlite_base + UART_RX_OFFSET);

	প্রত্যাগম -1;
}

স্থির সংরচনা sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

পূর্ণাঙ্ক xlnx_uartlite_init(unsigned long base)
{
	xlnx_uartlite_base = (চঞ্চল বর্ণ *)base;

	sbi_console_set_device(&xlnx_uartlite_console);

	প্রত্যাগম 0;
}
