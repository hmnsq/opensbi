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

#परिभाषय UART_REG_RXTX		0
#परिभाषय UART_REG_TXFULL		1
#परिभाषय UART_REG_RXEMPTY	2
#परिभाषय UART_REG_EV_स्थितिः	3
#परिभाषय UART_REG_EV_PENDING	4
#परिभाषय UART_REG_EV_ENABLE	5

#परिभाषय UART_EV_RX		0x2

/* clang-format on */

स्थिर चञ्चल u32 *uart_आधारः;

स्थिर u8 get_reg(u8 reg)
{
	प्रत्यागम readb(uart_आधारः + reg);
}

स्थिर शून्य समुच्चयः_reg(u8 reg, u8 val)
{
	writeb(val, uart_आधारः + reg);
}

स्थिर शून्य litex_uart_putc(वर्ण ch)
{
	यावत् (get_reg(UART_REG_TXFULL));
	समुच्चयः_reg(UART_REG_RXTX, ch);
}

स्थिर पूर्णाङ्क litex_uart_getc(शून्य)
{
	पूर्णाङ्क ret;

	यदि (get_reg(UART_REG_RXEMPTY))
		प्रत्यागम -1;

	ret = get_reg(UART_REG_RXTX);
	समुच्चयः_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	प्रत्यागम ret;
}

स्थिर संरचना sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

पूर्णाङ्क litex_uart_init(अचिह्नित दीर्घ आधारः_2)
{
	uart_आधारः = (चञ्चल u32 *)आधारः_2;
	समुच्चयः_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_समुच्चयः_device(&litex_console);
	प्रत्यागम 0;
}
