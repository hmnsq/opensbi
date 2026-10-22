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

#تعریف_کن UART_REG_داده		0
#تعریف_کن UART_REG_وضعیت		1
#تعریف_کن UART_REG_CTRL		2
#تعریف_کن UART_REG_SCALER		3

#تعریف_کن UART_داده_داده		0x000000ff
#تعریف_کن UART_وضعیت_FIFOFULL	0x00000200
#تعریف_کن UART_وضعیت_DATAREADY	0x00000001

#تعریف_کن UART_CTRL_DB		(1<<11)
#تعریف_کن UART_CTRL_FL		(1<<6)
#تعریف_کن UART_CTRL_TE		(1<<1)
#تعریف_کن UART_CTRL_RE		(1<<0)

/* clang-format on */

ساكن متطاير حرف *uart_پایه;

ساكن u32 get_reg(u32 رقم)
{
	أرجع readl(uart_پایه + (رقم * 0x4));
}

ساكن فراغ مجموعه_reg(u32 رقم, u32 val)
{
	writel(val, uart_پایه + (رقم * 0x4));
}

ساكن فراغ gaisler_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_وضعیت) & UART_وضعیت_FIFOFULL)
		;

	مجموعه_reg(UART_REG_داده, ch);
}

ساكن صحيح gaisler_uart_getc(فراغ)
{
	u32 ret = get_reg(UART_REG_وضعیت);
	إذا (!(ret & UART_وضعیت_DATAREADY))
		أرجع -1;
	أرجع get_reg(UART_REG_داده) & UART_داده_داده;
}

ساكن بنية sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

صحيح gaisler_uart_init(بیعلامت دراز پایه_2, u32 درون_freq, u32 baudrate)
{
	u32 ctrl;

	uart_پایه = (متطاير حرف *)پایه_2;

	/* Configure baudrate */
	إذا (درون_freq && baudrate)
		مجموعه_reg(UART_REG_SCALER, درون_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	مجموعه_reg(UART_REG_CTRL, ctrl);

	sbi_console_مجموعه_device(&gaisler_console);

	أرجع 0;
}
