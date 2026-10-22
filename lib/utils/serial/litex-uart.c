/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#घोषणा UART_REG_RXTX		0
#घोषणा UART_REG_TXFULL		1
#घोषणा UART_REG_RXEMPTY	2
#घोषणा UART_REG_EV_STATUS	3
#घोषणा UART_REG_EV_PENDING	4
#घोषणा UART_REG_EV_ENABLE	5

#घोषणा UART_EV_RX		0x2

/* clang-format on */

अटल अस्थिर u32 *uart_base;

अटल u8 get_reg(u8 reg)
{
	वापस readb(uart_base + reg);
}

अटल व्योम set_reg(u8 reg, u8 val)
{
	writeb(val, uart_base + reg);
}

अटल व्योम litex_uart_putc(अक्षर ch)
{
	जबतक (get_reg(UART_REG_TXFULL));
	set_reg(UART_REG_RXTX, ch);
}

अटल पूर्णांक litex_uart_getc(व्योम)
{
	पूर्णांक ret;

	अगर (get_reg(UART_REG_RXEMPTY))
		वापस -1;

	ret = get_reg(UART_REG_RXTX);
	set_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	वापस ret;
}

अटल काष्ठा sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

पूर्णांक litex_uart_init(अचिन्हित दीर्घ base)
{
	uart_base = (अस्थिर u32 *)base;
	set_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_set_device(&litex_console);
	वापस 0;
}
