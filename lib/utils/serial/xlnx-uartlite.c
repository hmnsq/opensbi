/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi_utils/serial/xlnx_uartlite.h>

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

நிலையான நிலையற்ற எழுத்து *xlnx_uartlite_base;

நிலையான வெறுமை xlnx_uartlite_putc(எழுத்து ch)
{
	போது((readb(xlnx_uartlite_base + UART_STATUS_OFFSET) & UART_STATUS_TXFULL))
		;

	writeb(ch, xlnx_uartlite_base + UART_TX_OFFSET);
}

நிலையான முழு xlnx_uartlite_getc(வெறுமை)
{
	u16 status = readb(xlnx_uartlite_base + UART_STATUS_OFFSET);

	எனில் (status & UART_STATUS_RXVALID)
		திரும்பு readb(xlnx_uartlite_base + UART_RX_OFFSET);

	திரும்பு -1;
}

நிலையான கட்டமைப்பு sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

முழு xlnx_uartlite_init(unsigned long base)
{
	xlnx_uartlite_base = (நிலையற்ற எழுத்து *)base;

	sbi_console_set_device(&xlnx_uartlite_console);

	திரும்பு 0;
}
