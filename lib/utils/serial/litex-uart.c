/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#నిర్వచించు UART_REG_RXTX		0
#నిర్వచించు UART_REG_TXFULL		1
#నిర్వచించు UART_REG_RXEMPTY	2
#నిర్వచించు UART_REG_EV_స్థితి	3
#నిర్వచించు UART_REG_EV_PENDING	4
#నిర్వచించు UART_REG_EV_ENABLE	5

#నిర్వచించు UART_EV_RX		0x2

/* clang-format on */

స్థిర అస్థిర u32 *uart_పునాది;

స్థిర u8 get_reg(u8 reg)
{
	తిరిగి readb(uart_పునాది + reg);
}

స్థిర శూన్య సమితి_reg(u8 reg, u8 val)
{
	writeb(val, uart_పునాది + reg);
}

స్థిర శూన్య litex_uart_putc(అక్షరం ch)
{
	అయితే (get_reg(UART_REG_TXFULL));
	సమితి_reg(UART_REG_RXTX, ch);
}

స్థిర పూర్ణాంకం litex_uart_getc(శూన్య)
{
	పూర్ణాంకం ret;

	ఒకవేళ (get_reg(UART_REG_RXEMPTY))
		తిరిగి -1;

	ret = get_reg(UART_REG_RXTX);
	సమితి_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	తిరిగి ret;
}

స్థిర నిర్మాణం sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

పూర్ణాంకం litex_uart_init(గుర్తులేని దీర్ఘ పునాది_2)
{
	uart_పునాది = (అస్థిర u32 *)పునాది_2;
	సమితి_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_సమితి_device(&litex_console);
	తిరిగి 0;
}
