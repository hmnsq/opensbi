/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/serial/shakti-uart.h>

#నిర్వచించు REG_BAUD	0x00
#నిర్వచించు REG_TX		0x04
#నిర్వచించు REG_RX		0x08
#నిర్వచించు REG_స్థితి	0x0C
#నిర్వచించు REG_DELAY	0x10
#నిర్వచించు REG_CONTROL	0x14
#నిర్వచించు REG_INT_EN	0x18
#నిర్వచించు REG_IQ_చక్రాలు	0x1C
#నిర్వచించు REG_RX_THRES	0x20

#నిర్వచించు UART_TX_నిండిన  0x2
#నిర్వచించు UART_RX_NOT_ఖాళీలు 0x4
#నిర్వచించు UART_RX_నిండిన  0x8

స్థిర అస్థిర అక్షరం *uart_పునాది;

స్థిర శూన్య shakti_uart_putc(అక్షరం ch)
{
	అయితే ((readb(uart_పునాది + REG_స్థితి) & UART_TX_నిండిన))
		;
	writeb(ch, uart_పునాది + REG_TX);
}

స్థిర పూర్ణాంకం shakti_uart_getc(శూన్య)
{
	ఒకవేళ (readb(uart_పునాది + REG_స్థితి) & UART_RX_NOT_ఖాళీలు)
		తిరిగి readb(uart_పునాది + REG_RX);
	తిరిగి -1;
}

స్థిర నిర్మాణం sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

పూర్ణాంకం shakti_uart_init(గుర్తులేని దీర్ఘ పునాది_2, u32 లోపల_freq, u32 baudrate)
{
	uart_పునాది = (అస్థిర అక్షరం *)పునాది_2;
	u16 baud;

	ఒకవేళ (baudrate) {
		baud = (u16)(లోపల_freq / (16 * baudrate));
		writew(baud, uart_పునాది + REG_BAUD);
	}

	sbi_console_సమితి_device(&shakti_console);

	తిరిగి 0;
}
