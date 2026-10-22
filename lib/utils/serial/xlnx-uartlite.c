/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/serial/xlnx_uartlite.h>

/* clang-format off */

#నిర్వచించు UART_RX_స్థానభ్రంశం		0x00
#నిర్వచించు UART_TX_స్థానభ్రంశం		0x04
#నిర్వచించు UART_స్థితి_స్థానభ్రంశం	0x08
# define UART_స్థితి_RXVALID	0x01
# define UART_స్థితి_RXFULL	0x02
# define UART_స్థితి_TXEMPTY	0x04
# define UART_స్థితి_TXFULL	0x08
# define UART_స్థితి_IE	0x10
# define UART_స్థితి_OVERRUN	0x20
# define UART_స్థితి_చట్రం	0x40
# define UART_స్థితి_PARITY	0x80
#నిర్వచించు UART_CTRL_స్థానభ్రంశం	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

స్థిర అస్థిర అక్షరం *xlnx_uartlite_పునాది;

స్థిర శూన్య xlnx_uartlite_putc(అక్షరం ch)
{
	అయితే((readb(xlnx_uartlite_పునాది + UART_స్థితి_స్థానభ్రంశం) & UART_స్థితి_TXFULL))
		;

	writeb(ch, xlnx_uartlite_పునాది + UART_TX_స్థానభ్రంశం);
}

స్థిర పూర్ణాంకం xlnx_uartlite_getc(శూన్య)
{
	u16 స్థితి_2 = readb(xlnx_uartlite_పునాది + UART_స్థితి_స్థానభ్రంశం);

	ఒకవేళ (స్థితి_2 & UART_స్థితి_RXVALID)
		తిరిగి readb(xlnx_uartlite_పునాది + UART_RX_స్థానభ్రంశం);

	తిరిగి -1;
}

స్థిర నిర్మాణం sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

పూర్ణాంకం xlnx_uartlite_init(గుర్తులేని దీర్ఘ పునాది_2)
{
	xlnx_uartlite_పునాది = (అస్థిర అక్షరం *)పునాది_2;

	sbi_console_సమితి_device(&xlnx_uartlite_console);

	తిరిగి 0;
}
