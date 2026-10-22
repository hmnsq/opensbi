/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi_utils/serial/xlnx_uartlite.h>

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

സ്ഥിതം അസ്ഥിര അക്ഷരം *xlnx_uartlite_base;

സ്ഥിതം ശൂന്യം xlnx_uartlite_putc(അക്ഷരം ch)
{
	ആകുമ്പോൾ((readb(xlnx_uartlite_base + UART_STATUS_OFFSET) & UART_STATUS_TXFULL))
		;

	writeb(ch, xlnx_uartlite_base + UART_TX_OFFSET);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ xlnx_uartlite_getc(ശൂന്യം)
{
	u16 status = readb(xlnx_uartlite_base + UART_STATUS_OFFSET);

	എങ്കിൽ (status & UART_STATUS_RXVALID)
		തിരികെ readb(xlnx_uartlite_base + UART_RX_OFFSET);

	തിരികെ -1;
}

സ്ഥിതം ഘടന sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

പൂർണ്ണസംഖ്യ xlnx_uartlite_init(unsigned long base)
{
	xlnx_uartlite_base = (അസ്ഥിര അക്ഷരം *)base;

	sbi_console_set_device(&xlnx_uartlite_console);

	തിരികെ 0;
}
