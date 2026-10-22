/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/xlnx_uartlite.h>

/* clang-format off */

#تعریف_کړه UART_RX_دننه_واټن		0x00
#تعریف_کړه UART_TX_دننه_واټن		0x04
#تعریف_کړه UART_حالت_دننه_واټن	0x08
# define UART_حالت_RXVALID	0x01
# define UART_حالت_RXFULL	0x02
# define UART_حالت_TXEMPTY	0x04
# define UART_حالت_TXFULL	0x08
# define UART_حالت_IE	0x10
# define UART_حالت_OVERRUN	0x20
# define UART_حالت_چوکاټ	0x40
# define UART_حالت_PARITY	0x80
#تعریف_کړه UART_CTRL_دننه_واټن	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

ساكن متطاير حرف *xlnx_uartlite_بنسټ;

ساكن فراغ xlnx_uartlite_putc(حرف ch)
{
	طالما((readb(xlnx_uartlite_بنسټ + UART_حالت_دننه_واټن) & UART_حالت_TXFULL))
		;

	writeb(ch, xlnx_uartlite_بنسټ + UART_TX_دننه_واټن);
}

ساكن صحيح xlnx_uartlite_getc(فراغ)
{
	u16 حالت = readb(xlnx_uartlite_بنسټ + UART_حالت_دننه_واټن);

	إذا (حالت & UART_حالت_RXVALID)
		أرجع readb(xlnx_uartlite_بنسټ + UART_RX_دننه_واټن);

	أرجع -1;
}

ساكن بنية sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

صحيح xlnx_uartlite_init(بینښه اوږد بنسټ_2)
{
	xlnx_uartlite_بنسټ = (متطاير حرف *)بنسټ_2;

	sbi_console_سېټ_device(&xlnx_uartlite_console);

	أرجع 0;
}
