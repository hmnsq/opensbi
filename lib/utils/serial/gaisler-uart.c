/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/gaisler-uart.h>

/* clang-format off */

#تعریف_کرو UART_REG_انکڑے		0
#تعریف_کرو UART_REG_حالت		1
#تعریف_کرو UART_REG_CTRL		2
#تعریف_کرو UART_REG_SCALER		3

#تعریف_کرو UART_انکڑے_انکڑے		0x000000ff
#تعریف_کرو UART_حالت_FIFOFULL	0x00000200
#تعریف_کرو UART_حالت_DATAREADY	0x00000001

#تعریف_کرو UART_CTRL_DB		(1<<11)
#تعریف_کرو UART_CTRL_FL		(1<<6)
#تعریف_کرو UART_CTRL_TE		(1<<1)
#تعریف_کرو UART_CTRL_RE		(1<<0)

/* clang-format on */

ساكن متطاير حرف *uart_ادھار;

ساكن u32 get_reg(u32 عدد)
{
	أرجع readl(uart_ادھار + (عدد * 0x4));
}

ساكن فراغ سیٹ_reg(u32 عدد, u32 val)
{
	writel(val, uart_ادھار + (عدد * 0x4));
}

ساكن فراغ gaisler_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_حالت) & UART_حالت_FIFOFULL)
		;

	سیٹ_reg(UART_REG_انکڑے, ch);
}

ساكن صحيح gaisler_uart_getc(فراغ)
{
	u32 ret = get_reg(UART_REG_حالت);
	إذا (!(ret & UART_حالت_DATAREADY))
		أرجع -1;
	أرجع get_reg(UART_REG_انکڑے) & UART_انکڑے_انکڑے;
}

ساكن بنية sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

صحيح gaisler_uart_init(بنانشانی لما ادھار_2, u32 وچ_freq, u32 baudrate)
{
	u32 ctrl;

	uart_ادھار = (متطاير حرف *)ادھار_2;

	/* Configure baudrate */
	إذا (وچ_freq && baudrate)
		سیٹ_reg(UART_REG_SCALER, وچ_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	سیٹ_reg(UART_REG_CTRL, ctrl);

	sbi_console_سیٹ_device(&gaisler_console);

	أرجع 0;
}
