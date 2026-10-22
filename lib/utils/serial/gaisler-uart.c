/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/serial/gaisler-uart.h>

/* clang-format off */

#నిర్వచించు UART_REG_దత్తాంశం		0
#నిర్వచించు UART_REG_స్థితి		1
#నిర్వచించు UART_REG_CTRL		2
#నిర్వచించు UART_REG_SCALER		3

#నిర్వచించు UART_దత్తాంశం_దత్తాంశం		0x000000ff
#నిర్వచించు UART_స్థితి_FIFOFULL	0x00000200
#నిర్వచించు UART_స్థితి_DATAREADY	0x00000001

#నిర్వచించు UART_CTRL_DB		(1<<11)
#నిర్వచించు UART_CTRL_FL		(1<<6)
#నిర్వచించు UART_CTRL_TE		(1<<1)
#నిర్వచించు UART_CTRL_RE		(1<<0)

/* clang-format on */

స్థిర అస్థిర అక్షరం *uart_పునాది;

స్థిర u32 get_reg(u32 అంకె)
{
	తిరిగి readl(uart_పునాది + (అంకె * 0x4));
}

స్థిర శూన్య సమితి_reg(u32 అంకె, u32 val)
{
	writel(val, uart_పునాది + (అంకె * 0x4));
}

స్థిర శూన్య gaisler_uart_putc(అక్షరం ch)
{
	అయితే (get_reg(UART_REG_స్థితి) & UART_స్థితి_FIFOFULL)
		;

	సమితి_reg(UART_REG_దత్తాంశం, ch);
}

స్థిర పూర్ణాంకం gaisler_uart_getc(శూన్య)
{
	u32 ret = get_reg(UART_REG_స్థితి);
	ఒకవేళ (!(ret & UART_స్థితి_DATAREADY))
		తిరిగి -1;
	తిరిగి get_reg(UART_REG_దత్తాంశం) & UART_దత్తాంశం_దత్తాంశం;
}

స్థిర నిర్మాణం sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

పూర్ణాంకం gaisler_uart_init(గుర్తులేని దీర్ఘ పునాది_2, u32 లోపల_freq, u32 baudrate)
{
	u32 ctrl;

	uart_పునాది = (అస్థిర అక్షరం *)పునాది_2;

	/* Configure baudrate */
	ఒకవేళ (లోపల_freq && baudrate)
		సమితి_reg(UART_REG_SCALER, లోపల_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	సమితి_reg(UART_REG_CTRL, ctrl);

	sbi_console_సమితి_device(&gaisler_console);

	తిరిగి 0;
}
