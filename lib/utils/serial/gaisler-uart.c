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

#تعریف_کړه UART_REG_معلومات		0
#تعریف_کړه UART_REG_حالت		1
#تعریف_کړه UART_REG_CTRL		2
#تعریف_کړه UART_REG_SCALER		3

#تعریف_کړه UART_معلومات_معلومات		0x000000ff
#تعریف_کړه UART_حالت_FIFOFULL	0x00000200
#تعریف_کړه UART_حالت_DATAREADY	0x00000001

#تعریف_کړه UART_CTRL_DB		(1<<11)
#تعریف_کړه UART_CTRL_FL		(1<<6)
#تعریف_کړه UART_CTRL_TE		(1<<1)
#تعریف_کړه UART_CTRL_RE		(1<<0)

/* clang-format on */

ساكن متطاير حرف *uart_بنسټ;

ساكن u32 get_reg(u32 عدد)
{
	أرجع readl(uart_بنسټ + (عدد * 0x4));
}

ساكن فراغ سېټ_reg(u32 عدد, u32 val)
{
	writel(val, uart_بنسټ + (عدد * 0x4));
}

ساكن فراغ gaisler_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_حالت) & UART_حالت_FIFOFULL)
		;

	سېټ_reg(UART_REG_معلومات, ch);
}

ساكن صحيح gaisler_uart_getc(فراغ)
{
	u32 ret = get_reg(UART_REG_حالت);
	إذا (!(ret & UART_حالت_DATAREADY))
		أرجع -1;
	أرجع get_reg(UART_REG_معلومات) & UART_معلومات_معلومات;
}

ساكن بنية sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

صحيح gaisler_uart_init(بینښه اوږد بنسټ_2, u32 دننه_freq, u32 baudrate)
{
	u32 ctrl;

	uart_بنسټ = (متطاير حرف *)بنسټ_2;

	/* Configure baudrate */
	إذا (دننه_freq && baudrate)
		سېټ_reg(UART_REG_SCALER, دننه_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	سېټ_reg(UART_REG_CTRL, ctrl);

	sbi_console_سېټ_device(&gaisler_console);

	أرجع 0;
}
