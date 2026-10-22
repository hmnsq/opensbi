/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/serial/xlnx_uartlite.h>

/* clang-format off */

#घोषणा UART_RX_OFFSET		0x00
#घोषणा UART_TX_OFFSET		0x04
#घोषणा UART_STATUS_OFFSET	0x08
# define UART_STATUS_RXVALID	0x01
# define UART_STATUS_RXFULL	0x02
# define UART_STATUS_TXEMPTY	0x04
# define UART_STATUS_TXFULL	0x08
# define UART_STATUS_IE	0x10
# define UART_STATUS_OVERRUN	0x20
# define UART_STATUS_FRAME	0x40
# define UART_STATUS_PARITY	0x80
#घोषणा UART_CTRL_OFFSET	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

अटल अस्थिर अक्षर *xlnx_uartlite_base;

अटल व्योम xlnx_uartlite_putc(अक्षर ch)
{
	जबतक((readb(xlnx_uartlite_base + UART_STATUS_OFFSET) & UART_STATUS_TXFULL))
		;

	writeb(ch, xlnx_uartlite_base + UART_TX_OFFSET);
}

अटल पूर्णांक xlnx_uartlite_getc(व्योम)
{
	u16 status = readb(xlnx_uartlite_base + UART_STATUS_OFFSET);

	अगर (status & UART_STATUS_RXVALID)
		वापस readb(xlnx_uartlite_base + UART_RX_OFFSET);

	वापस -1;
}

अटल काष्ठा sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

पूर्णांक xlnx_uartlite_init(अचिन्हित दीर्घ base)
{
	xlnx_uartlite_base = (अस्थिर अक्षर *)base;

	sbi_console_set_device(&xlnx_uartlite_console);

	वापस 0;
}
