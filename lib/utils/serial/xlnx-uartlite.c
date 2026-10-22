/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi_utils/serial/xlnx_uartlite.h>

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

સ્થિર અસ્થિર અક્ષર *xlnx_uartlite_base;

સ્થિર શૂન્ય xlnx_uartlite_putc(અક્ષર ch)
{
	જ્યારે((readb(xlnx_uartlite_base + UART_STATUS_OFFSET) & UART_STATUS_TXFULL))
		;

	writeb(ch, xlnx_uartlite_base + UART_TX_OFFSET);
}

સ્થિર પૂર્ણાંક xlnx_uartlite_getc(શૂન્ય)
{
	u16 status = readb(xlnx_uartlite_base + UART_STATUS_OFFSET);

	જો (status & UART_STATUS_RXVALID)
		પાછા readb(xlnx_uartlite_base + UART_RX_OFFSET);

	પાછા -1;
}

સ્થિર સંરચના sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

પૂર્ણાંક xlnx_uartlite_init(unsigned long base)
{
	xlnx_uartlite_base = (અસ્થિર અક્ષર *)base;

	sbi_console_set_device(&xlnx_uartlite_console);

	પાછા 0;
}
