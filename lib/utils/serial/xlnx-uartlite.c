/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/xlnx_uartlite.h>

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

ସ୍ଥିର ଅସ୍ଥିର ଅକ୍ଷର *xlnx_uartlite_base;

ସ୍ଥିର ଶୂନ୍ୟ xlnx_uartlite_putc(ଅକ୍ଷର ch)
{
	ଯେତେବେଳେ((readb(xlnx_uartlite_base + UART_STATUS_OFFSET) & UART_STATUS_TXFULL))
		;

	writeb(ch, xlnx_uartlite_base + UART_TX_OFFSET);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xlnx_uartlite_getc(ଶୂନ୍ୟ)
{
	u16 status = readb(xlnx_uartlite_base + UART_STATUS_OFFSET);

	ଯଦି (status & UART_STATUS_RXVALID)
		ଫେରନ୍ତୁ readb(xlnx_uartlite_base + UART_RX_OFFSET);

	ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ସଂରଚନା sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xlnx_uartlite_init(unsigned long base)
{
	xlnx_uartlite_base = (ଅସ୍ଥିର ଅକ୍ଷର *)base;

	sbi_console_set_device(&xlnx_uartlite_console);

	ଫେରନ୍ତୁ 0;
}
