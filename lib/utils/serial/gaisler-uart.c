/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/serial/gaisler-uart.h>

/* clang-format off */

#define UART_REG_DATA		0
#define UART_REG_STATUS		1
#define UART_REG_CTRL		2
#define UART_REG_SCALER		3

#define UART_DATA_DATA		0x000000ff
#define UART_STATUS_FIFOFULL	0x00000200
#define UART_STATUS_DATAREADY	0x00000001

#define UART_CTRL_DB		(1<<11)
#define UART_CTRL_FL		(1<<6)
#define UART_CTRL_TE		(1<<1)
#define UART_CTRL_RE		(1<<0)

/* clang-format on */

स्थिर अस्थिर अक्षर *uart_base;

स्थिर u32 get_reg(u32 अंक)
{
	परत readl(uart_base + (अंक * 0x4));
}

स्थिर शून्य set_reg(u32 अंक, u32 val)
{
	writel(val, uart_base + (अंक * 0x4));
}

स्थिर शून्य gaisler_uart_putc(अक्षर ch)
{
	पर्यंत (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_DATA, ch);
}

स्थिर पूर्णांक gaisler_uart_getc(शून्य)
{
	u32 ret = get_reg(UART_REG_STATUS);
	जर (!(ret & UART_STATUS_DATAREADY))
		परत -1;
	परत get_reg(UART_REG_DATA) & UART_DATA_DATA;
}

स्थिर संरचना sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

पूर्णांक gaisler_uart_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (अस्थिर अक्षर *)base;

	/* Configure baudrate */
	जर (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	परत 0;
}
