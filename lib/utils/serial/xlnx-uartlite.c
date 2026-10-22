/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/serial/xlnx_uartlite.h>

/* clang-format off */

#تعریف_کرو UART_RX_انحراف		0x00
#تعریف_کرو UART_TX_انحراف		0x04
#تعریف_کرو UART_حالت_انحراف	0x08
# define UART_حالت_RXVALID	0x01
# define UART_حالت_RXFULL	0x02
# define UART_حالت_TXEMPTY	0x04
# define UART_حالت_TXFULL	0x08
# define UART_حالت_IE	0x10
# define UART_حالت_OVERRUN	0x20
# define UART_حالت_چوکھٹا	0x40
# define UART_حالت_PARITY	0x80
#تعریف_کرو UART_CTRL_انحراف	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

ساکن متلون حرف *xlnx_uartlite_بنیاد;

ساکن خالی xlnx_uartlite_putc(حرف ch)
{
	جب((readb(xlnx_uartlite_بنیاد + UART_حالت_انحراف) & UART_حالت_TXFULL))
		;

	writeb(ch, xlnx_uartlite_بنیاد + UART_TX_انحراف);
}

ساکن صحیح xlnx_uartlite_getc(خالی)
{
	u16 حالت_2 = readb(xlnx_uartlite_بنیاد + UART_حالت_انحراف);

	اگر (حالت_2 & UART_حالت_RXVALID)
		واپس readb(xlnx_uartlite_بنیاد + UART_RX_انحراف);

	واپس -1;
}

ساکن ساخت sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

صحیح xlnx_uartlite_init(غیرعلامتی طویل بنیاد_2)
{
	xlnx_uartlite_بنیاد = (متلون حرف *)بنیاد_2;

	sbi_console_سیٹ_device(&xlnx_uartlite_console);

	واپس 0;
}
