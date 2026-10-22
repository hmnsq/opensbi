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

#تعریف_کن UART_RX_جابه_جایی		0x00
#تعریف_کن UART_TX_جابه_جایی		0x04
#تعریف_کن UART_وضعیت_جابه_جایی	0x08
# define UART_وضعیت_RXVALID	0x01
# define UART_وضعیت_RXFULL	0x02
# define UART_وضعیت_TXEMPTY	0x04
# define UART_وضعیت_TXFULL	0x08
# define UART_وضعیت_IE	0x10
# define UART_وضعیت_OVERRUN	0x20
# define UART_وضعیت_قاب	0x40
# define UART_وضعیت_PARITY	0x80
#تعریف_کن UART_CTRL_جابه_جایی	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

ساكن متطاير حرف *xlnx_uartlite_پایه;

ساكن فراغ xlnx_uartlite_putc(حرف ch)
{
	طالما((readb(xlnx_uartlite_پایه + UART_وضعیت_جابه_جایی) & UART_وضعیت_TXFULL))
		;

	writeb(ch, xlnx_uartlite_پایه + UART_TX_جابه_جایی);
}

ساكن صحيح xlnx_uartlite_getc(فراغ)
{
	u16 وضعیت = readb(xlnx_uartlite_پایه + UART_وضعیت_جابه_جایی);

	إذا (وضعیت & UART_وضعیت_RXVALID)
		أرجع readb(xlnx_uartlite_پایه + UART_RX_جابه_جایی);

	أرجع -1;
}

ساكن بنية sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

صحيح xlnx_uartlite_init(بیعلامت دراز پایه_2)
{
	xlnx_uartlite_پایه = (متطاير حرف *)پایه_2;

	sbi_console_مجموعه_device(&xlnx_uartlite_console);

	أرجع 0;
}
