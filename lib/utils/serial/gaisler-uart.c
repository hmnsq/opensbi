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

#وصف_ڪريو UART_REG_معلومات		0
#وصف_ڪريو UART_REG_حالت		1
#وصف_ڪريو UART_REG_CTRL		2
#وصف_ڪريو UART_REG_SCALER		3

#وصف_ڪريو UART_معلومات_معلومات		0x000000ff
#وصف_ڪريو UART_حالت_FIFOFULL	0x00000200
#وصف_ڪريو UART_حالت_DATAREADY	0x00000001

#وصف_ڪريو UART_CTRL_DB		(1<<11)
#وصف_ڪريو UART_CTRL_FL		(1<<6)
#وصف_ڪريو UART_CTRL_TE		(1<<1)
#وصف_ڪريو UART_CTRL_RE		(1<<0)

/* clang-format on */

ساكن متطاير حرف *uart_بنياد;

ساكن u32 get_reg(u32 انگ)
{
	أرجع readl(uart_بنياد + (انگ * 0x4));
}

ساكن فراغ سيٽ_reg(u32 انگ, u32 val)
{
	writel(val, uart_بنياد + (انگ * 0x4));
}

ساكن فراغ gaisler_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_حالت) & UART_حالت_FIFOFULL)
		;

	سيٽ_reg(UART_REG_معلومات, ch);
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

صحيح gaisler_uart_init(بنانشان ڊگهو بنياد_2, u32 اندريون_freq, u32 baudrate)
{
	u32 ctrl;

	uart_بنياد = (متطاير حرف *)بنياد_2;

	/* Configure baudrate */
	إذا (اندريون_freq && baudrate)
		سيٽ_reg(UART_REG_SCALER, اندريون_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	سيٽ_reg(UART_REG_CTRL, ctrl);

	sbi_console_سيٽ_device(&gaisler_console);

	أرجع 0;
}
