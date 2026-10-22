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

#परिभाषय UART_REG_दत्त		0
#परिभाषय UART_REG_स्थितिः		1
#परिभाषय UART_REG_CTRL		2
#परिभाषय UART_REG_SCALER		3

#परिभाषय UART_दत्त_दत्त		0x000000ff
#परिभाषय UART_स्थितिः_FIFOFULL	0x00000200
#परिभाषय UART_स्थितिः_DATAREADY	0x00000001

#परिभाषय UART_CTRL_DB		(1<<11)
#परिभाषय UART_CTRL_FL		(1<<6)
#परिभाषय UART_CTRL_TE		(1<<1)
#परिभाषय UART_CTRL_RE		(1<<0)

/* clang-format on */

स्थिर चञ्चल वर्ण *uart_आधारः;

स्थिर u32 get_reg(u32 अङ्कः)
{
	प्रत्यागम readl(uart_आधारः + (अङ्कः * 0x4));
}

स्थिर शून्य समुच्चयः_reg(u32 अङ्कः, u32 val)
{
	writel(val, uart_आधारः + (अङ्कः * 0x4));
}

स्थिर शून्य gaisler_uart_putc(वर्ण ch)
{
	यावत् (get_reg(UART_REG_स्थितिः) & UART_स्थितिः_FIFOFULL)
		;

	समुच्चयः_reg(UART_REG_दत्त, ch);
}

स्थिर पूर्णाङ्क gaisler_uart_getc(शून्य)
{
	u32 ret = get_reg(UART_REG_स्थितिः);
	यदि (!(ret & UART_स्थितिः_DATAREADY))
		प्रत्यागम -1;
	प्रत्यागम get_reg(UART_REG_दत्त) & UART_दत्त_दत्त;
}

स्थिर संरचना sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

पूर्णाङ्क gaisler_uart_init(अचिह्नित दीर्घ आधारः_2, u32 अन्तर्_freq, u32 baudrate)
{
	u32 ctrl;

	uart_आधारः = (चञ्चल वर्ण *)आधारः_2;

	/* Configure baudrate */
	यदि (अन्तर्_freq && baudrate)
		समुच्चयः_reg(UART_REG_SCALER, अन्तर्_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	समुच्चयः_reg(UART_REG_CTRL, ctrl);

	sbi_console_समुच्चयः_device(&gaisler_console);

	प्रत्यागम 0;
}
