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

स्थिर चञ्चल वर्ण *uart_base;

स्थिर u32 get_reg(u32 अङ्क)
{
	फर्काऊ readl(uart_base + (अङ्क * 0x4));
}

स्थिर शून्य set_reg(u32 अङ्क, u32 val)
{
	writel(val, uart_base + (अङ्क * 0x4));
}

स्थिर शून्य gaisler_uart_putc(वर्ण ch)
{
	जबसम्म (get_reg(UART_REG_STATUS) & UART_STATUS_FIFOFULL)
		;

	set_reg(UART_REG_DATA, ch);
}

स्थिर पूर्णाङ्क gaisler_uart_getc(शून्य)
{
	u32 ret = get_reg(UART_REG_STATUS);
	यदि (!(ret & UART_STATUS_DATAREADY))
		फर्काऊ -1;
	फर्काऊ get_reg(UART_REG_DATA) & UART_DATA_DATA;
}

स्थिर संरचना sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

पूर्णाङ्क gaisler_uart_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate)
{
	u32 ctrl;

	uart_base = (चञ्चल वर्ण *)base;

	/* Configure baudrate */
	यदि (in_freq && baudrate)
		set_reg(UART_REG_SCALER, in_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	set_reg(UART_REG_CTRL, ctrl);

	sbi_console_set_device(&gaisler_console);

	फर्काऊ 0;
}
