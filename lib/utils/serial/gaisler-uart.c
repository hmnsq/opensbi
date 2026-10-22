/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/serial/gaisler-uart.h>

/* clang-format off */

#تعریف_کرو UART_REG_ڈیٹا		0
#تعریف_کرو UART_REG_حالت		1
#تعریف_کرو UART_REG_CTRL		2
#تعریف_کرو UART_REG_SCALER		3

#تعریف_کرو UART_ڈیٹا_ڈیٹا		0x000000ff
#تعریف_کرو UART_حالت_FIFOFULL	0x00000200
#تعریف_کرو UART_حالت_DATAREADY	0x00000001

#تعریف_کرو UART_CTRL_DB		(1<<11)
#تعریف_کرو UART_CTRL_FL		(1<<6)
#تعریف_کرو UART_CTRL_TE		(1<<1)
#تعریف_کرو UART_CTRL_RE		(1<<0)

/* clang-format on */

ساکن متلون حرف *uart_بنیاد;

ساکن u32 get_reg(u32 عدد)
{
	واپس readl(uart_بنیاد + (عدد * 0x4));
}

ساکن خالی سیٹ_reg(u32 عدد, u32 val)
{
	writel(val, uart_بنیاد + (عدد * 0x4));
}

ساکن خالی gaisler_uart_putc(حرف ch)
{
	جب (get_reg(UART_REG_حالت) & UART_حالت_FIFOFULL)
		;

	سیٹ_reg(UART_REG_ڈیٹا, ch);
}

ساکن صحیح gaisler_uart_getc(خالی)
{
	u32 ret = get_reg(UART_REG_حالت);
	اگر (!(ret & UART_حالت_DATAREADY))
		واپس -1;
	واپس get_reg(UART_REG_ڈیٹا) & UART_ڈیٹا_ڈیٹا;
}

ساکن ساخت sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

صحیح gaisler_uart_init(غیرعلامتی طویل بنیاد_2, u32 اندر_freq, u32 baudrate)
{
	u32 ctrl;

	uart_بنیاد = (متلون حرف *)بنیاد_2;

	/* Configure baudrate */
	اگر (اندر_freq && baudrate)
		سیٹ_reg(UART_REG_SCALER, اندر_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	سیٹ_reg(UART_REG_CTRL, ctrl);

	sbi_console_سیٹ_device(&gaisler_console);

	واپس 0;
}
