/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#define UART_REG_RXTX		0
#define UART_REG_TXFULL		1
#define UART_REG_RXEMPTY	2
#define UART_REG_EV_STATUS	3
#define UART_REG_EV_PENDING	4
#define UART_REG_EV_ENABLE	5

#define UART_EV_RX		0x2

/* clang-format on */

स्थिर अस्थिर u32 *uart_base;

स्थिर u8 get_reg(u8 reg)
{
	परत readb(uart_base + reg);
}

स्थिर शून्य set_reg(u8 reg, u8 val)
{
	writeb(val, uart_base + reg);
}

स्थिर शून्य litex_uart_putc(अक्षर ch)
{
	पर्यंत (get_reg(UART_REG_TXFULL));
	set_reg(UART_REG_RXTX, ch);
}

स्थिर पूर्णांक litex_uart_getc(शून्य)
{
	पूर्णांक ret;

	जर (get_reg(UART_REG_RXEMPTY))
		परत -1;

	ret = get_reg(UART_REG_RXTX);
	set_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	परत ret;
}

स्थिर संरचना sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

पूर्णांक litex_uart_init(अचिन्हित दीर्घ base)
{
	uart_base = (अस्थिर u32 *)base;
	set_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_set_device(&litex_console);
	परत 0;
}
